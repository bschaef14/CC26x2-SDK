/** \mainpage Driver Overview
  *
  * \section section_drv_info Driver Information
  * This Sensor Controller Interface driver has been generated by the Texas Instruments Sensor Controller
  * Studio tool:
  * - <b>Project name</b>:     ADC Sample
  * - <b>Project file</b>:
  * - <b>Code prefix</b>:      -
  * - <b>Operating system</b>: TI-RTOS
  * - <b>Tool version</b>:     2.5.0.84
  * - <b>Tool patches</b>:     None
  * - <b>Target chip</b>:      CC1352P1F3, package QFN48 7x7 RGZ, revision E (2.1)
  * - <b>Created</b>:          2019-09-30 13:03:06.564
  *
  * No user-provided resource definitions were used to generate this driver.
  *
  * No user-provided procedure definitions were used to generate this driver.
  *
  * Do not edit the generated source code files other than temporarily for debug purposes. Any
  * modifications will be overwritten by the Sensor Controller Studio when generating new output.
  *
  * \section section_drv_modules Driver Modules
  * The driver is divided into three modules:
  * - \ref module_scif_generic_interface, providing the API for:
  *     - Initializing and uninitializing the driver
  *     - Task control (for starting, stopping and executing Sensor Controller tasks)
  *     - Task data exchange (for producing input data to and consume output data from Sensor Controller
  *       tasks)
  * - \ref module_scif_driver_setup, containing:
  *     - The AUX RAM image (Sensor Controller code and data)
  *     - I/O mapping information
  *     - Task data structure information
  *     - Driver setup data, to be used in the driver initialization
  *     - Project-specific functionality
  * - \ref module_scif_osal, for flexible OS support:
  *     - Interfaces with the selected operating system
  *
  * It is possible to use output from multiple Sensor Controller Studio projects in one application. Only
  * one driver setup may be active at a time, but it is possible to switch between these setups. When
  * using this option, there is one instance of the \ref module_scif_generic_interface and
  * \ref module_scif_osal modules, and multiple instances of the \ref module_scif_driver_setup module.
  * This requires that:
  * - The outputs must be generated using the same version of Sensor Controller Studio
  * - The outputs must use the same operating system
  * - The outputs must use different source code prefixes (inserted into all globals of the
  *   \ref module_scif_driver_setup)
  *
  *
  * \section section_project_info Project Description
  * No description entered
  *
  *
  * \subsection section_io_mapping I/O Mapping
  * Task I/O functions are mapped to the following pins:
  * - ADC Sample:
  *     - <b>A: ADC input</b>: DIO26
  *
  *
  * \section section_task_info Task Description(s)
  * This driver supports the following task(s):
  *
  *
  * \subsection section_task_desc_adc_sample ADC Sample
  * Samples the SFH5711 light sensor on the SmartRF06 Evaluation Board.
  *
  * The ADC value range (0-4095) is divided into a configurable number of bins, with run-time
  * configurable hysteresis and bin thresholds. The application must set the first threshold to 0, and
  * the last threshold to 4095.
  *
  * An ALERT interrupt is generated to the System CPU application when the ADC value changes from one bin
  * to another.
  *
  */




/** \addtogroup module_scif_driver_setup Driver Setup
  *
  * \section section_driver_setup_overview Overview
  *
  * This driver setup instance has been generated for:
  * - <b>Project name</b>:     ADC Sample
  * - <b>Code prefix</b>:      -
  *
  * The driver setup module contains the generated output from the Sensor Controller Studio project:
  * - Location of task control and scheduling data structures in AUX RAM
  * - The AUX RAM image, and the size the image
  * - Task data structure information (location, size and buffer count)
  * - I/O pin mapping translation table
  * - Task resource initialization and uninitialization functions
  * - Hooks for run-time logging
  *
  * @{
  */
#ifndef SCIF_H
#define SCIF_H

#include <stdint.h>
#include <stdbool.h>
#include "scif_framework.h"
#include "scif_osal_tirtos.h"


/// Target chip name
#define SCIF_TARGET_CHIP_NAME_CC1352P1F3
/// Target chip package
#define SCIF_TARGET_CHIP_PACKAGE_QFN48_7X7_RGZ

/// Number of tasks implemented by this driver
#define SCIF_TASK_COUNT 1

/// ADC Sample: Task ID
#define SCIF_ADC_SAMPLE_TASK_ID 0


/// ADC Sample: Number of ADC value bins
#define SCIF_ADC_SAMPLE_BIN_COUNT 5
/// ADC Sample: Number of bin thresholds
#define SCIF_ADC_SAMPLE_THRESHOLD_COUNT 6
/// ADC Sample I/O mapping: ADC input
#define SCIF_ADC_SAMPLE_DIO_A_ADC_INPUT 26


// All shared data structures in AUX RAM need to be packed
#pragma pack(push, 2)


/// ADC Sample: Task configuration structure
typedef struct {
    uint16_t changeMask;        ///<
    uint16_t minReportInterval; ///<
} SCIF_ADC_SAMPLE_CFG_T;


/// ADC Sample: Task output data structure
typedef struct {
    uint16_t adcValue; ///<
} SCIF_ADC_SAMPLE_OUTPUT_T;


/// ADC Sample: Task state structure
typedef struct {
    uint16_t oldAdcMaskedBits;       ///<
    uint16_t samplesSinceLastReport; ///< The number of samples since last report was sent
} SCIF_ADC_SAMPLE_STATE_T;


/// Sensor Controller task data (configuration, input buffer(s), output buffer(s) and internal state)
typedef struct {
    struct {
        SCIF_ADC_SAMPLE_CFG_T cfg;
        SCIF_ADC_SAMPLE_OUTPUT_T output;
        SCIF_ADC_SAMPLE_STATE_T state;
    } adcSample;
} SCIF_TASK_DATA_T;

/// Sensor Controller task generic control (located in AUX RAM)
#define scifTaskData    (*((volatile SCIF_TASK_DATA_T*) 0x400E015C))


// Initialized internal driver data, to be used in the call to \ref scifInit()
extern const SCIF_DATA_T scifDriverSetup;


// Restore previous struct packing setting
#pragma pack(pop)


// AUX I/O re-initialization functions
void scifReinitTaskIo(uint32_t bvTaskIds);


// RTC-based tick generation control
void scifStartRtcTicks(uint32_t tickStart, uint32_t tickPeriod);
void scifStartRtcTicksNow(uint32_t tickPeriod);
void scifStopRtcTicks(void);


#endif
//@}


// Generated on 2019-09-30 13:03:06.564
