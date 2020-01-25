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
#include <usb/usb.h>
#include <usb/usb_device_hid.h>

#include <string.h>

#include <system.h>
#include "BEAN.h"
#include "app_device_custom_hid.h"
#include "../../i2c/i2c_my.h"
#include "../../i2c/i2cCmds.h"

/** VARIABLES ******************************************************/
/* Some processors have a limited range of RAM addresses where the USB module
 * is able to access.  The following section is for those devices.  This section
 * assigns the buffers that need to be used by the USB module into those
 * specific areas.
 */
#if defined(FIXED_ADDRESS_MEMORY)
    #if defined(COMPILER_MPLAB_C18)
        #pragma udata HID_CUSTOM_OUT_DATA_BUFFER = HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS
        unsigned char ReceivedDataBuffer[64];
        #pragma udata HID_CUSTOM_IN_DATA_BUFFER = HID_CUSTOM_IN_DATA_BUFFER_ADDRESS
        unsigned char ToSendDataBuffer[64];
        #pragma udata

    #else defined(__XC8)
        unsigned char ReceivedDataBuffer[64] @ HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS;
        unsigned char ToSendDataBuffer[64] @ HID_CUSTOM_IN_DATA_BUFFER_ADDRESS;
    #endif
#else
    unsigned char ReceivedDataBuffer[64];
    unsigned char ToSendDataBuffer[64];
#endif

volatile USB_HANDLE USBOutHandle;    
volatile USB_HANDLE USBInHandle;

unsigned char ComDebugBuff[20];

/** FUNCTIONS ******************************************************/

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
void APP_DeviceCustomHIDInitialize()
{
    //initialize the variable holding the handle for the last
    // transmission
    USBInHandle = 0;

    //enable the HID endpoint
    USBEnableEndpoint(CUSTOM_DEVICE_HID_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = (volatile USB_HANDLE)HIDRxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&ReceivedDataBuffer,64);
}

void USBDebug (unsigned char *b, unsigned char n)
{
//    if(!HIDTxHandleBusy(USBInHandle))
    {
        //ToSendDataBuffer[0] = 0x25;				//Echo back to the host PC the command we are fulfilling in the first byte.  In this case, the Get Pushbutton State command.
    				//Prepare the USB module to send the data packet to the host
        ComDebugBuff[0] = 0x25;
        memcpy (&ComDebugBuff[1], b, (n > 20? 20:n));
        CmdInPr = CMD_COM_DEBUG;
        //USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
    }
}

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
void APP_DeviceCustomHIDTasks()
{   
    //Check if we have received an OUT data packet from the host
    if(HIDRxHandleBusy(USBOutHandle) == false)
    {   
        //We just received a packet of data from the USB host.
        //Check the first uint8_t of the packet to see what command the host
        //application software wants us to fulfill.
        switch(ReceivedDataBuffer[0])				//Look at the data the host sent, to see what kind of application specific command it sent.
        {
            case 0x80:  //Go to bootloader
                asm ("goto 0x001C");
                break;
            case 0x25:
                USBDebug ("COM Debug Started!", 19);
                //if(!HIDTxHandleBusy(USBInHandle))
                break;
             case 0x23:
              //  OutRet(ReceivedDataBuffer[1], &ReceivedDataBuffer[2], 0);
                if(!HIDTxHandleBusy(USBInHandle))
                {
                    
                    ToSendDataBuffer[0] = 0x24;				//Echo back to the host PC the command we are fulfilling in the first byte.  In this case, the Get Pushbutton State command.
    				//Prepare the USB module to send the data packet to the host
//                    pBEANSndBuff[0] =  ReceivedDataBuffer[1];
//                    BEANbitPos =  ReceivedDataBuffer[2];
//                    DecrBitPos (ReceivedDataBuffer[3]);
                    ToSendDataBuffer[1] = 22;//pBEANSndBuff[0];
                    ToSendDataBuffer[2] = 33;//BEANbitPos;
//USBDebug (ToSendDataBuffer, 3);
                    //BEAN_OUT = !BEAN_OUT;
                    //USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                }
                break;
            case 0x21:
                if (CmdInPr != ReceivedDataBuffer[1] || ReceivedDataBuffer[1] == CMD_NOCMD)       // New cmd received. Stop Firstly
                {
                    // Everithing OFF
                //    InitBEANVars(0);
                    Timer0ONOFF(0, TMR0_NOTRESET);
                    Timer1ONOFF(0, 0, 0);
                    IOCONOFF (0);
                      
                    BEANNumToSend = 0;
                }

                CmdInPr = ReceivedDataBuffer[1];


                switch (CmdInPr)
                {
                    case CMD_NOCMD:
                        break;

                    case CMD_BEAN_CODE:
                    case CMD_CODE_SORT:
                        InitBEANVars(0);
                        IOCONOFF(1); // With IOC
                        break;
                    case CMD_SEND_BEAN_CMD:
                        InitBEANVars(0);
                        memcpy(BEANCmdToSend, &ReceivedDataBuffer[4], (ReceivedDataBuffer[4] & 0x0F) + 1);

                        BEANFirstByteInSnd = ReceivedDataBuffer[4];
                        BEANNumToSend = ReceivedDataBuffer[2] - 1;
                        BEANDelayBtwCmd = ReceivedDataBuffer[3];

                        // To start sending we should firstly verify that no transmission in progress
                        IOCONOFF(1); // With IOC
                        Timer0ONOFF(1, TMR0_NOTRESET);
                        break;
                    case CMD_I2C_TESTINOUT:
                        if (!HIDTxHandleBusy(USBInHandle))
                        {
                            i2c_init(0);
                            ToSendDataBuffer[0] = CmdInPr;
                            ToSendDataBuffer[1] = ReceivedDataBuffer[2];
                            
                            unsigned char cmd[4];
                            switch (ReceivedDataBuffer[2])
                            {
                        //        ToSendDataBuffer[2] = ReceivedDataBuffer[2];
                                case 0x0:
                                    i2cTestInOut (&ReceivedDataBuffer[1], &ToSendDataBuffer[2]);
                                    break;
                                case 0x1:   // Set i2cHoldTmr
                                    cmd[0] = 0b01000010;
                                    cmd[1] = _I2C_HOLDTMR;
                                    cmd[2] = ReceivedDataBuffer[3];
                                    ToSendDataBuffer[2] = TXCmdToPIC16 (cmd, 3, 10);
                                    break;
                                case 0x2:
                                    cmd[0] = 0b01000010;
                                    cmd[1] = ReceivedDataBuffer[3];
                                    cmd[2] = ReceivedDataBuffer[4];
                                    ToSendDataBuffer[2] = TXCmdToPIC16 (cmd, 3, 10);
                                    break;
                            }
                            USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                            CmdInPr = CMD_NOCMD;
                        }
                        break;

                    default:
                        break;
                }
                break;
        }
        //Re-arm the OUT endpoint, so we can receive the next OUT data packet 
        //that the host may try to send us.
        USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP, (uint8_t*)&ReceivedDataBuffer, 64);

    }

    switch(CmdInPr)
    {
        case CMD_COM_DEBUG:
            if(!HIDTxHandleBusy(USBInHandle))
            {
                memcpy (ToSendDataBuffer, ComDebugBuff, 20);
                    USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                    CmdInPr = CMD_NOCMD;
                }
            break;
        case CMD_I2C_READMEM:
            if (!HIDTxHandleBusy(USBInHandle))
            {
                            i2c_init(0);
                            ToSendDataBuffer[0] = CmdInPr;
                            ToSendDataBuffer[1] = (ReceivedDataBuffer[4] > 32 ? 32:ReceivedDataBuffer[4]);
                            ToSendDataBuffer[2] = RXNbytes (ReceivedDataBuffer[2], ReceivedDataBuffer[3], ToSendDataBuffer[1], &ToSendDataBuffer[3]);
                            USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                            CmdInPr = CMD_NOCMD;
            }
            break;
        case CMD_I2C_DOCMD:
            if (!HIDTxHandleBusy(USBInHandle))
            {
                            i2c_init(0);
                            ToSendDataBuffer[0] = CmdInPr;
                            i2cDoCmd (&ReceivedDataBuffer[2], &ToSendDataBuffer[1], 20);
                            USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                            CmdInPr = CMD_NOCMD;
                        }
                        break;

        case CMD_SEND_BEAN_CMD:
            if (BEANCmdToSend[0] == 0 && BEANNumToSend > 0 && T1CONbits.TMR1ON == 0 && Tmr1Repeat == 0)    // We've not set it yet
            {
//                pBEANSndBuff[pBEANSndBuff[0] + 1] = BEANNumToSend;
//                pBEANSndBuff[0]++;

                if (BEANDelayBtwCmd > 3)
                    Tmr1Repeat = (BEANDelayBtwCmd / 4);

                switch (BEANDelayBtwCmd - (Tmr1Repeat * 4))
                {
                    case 0:
                        Timer1ONOFF (1, 0x15, 0x9F);    // 0xFFFF - 60000
                        break;
                    case 1:
                        Timer1ONOFF (1, 0xc5, 0x67);    // 15000
                        break;
                    case 2:
                        Timer1ONOFF (1, 0x8a, 0xcf);    // 30000
                        break;
                    case 3:
                        Timer1ONOFF (1, 0x50, 0x37);    // 45000
                        break;
                }
            }
            //break; // DON'T break, as we can send something to host (if we have something)
     /*    if(BEANCmdToSend[0] == 0 && BEANNumToSend && BEANTrSt == BEAN_NO_TR && (!HIDTxHandleBusy(USBInHandle)))
            {
                ToSendDataBuffer[0] = CmdInPr;
                ToSendDataBuffer[1] = 20;
                ToSendDataBuffer[2] = BEANSndBuff1[0];
                ToSendDataBuffer[3] = BEANSndBuff2[0];
                ToSendDataBuffer[4] = BEANSndBuff3[0];
                ToSendDataBuffer[5] = BEANSndBuff4[0];
                ToSendDataBuffer[6] = BEANSndByteCount;
                ToSendDataBuffer[7] = BEANSndSt;
                ToSendDataBuffer[8] = BEANTrSt;

                memcpy(&ToSendDataBuffer[9], &pBEANSndBuff[0], pBEANSndBuff[0] + 1);
                memcpy(&ToSendDataBuffer[pBEANSndBuff[0] + 10], BEANCmdToSend, 5);
                USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
                BEANNumToSend = 0;

            }
            break;*/
        case CMD_BEAN_CODE:
        case CMD_CODE_SORT:
            if(BEANBuffInUse != BEANBuffProcessed && (!HIDTxHandleBusy(USBInHandle)))
            {
                ToSendDataBuffer[0] = CmdInPr;

                switch (BEANBuffProcessed)
                {
                    case 1:
                        memcpy(&ToSendDataBuffer[1], &BEANRecBuff1[0], BEANRecBuff1[0] + 1);
                        break;
                    case 2:
                        memcpy(&ToSendDataBuffer[1], &BEANRecBuff2[0], BEANRecBuff2[0] + 1);
                        break;
                    case 3:
                        memcpy(&ToSendDataBuffer[1], &BEANRecBuff3[0], BEANRecBuff3[0] + 1);
                        break;
                    case 4:
                        memcpy(&ToSendDataBuffer[1], &BEANRecBuff4[0], BEANRecBuff4[0] + 1);
                        break;
                }

                BEANBuffProcessed++;
                if (BEANBuffProcessed > 4)
                    BEANBuffProcessed = 1;

                USBInHandle = HIDTxPacket(CUSTOM_DEVICE_HID_EP,(BYTE*)&ToSendDataBuffer[0],64);
            }
            break;


        default:
            break;
    }

}

void Timer1ONOFF(unsigned char ON, unsigned char cTMR1H, unsigned char cTMR1L)
{
#if defined (__18F4550)
    if (ON)
    {
        T1CON = 0b11110000;
        TMR1H = cTMR1H;
        TMR1L = cTMR1L;
        PIR1bits.TMR1IF = 0;
        PIE1bits.TMR1IE = 1;
        IPR1bits.TMR1IP = 1;
        T1CONbits.TMR1ON = 1;
    }
    else
        T1CONbits.TMR1ON = 0;
#endif
}

void Tmr1Int()
{
    if (Tmr1Repeat == 0)    // Send
    {
        if (BEANNumToSend != 0)
        {
            Timer1ONOFF(0, 0, 0);
            BEANCmdToSend[0] = BEANFirstByteInSnd;
            if (BEANNumToSend != 0xFF)  // FF means send indefinitely
                BEANNumToSend--;

            if (BEANTrSt == BEAN_NO_TR)     // Nothing trnasferred now
                                            // Otherwise Tmr0Int will launch sending
            {
                IOCONOFF(1); // With IOC
                Timer0ONOFF(1, TMR0_NOTRESET);
            }
        }
    }
    else
    {
        Timer1ONOFF (1, 0x15, 0x9F);    // 0xFFFF - 60000
        Tmr1Repeat--;
    }
}