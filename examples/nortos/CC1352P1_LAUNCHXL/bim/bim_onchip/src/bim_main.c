/******************************************************************************

 @file  bim_main.c

 @brief This module contains the definitions for the main functionality of a
        Boot  Image Manager for on chip OAD.

 Group: CMCU
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2018-2020, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*******************************************************************************
 *                                          Includes
 */
#include <stdint.h>
#include <string.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/watchdog.h)
#include DeviceFamily_constructPath(inc/hw_prcm.h)

#include "common/cc26xx/crc/crc32.h"
#include "common/cc26xx/flash_interface/flash_interface.h"
#include "common/cc26xx/bim/bim_util.h"
#include "common/cc26xx/oad/oad_image_header.h"

#ifdef __IAR_SYSTEMS_ICC__
#include <intrinsics.h>
#endif

#ifdef DEBUG
#include DeviceFamily_constructPath(driverlib/gpio.h)
#include "common/flash/no_rtos/extFlash/bsp.h"
#include "common/cc26xx/debug/led_debug.h"
#endif

#if defined(SECURITY)
#include "sign_util.h"
#include "sha2_driverlib.h"
#endif
/*******************************************************************************
 *                                          Constants
 */

#define BIM_ONCHIP_MAX_NUM_SEARCHES (3)
#if defined (SECURITY)
#define SHA_BUF_SZ                      EFL_PAGE_SIZE
#endif

#define SUCCESS                         0
#define FAIL                           -1

/*******************************************************************************
 * LOCAL VARIABLES
 */

#ifndef DEBUG
static uint32_t intFlashPageSize;       /* Size of internal flash page */
#endif

#if (defined(SECURITY))

#if defined(__IAR_SYSTEMS_ICC__)
__no_init uint8_t shaBuf[SHA_BUF_SZ];
#elif defined(__TI_COMPILER_VERSION__)
uint8_t shaBuf[SHA_BUF_SZ];
#endif

/* Cert element stored in flash where public keys in Little endian format*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION(_secureCertElement, ".cert_element")
#pragma RETAIN(_secureCertElement)
const certElement_t _secureCertElement =
#elif  defined(__IAR_SYSTEMS_ICC__)
#pragma location=".cert_element"
const certElement_t _secureCertElement @ ".cert_element" =
#endif
{
  .version    = SECURE_SIGN_TYPE,
  .len        = SECURE_CERT_LENGTH,
  .options    = SECURE_CERT_OPTIONS,
  .signerInfo = {0xb0,0x17,0x7d,0x51,0x1d,0xec,0x10,0x8b},
  .certPayload.eccKey.pubKeyX = {0xd8,0x51,0xbc,0xa2,0xed,0x3d,0x9e,0x19,0xb7,0x33,0xa5,0x2f,0x33,0xda,0x05,0x40,0x4d,0x13,0x76,0x50,0x3d,0x88,0xdf,0x5c,0xd0,0xe2,0xf2,0x58,0x30,0x53,0xc4,0x2a},
  .certPayload.eccKey.pubKeyY = {0xb9,0x2a,0xbe,0xef,0x66,0x5f,0xec,0xcf,0x56,0x16,0xcc,0x36,0xef,0x2d,0xc9,0x5e,0x46,0x2b,0x7c,0x3b,0x09,0xc1,0x99,0x56,0xd9,0xaf,0x95,0x81,0x63,0x23,0x7b,0xe7}
 };

uint32_t eccWorkzone[SECURE_FW_ECC_NIST_P256_WORKZONE_LEN_IN_BYTES + SECURE_FW_ECC_BUF_TOTAL_LEN(SECURE_FW_ECC_NIST_P256_KEY_LEN_IN_BYTES)*5] = {0};
uint8_t headerBuf[HDR_LEN_WITH_SECURITY_INFO];
#endif

/*******************************************************************************
 * EXTERN FUNCTIONS
 */


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void BIM_checkPgBoundary(uint8_t *flashPageNum, uint8_t *imgType, uint8_t *searchItrNum);
static void Bim_findImage(uint8_t *flashPageNum, uint8_t *imgType, uint8_t *searchItrNum);

#if defined(SECURITY)

#ifndef DEBUG

static bool    Bim_checkForSecSegmnt(uint32_t iflStartAddr, uint32_t imgLen);
static uint8_t Bim_verifyImage(uint32_t iflStartAddr);

#endif

// Function which is used to verify the authenticity of the OAD commands
int8_t Bim_payloadVerify(uint8_t ver, uint32_t cntr, uint32_t payloadlen,
                          uint8_t  *dataPayload, uint8_t *signPayload,
                          ecdsaSigVerifyBuf_t *ecdsaSigVerifyBuf);

// Creating a section for the function pointer, so that it can be easily accessed by OAD application(s)
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION(_fnPtr, ".fnPtr")
#pragma RETAIN(_fnPtr)
const uint32_t _fnPtr = (uint32_t)&Bim_payloadVerify;
#elif  defined(__IAR_SYSTEMS_ICC__)
#pragma location=".fnPtr"
const uint32_t _fnPtr @ ".fnPtr" = (uint32_t)&Bim_payloadVerify;
#endif // #ifdef __TI_COMPILER_VERSION__

#endif //#if defined (SECURITY)


#if defined(SECURITY)
#ifndef DEBUG
/*******************************************************************************
 * @fn         Bim_checkForSecSegmnt
*
*  @brief      Check for Security Segment. Reads through the headers in the .bin
*              file. If a security header is found the function checks to see if
*              the header has a populated segment.
*
*  @param       iFlStrAddr - The start address in internal flash of the binary image
*  @param       imgLen - Length of the image over which presence of security segment
*               is searched
*  @return      0  - valid security segment not found
*  @return      1  - valid security segment found
*
*/
static bool Bim_checkForSecSegmnt(uint32_t iFlStrAddr, uint32_t imgLen)
{
    bool securityFound = false;
    uint8_t endOfSegment = 0;
    uint8_t segmentType = DEFAULT_STATE;
    uint32_t segmentLength = 0;
    uint32_t searchAddr =  iFlStrAddr+OAD_IMG_HDR_LEN;

    while(!endOfSegment)
    {
        //extFlashRead(searchAddr, 1, &segmentType);
        readFlash(searchAddr, &segmentType, 1);

        if(segmentType == IMG_SECURITY_SEG_ID)
        {
            /* In this version of BIM, the security header will ALWAYS be present
               But the payload will sometimes not be there. If this finds the
               header, the payload is also checked for existence. */
            searchAddr += SIG_OFFSET;
            uint32_t sigVal = 0;
            //extFlashRead(searchAddr, sizeof(uint32_t), (uint8_t *)&sigVal);
            readFlash(searchAddr, (uint8_t *)&sigVal, sizeof(uint32_t));

            if(sigVal != 0) //Indicates the presence of a signature
            {
                endOfSegment = 1;
                securityFound = true;
            }
            else
            {
                break;
            }
        }
        else
        {
            searchAddr += SEG_LEN_OFFSET;
            if((searchAddr + sizeof(uint32_t)) > (iFlStrAddr + imgLen))
            {
                break;
            }
            //extFlashRead(searchAddr, sizeof(uint32_t), (uint8_t *)&segmentLength);
            readFlash(searchAddr, (uint8_t *)&segmentLength, sizeof(uint32_t));

            searchAddr += (segmentLength - SEG_LEN_OFFSET);
            if((searchAddr) > (iFlStrAddr + imgLen))
            {
                break;
            }
        }
    }

    return securityFound;
}//end of function

/*******************************************************************************
 * @fn      Bim_verifyImage
 *
 * @brief   Verifies the image stored on internal flash using ECDSA-SHA256
 *
 * @param   iflStartAddr - internal flash address of the image to be verified.
 *
 * @return  Zero when successful. Non-zero, otherwise..
 */
static uint8_t Bim_verifyImage(uint32_t iflStartAddr)
{
    uint8_t verifyStatus = (uint8_t)FAIL;

    /* clear the ECC work zone Buffer */
    uint32_t *eccPayloadWorkzone = eccWorkzone;
    memset(eccPayloadWorkzone, 0, sizeof(eccWorkzone));

    /* Read in the offchip header to get the image signature */
    readFlash(iflStartAddr, headerBuf, HDR_LEN_WITH_SECURITY_INFO);

    // First verify signerInfo
    verifyStatus = verifyCertElement(&headerBuf[SEG_SIGERINFO_OFFSET]);
    if(verifyStatus != SUCCESS)
    {
      return verifyStatus;
    }

    // Get the hash of the image
    uint8_t *finalHash;

    finalHash = computeSha2Hash(iflStartAddr, shaBuf, SHA_BUF_SZ, false);

    // Verify the hash
    // Create temp buffer used for ECDSA sign verify, it should 6*ECDSA_KEY_LEN
    uint8_t tempWorkzone[ECDSA_SHA_TEMPWORKZONE_LEN];
    memset(tempWorkzone, 0, ECDSA_SHA_TEMPWORKZONE_LEN);

    verifyStatus = bimVerifyImage_ecc(_secureCertElement.certPayload.eccKey.pubKeyX,
                                      _secureCertElement.certPayload.eccKey.pubKeyY,
                                       finalHash,
                                       &headerBuf[SEG_SIGNR_OFFSET],
                                       &headerBuf[SEG_SIGNS_OFFSET],
                                       eccWorkzone,
                                       tempWorkzone);

    if(verifyStatus == SECURE_FW_ECC_STATUS_VALID_SIGNATURE)
    {
       verifyStatus = SUCCESS;
    }
    return verifyStatus;

}//end of function

#endif // DEBUG

/*******************************************************************************
 * @fn      Bim_payloadVerify
 *
 * @brief   Function in BIM to verify the payload of an OTA command.
 *
 * @param   ver - version of the security algorithm
 *          cntr - time-stamp /counter value use to verify the payload
 *          payloadlen - payload length in bytes
 *          dataPayload - pointer to data payload to be verified
 *          signPayload - pointer to sign payload
 *          eccPayloadWorkzone - pointer to the workzone used to verify the command
 *          shaPayloadWorkzone - pointer to the workzone used to generate a hash of the command
 *
 *
 * @return  Zero when successful. Non-zero, otherwise..
 */
int8_t Bim_payloadVerify(uint8_t ver, uint32_t cntr, uint32_t payloadlen,
                         uint8_t  *dataPayload, uint8_t *signPayload,
                         ecdsaSigVerifyBuf_t *ecdsaSigVerifyBuf)
{
    signPld_ECDSA_P256_t *signPld = (signPld_ECDSA_P256_t*)signPayload;

    uint8_t *sig1 = signPld->signature;
    uint8_t *sig2 = &signPld->signature[32];
    int8_t status = FAIL;
    int8_t verifyStatus = FAIL;
    uint8_t *finalHash = ecdsaSigVerifyBuf->tempWorkzone;

    memset(ecdsaSigVerifyBuf->tempWorkzone, 0, sizeof(ECDSA_SHA_TEMPWORKZONE_LEN));

    if (ver == 1)
    {
        SHA2_open();
        SHA2_addData(dataPayload, payloadlen);
        SHA2_finalize(finalHash);
        SHA2_close();

        // First verify signerInfo
        verifyStatus = verifyCertElement(signPld->signerInfo);
        if(verifyStatus != SUCCESS)
        {
          return FAIL;
        }

        verifyStatus = bimVerifyImage_ecc(_secureCertElement.certPayload.eccKey.pubKeyX,
                                          _secureCertElement.certPayload.eccKey.pubKeyY,
                                           finalHash, sig1, sig2,
                                           ecdsaSigVerifyBuf->eccWorkzone,
                                           (ecdsaSigVerifyBuf->tempWorkzone + ECDSA_KEY_LEN));
        if(verifyStatus == (int8_t)SECURE_FW_ECC_STATUS_VALID_SIGNATURE)
        {
            status = SUCCESS;
        }
    }
    return status;
}//end of function

#endif /* if defined(SECURITY) */


/*******************************************************************************
 * @fn     BIM_checkPgBoundary
 *
 * @brief  Checks if the last page of the internal flash page is reached if not
 *         increments the page and returns the page number. If it reached at the
 *         last flash page then reset the flash page number and switch the image
 *         type if needed and increments the search iteration number.
 *
 * @param  flashPageNum - flash page number
 *         imgType      - image type to be searched
 *         searchItrNum - search iteration number
 *
 * @return None.
 */
static void BIM_checkPgBoundary(uint8_t *flashPageNum, uint8_t *imgType, uint8_t *searchItrNum)
{
    *flashPageNum += 1;

    if (*flashPageNum >= MAX_ONCHIP_FLASH_PAGES) /* End of flash reached */
    {
        /* If the search was for user application, and unable to find that switch to
           persistent image or vice versa
         */
        *flashPageNum = 0;
        if (*imgType == OAD_IMG_TYPE_APPSTACKLIB)
        {
            *imgType = OAD_IMG_TYPE_PERSISTENT_APP;
        }
        else
        {
            *imgType = OAD_IMG_TYPE_APPSTACKLIB;
        }

        *searchItrNum += 1;
    }
}

/*******************************************************************************
 * @fn     Bim_findImage
 *
 * @brief  Finds the image type specified in FN and IT parameters, starting with
 *         the page number flashPageNum and executes the image it finds the valid
 *         image. If not, sets the imageType, flashPageNum, searchItrNum to restart
 *         the search for alternative image.
 *
 * @param  FN - Flash page number as per RAM variable
 *         IT - Image Type as per RAM variable
 *         flashPageNum - flash page number, serves both i/p a o/p parameter
 *         imgType      - image type to be searched
 *         searchItrNum - search iteration number
 *
 * @return None.
 */
static void Bim_findImage(uint8_t *flashPageNum, uint8_t *imgType, uint8_t *searchItrNum)
{
    imgHdr_t imgHdr;
    uint8_t securityStatus = VERIFY_FAIL;

    /* Read flash to find OAD image identification value */
    readFlashPg(*flashPageNum, 0, &imgHdr.fixedHdr.imgID[0], OAD_IMG_ID_LEN);

    /* Check imageID bytes */
    if (imgIDCheck(&(imgHdr.fixedHdr)) != true)
    {
        /* go to next flash page */
        BIM_checkPgBoundary(flashPageNum, imgType, searchItrNum);

        /* and return so that same process can be repeated*/
        return;
    }
    else //valid OAD image ID value is found
    {
        /* Read whole of fixed header in the image header */
        readFlashPg(*flashPageNum, 0, (uint8_t *)&imgHdr, OAD_IMG_HDR_LEN);

        if(*imgType != imgHdr.fixedHdr.imgType) /* dint find the image type we are looking for */
        {
            /* increment flash page number and proceed; change image type if end of flash reached */
            BIM_checkPgBoundary(flashPageNum, imgType, searchItrNum);

            /* and return so that same process can be repeated*/
            return;
        }

        /* Image type matched: proceed with further checks */

        /* check BIM and Metadata version and CRC status (if need be). If DEBUG is enabled
         * skip the crc checking and updating the crc status- as crc wouldn't have been
         * calculated at the first place */

        if( (imgHdr.fixedHdr.bimVer != BIM_VER  || imgHdr.fixedHdr.metaVer != META_VER) /* Invalid metadata version */
#ifndef DEBUG
              ||
           (imgHdr.fixedHdr.crcStat == 0xFC)  /* Invalid CRC */
#endif
          )
        {
            /* We did not find a usable image: continue searching  */
            /* Continue searching with same image type */
            BIM_checkPgBoundary(flashPageNum, imgType, searchItrNum);

            /* and return so that same process can be repeated*/
            return;
        }
        else if(imgHdr.fixedHdr.imgType == OAD_IMG_TYPE_APPSTACKLIB && imgHdr.fixedHdr.crcStat == 0xFF) /* CRC not calculated yet */
        {

#ifndef DEBUG

            /* Calculate the CRC over the data buffer and update status */
            uint32_t crc32 = 0;
            uint8_t  crcstat = CRC_VALID;
            crc32 = CRC32_calc(*flashPageNum, intFlashPageSize, 0, imgHdr.fixedHdr.len, false);

            /* Check if calculated CRC matched with the image header */
            if (crc32 != imgHdr.fixedHdr.crc32)
            {
                /* Update CRC status */
                crcstat = CRC_INVALID;
                writeFlashPg(*flashPageNum, CRC_STAT_OFFSET, (uint8_t *)&crcstat, 1);

                /* CRC failed: invalid app image: search for persistent image */
                *flashPageNum = 0;
                *imgType = OAD_IMG_TYPE_PERSISTENT_APP;
                 *searchItrNum = *searchItrNum + 1;
                return;

            } /* if (crc32 != imgHdr.crc32) */

            /* if we have come here: CRC check has passed */
            /* Update CRC status */
            writeFlashPg(*flashPageNum, CRC_STAT_OFFSET, (uint8_t *)&crcstat, 1);
#endif
        } /* else if(imgHdr.crcStat == 0xFF) */


#if defined(SECURITY)

#ifndef DEBUG //during debug: the sign is not populated to even verify

    if(imgHdr.fixedHdr.imgType == OAD_IMG_TYPE_APPSTACKLIB)
    {
        uint32_t iFlStrAddr;
        uint8_t securityPresence = false;
        int8_t signVrfyStatus = FAIL;

        /* populate the start address of the image in the internal flash */
        iFlStrAddr = FLASH_ADDRESS(*flashPageNum, 0);

        /* check if security segment is present or not in the image */
        securityPresence = Bim_checkForSecSegmnt(iFlStrAddr, imgHdr.fixedHdr.len); //mvtodo: will len be always available?

        if(securityPresence)
        {
            /* Calculate the SHA256 of the image */
            uint8_t readSecurityByte[SEC_VERIF_STAT_OFFSET + 1];

            /* Read in the header to check if the signature has already been denied */
            readFlashPg(*flashPageNum, 0, &readSecurityByte[0], (SEC_VERIF_STAT_OFFSET + 1));

            if(readSecurityByte[SEC_VERIF_STAT_OFFSET] != VERIFY_FAIL)
            {
                signVrfyStatus = Bim_verifyImage(iFlStrAddr);

                /* If the signature is invalid, update the sign verification status */
                if((uint8_t)signVrfyStatus != SUCCESS)
                {
                    readSecurityByte[SEC_VERIF_STAT_OFFSET] = VERIFY_FAIL;
                    writeFlashPg(*flashPageNum, SEC_VERIF_STAT_OFFSET,  &readSecurityByte[SEC_VERIF_STAT_OFFSET], 1);
                }
            }

        } /* if(securityPresence) */

        /* sign verification has failed or dint find the security segment at the first place */
        if((uint8_t)signVrfyStatus != SUCCESS)
        {
            *flashPageNum = 0;
            *imgType = OAD_IMG_TYPE_PERSISTENT_APP;
            *searchItrNum = *searchItrNum + 1;
            return;
        }
        else
        {
            securityStatus = VERIFY_PASS;
        }
    }//end of if image type == OAD_IMG_TYPE_APPSTACKLIB
    else //type persistent app
    {
        securityStatus = VERIFY_PASS;
    }

#else //DEBUG is defined
    securityStatus = VERIFY_PASS;
#endif

#else //SECURITY not defined
    securityStatus = VERIFY_PASS;
#endif

        /*if we get here, its highly likely we found a valid image to boot to */

        if (((imgHdr.fixedHdr.imgType == OAD_IMG_TYPE_PERSISTENT_APP) ||
            (imgHdr.fixedHdr.imgType == OAD_IMG_TYPE_APPSTACKLIB)) && \
            (VERIFY_PASS == securityStatus))
        {
            jumpToPrgEntry(imgHdr.fixedHdr.prgEntry);
        }
        else
        {
           //if we go here something is wrong as imageType check was performed before we get here
            *searchItrNum = BIM_ONCHIP_MAX_NUM_SEARCHES;
            return;
        }
    }//valid imageID found

    return;
}

/*******************************************************************************
 * @fn          main
 *
 * @brief       C-code main function.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
int main(void)
{
#ifdef __IAR_SYSTEMS_ICC__
  __set_CONTROL(0);
#endif

#ifndef DEBUG
    /* Read and populate the static variable intFlashPageSize */
    intFlashPageSize = FlashSectorSizeGet();
#endif

    /* Declare local variables used in the main function */
    uint8_t searchItrNum = 0;
    uint8_t flashPgNum = 0;
    uint8_t imgType = 0;

    /* Set Image type and Internal flash page number */
    setSearchValidImg(flashPgNum, &imgType);
   
    /* Search for the appropriate image: update image type if need be
    * Jump to the relevant image i.e. either u-app or p-app 
    */
    while (searchItrNum < BIM_ONCHIP_MAX_NUM_SEARCHES)
    {
        Bim_findImage(&flashPgNum, &imgType, &searchItrNum);
    }
    
    /* If we get here, that means there is an Issue: No valid image found */
    
#ifdef DEBUG

    powerUpGpio();
    while(1)
    {
        lightRedLed();
    }

#else /* ifdef DEBUG */
    /* Set the device to the lowest power state. Does not return. */
    setLowPowerMode();

    return(0);
#endif
        
} /* end of main function */

/**************************************************************************************************
*/
