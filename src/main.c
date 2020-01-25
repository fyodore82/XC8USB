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
#include <system_config.h>

#include <usb/usb.h>
#include <usb/usb_device_hid.h>
#include <usb_device_cdc2.h>

#include <app_device_custom_hid.h>
#include "app_device_cdc_basic.h"

#include "BEAN.h"

void UserInit ()
{
    InitBEANVars(0);
    CmdInPr = CMD_NOCMD;

    PORTD = 0;
    LATD = 0;
    PORTB = 0;
    LATD = 0;

    TRISA = 0;
    TRISB = 0b11011111;     // RB0, RB1 - inputs for I2C module
                            // RB4 - input for IOC
                            // RB5 - out
    TRISC = 0xFF; // All inputs
    TRISD = 0b11101111;     // RD2 = BEAN, input
                            // RD4 = BEAN output
    TRISE = 0;
    ADCON1 = 0b00001111;
    
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
//    Timer0ONOFF(1);
}

MAIN_RETURN main(void)
{
    // !!! IMPORTANT !!!
    // In File usb_hal_pic18.h in framework dir following string MUST BE changed to make USB interrupts low priority
//    #if defined(USB_INTERRUPT)
//        #define USBEnableInterrupts() {RCONbits.IPEN = 1; !!!IPR2bits.USBIP = SET TO 0;ADD INTCONbits.GIEL = 1; PIE2bits.USBIE = 1;INTCONbits.GIEH = 1;}
//    #else
//        #define USBEnableInterrupts()
//    #endif



    UserInit ();
    SYSTEM_Initialize(SYSTEM_STATE_USB_START);

    // USB init routines
    USBDeviceInit();
    USBDeviceAttach();
    char debugonce = 0;

//    CMDInPr = 
    while(1)
    {
        SYSTEM_Tasks();

        #if defined(USB_POLLING)
            // Interrupt or polling method.  If using polling, must call
            // this function periodically.  This function will take care
            // of processing and responding to SETUP transactions
            // (such as during the enumeration process when you first
            // plug in).  USB hosts require that USB devices should accept
            // and process SETUP packets in a timely fashion.  Therefore,
            // when using polling, this function should be called
            // regularly (such as once every 1.8ms or faster** [see
            // inline code comments in usb_device.c for explanation when
            // "or faster" applies])  In most cases, the USBDeviceTasks()
            // function does not take very long to execute (ex: <100
            // instruction cycles) before it returns.
            USBDeviceTasks();
        #endif

        /* If the USB device isn't configured yet, we can't really do anything
         * else since we don't have a host to talk to.  So jump back to the
         * top of the while loop. */
        if( USBGetDeviceState() < CONFIGURED_STATE )
        {
            /* Jump back to the top of the while loop. */
            continue;
        }

        /* If we are currently suspended, then we need to see if we need to
         * issue a remote wakeup.  In either case, we shouldn't process any
         * keyboard commands since we aren't currently communicating to the host
         * thus just continue back to the start of the while loop. */
        if( USBIsDeviceSuspended() == true )
        {
            /* Jump back to the top of the while loop. */
            continue;
        }

        //Application specific tasks
  //          SetTime(&min, &sec);
//        if (BEANTrSt == BEAN_NO_TR)

        {
            APP_DeviceCustomHIDTasks();
            APP_DeviceCDCBasicDemoTasks();
        }

        //if (Tmr2Sync)
        //{
 //          Tmr2Int();
        //    Tmr2Sync = 0;
        //}

//        ProcessTimerInterrupt();       // Process Timer Interrupt


    }//end while
}//end main


bool USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, uint16_t size)
{
    switch((int)event)
    {
        case EVENT_TRANSFER:
            break;

        case EVENT_SOF:
            /* We are using the SOF as a timer to time the LED indicator.  Call
             * the LED update function here. */
     //       APP_LEDUpdateUSBStatus();
            break;

        case EVENT_SUSPEND:
            /* Update the LED status for the suspend event. */
         //   APP_LEDUpdateUSBStatus();
            break;

        case EVENT_RESUME:
            /* Update the LED status for the resume event. */
        //    APP_LEDUpdateUSBStatus();
            break;

        case EVENT_CONFIGURED:
            /* When the device is configured, we can (re)initialize the demo
             * code. */
            APP_DeviceCDCBasicDemoInitialize();
            APP_DeviceCustomHIDInitialize();
            break;

        case EVENT_SET_DESCRIPTOR:
            break;

        case EVENT_EP0_REQUEST:
            /* We have received a non-standard USB request.  The HID driver
             * needs to check to see if the request was for it. */
            USBCheckCDCRequest();
            USBCheckHIDRequest();
            break;

        case EVENT_BUS_ERROR:
            break;

        case EVENT_TRANSFER_TERMINATED:
            break;

        default:
            break;
    }
    return true;
}

/*******************************************************************************
 End of File
*/

