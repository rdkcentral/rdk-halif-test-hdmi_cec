### Test 1

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_AddAndGetLogicalAddress`|
|Description|Establish my logical address (through HAL APIs) as valid, then retrieve it to verify correct functionality.|
|Test Group|Module : 02|
|Test Case ID|001|
|Priority|High|

**Pre-Conditions :** None

**Dependencies :** None

**User Interaction :** If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Open HDMI CEC HAL using HdmiCecOpen | handle = valid buffer | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Add a valid Logical Address using HdmiCecAddLogicalAddress | handle = valid handle, logicalAddress = 0x0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Get Logical Address using HdmiCecGetLogicalAddress and verify against added address | handle = valid handle, getLogicalAddress = valid buffer | HDMI_CEC_IO_SUCCESS, getLogicalAddress = logicalAddress | Should be successful |
| 04 | Close HDMI CEC HAL using HdmiCecClose | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TB
    A[Call HdmiCecOpen] -->|success| B[Call HdmiCecAddLogicalAddress]
    A -->|fail| E[Test case fail]
    B -->|success| C[Call HdmiCecGetLogicalAddress]
    B -->|fail| F[Test case fail]
    C -->|success| D[Verify logical address]
    C -->|fail| G[Test case fail]
    D -->|success| H[Call HdmiCecClose]
    D -->|fail| I[Test case fail]
    H -->|success| J[End of test case]
    H -->|fail| K[Test case fail]
```


### Test 2

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_ValidateLogicalAddress`|
|Description|If the logical address is set to anything other than 0 (Allocated TV address) or 14 (Wild card address), it should result in a failure of a TV device.|
|Test Group|Module : 02|
|Test Case ID|002|
|Priority|High|

**Pre-Conditions :** None

**Dependencies :** None

**User Interaction :** If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Open the HDMI CEC driver using HdmiCecOpen | handle = valid buffer | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Add an invalid logical address to the HDMI CEC driver using HdmiCecAddLogicalAddress | handle = valid handle, logicalAddresses = 0x1 | HDMI_CEC_IO_INVALID_ARGUMENT | Should fail |
| 03 | Close the HDMI CEC driver using HdmiCecClose | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TD
A[HdmiCecOpen] -->|Success| B{Handle Stored?}
B -->|Yes| C[Invoke HdmiCecAddLogicalAddress with invalid address]
B -->|No| BF[Test case fail]
C -->|HDMI_CEC_IO_INVALID_ARGUMENT| D{Is Result As Expected?}
C -->|Else| CF[Test case fail]
D -->|Yes| E[HdmiCecClose]
D -->|No| DF[Test case fail]
E -->|HDMI_CEC_IO_SUCCESS| F{Is Result As Expected?}
E -->|Else| EF[Test case fail]
F -->|Yes| G[Test Case Success]
F -->|No| FF[Test case fail]
```


### Test 3

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_checkLogicalAddress`|
|Description|Invoke the HAL API to delete my logical address and verify that it is removed successfully.|
|Test Group|Module : 02|
|Test Case ID|003|
|Priority|High|

**Pre-Conditions :** None

**Dependencies :** None

**User Interaction :** If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Open HDMI CEC HAL using HdmiCecOpen | handle = valid buffer | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Add Logical Address using HdmiCecAddLogicalAddress | handle = valid handle, logicalAddress = 0x0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Remove Logical Address using HdmiCecRemoveLogicalAddress | handle = valid handle, logicalAddress = 0x0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 04 | Get Logical Address using HdmiCecGetLogicalAddress | handle = valid handle, logicalAddress = valid buffer | HDMI_CEC_IO_SUCCESS and logicalAddress = 0x0F | Should fail |
| 05 | Close HDMI CEC HAL using HdmiCecClose | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TB
    A[Invoke HdmiCecOpen] -->|Success| B[Invoke HdmiCecAddLogicalAddress]
    A -->|Failure| AFail[Test case fail]
    B -->|Success| C[Verify HdmiCecAddLogicalAddress Success]
    B -->|Failure| BFail[Test case fail]
    C -->|Success| D[Invoke HdmiCecRemoveLogicalAddress]
    C -->|Failure| CFail[Test case fail]
    D -->|Success| E[Verify HdmiCecRemoveLogicalAddress Success]
    D -->|Failure| DFail[Test case fail]
    E -->|Success| F[Invoke HdmiCecGetLogicalAddress]
    E -->|Failure| EFail[Test case fail]
    F -->|Success| G[Verify successful execution of HdmiCecRemoveLogicalAddress]
    F -->|Failure| FFail[Test case fail]
    G -->|Success| H[Invoke HdmiCecClose]
    G -->|Failure| GFail[Test case fail]
    H -->|Success| I[Test case success]
    H -->|Failure| HFail[Test case fail]
```


### Test 4

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_ValidateHALTransmissionAfterAddressRemoval`|
|Description|After deleting the logical address, the test tries to send a broadcast command. It should fail to send during HAL Transmission call.|
|Test Group|Module : 02|
|Test Case ID|004|
|Priority|High|

**Pre-Conditions :** None

**Dependencies :** None

**User Interaction :**
If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Open HDMI CEC with HdmiCecOpen | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Set Rx Callback with HdmiCecSetRxCallback | handle = valid handle, callback = NULL, user_data = NULL | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Add Logical Address with HdmiCecAddLogicalAddress | handle = valid handle, logicalAddresses = 0x0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 04 | Remove Logical Address with HdmiCecRemoveLogicalAddress | handle = valid handle, logicalAddresses = 0x0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 05 | Transmit with HdmiCecTx after removing logical address | handle = valid handle, buf = {0x3F, CEC_GET_CEC_VERSION}, len = 1, result = valid result buffer | return status != HDMI_CEC_IO_SUCCESS | Should be successful |
| 06 | Close HDMI CEC with HdmiCecClose | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TD
  A[HdmiCecOpen API Call]
  A -->|Success| B[HdmiCecSetRxCallback API Call]
  A -->|Failure| A1[Test case fail]
  B -->|Success| C[HdmiCecAddLogicalAddress API Call]
  B -->|Failure| B1[Test case fail]
  C -->|Success| D[HdmiCecRemoveLogicalAddress API Call]
  C -->|Failure| C1[Test case fail]
  D -->|Success| E[HdmiCecTx API Call]
  D -->|Failure| D1[Test case fail]
  E -->|Expected Failure| G[Test case pass] 
  E -->|Unexpected Success| E1[Test case fail]
  G --> H[HdmiCecClose API Call]
  E1 --> H 
```


### Test 5

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_VerifyPhysicalAddressAllocation`|
|Description|Verify the valid physical address allocated through the HAL function.|
|Test Group|Module : 02|
|Test Case ID|005|
|Priority|High|

**Pre-Conditions :** None

**Dependencies :** None

**User Interaction :** If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Open the HDMI CEC driver using HdmiCecOpen | handle = valid buffer | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Get the physical address using HdmiCecGetPhysicalAddress | handle = valid handle, physicalAddress = valid buffer | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Check if the physical address is within the valid range | physicalAddress = obtained value | physicalAddress should be between 0 and max_address | Should be successful |
| 04 | Close the HDMI CEC driver using HdmiCecClose | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TB
    Initialise("Call HdmiCecOpen") -->|HDMI_CEC_IO_SUCCESS| CheckHdmiCecOpen("Check return status")
    CheckHdmiCecOpen -- |HDMI_CEC_IO_SUCCESS| --> CallHdmiCecGetPhysicalAddress("Call HdmiCecGetPhysicalAddress")
    CheckHdmiCecOpen -- |Failure| --> TestFailOpen("Test case fail: HdmiCecOpen")
    CallHdmiCecGetPhysicalAddress -->|HDMI_CEC_IO_SUCCESS| CheckHdmiCecGetPhysicalAddress("Check return status")
    CheckHdmiCecGetPhysicalAddress -- |HDMI_CEC_IO_SUCCESS| --> VerifyPhysicalAddressRange("Verify physical address range")  
    CheckHdmiCecGetPhysicalAddress -- |Failure| --> TestFailGetPhysicalAddress("Test case fail: HdmiCecGetPhysicalAddress")
    VerifyPhysicalAddressRange -- |Valid| --> CallHdmiCecClose("Call HdmiCecClose")  
    VerifyPhysicalAddressRange -- |Invalid| --> TestFailPhysicalAddressRange("Test case fail: PhysicalAddressRange")
    CallHdmiCecClose -->|HDMI_CEC_IO_SUCCESS| CheckHdmiCecClose("Check return status")
    CheckHdmiCecClose -- |HDMI_CEC_IO_SUCCESS| --> TestPass("Test case pass")
    CheckHdmiCecClose -- |Failure| --> TestFailClose("Test case fail: HdmiCecClose")
    TestFailPhysicalAddressRange --> CallHdmiCecClose
    TestFailGetPhysicalAddress -->  CallHdmiCecClose
```


