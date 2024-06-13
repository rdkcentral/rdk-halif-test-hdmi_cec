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

#ifndef __VCOMPONENT_HDMI_CEC_H
#define __VCOMPONENT_HDMI_CEC_H

#define VC_LOG(format, ...)                 UT_logPrefix(__FILE__, __LINE__, UT_LOG_ASCII_YELLOW"vcHdmiCec[LOG]   "UT_LOG_ASCII_NC, format, ## __VA_ARGS__)
#define VC_LOG_ERROR(format, ...)           UT_logPrefix(__FILE__, __LINE__, UT_LOG_ASCII_RED"vcHdmiCec[ERROR] "UT_LOG_ASCII_NC, format, ## __VA_ARGS__)

/**! Status codes for the HDMI CEC Virtual Component */
typedef enum
{
    VC_HDMICEC_STATUS_SUCCESS = 0,         /**!< Operation successful. */
    VC_HDMICEC_STATUS_NOT_INITIALIZED,     /**!< Virtual Component not initialized. */
    VC_HDMICEC_STATUS_ALREADY_INITIALIZED, /**!< Invalid parameter passed. */
    VC_HDMICEC_STATUS_NOT_OPENED,          /**!< Not Opened. */
    VC_HDMICEC_STATUS_ALREADY_OPENED,      /**!< ALready Opened. */
    VC_HDMICEC_STATUS_INVALID_PARAM,       /**!< Invalid parameter passed. */
    VC_HDMICEC_STATUS_INVALID_HANDLE,      /**!< Invalid virtual component handle. */
    VC_HDMICEC_STATUS_PROFILE_READ_ERROR,  /**!< Error reading the profile path from file */
    VC_HDMICEC_STATUS_OUT_OF_MEMORY,       /**!< Out f memory. */
    VC_HDMICEC_STATUS_MAX                  /**!< Out of range marker (not a valid status). */
} vComponent_HdmiCec_Status_t;

typedef void vComponent_HdmiCec_t;

/**
 * @brief Intitialize the HDMI CEC Virtual Component and the control plane
 * This will setup the initial state machine of the Virtual Component
 * @param[out] handle - Output variable that holds the pointer to VC instance.
 * @param[in] pProfilePath - File path containing the profile config YAML for the HAL driver
 * 
 * @return Status of Virtual Component Init (vComponent_HdmiCec_Status)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully Created and initialized.
 * @retval VC_HDMICEC_STATUS_OUT_OF_MEMORY - Memory allocation error
 * @retval VC_HDMICEC_STATUS_ALREADY_INITIALIZED - Initialize called again.
 */
vComponent_HdmiCec_t* vComponent_HdmiCec_Initialize( void );

/**
 * @brief Opens the HDMI CEC Virtual Component and sets up the profile.
 * This will setup the initial state machine of the Virtual Component
 * @param[out] pVCHdmiCec -  pointer to VC instance.
 * @param[in] pProfilePath - File path containing the profile config YAML for the HAL driver
 * 
 * @return Status of Virtual Component Init (vComponent_HdmiCec_Status)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully Created and initialized.
 * @retval VC_HDMICEC_STATUS_INVALID_PARAM - One or more parameters are invalid.
 * @retval VC_HDMICEC_STATUS_OUT_OF_MEMORY - Memory allocation error
 * @retval VC_HDMICEC_STATUS_ALREADY_INITIALIZED - Initialize called again.
 * @retval VC_HDMICEC_STATUS_ALREADY_OPENED - Open called again.
 * @retval VC_HDMICEC_STATUS_PROFILE_READ_ERROR - Error reading the profile path from file
 */
vComponent_HdmiCec_Status_t vComponent_HdmiCec_Open( vComponent_HdmiCec_t* pVCHdmiCec, char* pProfilePath, bool enableCPMsgs );

/**
 * @brief Closes the HDMI CEC Virtual Component and the state machine asociated with it.
 * @param[out] pVCHdmiCec -  pointer to VC instance.
 * 
 * @return Status of Virtual Component Init (vComponent_HdmiCec_Status)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully Created and initialized.
 * @retval VC_HDMICEC_STATUS_INVALID_PARAM - One or more parameters are invalid.
 * @retval VC_HDMICEC_STATUS_OUT_OF_MEMORY - Memory allocation error
 * @retval VC_HDMICEC_STATUS_ALREADY_INITIALIZED - Initialize called again.
 * @retval VC_HDMICEC_STATUS_NOT_OPENED - Close called before Open.
 * @retval VC_HDMICEC_STATUS_PROFILE_READ_ERROR - Error reading the profile path from file
 */
vComponent_HdmiCec_Status_t vComponent_HdmiCec_Close( vComponent_HdmiCec_t* pVCHdmiCec );

/**
 * @brief Deintitialize the Virtual Component. Free up all memory associated.
 * 
 * @param[in] pvComponent - Pointer to the vComponent instance.
 * 
 * @return Status of Virtual Component Deinit (vComponent_HdmiCec_Status)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully deinitialized.
 * @retval VC_HDMICEC_STATUS_INVALID_HANDLE - Provided vComponent_HdmiCec_t* is not a valid handle.
 */
vComponent_HdmiCec_Status_t vComponent_HdmiCec_Deinitialize( vComponent_HdmiCec_t *pvComponent );




#endif //__VCOMPONENT_HDMI_CEC_H