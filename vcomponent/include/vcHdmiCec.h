/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2023 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef __VCHDMICEC_H
#define __VCHDMICEC_H

#include "ut_log.h"

#define VC_LOG(format, ...)                 UT_logPrefix(__FILE__, __LINE__, UT_LOG_ASCII_YELLOW"vcHdmiCec[LOG]   "UT_LOG_ASCII_NC, format, ## __VA_ARGS__)
#define VC_LOG_ERROR(format, ...)           UT_logPrefix(__FILE__, __LINE__, UT_LOG_ASCII_RED"vcHdmiCec[ERROR] "UT_LOG_ASCII_NC, format, ## __VA_ARGS__)

/**! Status codes for the HDMI CEC Virtual Component */
typedef enum
{
    VC_HDMICEC_STATUS_SUCCESS = 0,         /**!< Operation successful. */
    VC_HDMICEC_STATUS_ALREADY_INITIALIZED, /**!< Invalid parameter passed. */
    VC_HDMICEC_STATUS_NOT_OPENED,          /**!< Not Opened. */
    VC_HDMICEC_STATUS_INVALID_HANDLE,      /**!< Invalid virtual component handle. */
    VC_HDMICEC_STATUS_INVALID_PARAM,       /**!< Invalid parameter. */
    VC_HDMICEC_STATUS_PROFILE_READ_ERROR,  /**!< Error reading the profile path from file */
    VC_HDMICEC_STATUS_OUT_OF_MEMORY,       /**!< Out f memory. */
    VC_HDMICEC_STATUS_MAX                  /**!< Out of range marker (not a valid status). */
} vcHdmiCec_Status_t;

typedef void vcHdmiCec_t;

/**
 * @brief Intitialize the HDMI CEC Virtual Component and the control plane
 * This will setup the initial state machine of the Virtual Component
 * @param[out] handle - Output variable that holds the pointer to VC instance.
 * @param[in] pProfilePath - File path containing the profile config YAML for the HAL driver
 * 
 * @return Status of Virtual Component Init (vcHdmiCec_Status_t)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully Created and initialized.
 * @retval VC_HDMICEC_STATUS_OUT_OF_MEMORY - Memory allocation error
 * @retval VC_HDMICEC_STATUS_ALREADY_INITIALIZED - Initialize called again.
 */
vcHdmiCec_t* vcHdmiCec_Initialize( void );

/**
 * @brief Opens the HDMI CEC Virtual Component and sets up the profile.
 * This will setup the initial state machine of the Virtual Component
 * @param[out] pVCHdmiCec -  pointer to VC instance.
 * @param[in] pProfilePath - File path containing the profile config YAML for the HAL driver
 * 
 * @return Status of Virtual Component Init (vcHdmiCec_Status_t)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully Created and initialized.
 * @retval VC_HDMICEC_STATUS_INVALID_HANDLE - Invalid vcHdmiCec_t* handle
 * @retval VC_HDMICEC_STATUS_INVALID_PARAM - pProfilePath is NULL
 * @retval VC_HDMICEC_STATUS_OUT_OF_MEMORY - Memory allocation error
 * @retval VC_HDMICEC_STATUS_PROFILE_READ_ERROR - Error reading the profile path from file
 */
vcHdmiCec_Status_t vcHdmiCec_Open( vcHdmiCec_t* pVCHdmiCec, char* pProfilePath, bool enableCPMsgs );

/**
 * @brief Closes the HDMI CEC Virtual Component and the state machine asociated with it.
 * @param[out] pVCHdmiCec -  pointer to VC instance.
 * 
 * @return Status of Virtual Component Init (vcHdmiCec_Status_t)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully Created and initialized.
 * @retval VC_HDMICEC_STATUS_INVALID_HANDLE - Invalid vcHdmiCec_t* handle
 * @retval VC_HDMICEC_STATUS_INVALID_PARAM - One or more parameters are invalid.
 * @retval VC_HDMICEC_STATUS_NOT_OPENED - Close called before Open.
 */
vcHdmiCec_Status_t vcHdmiCec_Close( vcHdmiCec_t* pVCHdmiCec );

/**
 * @brief Deintitialize the Virtual Component. Free up all memory associated.
 * 
 * @param[in] pvComponent - Pointer to the vComponent instance.
 * 
 * @return Status of Virtual Component Deinit (vcHdmiCec_Status_t)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully deinitialized.
 * @retval VC_HDMICEC_STATUS_INVALID_HANDLE - Provided vcHdmiCec_t* is not a valid handle.
 */
vcHdmiCec_Status_t vcHdmiCec_Deinitialize( vcHdmiCec_t *pvComponent );




#endif //__VCOMPONENT_HDMI_CEC_H
