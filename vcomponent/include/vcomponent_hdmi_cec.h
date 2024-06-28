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


/**! Status codes for the HDMI CEC Virtual Component */
typedef enum
{
    VC_HDMICEC_STATUS_SUCCESS = 0,         /**!< Operation successful. */
    VC_HDMICEC_STATUS_NOT_INITIALIZED,     /**!< Virtual Component not initialized. */
    VC_HDMICEC_STATUS_ALREADY_INITIALIZED, /**!< Invalid parameter passed. */
    VC_HDMICEC_STATUS_INVALID_PARAM,       /**!< Invalid parameter passed. */
    VC_HDMICEC_STATUS_INVALID_HANDLE,      /**!< Invalid virtual component handle. */
    VC_HDMICEC_STATUS_PROFILE_READ_ERROR,  /**!< Error reading the profile path from file */
    VC_HDMICEC_STATUS_OUT_OF_MEMORY,       /**!< Out f memory. */
    VC_HDMICEC_STATUS_MAX                  /**!< Out of range marker (not a valid status). */
} vComponent_HdmiCec_Status;

typedef void vComponent_HdmiCec_t;

/**
 * @brief Intitialize the HDMI CEC Virtual Component and the control plane
 * This will setup the initial state machine of the Virtual Component
 * @param[in] pProfilePath - File path containing the profile config YAML for the HAL driver
 * @param[in] cpPort - Control plane listening port. 
 * @param[in] pCPUrl - Control plane Path URL e.g, "/hdmicec". Passing NULL is valid.
 * @param[out] handle - Output variable that holds the pointer to VC instane.
 * 
 *  The control plane url will look like "http://<ip>:<cpPort>/pCPUrl"
 * 
 * @return Status of Virtual Component Init (vComponent_HdmiCec_Status)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully Created and initialized.
 * @retval VC_HDMICEC_STATUS_INVALID_PARAM - One or more parameters are invalid.
 * @retval VC_HDMICEC_STATUS_OUT_OF_MEMORY - Memory allocation error
 * @retval VC_HDMICEC_STATUS_ALREADY_INITIALIZED - Initialize called again.
 * @retval VC_HDMICEC_STATUS_PROFILE_READ_ERROR - Error reading the profile path from file
 */
vComponent_HdmiCec_Status vComponent_HdmiCec_Initialize( char* pProfilePath, unsigned short cpPort, char* pCPUrl, vComponent_HdmiCec_t** handle );


/**
 * @brief Deintitialize the Virtual Component and the control plane. Free up all memory associated.
 * 
 * @param[in] pvComponent - Pointer to the vComponent instance.
 * 
 * @return Status of Virtual Component Deinit (vComponent_HdmiCec_Status)
 * @retval VC_HDMICEC_STATUS_SUCCESS - Virtual Component successfully deinitialized.
 * @retval VC_HDMICEC_STATUS_INVALID_HANDLE - Provided vComponent_HdmiCec_t* is not a valid handle.
 */
vComponent_HdmiCec_Status vComponent_HdmiCec_Deinitialize( vComponent_HdmiCec_t *pvComponent );


#endif //__VCOMPONENT_HDMI_CEC_H