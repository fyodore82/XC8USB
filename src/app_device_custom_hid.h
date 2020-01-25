/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

// ReceivedDataBuffer[1] - actual command, one from the list below

#define CMD_NOCMD   0
#define CMD_BEAN    1
#define CMD_BEAN_DIG    2   // Get BEAN data using 10kbps (or another) bit rate
                            // ReceivedDataBuffer[2] - value for T0CON register
                            // ReceivedDataBuffer[3] - value for TMR0L
#define CMD_BEAN_CODE   3
#define CMD_CODE_SORT   4
#define CMD_SEND_BEAN_CMD   5

#define CMD_I2C_READMEM 0x10
#define CMD_I2C_TESTINOUT   0x11    // Test Relays and Inputs
#define CMD_I2C_DOCMD   0x12        // Do Any i2c Cmd
#define CMD_COM_DEBUG   0x25

unsigned char CmdInPr;  // What command is in progress (we send data continiously)
unsigned char BEANNumToSend, BEANDelayBtwCmd, BEANFirstByteInSnd;
unsigned char Tmr1Repeat;

/*********************************************************************
* Function: void APP_DeviceCustomHIDInitialize(void);
*
* Overview: Initializes the Custom HID demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCustomHIDInitialize();

/*********************************************************************
* Function: void APP_DeviceCustomHIDStart(void);
*
* Overview: Starts running the Custom HID demo.
*
* PreCondition: The device should be configured into the configuration
*   that contains the custome HID interface.  The APP_DeviceCustomHIDInitialize()
*   function should also have been called before calling this function.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCustomHIDStart();

/*********************************************************************
* Function: void APP_DeviceCustomHIDTasks(void);
*
* Overview: Keeps the Custom HID demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceCustomHIDInitialize() and APP_DeviceCustomHIDStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCustomHIDTasks();

void Timer1ONOFF(unsigned char, unsigned char, unsigned char);
void Tmr1Int();
void USBDebug (unsigned char*, unsigned char);

