/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/***** Includes *****/
/* Standard C Libraries */
#include <string.h>
#include <stdlib.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/cpu.h)

/* Board Header files */
#include "ti_drivers_config.h"

/* OAD header files */
#include "oad/native_oad/oad_protocol.h"
#include "oad/native_oad/oad_server.h"
#include "oad/native_oad/oad_storage.h"

#if (defined(DeviceFamily_CC13X2) || defined(DeviceFamily_CC26X2))
#include "common/cc26xx/oad/ext_flash_layout.h"
#else
#include "oad/native_oad/ext_flash_layout.h"
#endif

/* Application Header files */
#include "ConcentratorRadioTask.h"
#include "ConcentratorTask.h"

/***** Defines *****/
#define CONCENTRATOR_TASK_STACK_SIZE 1024
#define CONCENTRATOR_TASK_PRIORITY   3

#define CONCENTRATOR_EVENT_ALL                    0xFFFFFFFF
#define CONCENTRATOR_EVENT_UPDATE_SENSOR_VALUE    (uint32_t)(1 << 0)
#define CONCENTRATOR_EVENT_ACTION                 (uint32_t)(1 << 1)
#define CONCENTRATOR_EVENT_UPDATE_DISPLAY         (uint32_t)(1 << 2)
#define CONCENTRATOR_EVENT_OAD_MSG                (uint32_t)(1 << 3)

#define CONCENTRATOR_MAX_NODES 7

#define CONCENTRATOR_DISPLAY_LINES 10

/***** Type declarations *****/
struct AdcSensorNode
{
    uint8_t address;
    uint16_t latestAdcValue;
    uint8_t button;
    int8_t latestRssi;
    char fwVersion[OADProtocol_FW_VERSION_STR_LEN];
    uint16_t oadBlock;
    uint16_t oadTotalBlocks;
};

/// \brief UI actions
typedef enum
{
    Concentrator_Actions_Start = 0,
    Concentrator_Actions_UpdateAvailableFw = 0,
    Concentrator_Actions_FwVerReq,
    Concentrator_Actions_UpdateNodeFw,
    Concentrator_Actions_End
} Concentrator_Actions_t;

static char* selectedActionStr[] =
{
    "Update available FW",
    "Send FW Ver Req",
    "Update node FW",
};

Concentrator_Actions_t selectedAction = Concentrator_Actions_Start;

char availableFwVersion[OADProtocol_FW_VERSION_STR_LEN] = "unknown";

static bool availableFwUpdateInProgress = false;
static ConcentratorTask_NodeOadStatus_t nodeFwOadStatus = ConcentratorTask_NodeOadStatus_None;

/*
 * Application button pin configuration table:
 *   - Buttons interrupts are configured to trigger on falling edge.
 */
PIN_Config buttonPinTable[] = {
    CONFIG_PIN_BTN1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    CONFIG_PIN_BTN2  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

/***** Variable declarations *****/
static Task_Params concentratorTaskParams;
Task_Struct concentratorTask;    /* not static so you can see in ROV */
static uint8_t concentratorTaskStack[CONCENTRATOR_TASK_STACK_SIZE];
Event_Struct concentratorEvent;  /* not static so you can see in ROV */
static Event_Handle concentratorEventHandle;
static struct AdcSensorNode latestActiveAdcSensorNode;
struct AdcSensorNode knownSensorNodes[CONCENTRATOR_MAX_NODES];
static struct AdcSensorNode* lastAddedSensorNode = knownSensorNodes;
static uint8_t selectedNode = 0;
static Display_Handle hDisplayLcd;
static Display_Handle hDisplaySerial;
static PIN_Handle buttonPinHandle;
static PIN_State buttonPinState;

/***** Prototypes *****/
static void concentratorTaskFunction(UArg arg0, UArg arg1);
static void packetReceivedCallback(union ConcentratorPacket* packet, int8_t rssi);
static void updateLcd(void);
static void addNewNode(struct AdcSensorNode* node);
static void updateNode(struct AdcSensorNode* node);
static uint8_t isKnownNodeAddress(uint8_t address);
static uint8_t getNodeIdx(uint8_t address);
static void removeNode(uint8_t address);
void buttonCallback(PIN_Handle handle, PIN_Id pinId);

/***** Function definitions *****/
void ConcentratorTask_init(void) {
    OADServer_Params_t oadServerParams = {0};

    /* Create event used internally for state changes */
    Event_Params eventParam;
    Event_Params_init(&eventParam);
    Event_construct(&concentratorEvent, &eventParam);
    concentratorEventHandle = Event_handle(&concentratorEvent);

    /* Create the concentrator radio protocol task */
    Task_Params_init(&concentratorTaskParams);
    concentratorTaskParams.stackSize = CONCENTRATOR_TASK_STACK_SIZE;
    concentratorTaskParams.priority = CONCENTRATOR_TASK_PRIORITY;
    concentratorTaskParams.stack = &concentratorTaskStack;
    Task_construct(&concentratorTask, concentratorTaskFunction, &concentratorTaskParams, NULL);

    oadServerParams.eventBit = CONCENTRATOR_EVENT_OAD_MSG;
    oadServerParams.eventHandle = concentratorEventHandle;
    OADProtocol_Status_t status = OADServer_open(&oadServerParams);

    if (status != OADProtocol_Status_Success)
    {
        System_abort("Error initializing OAD server\n");
    }
}

void ConcentratorTask_updateNodeFWVer(uint8_t addr, char* fwVersionStr)
{
    uint8_t nodeIdx = getNodeIdx(addr);

    if (nodeIdx < CONCENTRATOR_MAX_NODES)
    {
        /* Save the values */
        memcpy(knownSensorNodes[nodeIdx].fwVersion, fwVersionStr, OADProtocol_FW_VERSION_STR_LEN);
        Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_UPDATE_DISPLAY);
    }
}

void ConcentratorTask_updateAvailableFWVer(bool success)
{
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

    availableFwUpdateInProgress = false;

    if(success)
    {
#if (defined(DeviceFamily_CC13X2) || defined(DeviceFamily_CC26X2))
        /* get Available FW version*/
        OADStorage_imgIdentifyPld_t remoteAppImageId;

        OADStorage_init();

        /* get Available FW version*/
        OADStorage_imgIdentifyRead(OAD_IMG_TYPE_USR_BEGIN, &remoteAppImageId);

        System_sprintf((xdc_Char*)availableFwVersion,"rfWsnNode sv:%c%c%c%c bv:%02x",
                       remoteAppImageId.softVer[0],
                       remoteAppImageId.softVer[1],
                       remoteAppImageId.softVer[2],
                       remoteAppImageId.softVer[3],
                       remoteAppImageId.bimVer);

        OADStorage_close();
#else
        OADTarget_ImgHdr_t remoteAppImageHdr;

        /* get Available FW version*/
        OADTarget_getImageHeader(EFL_OAD_IMG_TYPE_REMOTE_APP, &remoteAppImageHdr);
        if ((remoteAppImageHdr.ver != 0xFFFF) || (remoteAppImageHdr.ver == 0))
        {
            System_sprintf((xdc_Char*)availableFwVersion,"rfWsnNode v%02d.%02d.00", ((remoteAppImageHdr.ver & 0xFF00)>>8), (remoteAppImageHdr.ver & 0xFF));
        }
#endif
    }
    else
    {
        System_sprintf((xdc_Char*)availableFwVersion,"update failed");
    }

    /* Re-enable UART and LCD */
    Display_control(hDisplayLcd, DISPLAY_CMD_TRANSPORT_OPEN, NULL);
    hDisplaySerial = Display_open(Display_Type_UART, &params);

    Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_UPDATE_DISPLAY);
}

void ConcentratorTask_updateNodeOadStatus(ConcentratorTask_NodeOadStatus_t status)
{
    static ConcentratorTask_NodeOadStatus_t prevStatus = ConcentratorTask_NodeOadStatus_None;

    if( (status == ConcentratorTask_NodeOadStatus_InProgress) &&
        (prevStatus != ConcentratorTask_NodeOadStatus_InProgress) )
    {
        /* free LCD display for SPI driver (needed for ext flash) */
        Display_print0(hDisplayLcd, 0, 0, "Updating Node FW...");
        Display_control(hDisplayLcd, DISPLAY_CMD_TRANSPORT_CLOSE, NULL);
    }
    else if( (status != ConcentratorTask_NodeOadStatus_InProgress) &&
        (prevStatus == ConcentratorTask_NodeOadStatus_InProgress) )
    {
        /* Re-enableLCD */
        Display_control(hDisplayLcd, DISPLAY_CMD_TRANSPORT_OPEN, NULL);
    }

    prevStatus = status;
    nodeFwOadStatus = status;

    /* update display */
    Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_UPDATE_DISPLAY);
}

void ConcentratorTask_updateNodeOadBlock(uint8_t addr, uint16_t block)
{
    uint8_t nodeIdx = getNodeIdx(addr);

    if (nodeIdx < CONCENTRATOR_MAX_NODES)
    {
        /* Save the values */
        knownSensorNodes[nodeIdx].oadBlock = block;
        Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_UPDATE_DISPLAY);

        if(knownSensorNodes[nodeIdx].oadBlock == knownSensorNodes[nodeIdx].oadTotalBlocks - 1)
        {
            /* Re-enable LCD */
            Display_control(hDisplayLcd, DISPLAY_CMD_TRANSPORT_OPEN, NULL);
        }
    }
}

void ConcentratorTask_setNodeOadStatusReset(void)
{
    nodeFwOadStatus = ConcentratorTask_NodeOadStatus_ResetComplete;

    Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_ACTION);
}

static void concentratorTaskFunction(UArg arg0, UArg arg1)
{
#if (defined(DeviceFamily_CC13X2) || defined(DeviceFamily_CC26X2))
    OADStorage_imgIdentifyPld_t remoteAppImageId;

    OADStorage_init();

    /* get Available FW version*/
    if(OADStorage_imgIdentifyRead(OAD_IMG_TYPE_USR_BEGIN, &remoteAppImageId) != 0)
    {
        System_sprintf((xdc_Char*)availableFwVersion,"rfWsnNode sv:%c%c%c%c bv:%02x",
                       remoteAppImageId.softVer[0],
                       remoteAppImageId.softVer[1],
                       remoteAppImageId.softVer[2],
                       remoteAppImageId.softVer[3],
                       remoteAppImageId.bimVer);
    }

    OADStorage_close();
#else
        OADTarget_ImgHdr_t remoteAppImageHdr;

    /* get Available FW version*/
    OADTarget_getImageHeader(EFL_OAD_IMG_TYPE_REMOTE_APP, &remoteAppImageHdr);
    if ((remoteAppImageHdr.ver != 0xFFFF) || (remoteAppImageHdr.ver == 0))
    {
        System_sprintf((xdc_Char*)availableFwVersion,"rfWsnNode v%02d.%02d.00", ((remoteAppImageHdr.ver & 0xFF00)>>8), (remoteAppImageHdr.ver & 0xFF));
    }
#endif

    /* Initialize display and try to open both UART and LCD types of display. */
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

    /* Open both an available LCD display and an UART display.
     * Whether the open call is successful depends on what is present in the
     * Display_config[] array of the board file.
     *
     * Note that for SensorTag evaluation boards combined with the SHARP96x96
     * Watch DevPack, there is a pin conflict with UART such that one must be
     * excluded, and UART is preferred by default. To display on the Watch
     * DevPack, add the precompiler define BOARD_DISPLAY_EXCLUDE_UART.
     */
    hDisplayLcd = Display_open(Display_Type_LCD, &params);
    hDisplaySerial = Display_open(Display_Type_UART, &params);

    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
    if(!buttonPinHandle)
    {
        System_abort("Error initializing button pins\n");
    }

    /* Setup callback for button pins */
    if (PIN_registerIntCb(buttonPinHandle, &buttonCallback) != 0)
    {
        System_abort("Error registering button callback function");
    }

    /* Register a packet received callback with the radio task */
    ConcentratorRadioTask_registerPacketReceivedCallback(packetReceivedCallback);

    //set selected node to 0
    selectedNode = 0;

    /* Update the values on the LCD */
    updateLcd();

    /* Enter main task loop */
    while (1)
    {
        /* Wait for event */
        uint32_t events = Event_pend(concentratorEventHandle, 0, CONCENTRATOR_EVENT_ALL, BIOS_WAIT_FOREVER);

        /* If we got a new ADC sensor value */
        if (events & CONCENTRATOR_EVENT_UPDATE_SENSOR_VALUE)
        {
            /* If we knew this node from before, update the value */
            if (isKnownNodeAddress(latestActiveAdcSensorNode.address))
            {
                updateNode(&latestActiveAdcSensorNode);
            }
            else
            {
                /* Clear FW version string */
                strcpy(latestActiveAdcSensorNode.fwVersion, "Unknown\0");
                /* add new node */
                addNewNode(&latestActiveAdcSensorNode);
            }

            /* Update the values on the LCD */
            updateLcd();
        }

        /* If the action button was pressed */
        if (events & CONCENTRATOR_EVENT_ACTION)
        {
            uint16_t totalBlocks = 0;

            switch(selectedAction)
            {
            case Concentrator_Actions_UpdateAvailableFw:
                if( (nodeFwOadStatus != ConcentratorTask_NodeOadStatus_InProgress) &&
                        !availableFwUpdateInProgress)
                {
                    /* get FW from UART */
                    availableFwUpdateInProgress = true;

                    /* free LCD display for SPI driver (needed for ext flash) */
                    Display_print0(hDisplayLcd, 0, 0, "Waiting for Node FW update...");
                    Display_control(hDisplayLcd, DISPLAY_CMD_TRANSPORT_CLOSE, NULL);

                    /* free Serial display for UART Driver (needed to receive new FW image) */
                    Display_print0(hDisplaySerial, 0, 0, "\033[2J \033[0;0HWaiting for Node FW update...");
                    Display_close(hDisplaySerial);

                    OADServer_updateAvailableFwVer();
                }
                break;

            case Concentrator_Actions_FwVerReq:
                /* get FW version of selected node */
                if(knownSensorNodes[selectedNode].address != 0)
                {
                    /* abort any other message */
                    ConcentratorRadioTask_abortNodeMsg();

                    OADServer_getFwVer(knownSensorNodes[selectedNode].address);
                }
                break;

            case Concentrator_Actions_UpdateNodeFw:
#if (defined(DeviceFamily_CC13X2) || defined(DeviceFamily_CC26X2))
                if ((knownSensorNodes[selectedNode].address != 0) &&
                    (nodeFwOadStatus == ConcentratorTask_NodeOadStatus_ResetComplete) &&
                    !availableFwUpdateInProgress)
                {
                    /* abort any other message */
                    ConcentratorRadioTask_abortNodeMsg();

                    /* update FW on selected node */
                    totalBlocks = OADServer_updateNodeFw(knownSensorNodes[selectedNode].address);

                        /* store total blocks */
                    knownSensorNodes[selectedNode].oadTotalBlocks = totalBlocks;

                        /* update display */
                    ConcentratorTask_updateNodeOadBlock(knownSensorNodes[selectedNode].address, 0);
                }
                else if((knownSensorNodes[selectedNode].address != 0) &&
                        (nodeFwOadStatus != ConcentratorTask_NodeOadStatus_InProgress) &&
                        !availableFwUpdateInProgress)
                    {
                    /* abort any other message */
                    ConcentratorRadioTask_abortNodeMsg();

                    /* Send reset command to Node */
                    OADServer_resetNode(knownSensorNodes[selectedNode].address);
                }
                break;
#else
                if((knownSensorNodes[selectedNode].address != 0) &&
                   (nodeFwOadStatus != ConcentratorTask_NodeOadStatus_InProgress) &&
                   !availableFwUpdateInProgress)
                {
                    /* abort any other message */
                    ConcentratorRadioTask_abortNodeMsg();

                    /* update FW on selected node */
                    totalBlocks = OADServer_updateNodeFw(knownSensorNodes[selectedNode].address);

                    /* store total blocks */
                    knownSensorNodes[selectedNode].oadTotalBlocks = totalBlocks;

                    /* update display */
                    ConcentratorTask_updateNodeOadBlock(knownSensorNodes[selectedNode].address, 0);
                }
                break;
#endif
            default:
                break;
            }

            /* Update the values on the LCD */
            updateLcd();
        }
        /* If the action button was pressed */
        if (events & CONCENTRATOR_EVENT_UPDATE_DISPLAY)
        {
            /* Update the values on the LCD */
            updateLcd();
        }
        /* If the action button was pressed */
        if (events & CONCENTRATOR_EVENT_OAD_MSG)
        {
            /* service OAD event */
            OADServer_processEvent(&events);
        }
    }
}

static void packetReceivedCallback(union ConcentratorPacket* packet, int8_t rssi)
{
    /* If we received an ADC sensor packet, for backward compatibility */
    if (packet->header.packetType == RADIO_PACKET_TYPE_ADC_SENSOR_PACKET)
    {
        /* Save the values */
        latestActiveAdcSensorNode.address = packet->header.sourceAddress;
        latestActiveAdcSensorNode.latestAdcValue = packet->adcSensorPacket.adcValue;
        latestActiveAdcSensorNode.button = 0; //no button value in ADC packet
        latestActiveAdcSensorNode.latestRssi = rssi;

        Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_UPDATE_SENSOR_VALUE);
    }
    /* If we received an DualMode ADC sensor packet*/
    else if(packet->header.packetType == RADIO_PACKET_TYPE_DM_SENSOR_PACKET)
    {

        /* Save the values */
        latestActiveAdcSensorNode.address = packet->header.sourceAddress;
        latestActiveAdcSensorNode.latestAdcValue = packet->dmSensorPacket.adcValue;
        latestActiveAdcSensorNode.button = packet->dmSensorPacket.button;
        latestActiveAdcSensorNode.latestRssi = rssi;

        Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_UPDATE_SENSOR_VALUE);
    }
    /* If we received an OAD packet*/
    else if(packet->header.packetType == RADIO_PACKET_TYPE_OAD_PACKET)
    {
        uint8_t srcAddr = packet->header.sourceAddress;
        OADProtocol_ParseIncoming(&srcAddr, packet->oadPacket.oadPayload);
    }
}

static uint8_t isKnownNodeAddress(uint8_t address)
{
    uint8_t found = 0;
    uint8_t i;
    for (i = 0; i < CONCENTRATOR_MAX_NODES; i++)
    {
        if (knownSensorNodes[i].address == address)
        {
            found = 1;
            break;
        }
    }
    return found;
}

static uint8_t getNodeIdx(uint8_t address)
{

    uint8_t nodeIdx = CONCENTRATOR_MAX_NODES;
    uint8_t i;

    for (i = 0; i < CONCENTRATOR_MAX_NODES; i++)
    {
        if (knownSensorNodes[i].address == address)
        {
            nodeIdx = i;
            break;
        }
    }

    return nodeIdx;
}

static void updateNode(struct AdcSensorNode* node)
{
    uint8_t i;
    for (i = 0; i < CONCENTRATOR_MAX_NODES; i++)
    {
        if (knownSensorNodes[i].address == node->address)
        {
            knownSensorNodes[i].latestAdcValue = node->latestAdcValue;
            knownSensorNodes[i].latestRssi = node->latestRssi;
            knownSensorNodes[i].button = node->button;
           break;
        }
    }
}

static void addNewNode(struct AdcSensorNode* node)
{
    *lastAddedSensorNode = *node;
    uint8_t i;

    /* Increment and wrap */
    lastAddedSensorNode++;
    if (lastAddedSensorNode > &knownSensorNodes[CONCENTRATOR_MAX_NODES-1])
    {
        lastAddedSensorNode = knownSensorNodes;
    }

    /* check if node OAD complete and remove node */
    for (i = 0; i < CONCENTRATOR_MAX_NODES; i++)
    {
        if(knownSensorNodes[i].oadBlock == (knownSensorNodes[i].oadTotalBlocks - 1))
        {
            removeNode(knownSensorNodes[i].address);
        }
    }
}

static void removeNode(uint8_t address)
{
    uint8_t i;
    bool removed = false;

    for (i = 0; i < (CONCENTRATOR_MAX_NODES - 1); i++)
    {
        if ((knownSensorNodes[i].address == address) || removed)
        {
            memcpy(&knownSensorNodes[i], &knownSensorNodes[i+1], sizeof(struct AdcSensorNode));
            memset(&knownSensorNodes[i+1], 0, sizeof(struct AdcSensorNode));
            if(selectedNode == i + 1)
            {
                selectedNode--;
            }
            removed = true;
        }
    }

    if ((knownSensorNodes[i].address == address) || removed)
    {
        memset(&knownSensorNodes[i], 0, sizeof(struct AdcSensorNode));
        if(selectedNode == i + 1)
        {
            selectedNode--;
        }
        removed = true;
    }

    if(removed)
    {
        lastAddedSensorNode--;
    }
}

static void updateLcd(void)
{
    struct AdcSensorNode* nodePointer = knownSensorNodes;
    uint8_t currentLcdLine;
    char selectedChar;

    /* Don't use LCD as available FW is being update, UART and
     * SPI (for ext flash) are in-use
     */
    if(availableFwUpdateInProgress == false)
    {
        if(nodeFwOadStatus != ConcentratorTask_NodeOadStatus_InProgress)
        {
            /* Clear the display and write header on first line */
            Display_clear(hDisplayLcd);
            Display_printf(hDisplayLcd, 0, 0, "Nodes Value SW  RSSI");
        }

        //clear screen, put curser to beginning of terminal and print the header
        Display_printf(hDisplaySerial, 0, 0, "\033[2J \033[0;0HNodes   Value   SW    RSSI");

        /* Start on the second line */
        currentLcdLine = 1;

        /* Write one line per node */
        while ((nodePointer < &knownSensorNodes[CONCENTRATOR_MAX_NODES]) &&
              (nodePointer->address != 0) &&
              (currentLcdLine < CONCENTRATOR_DISPLAY_LINES))
        {
            if ( currentLcdLine == selectedNode + 1)
            {
                selectedChar = '*';
            }
            else
            {
                selectedChar = ' ';
            }

            if(nodeFwOadStatus != ConcentratorTask_NodeOadStatus_InProgress)
            {
                /* print to LCD */
                Display_printf(hDisplayLcd, currentLcdLine, 0, "%c0x%02x  %04d  %d   %04d", selectedChar,
                    nodePointer->address, nodePointer->latestAdcValue, nodePointer->button,
                    nodePointer->latestRssi);
            }

            /* print to UART */
            Display_printf(hDisplaySerial, 0, 0, "%c0x%02x    %04d    %d    %04d", selectedChar,
                    nodePointer->address, nodePointer->latestAdcValue, nodePointer->button,
                    nodePointer->latestRssi);

            nodePointer++;
            currentLcdLine++;
        }

        if(currentLcdLine == 1)
        {
            if(nodeFwOadStatus != ConcentratorTask_NodeOadStatus_InProgress)
            {
                /* print to LCD */
                Display_printf(hDisplayLcd, currentLcdLine, 0, "Waiting for nodes...");
            }

            /* print to UART */
            Display_printf(hDisplaySerial, 0, 0, "Waiting for nodes...");
            /* print to UART */
            Display_printf(hDisplaySerial, 0, 0, "Left Btn: Select node, Right: Select action, Both: Execute action");

            currentLcdLine++;
        }

        if(nodeFwOadStatus != ConcentratorTask_NodeOadStatus_InProgress)
        {
            /* print to LCD */
            Display_printf(hDisplayLcd, currentLcdLine + 1, 0, "Action: %s",
                       selectedActionStr[selectedAction]);
        }

        /* print to UART */
        Display_printf(hDisplaySerial, 0, 0, "Action: %s",
                       selectedActionStr[selectedAction]);

        switch(selectedAction)
        {
        case Concentrator_Actions_UpdateAvailableFw:
            /* Available FW*/

            if(!nodeFwOadStatus)
            {
                /* print to LCD */
                Display_printf(hDisplayLcd, currentLcdLine, 0, "Info: Available FW %s",
                           availableFwVersion);
            }

            /* print to UART */
            Display_printf(hDisplaySerial, 0, 0, "Info: Available FW %s",
                           availableFwVersion);

            break;
        case Concentrator_Actions_FwVerReq:
            /* FW version of selected node */
            /* print to LCD */
            if(nodeFwOadStatus != ConcentratorTask_NodeOadStatus_InProgress)
            {
                Display_printf(hDisplayLcd, currentLcdLine, 0, "Info: Node 0x%02x FW %s",
                           knownSensorNodes[selectedNode].address,
                           knownSensorNodes[selectedNode].fwVersion);
            }

            /* print to UART */
            Display_printf(hDisplaySerial, 0, 0, "Info: Node 0x%02x FW %s",
                           knownSensorNodes[selectedNode].address,
                           knownSensorNodes[selectedNode].fwVersion);

            break;
        case Concentrator_Actions_UpdateNodeFw:
            if(nodeFwOadStatus == ConcentratorTask_NodeOadStatus_InProgress)
            {
                /* print to UART */
                Display_printf(hDisplaySerial, 0, 0, "Info: OAD Block %d of %d",
                           knownSensorNodes[selectedNode].oadBlock,
                           knownSensorNodes[selectedNode].oadTotalBlocks);
            }
            else if(nodeFwOadStatus == ConcentratorTask_NodeOadStatus_Completed)
            {
                /* print to UART */
                Display_printf(hDisplaySerial, 0, 0, "Info: OAD Complete");
            }
            else if(nodeFwOadStatus == ConcentratorTask_NodeOadStatus_Aborted)
            {
                /* print to UART */
                Display_printf(hDisplaySerial, 0, 0, "Info: OAD Aborted");
            }

            break;
        default:
            break;
        }
    }
}

/*
 *  ======== buttonCallback ========
 *  Pin interrupt Callback function board buttons configured in the pinTable.
 */
void buttonCallback(PIN_Handle handle, PIN_Id pinId)
{
    /* Debounce logic, only toggle if the button is still pushed (low) */
    CPUdelay(8000*50);
    static bool prevPressBoth = false;

    if ((PIN_getInputValue(CONFIG_PIN_BTN1) == 0) && (PIN_getInputValue(CONFIG_PIN_BTN2) == 0))
    {
        //handle case where callback is trigger for both presses
        if(!prevPressBoth)
        {
            //trigger action
            Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_ACTION);
        }

        prevPressBoth = true;
    }
    else if (PIN_getInputValue(CONFIG_PIN_BTN1) == 0)
    {
        selectedNode++;
        if ( (selectedNode > CONCENTRATOR_MAX_NODES)  ||
             (knownSensorNodes[selectedNode].address == 0))
        {
            selectedNode = 0;
        }

        //trigger LCD update
        Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_UPDATE_DISPLAY);

        prevPressBoth = false;
    }
    else if (PIN_getInputValue(CONFIG_PIN_BTN2) == 0)
    {
        selectedAction++;
        if(selectedAction == Concentrator_Actions_End)
        {
            selectedAction = Concentrator_Actions_Start;
        }

        //trigger LCD update
        Event_post(concentratorEventHandle, CONCENTRATOR_EVENT_UPDATE_DISPLAY);

        prevPressBoth = false;
    }
}

