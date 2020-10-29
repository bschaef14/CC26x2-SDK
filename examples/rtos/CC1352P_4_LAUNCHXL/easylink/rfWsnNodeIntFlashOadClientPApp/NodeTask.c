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

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/drivers/PIN.h>
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)

/* Board Header files */
#include "ti_drivers_config.h"

#include "SceAdc.h"
#include "NodeTask.h"
#include "NodeRadioTask.h"
#include "oad/native_oad/oad_client.h"

#ifdef OAD_U_APP
#include "NodeStorage.h"
#endif


/***** Defines *****/
#define NODE_TASK_STACK_SIZE 1024
#define NODE_TASK_PRIORITY   3

/* A change mask of 0xFF0 means that changes in the lower 4 bits does not trigger a wakeup. */
#define NODE_ADCTASK_CHANGE_MASK                    0xFF0

/* Minimum slow Report interval is 15s (in units of samplingTime)*/
#define NODE_ADCTASK_REPORTINTERVAL_SLOW                15
/* Minimum fast Report interval is 1s (in units of samplingTime) for 30s*/
#define NODE_ADCTASK_REPORTINTERVAL_FAST                1
#define NODE_ADCTASK_REPORTINTERVAL_FAST_DURIATION_MS   30000


/***** Variable declarations *****/
static Task_Params nodeTaskParams;
Task_Struct nodeTask;    /* not static so you can see in ROV */
static uint8_t nodeTaskStack[NODE_TASK_STACK_SIZE];
Event_Struct nodeEvent;  /* not static so you can see in ROV */
static Event_Handle nodeEventHandle;
static uint16_t latestAdcValue;

/* Clock for the fast report timeout */
Clock_Struct fastReportTimeoutClock;     /* not static so you can see in ROV */
static Clock_Handle fastReportTimeoutClockHandle;

/* Pin driver handle */
static PIN_Handle buttonPinHandle;
static PIN_Handle ledPinHandle;
static PIN_State buttonPinState;
static PIN_State ledPinState;

static bool oadInProgress = 0;
static uint16_t oadBlock = 0;
static uint16_t oadTotalBlocks = 0;
static uint32_t oadRetries = 0;
static int8_t oadStatus = -1;

/* Display driver handles */
static Display_Handle hDisplayLcd;
static Display_Handle hDisplaySerial;
/* Enable the 3.3V power domain used by the LCD */
PIN_Config pinTable[] = {
    NODE_ACTIVITY_LED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/*
 * Application button pin configuration table:
 *   - Buttons interrupts are configured to trigger on falling edge.
 */
PIN_Config buttonPinTable[] = {
    CONFIG_PIN_BTN1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    CONFIG_PIN_BTN2  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

static uint8_t nodeAddress = 0;

/***** Prototypes *****/
static void nodeTaskFunction(UArg arg0, UArg arg1);
static void updateLcd(void);
void fastReportTimeoutCallback(UArg arg0);
void adcCallback(uint16_t adcValue);
void buttonCallback(PIN_Handle handle, PIN_Id pinId);

/***** Function definitions *****/
void NodeTask_init(void)
{
    OADClient_Params_t oadclientParams = {0};

    /* Create event used internally for state changes */
    Event_Params eventParam;
    Event_Params_init(&eventParam);
    Event_construct(&nodeEvent, &eventParam);
    nodeEventHandle = Event_handle(&nodeEvent);

    /* Create clock object which is used for fast report timeout */
    Clock_Params clkParams;
    Clock_Params_init(&clkParams);

    clkParams.period = 0;
    clkParams.startFlag = FALSE;
    Clock_construct(&fastReportTimeoutClock, fastReportTimeoutCallback, 1, &clkParams);
    fastReportTimeoutClockHandle = Clock_handle(&fastReportTimeoutClock);

    /* Create the node task */
    Task_Params_init(&nodeTaskParams);
    nodeTaskParams.stackSize = NODE_TASK_STACK_SIZE;
    nodeTaskParams.priority = NODE_TASK_PRIORITY;
    nodeTaskParams.stack = &nodeTaskStack;
    Task_construct(&nodeTask, nodeTaskFunction, &nodeTaskParams, NULL);

    oadclientParams.eventHandle = nodeEventHandle;
    oadclientParams.oadReqEventBit = NODE_EVENT_OAD_REQ;
    /* Reuse ADC message as poll */
    oadclientParams.oadRspPollEventBit = NODE_EVENT_NEW_ADC_VALUE;
    OADClient_open(&oadclientParams);
}

/*!
 The application calls this function to indicate oad block.

 Public function defined in ssf.h
 */
void NodeTask_displayOadBlockUpdate(uint16_t newOadBlock, uint16_t oadBNumBlocks, uint32_t retries)
{
    if(!oadInProgress)
    {
        oadInProgress = true;

        /* free LCD display for SPI driver (needed for ext flash) */
        Display_print0(hDisplayLcd, 0, 0, "Waiting for Node FW update...");
        Display_control(hDisplayLcd, DISPLAY_CMD_TRANSPORT_CLOSE, NULL);
    }
    oadBlock = newOadBlock;
    oadTotalBlocks = oadBNumBlocks;
    oadRetries = retries;


    Event_post(nodeEventHandle, NODE_EVENT_UPDATE_LCD);
}

/*!
 The application calls this function to indicate oad status.

 Public function defined in ssf.h
 */
void NodeTask_displayOadStatusUpdate(OADStorage_Status_t status)
{
    oadInProgress = false;
    oadStatus = (int8_t) status;

    if(status != OADStorage_Status_Success)
    {
        Display_Params params;
        Display_Params_init(&params);
        params.lineClearMode = DISPLAY_CLEAR_BOTH;

        /* Re-enable LCD */
        Display_control(hDisplayLcd, DISPLAY_CMD_TRANSPORT_OPEN, NULL);
    }

    Event_post(nodeEventHandle, NODE_EVENT_UPDATE_LCD);
}

#ifdef OAD_U_APP
/*!
 This function is called when U-App receives OAD reset request
 */
void NodeTask_resetUserApp(void* pDstAddr)
{
    NodeStorage_Status status;
    uint8_t nodeAddr = nodeRadioTask_getNodeAddr();

    NodeStorage_init();

    /* Write the node and server address to NVS*/
    NodeStorage_eraseAll();
    status = NodeStorage_writeStartSeq();
    status |= NodeStorage_write(NODESTORAGE_NODE_ADDR_OFFSET, &nodeAddr, 1);
    status |= NodeStorage_write(NODESTORAGE_SERVER_ADDR_OFFSET, pDstAddr, 1);

    if (status != NodeStorage_Success)
    {
        /* If one of the writes was not successful clear the entire storage */
        NodeStorage_eraseAll();
    }

    /* Invalidate OAD image header so the bim will boot into the P-App */
    OADClient_invalidateHeader();

    /* Reset device */
    SysCtrlSystemReset();
}
#endif

static void nodeTaskFunction(UArg arg0, UArg arg1)
{
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

    /* Check if the selected Display type was found and successfully opened */
    if (hDisplaySerial)
    {
        Display_printf(hDisplaySerial, 0, 0, "Waiting for SCE ADC reading...");
    }

    /* Check if the selected Display type was found and successfully opened */
    if (hDisplayLcd)
    {
        Display_printf(hDisplayLcd, 0, 0, "Waiting for ADC...");
    }

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, pinTable);
    if (!ledPinHandle)
    {
        System_abort("Error initializing board 3.3V domain pins\n");
    }

    /* Start the SCE ADC task with 1s sample period and reacting to change in ADC value. */
    SceAdc_init(0x00010000, NODE_ADCTASK_REPORTINTERVAL_FAST, NODE_ADCTASK_CHANGE_MASK);
    SceAdc_registerAdcCallback(adcCallback);
    SceAdc_start();

    /* setup timeout for fast report timeout */
    Clock_setTimeout(fastReportTimeoutClockHandle,
            NODE_ADCTASK_REPORTINTERVAL_FAST_DURIATION_MS * 1000 / Clock_tickPeriod);

    /* start fast report and timeout */
    Clock_start(fastReportTimeoutClockHandle);


    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
    if (!buttonPinHandle)
    {
        System_abort("Error initializing button pins\n");
    }

    /* Setup callback for button pins */
    if (PIN_registerIntCb(buttonPinHandle, &buttonCallback) != 0)
    {
        System_abort("Error registering button callback function");
    }

    while (1)
    {
        /* Wait for event */
        uint32_t events = Event_pend(nodeEventHandle, 0, NODE_EVENT_ALL, BIOS_WAIT_FOREVER);

        /* If new ADC value, send this data */
        if (events & NODE_EVENT_NEW_ADC_VALUE)
        {
            /* Toggle activity LED */
            PIN_setOutputValue(ledPinHandle, NODE_ACTIVITY_LED,!PIN_getOutputValue(NODE_ACTIVITY_LED));

            /* Send ADC value to concentrator */
            NodeRadioTask_sendAdcData(latestAdcValue);

            /* update display */
            updateLcd();
        }
        /* If new ADC value, send this data */
        if (events & NODE_EVENT_UPDATE_LCD)
        {
            /* update display */
            updateLcd();
        }
        /* If new OAD message */
        if (events & NODE_EVENT_OAD_REQ)
        {
            OADClient_processEvent(&events);
        }

    }
}

static void updateLcd(void)
{
    /* get node address if not already done */
    if (nodeAddress == 0)
    {
        nodeAddress = nodeRadioTask_getNodeAddr();
    }

    if(!oadInProgress)
    {
        /* print to LCD */
        Display_clear(hDisplayLcd);
        Display_printf(hDisplayLcd, 0, 0, "NodeID: 0x%02x", nodeAddress);
        Display_printf(hDisplayLcd, 1, 0, "ADC: %04d", latestAdcValue);
    }

    /* Print to UART clear screen, put cursor to beginning of terminal and print the header */
    Display_printf(hDisplaySerial, 0, 0, "\033[2J \033[0;0HNode ID: 0x%02x", nodeAddress);
    Display_printf(hDisplaySerial, 0, 0, "Node ADC Reading: %04d", latestAdcValue);

    if(oadInProgress)
    {
        Display_printf(hDisplaySerial, 0, 0, "OAD Block: %d of %d", oadBlock, oadTotalBlocks);
        Display_printf(hDisplaySerial, 0, 0, "OAD Block Retries: %d", oadRetries);
    }
    else if(oadStatus != -1)
    {
        switch( ((OADStorage_Status_t)oadStatus) )
        {
        case OADStorage_Status_Success:
            Display_printf(hDisplaySerial, 0, 0, "OAD: completed successfully");

            /* close Serial display for UART Driver */
            Display_close(hDisplaySerial);

            break;
        case OADStorage_CrcError:
            Display_printf(hDisplaySerial, 0, 0, "OAD: CRC failed");
            break;
        case OADStorage_Failed:
            Display_printf(hDisplaySerial, 0, 0, "OAD: aborted");
            break;
        default:
            Display_printf(hDisplaySerial, 0, 0, "OAD: error");
            break;
        }
    }
}

void adcCallback(uint16_t adcValue)
{
    /* Save latest value */
    latestAdcValue = adcValue;

    /* Post event */
    Event_post(nodeEventHandle, NODE_EVENT_NEW_ADC_VALUE);
}

/*
 *  ======== buttonCallback ========
 *  Pin interrupt Callback function board buttons configured in the pinTable.
 */
void buttonCallback(PIN_Handle handle, PIN_Id pinId)
{
    /* Debounce logic, only toggle if the button is still pushed (low) */
    CPUdelay(8000*50);

    if (PIN_getInputValue(CONFIG_PIN_BTN1) == 0)
    {
        //start fast report and timeout
        SceAdc_setReportInterval(NODE_ADCTASK_REPORTINTERVAL_FAST, NODE_ADCTASK_CHANGE_MASK);
        Clock_start(fastReportTimeoutClockHandle);
    }
}

void fastReportTimeoutCallback(UArg arg0)
{
    //stop fast report
    SceAdc_setReportInterval(NODE_ADCTASK_REPORTINTERVAL_SLOW, NODE_ADCTASK_CHANGE_MASK);
}
