/** \addtogroup module_scif_osal Operating System Abstraction Layer
  *
  * \section section_osal_overview Overview
  * The OSAL provides a set of functions for internal use by the SCIF driver, which allows it to
  * interface seamlessly with the real-time operating system or other run-time framework running on the
  * System CPU. These functions are listed below.
  *
  * The OSAL also implements OS-dependent functions that must be called by the application. These
  * functions are also listed below.
  *
  * The OSAL C source file can, but does not need to, be included in the application project.
  *
  *
  * \section section_osal_implementation Implementation "TI-RTOS"
  * This OSAL uses the following TI-RTOS mechanisms:
  * - \b HWI objects to implement the SCIF READY and ALERT callbacks
  * - Critical sections by disabling HWIs globally
  * - A \b Semaphore object to implement the \ref scifWaitOnNbl() timeout
  *
  * The SCIF driver performs I/O initialization internally without use of the \b PIN driver for TI-RTOS.
  * The System CPU application should not use the PIN driver to initialize or open any I/O pins used by
  * Sensor Controller. The \ref module_scif_driver_setup defines the DIO numbers of all mapped I/O
  * pins, allowing for compile-time checking of the I/O mapping.
  *
  * For CC13x0/CC26x0 devices, the TI Power driver enables and disables automatically access to the AUX
  * domain from the System CPU application. For CC13x2/CC26x2 devices, this is handled in hardware.
  *
  *
  * \section section_osal_app_func Application Functions
  * This SCIF OSAL implementation provides the following functions for use by the application:
  * - OSAL initialization:
  *     - \ref scifOsalInit()
  * - Callback registering:
  *     - \ref scifOsalRegisterCtrlReadyCallback()
  *     - \ref scifOsalRegisterTaskAlertCallback()
  *
  * \section section_osal_int_func Mandatory Internal Functions
  * The SCIF OSAL must provide the following functions for internal use by the driver:
  * - Task control READY interrupt:
  *     - \ref osalCtrlReadyIsr()
  *     - \ref osalRegisterCtrlReadyInt()
  *     - \ref osalUnregisterCtrlReadyInt()
  *     - \ref osalEnableCtrlReadyInt()
  *     - \ref osalDisableCtrlReadyInt()
  *     - \ref osalClearCtrlReadyInt()
  * - Task ALERT interrupt:
  *     - \ref osalTaskAlertIsr()
  *     - \ref osalRegisterTaskAlertInt()
  *     - \ref osalUnregisterTaskAlertInt()
  *     - \ref scifOsalEnableTaskAlertInt()
  *     - \ref scifOsalDisableTaskAlertInt()
  *     - \ref osalClearTaskAlertInt()
  * - Thread-safe operation:
  *     - \ref scifOsalEnterCriticalSection()
  *     - \ref scifOsalLeaveCriticalSection()
  *     - \ref osalLockCtrlTaskNbl()
  *     - \ref osalUnlockCtrlTaskNbl()
  * - Task control support:
  *     - \ref osalWaitOnCtrlReady()
  * - Application notifications:
  *     - \ref osalIndicateCtrlReady()
  *     - \ref osalIndicateTaskAlert()
  *
  * @{
  */
#ifndef SCIF_OSAL_TIRTOS_H
#define SCIF_OSAL_TIRTOS_H


// OSAL "TI-RTOS": Callback registering functions
void scifOsalRegisterCtrlReadyCallback(SCIF_VFPTR callback);
void scifOsalRegisterTaskAlertCallback(SCIF_VFPTR callback);

// OSAL "TI-RTOS": Functions for temporarily disabling ALERT interrupts/callbacks
void scifOsalEnableTaskAlertInt(void);
void scifOsalDisableTaskAlertInt(void);

// OSAL "TI-RTOS": Init function
void scifOsalInit(void);


#endif
//@}


// Generated on 2019-09-30 11:33:18.740
