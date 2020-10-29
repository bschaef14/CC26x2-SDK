/******************************************************************************

 @file oad_storage.c

 @brief OAD Storage

 Group: CMCU LPRF
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2016-2020, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)

#include <common/cc26xx/oad/oad_image_header.h>
#include <oad/native_oad/oad_image_header_app.h>
#include <common/cc26xx/crc/crc32.h>

#include <oad/native_oad/oad_storage.h>
#include <ti_easylink_oad_config.h>
#include <oad/native_oad/oad_protocol.h>
#include <common/cc26xx/oad/ext_flash_layout.h>
#include <common/cc26xx/flash_interface/flash_interface.h>

/*********************************************************************
 * CONSTANTS
 */
#define HAL_FLASH_WORD_SIZE  4

#define OAD_PROFILE_VERSION     0x01

#define OAD_IMG_PG_INVALID                  0xFF

/*********************************************************************
 * MACROS
 */
#define OADStorage_BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32_t) (((Byte0) & 0xFF) + (((Byte1) & 0xFF) << 8) + (((Byte2) & 0xFF) << 16) + (((Byte3) & 0xFF) << 24) ))

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Page that metadata is stored on
uint16_t metaPage = 0;

static imgHdr_t candidateImageHeader;

static uint32_t oadBlkTot = 0xFFFFFFFF;
static uint16_t oadImgBytesPerBlock = OADStorage_BLOCK_SIZE - OADStorage_BLK_NUM_HDR_SZ;
static uint8_t numBlksInImgHdr = 0;

/* Information about image that is currently being downloaded */
static uint32_t imageAddress = 0;
static uint16_t imagePage = 0;
static uint32_t candidateImageLength = 0xFFFFFFFF;
static uint32_t candidateImageType = 0xFFFFFFFF;

static bool useExternalFlash = false;

static uint32_t flashPageSize;

#ifndef FEATURE_OAD_SERVER_ONLY
    static uint32_t flashNumPages;
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */
#ifndef FEATURE_OAD_SERVER_ONLY
static uint8_t oadFindCurrentImageHdr(void);
#endif
static uint32_t oadFindExtFlImgAddr(uint8_t imgType);
static uint8_t oadFindExtFlMetaPage(void);
static OADStorage_Status_t oadValidateCandidateHdr(imgHdr_t *receivedHeader);
static uint8_t oadCheckDL(void);
static uint8_t oadCheckImageID(OADStorage_imgIdentifyPld_t *idPld);

/*********************************************************************
 * @fn      OADStorage_open
 *
 * @brief   open storage
 *
 * @param   None.
 *
 * @return  None.
 */
void OADStorage_init(void)
{
    // Set flash size variables
    flashPageSize = FlashSectorSizeGet();

#ifndef FEATURE_OAD_SERVER_ONLY
    flashNumPages =  FlashSizeGet() / flashPageSize;
#endif
    // Open the flash interface
    flash_init();
    flash_open();

    // This variable controls whether the OAD module uses internal or external
    // flash memory
    useExternalFlash = hasExternalFlash();
}

/*********************************************************************
 * @fn      OADStorage_imgIdentifyRead
 *
* @brief   Read Image header and return number of blocks.
 *
 * @param   imageType   - image type indicating which image to read
 * @param   pImgId     - pointer to image header data
 *
 * @return  Total Blocks if image accepted, 0 if Image invalid
 */
uint16_t OADStorage_imgIdentifyRead(uint8_t imageType, OADStorage_imgIdentifyPld_t* pImgId)
{
    uint16_t oadBlkTot;
    uint8_t imgID[] = OAD_IMG_ID_VAL;
    uint8_t imgvmID[] = OAD_EXTFL_ID_VAL;
    imgHdr_t imgHdr;

    if(pImgId == NULL)
    {
        return 0;
    }

    imageAddress = oadFindExtFlImgAddr(imageType);

    // Determine where image will be read from.
    imagePage = EXT_FLASH_PAGE(imageAddress);

    readFlashPg(imagePage, 0,
                    (uint8_t * ) &imgHdr,
                    sizeof(imgHdr_t));

    //copy image identify payload
    memcpy(pImgId->imgID, imgHdr.fixedHdr.imgID, 8);
    pImgId->bimVer = imgHdr.fixedHdr.bimVer;
    pImgId->metaVer = imgHdr.fixedHdr.metaVer;
    pImgId->imgCpStat = imgHdr.fixedHdr.imgCpStat;
    pImgId->crcStat = imgHdr.fixedHdr.crcStat;
    pImgId->imgType = imgHdr.fixedHdr.imgType;
    pImgId->imgNo = imgHdr.fixedHdr.imgNo;
    pImgId->len = imgHdr.fixedHdr.len;
    memcpy(pImgId->softVer, imgHdr.fixedHdr.softVer, 4);

    // Validate the Image header preamble
    if( (memcmp(&imgID, pImgId->imgID, OAD_IMG_ID_LEN) != 0) ||
        (memcmp(&imgvmID, pImgId->imgID, OAD_IMG_ID_LEN) != 0)  )
   {

        // Calculate block total of the new image.
        oadBlkTot = pImgId->len /
                    (OADStorage_BLOCK_SIZE - OADStorage_BLK_NUM_HDR_SZ);

        // If there is a remainder after division, round up
        if( 0 != (pImgId->len % (OADStorage_BLOCK_SIZE - OADStorage_BLK_NUM_HDR_SZ)))
        {
            oadBlkTot += 1;
        }

        return (int32_t) oadBlkTot;
    }
    else
    {
        return 0;
    }
}

/*********************************************************************
 * @fn      OADStorage_imgIdentifyWrite
 *
 * @brief   Process the Image Identify Write.  Determine from the received OAD
 *          Image Header if the Downloaded Image should be acquired.
 *
 * @param   pBlockData     - pointer to image header data
 *
 * @return  Total Blocks if image accepted, 0 if Image rejected
 */
uint16_t OADStorage_imgIdentifyWrite(uint8_t *pBlockData)
{
    uint8_t idStatus;

    // Find the number of blocks in the image header, round up if necessary
    numBlksInImgHdr = sizeof(imgHdr_t) / (oadImgBytesPerBlock)  +  \
                        (sizeof(imgHdr_t) % (oadImgBytesPerBlock) != 0);

    // Cast the pBlockData byte array to OADStorage_imgIdentifyPld_t
    OADStorage_imgIdentifyPld_t *idPld = (OADStorage_imgIdentifyPld_t *)(pBlockData);

    // Validate the ID
    idStatus = oadCheckImageID(idPld);

    // If image ID is accepted, set variables and pre-erase flash pages
    if(idStatus == OADStorage_Status_Success)
    {
        if(!useExternalFlash)
        {
            imageAddress = 0;
            imagePage = 0;
            metaPage = 0;
        }
        else
        {
            imageAddress = oadFindExtFlImgAddr(idPld->imgType);
            imagePage = EXT_FLASH_PAGE(imageAddress);
            metaPage = oadFindExtFlMetaPage();
        }

        // Calculate total number of OAD blocks, round up if needed
        oadBlkTot = candidateImageLength / (oadImgBytesPerBlock);

        // If there is a remainder after division, round up
        if( 0 != (candidateImageLength % (oadImgBytesPerBlock)))
        {
            oadBlkTot += 1;
        }
    }
    else
    {
        oadBlkTot = 0;
    }

    return oadBlkTot;
}

/*********************************************************************
 * @fn      OADStorage_imgBlockWrite
 *
 * @brief   Write an Image Block.
 *
 * @param   blockNum   - block number to be written
 * @param   pBlockData - pointer to data to be written
 * @param   len        - length of data to be written
 *
 * @return  status
 */
uint8_t OADStorage_imgBlockWrite(uint32_t blkNum, uint8_t *pBlockData, uint8_t len)
{
    OADStorage_Status_t status = OADStorage_Status_Success;
    uint8_t flashStat;
    uint8_t page;

    uint8_t offset = (oadImgBytesPerBlock)*blkNum;

    // Remaining bytes that are included in this packet that are not
    // part of the image header
    uint8_t nonHeaderBytes = 0;

    // Number of bytes in this packet that need to be copied to header
    uint8_t remainder = 0;

    // The destination in RAM to copy the payload info
    uint8_t *destAddr = (uint8_t *)(&candidateImageHeader) + offset;

    // Don't start store to flash until entire header is received
    if((blkNum < (numBlksInImgHdr - 1)) && (sizeof(imgHdr_t) >= oadImgBytesPerBlock))
    {
        memcpy(destAddr, pBlockData + OADStorage_BLK_NUM_HDR_SZ,
                (oadImgBytesPerBlock));
    }
    else if (blkNum == (numBlksInImgHdr - 1))
    {
        if((oadImgBytesPerBlock)  >= sizeof(imgHdr_t))
        {
            // if we can fit the entire header in a single block
            memcpy(destAddr,
                    pBlockData+OADStorage_BLK_NUM_HDR_SZ,
                    sizeof(imgHdr_t));

            nonHeaderBytes = (oadImgBytesPerBlock) - sizeof(imgHdr_t);
            remainder = sizeof(imgHdr_t);
        }
        else
        {
            // Find out how much of the block contains header data
            remainder = sizeof(imgHdr_t) % (oadImgBytesPerBlock);
            if(remainder == 0)
            {
                remainder = oadImgBytesPerBlock;
            }

            nonHeaderBytes = (oadImgBytesPerBlock) - remainder;
            // if this block contains the last part of the header
            memcpy(destAddr,
                    pBlockData+OADStorage_BLK_NUM_HDR_SZ,
                    remainder);
        }

        status = oadValidateCandidateHdr((imgHdr_t * )&candidateImageHeader);
        if(status == OADStorage_Status_Success)
        {
            // Calculate number of flash pages to pre-erase
            uint8_t numFlashPages;
            if(!useExternalFlash)
            {
                numFlashPages = candidateImageHeader.fixedHdr.len / flashPageSize;
                if(0 != (candidateImageHeader.fixedHdr.len % flashPageSize))
                {
                    numFlashPages += 1;
                }
            }
            else
            {
                numFlashPages = candidateImageHeader.fixedHdr.len / EFL_PAGE_SIZE;
                if(0 != (candidateImageHeader.fixedHdr.len % EFL_PAGE_SIZE))
                {
                    numFlashPages += 1;
                }
            }

            // Pre-erase the correct amount of pages before starting OAD
            for(page = imagePage; page < (imagePage + numFlashPages); ++page)
            {
                flashStat = eraseFlashPg(page);
                if(flashStat == FLASH_FAILURE)
                {
                    // If we fail to pre-erase, then halt the OAD process
                    status = OADStorage_FlashError;
                    break;
                }

            }

            // Write a OAD_BLOCK to Flash.
            flashStat = writeFlashPg(imagePage, 0,
                                    (uint8_t * ) &candidateImageHeader,
                                    sizeof(imgHdr_t));

            // Cancel OAD due to flash program error
            if(FLASH_SUCCESS != flashStat)
            {
                return (OADStorage_FlashError);
            }

            // If there are non header (image data) bytes in this packet
            // write them to flash as well
            if(nonHeaderBytes)
            {
                // Write a OAD_BLOCK to Flash.
                flashStat = writeFlashPg(imagePage,
                                        sizeof(imgHdr_t),
                                        (pBlockData+OADStorage_BLK_NUM_HDR_SZ+remainder),
                                        nonHeaderBytes);

                // Cancel OAD due to flash program error
                if(FLASH_SUCCESS != flashStat)
                {
                    return (OADStorage_FlashError);
                }
            }
        }
        else
        {
            // Cancel OAD due to boundary error
            return (status);
        }
    }
    else
    {
        // Calculate address to write as (start of OAD range) + (offset into range)
        uint32_t blkStartAddr = (oadImgBytesPerBlock)*blkNum + imageAddress;

        if (!useExternalFlash)
        {
            flashStat = writeFlash(blkStartAddr,
                                   pBlockData+OADStorage_BLK_NUM_HDR_SZ,
                                   (len - OADStorage_BLK_NUM_HDR_SZ));
        }
        else
        {
            uint8_t page = (blkStartAddr >> 12);
            uint32_t offset = (blkStartAddr & 0x00000FFF);

            // Write a OAD_BLOCK to Flash.
            flashStat = writeFlashPg(page, offset,
                                     pBlockData+OADStorage_BLK_NUM_HDR_SZ,
                                     (len - OADStorage_BLK_NUM_HDR_SZ));
        }

        // Cancel OAD due to flash program error
        if(FLASH_SUCCESS != flashStat)
        {
            return (OADStorage_FlashError);
        }
    }

    // Return and request the next block
    return (status);
}

/*********************************************************************
 * @fn      OADStorage_eraseImgPage
 *
 * @brief   Erases an Image page. Note this is only needed if an image
 *          page has been corrupted typically OADStorage_imgBlockWrite
 *          pre-erase all pages
 *
 * @param  none
 *
 * @return  OADStorage_Status_t
 */
OADStorage_Status_t OADStorage_eraseImgPage(uint32_t page)
{
    uint8_t flashStat;
    OADStorage_Status_t status = OADStorage_FlashError;

    page += imagePage;

    flashStat = eraseFlashPg(page);
    if(flashStat == FLASH_SUCCESS)
    {
        status = OADStorage_Status_Success;
    }

    return status;
}

/*********************************************************************
 * @fn      OADStorage_imgFinalise
 *
 * @brief   Process the Image Block Write.
 *
 * @param  none
 *
 * @return  OADStorage_Status_t
 */
OADStorage_Status_t OADStorage_imgFinalise(void)
{
    // Run CRC check on new image.
    if (OADStorage_Status_Success != oadCheckDL())
    {
        // CRC error
        return (OADStorage_CrcError);
    }

    //Write meta data
    if(useExternalFlash)
    {
        // Copy the metadata to the meta page
        imgFixedHdr_t storedImgHdr;
        readFlashPg(imagePage, 0, (uint8_t *)&storedImgHdr,
                    OAD_IMG_HDR_LEN);

        // Populate ext imge info struct
        ExtImageInfo_t extFlMetaHdr;

        // ExtImgInfo and imgHdr are identical for the first
        // EFL_META_COPY_SZ bytes
        memcpy((uint8_t *)&extFlMetaHdr, (uint8_t *)&storedImgHdr,
                EFL_META_COPY_SZ);

        uint8_t imgIDExtFl[] = OAD_EXTFL_ID_VAL;
        memcpy((uint8_t *)&extFlMetaHdr, imgIDExtFl, OAD_IMG_ID_LEN);

        extFlMetaHdr.extFlAddr = imageAddress;
        extFlMetaHdr.counter =  0x00000000;

        if(candidateImageType <= OAD_IMG_TYPE_APPSTACKLIB &&
                candidateImageType > OAD_IMG_TYPE_PERSISTENT_APP)
        {
            extFlMetaHdr.fixedHdr.imgCpStat = NEED_COPY;
            extFlMetaHdr.fixedHdr.crcStat = CRC_VALID;
        }
        //Erase the old meta data
        eraseFlashPg(metaPage);

        // Store the metadata
        uint8_t flashStatus = OADStorage_Status_Success;
        flashStatus =  writeFlashPg(metaPage, 0,
                                        (uint8_t *)&extFlMetaHdr,
                                        sizeof(ExtImageInfo_t));

        if(flashStatus != FLASH_SUCCESS)
        {
            return (OADStorage_FlashError);
        }
    }

    // Indicate a successful download and CRC
    return (OADStorage_Status_Success);
}

/*********************************************************************
 * @fn      oadFindCurrentImageHdr
 *
 * @brief   Search internal flash for the currently running image header
 *
 * @return  headerPg - Page of the current image's header
 */
#ifndef FEATURE_OAD_SERVER_ONLY
static uint8_t oadFindCurrentImageHdr(void)
{
    uint8_t headerPage = OAD_IMG_PG_INVALID;
    uint8_t page;
#if defined(USE_ICALL) && !defined(ICALL_LITE)
    // When in the persist app, we need to search for the header
    // Start at page 0 and search up to the stack boundary
    uint8_t stackStartPg =  FLASH_PAGE((uint32_t)(stackImageHeader));


    for(page = 0; page < stackStartPg; ++page)
#endif
    for(page = 0; page < flashNumPages; ++page)
    {
        uint8_t imgID[] = OAD_IMG_ID_VAL;
        uint8_t imgvmID[] = OAD_EXTFL_ID_VAL;
        uint8_t imgIdReceived[OAD_IMG_ID_LEN];
        uint32_t *pgAddr = (uint32_t *)FLASH_ADDRESS(page, 0);
        memcpy(imgIdReceived, pgAddr, OAD_IMG_ID_LEN);

        // is this a vilid image header
        if( (memcmp(&imgID, &imgIdReceived, OAD_IMG_ID_LEN) == 0) ||
            (memcmp(&imgvmID, &imgIdReceived, OAD_IMG_ID_LEN) == 0)  )
        {
            uint8_t imgType;
            pgAddr = (uint32_t *)FLASH_ADDRESS(page, IMG_TYPE_OFFSET);
            memcpy(&imgType, pgAddr, sizeof(imgType));
            if(imgType <= OAD_IMG_TYPE_APPSTACKLIB)
            {
                // If the ids match, we found the header, break loop
                headerPage = page;
                break;
            }
        }
    }

    return (headerPage);
}
#endif

/*********************************************************************
 * @fn      oadFindExtFlImgAddr
 *
 * @brief   Find a location in external flash for the current image's
 *          metadata
 *
 * @param   metaPage - meta page in external flash
 *
 * @return  None.
 */
 static uint8_t oadFindExtFlMetaPage(void)
 {
    uint8_t curPg;
    // Variable to store the least recently used metadata page
    uint8_t lruMetaPg = 0xFF;
    uint8_t metaPage = 0xFF;

    // Store lowest counter variable
    uint32_t lowestCounter = 0xFFFFFFFF;

    for(curPg = EFL_NUM_FACT_IMAGES; curPg < EFL_MAX_META; ++curPg)
    {
        // Buffer to hold imgID data
        uint8_t readID[8];

        uint8_t hdrID[] = OAD_EFL_MAGIC;

        // Read imageID into buffer
        readFlashPg(curPg, 0, readID, OAD_IMG_ID_LEN);

        if(0 != memcmp(readID, hdrID, OAD_IMG_ID_LEN))
        {
            // We have found an empty pg
            metaPage = curPg;

            // Erase the meta page.
            // The image pages will be erased later
            eraseFlashPg(metaPage);
            return (metaPage);
        }
        else
        {
            uint32_t counter;
            ExtImageInfo_t extFlMetaHdr;
            // Read in the meta header
            readFlashPg(curPg, 0, (uint8_t *)&extFlMetaHdr,
                            sizeof(ExtImageInfo_t));

            counter = extFlMetaHdr.counter;

            // If this counter is the least recently used
            if(counter < lowestCounter)
            {
                // Setup LRU values
                lowestCounter = counter;
                lruMetaPg = curPg;
            }
        }

    }

    // If we didn't find an empty pg, use the LRU
    if(lruMetaPg != 0xFF)
    {
        metaPage = lruMetaPg;
    }

    // Erase the meta page.
    // The image pages will be erased later
    eraseFlashPg(metaPage);

    return (metaPage);
 }

static uint32_t oadFindExtFlImgAddr(uint8_t imgType)
{
    uint32_t imgAddr = 0;
    if(imgType == OAD_IMG_TYPE_APP)
    {
        imgAddr = EFL_IMG_SPACE_START;
    }
    else
    {
        imgAddr = EFL_IMG_SPACE_START + EFL_APP_IMG_SZ;
    }
    return (imgAddr);
}

/*********************************************************************
 * @fn      oadValidateCandidateHdr
 *
 * @brief   Validate the header of the incoming image
 *
 * @param   receivedHeader - pointer to the candidate image's header
 *
 * @return  Status - OADStorage_Status_Success if segment valid or not a segment
                     OADStorage_Rejected - if the boundary check fails
 */
static OADStorage_Status_t oadValidateCandidateHdr(imgHdr_t *receivedHeader)
{
    OADStorage_Status_t status = OADStorage_Status_Success;

#ifndef FEATURE_OAD_SERVER_ONLY
    // Read the active app's image header
    uint8_t  appImageHdrPage = oadFindCurrentImageHdr();
    uint32_t appImageHeaderAddr = FLASH_ADDRESS(appImageHdrPage, 0);
    imgHdr_t *currentImageHeader = (imgHdr_t *)(appImageHeaderAddr);

    // If on-chip and the user app is erased, use persist app
    if(!useExternalFlash && (appImageHdrPage == OAD_IMG_PG_INVALID))
    {
        currentImageHeader = (imgHdr_t *)&_imgHdr;
    }
    else if(appImageHdrPage == OAD_IMG_PG_INVALID)
    {
        return OADStorage_Rejected;
    }

    /*
     * For on-chip OAD the tech type must match the currently running one
     * for off-chip OAD the tech type must match with the exception of
     * merged images which can be of a different tech type
     */
    if(!(useExternalFlash && (receivedHeader->fixedHdr.imgType == OAD_IMG_TYPE_APP_STACK)))
    {
        if( currentImageHeader->fixedHdr.techType !=  receivedHeader->fixedHdr.techType)
        {
            status = OADStorage_Rejected;
        }
    }
#endif //FEATURE_OAD_SERVER_ONLY

    // Check that the incoming image is page aligned
    if((receivedHeader->imgPayload.startAddr & (flashPageSize - 1)) != 0)
    {
        status = OADStorage_Rejected;
    }

#if defined(USE_ICALL) && !defined(ICALL_LITE)
    // Check that the incoming app image not overlapping stack image
    if(receivedHeader->imgType == OAD_IMG_TYPE_APP)
    {
        uint32_t appEndAddr = receivedHeader->startAddr + receivedHeader->len;

        // Ensure that the candidate image would not overwrite the stack
        if(appEndAddr > stackImageHeader->startAddr)
        {
            status = OADStorage_Rejected;
        }

        // Ensure that the stack RAM boundary is not violated
        if(receivedHeader->ram0EndAddr >= stackImageHeader->ram0StartAddr)
        {
            status = OADStorage_Rejected;
        }

        // Be sure that the the app image's RAM doesn't dip into RAM rsvd for ROM
        if(receivedHeader->ram0StartAddr < currentImageHeader->ram0StartAddr)
        {
            status = OADStorage_Rejected;
        }
    }
    else if (receivedHeader->imgType == OAD_IMG_TYPE_STACK_ONLY)
    {
        uint32_t nvSize = flashPageSize * OAD_NUM_NV_PGS;
        /*
         * In on-chip OAD, stack only OAD requires the app to be udpated as well
         * so the currently app will be wiped out, but the new stack cannot
         * grow into the existing stack (pre-copy in BIM)
         *
         * In off-chip OAD the stack can grow so long as it doesn't overwrite
         * the last address of the current application
         */
        if(!useExternalFlash)
        {
            if(receivedHeader->len > stackImageHeader->startAddr)
            {
                status = OADStorage_Rejected;
            }

            // Check that the stack will not overwrite the persistent app
            if(receivedHeader->imgEndAddr + nvSize > _imgHdr.startAddr)
            {
                status = OADStorage_Rejected;
            }
        }
        else
        {
#ifdef FEATURE_OAD_SERVER_ONLY
            if(currentImageHeader->imgType != OAD_IMG_TYPE_APP_STACK)
            {
                /*
                 * If the current image is not of merged type then its
                 * end addr is the end of the app. The stack cannot overwrite
                 * the app
                 */
                if(receivedHeader->startAddr <= currentImageHeader->imgEndAddr)
                {
                    status = OADStorage_Rejected;
                }

                // Check that the stack will not overwrite the BIM
                if(receivedHeader->imgEndAddr + nvSize > BIM_START)
                {
                    status = OADStorage_Rejected;
                }
            }
            else
            {
                /*
                 * If the current image is of merged type then its not possible
                 * to know exactly where the app ends, just make sure
                 * ICALL_STACK0 isn't violated.
                 */
                if(receivedHeader->startAddr < stackImageHeader->stackStartAddr)
                {
                    status = OADStorage_Rejected;
                }
            }
#endif //FEATURE_OAD_SERVER_ONLY
        }

        /*
         *  In on-chip use cases this will check that the persistent app's
         *  RAM boundary is not violated
         *  in offchip use cases this will check the user app's RAM boundary
         *  is not violated
         */
        if(receivedHeader->ram0StartAddr <= _imgHdr.ram0EndAddr)
        {
            status = OADStorage_Rejected;
        }
    }
#endif /* defined(USE_ICALL) && !defined(ICALL_LITE) */

    // Merged image types and images not targed to run on CC26xx are not checked
    return (status);
}

/*********************************************************************
 * @fn      oadCheckImageID
 *
 * @brief   Check image identify header (determines OAD start)
 *
 * @param   idPld - pointer to imageID payload
 *
 * @return  headerValid - SUCCESS or fail code
 */
static uint8_t oadCheckImageID(OADStorage_imgIdentifyPld_t *idPld)
{
    uint8_t status = OADStorage_Status_Success;
    uint8_t imgID[] = OAD_IMG_ID_VAL;
    uint8_t imgvmID[] = OAD_EXTFL_ID_VAL;

#ifndef FEATURE_OAD_SERVER_ONLY
    // Read the active app's image header
    uint8_t  appImageHdrPage = oadFindCurrentImageHdr();
    uint32_t appImageHeaderAddr = FLASH_ADDRESS(appImageHdrPage, 0);
    imgHdr_t *currentImageHeader = (imgHdr_t *)(appImageHeaderAddr);

    if(appImageHdrPage == OAD_IMG_PG_INVALID)
    {
        currentImageHeader = (imgHdr_t *)&_imgHdr;
    }

    // If on-chip and the user app is erased, use persist app
    if(!useExternalFlash && (appImageHdrPage == OAD_IMG_PG_INVALID))
    {
        currentImageHeader = (imgHdr_t *)&_imgHdr;
    }

    // Ensure the image is built with a compatible BIM and META
    if(idPld->bimVer  != currentImageHeader->fixedHdr.bimVer  ||
        idPld->metaVer != currentImageHeader->fixedHdr.metaVer )
    {
        status = OADStorage_Rejected;
    }
#endif //FEATURE_OAD_SERVER_ONLY

    // Validate the Image header preamble
    if( (memcmp(&imgID, idPld->imgID, OAD_IMG_ID_LEN) != 0)  &&
        (memcmp(&imgvmID, idPld->imgID, OAD_IMG_ID_LEN) != 0) )
    {
        status = OADStorage_Rejected;
    }

#if defined(USE_ICALL) && !defined(ICALL_LITE)
    // Check that there is space to store the image
    // Host and USR images are not checked for size
    if(idPld->imgType == OAD_IMG_TYPE_APP)
    {
        /*
         * Ensure that the propsed image doesn't run into stack start
         * Note this just checks the size of the image, actual start addr will
         * be checked later when the segement comes along
         */
        if(idPld->len > (stackImageHeader->stackStartAddr - 1))
        {
            status = OADStorage_Rejected;
        }
    }
    else if (idPld->imgType == OAD_IMG_TYPE_STACK_ONLY)
    {
        if(!useExternalFlash)
        {
            /*
             * On-chip OAD, stack only OAD must be followed by app only oad
             * (user application is erased), thus we must only check that
             * the proposed stack can fit within the space between the existing
             * scratch space (usr app region)
             */
            if(idPld->len > (stackImageHeader->stackStartAddr - 1))
            {
              status = OADStorage_Rejected;
            }
        }
        else
        {
            /*
             * Off-chip OAD, stack only can be done without changing the
             * existing application, must fit between app end and BIM_START
             */
            if(currentImageHeader->imgType != OAD_IMG_TYPE_APP_STACK)
            {
                if((idPld->len - 1) >= (BIM_START - currentImageHeader->imgEndAddr - 1))
                {
                  status = OADStorage_Rejected;
                }
            }
            else
            {
                if((idPld->len - 1) >= (BIM_START - stackImageHeader->stackStartAddr - 1))
                {
                  status = OADStorage_Rejected;
                }
            }
        }
    }
    else if (idPld->imgType == OAD_IMG_TYPE_APP_STACK)
    {
        if(!useExternalFlash)
        {
            // On-chip OAD doesn't support App + Stack merged images
            status = OADStorage_Rejected;
        }
        else
        {
            // Off-chip OAD
            if((idPld->len - 1) > (BIM_START - 1))
            {
                status = OADStorage_Rejected;
            }
        }
    }
#endif
    else if (idPld->imgType == OAD_IMG_TYPE_PERSISTENT_APP     ||
             idPld->imgType == OAD_IMG_TYPE_BIM         ||
             idPld->imgType == OAD_IMG_TYPE_FACTORY ||
             idPld->imgType >= OAD_IMG_TYPE_RSVD_BEGIN)
    {
        // Persistent app, BIM, factory are not currently upgradeable
        // Image type must also not be in the reserved range
        status = OADStorage_Rejected;
    }

    if(status == OADStorage_Status_Success)
    {
        // If we are about to accept the image, store the image data
        candidateImageLength = idPld->len;
        candidateImageType = idPld->imgType;
    }
    return (status);
}

/*********************************************************************
 * @fn      oadCheckDL
 *
 * @brief   Check validity of the downloaded image.
 *
 * @param   None.
 *
 * @return  TRUE or FALSE for image valid.
 */
static uint8_t oadCheckDL(void)
{
    uint32_t crcFromHdr;
    uint32_t crcCalculated;
    uint8_t crcStatus;


    uint8_t status = OADStorage_Status_Success;

    // Read in the CRC
    readFlashPg(imagePage, CRC_OFFSET, (uint8_t *)(&crcFromHdr),
                sizeof(uint32_t));

    // Read in the image info word
    readFlashPg(imagePage, CRC_STAT_OFFSET, &crcStatus,
                sizeof(uint8_t));

    // If for some reason the header shows the CRC is invalid reject the image now
    if (crcStatus == CRC_INVALID)
    {
        return (OADStorage_CrcError);
    }

    // Calculate CRC of downloaded image.
    if(useExternalFlash)
    {
        crcCalculated = CRC32_calc(imagePage, EFL_PAGE_SIZE, 0, candidateImageLength, useExternalFlash);
    }
    else
    {
        crcCalculated = CRC32_calc(imagePage, flashPageSize, 0, candidateImageLength, useExternalFlash);
    }

    if (crcCalculated == crcFromHdr)
    {
        // Set CRC stat to valid
        crcStatus = CRC_VALID;

        // Only write to the CRC flag if using internal flash
        if(!useExternalFlash)
        {
            // Write CRC status back to flash
            writeFlashPg(imagePage, CRC_STAT_OFFSET, &crcStatus,
                            sizeof(uint8_t));
        }
        status = OADStorage_Status_Success;

    }
    else
    {
        status = OADStorage_CrcError;
    }

    return (status);
}

/*********************************************************************
 * @fn      OADStorage_imgBlockRead
 *
 * @brief   Read an Image block.
 *
 * @param   blockNum   - block number to be read
 * @param   pBlockData - pointer for data to be read
 *
 * @return  none
 */
void OADStorage_imgBlockRead(uint16_t blockNum, uint8_t *pBlockData)
{
    // Calculate address to write as (start of OAD range) + (offset into range)
    uint32_t blkStartAddr = (oadImgBytesPerBlock)*blockNum + imageAddress;

    uint8_t page = (blkStartAddr >> 12);
    uint32_t offset = (blkStartAddr & 0x00000FFF);

    // Read a block from Flash.
    readFlashPg(page, offset, pBlockData,
                          (OADStorage_BLOCK_SIZE - OADStorage_BLK_NUM_HDR_SZ));
}

/*********************************************************************
 * @fn      OADStorage_close
 *
 * @brief   closes storage.
 *
 * @param  none
 *
 * @return none
 */
void OADStorage_close(void)
{
    // close the flash interface
    flash_close();
}

/*********************************************************************
*********************************************************************/
