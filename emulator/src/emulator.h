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

#ifndef __EMULATOR_H
#define __EMULATOR_H


typedef void Emulator_t;

/**
 * @brief Intitialize the Emulator and the control plane
 * This will setup the initial state machine of the emulator
 * @param[in] pProfilePath - File path containing the profile config YAML for the HAL driver
 * @param[in] pCPPort - Control plane listening port. 
 * @param[in] pCPUrl - Control plane Path URL e.g, "/hdmicec". Passing NULL is valid.
 * 
 *  The control plane url will look like "http://<ip>:<pCPPort>/pCPUrl"
 * 
 * @return Emulator_t* - Pointer to the Emulator.
 */
Emulator_t *Emulator_Initialize(const char* pProfilePath, unsigned short pCPPort, const char* pCPUrl);


/**
 * @brief Deintitialize the Emulator and the control plane. Free up all memory associated.
 * 
 * @param[in] pEmulator - Pointer to the Emulator.
 * 
 * @return None
 */
void Emulator_Deinitialize(Emulator_t *pEmulator);

#endif //__EMULATOR_H