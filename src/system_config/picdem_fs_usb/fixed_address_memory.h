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

#ifndef FIXED_MEMORY_ADDRESS_H
#define FIXED_MEMORY_ADDRESS_H

#define FIXED_ADDRESS_MEMORY

#define IN_DATA_BUFFER_ADDRESS_TAG1      @0x500
#define OUT_DATA_BUFFER_ADDRESS_TAG1     @0x540
#define CONTROL_BUFFER_ADDRESS_TAG1      @0x580

#define HID_CUSTOM_OUT_DATA_BUFFER_ADDRESS 0x5C0
#define HID_CUSTOM_IN_DATA_BUFFER_ADDRESS 0x600

#define IN_DATA_BUFFER_ADDRESS_TAG2      @0x640
#define OUT_DATA_BUFFER_ADDRESS_TAG2     @0x680
#define CONTROL_BUFFER_ADDRESS_TAG2      @0x6C0


#define APP_SIGNATURE_ADDRESS                   0x15B6 //Signature location that must be kept at blaknk value (0xFFFF) in this project (has special purpose for bootloader).
#define APP_VERSION_ADDRESS                     0x15C6 //Fixed location, so the App FW image version can be read by the bootloader.

    //--------------------------------------------------------------------------
    //Application firmware image version values, as reported to the bootloader
    //firmware.  These are useful so the bootloader can potentially know if the
    //user is trying to program an older firmware image onto a device that
    //has already been programmed with a with a newer firmware image.
    //Format is APP_FIRMWARE_VERSION_MAJOR.APP_FIRMWARE_VERSION_MINOR.
    //The valid minor version is from 00 to 99.  Example:
    //if APP_FIRMWARE_VERSION_MAJOR == 1, APP_FIRMWARE_VERSION_MINOR == 1,
    //then the version is "1.01"
    #define APP_FIRMWARE_VERSION_MAJOR  1   //valid values 0-255
    #define APP_FIRMWARE_VERSION_MINOR  0   //valid values 0-99
    //--------------------------------------------------------------------------

const unsigned char AppVersion[2] @ APP_VERSION_ADDRESS = {APP_FIRMWARE_VERSION_MINOR, APP_FIRMWARE_VERSION_MAJOR};
const unsigned short int SignaturePlaceholder @ APP_SIGNATURE_ADDRESS = 0xFFFF;


#endif //FIXED_MEMORY_ADDRESS