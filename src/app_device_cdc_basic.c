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

/** INCLUDES *******************************************************/
#include <system.h>

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include <usb/usb.h>

//#include <app_led_usb_status.h>
#include <app_device_cdc_basic.h>
#include "app_device_custom_hid.h"
#include <usb_config.h>

/** VARIABLES ******************************************************/

static bool buttonPressed;
static char buttonMessage[] = "Button pressed.\r\n";
static uint8_t readBuffer1[CDC_DATA_OUT_EP_SIZE];
static uint8_t writeBuffer1[CDC_DATA_IN_EP_SIZE];
static uint8_t readBuffer2[CDC_DATA_OUT_EP_SIZE];
static uint8_t writeBuffer2[CDC_DATA_IN_EP_SIZE];

/*********************************************************************
* Function: void APP_DeviceCDCBasicDemoInitialize(void);
*
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCDCBasicDemoInitialize()
{
    CDCInitEP();

    
    line_coding1.bCharFormat = 0;
    line_coding1.bDataBits = 8;
    line_coding1.bParityType = 0;
    line_coding1.dwDTERate = 9600;

    line_coding2.bCharFormat = 0;
    line_coding2.bDataBits = 8;
    line_coding2.bParityType = 0;
    line_coding2.dwDTERate = 9600;
    
    buttonPressed = false;
}

/*********************************************************************
* Function: void APP_DeviceCDCBasicDemoTasks(void);
*
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_DeviceCDCBasicDemoInitialize() and APP_DeviceCDCBasicDemoStart() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_DeviceCDCBasicDemoTasks()
{
    /* If the user has pressed the button associated with this demo, then we
     * are going to send a "Button Pressed" message to the terminal.
     */

    /* Check to see if there is a transmission in progress, if there isn't, then
     * we can see about performing an echo response to data received.
     */
     uint8_t i;
    uint8_t numBytesRead;
    
    /* Check to see if there is a transmission in progress, if there isn't, then
     * we can see about performing an echo response to data received.
     */
    
    if( USBUSARTIsTxTrfReady1() == true)
    {


        numBytesRead = getsUSBUSART1(readBuffer1, sizeof(readBuffer1));

        // For every byte that was read... 
        for(i=0; i<numBytesRead; i++)
        {
            switch(readBuffer1[i])
            {
                // If we receive new line or line feed commands, just echo
                 // them direct.
                 
                case 0x0A:
                case 0x0D:
                    writeBuffer1[i] = readBuffer1[i];
                    break;

                // If we receive something else, then echo it plus one
                // so that if we receive 'a', we echo 'b' so that the
                // user knows that it isn't the echo enabled on their
                // terminal program.
                 
                default:
                    writeBuffer1[i] = readBuffer1[i] + 1;
                    break;
            }
        }

        if(numBytesRead > 0)
        {
            // After processing all of the received data, we need to send out
            // the "echo" data now.
            
            putUSBUSART1(writeBuffer1,numBytesRead);
        }
        
    }

    if( USBUSARTIsTxTrfReady2() == true)
    {
        numBytesRead = getsUSBUSART2(readBuffer2, sizeof(readBuffer2));
 //       if (numBytesRead > 0)
 //           USBDebug (readBuffer2, sizeof(readBuffer2));
        


        /* For every byte that was read... */
        for(i=0; i<numBytesRead; i++)
        {
            switch(readBuffer2[i])
            {
                /* If we receive new line or line feed commands, just echo
                 * them direct.
                 */
                case 0x0A:
                case 0x0D:
                    writeBuffer2[i] = readBuffer2[i];
                    break;

                /* If we receive something else, then echo it plus one
                 * so that if we receive 'a', we echo 'b' so that the
                 * user knows that it isn't the echo enabled on their
                 * terminal program.
                 */
                default:
                    writeBuffer2[i] = readBuffer2[i] + 2;
                    break;
            }
        }

        if(numBytesRead > 0)
        {
            /* After processing all of the received data, we need to send out
             * the "echo" data now.
             */
            putUSBUSART2(writeBuffer2,numBytesRead);
            //        if (numBytesRead > 0)
            //USBDebug (writeBuffer2, numBytesRead);
        }
        
    }
    CDCTxService1();
    CDCTxService2();
}