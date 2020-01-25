#include "i2c.h"
#include "../../i2c/i2c_my.h"
#include "../../i2c/i2cCmds.h"
#include "app_device_custom_hid.h"

void i2cTestInOut (BYTE* pRecBuff, BYTE* pToSendBuff)
{
    unsigned char cmd[5];
    unsigned char i = _I2C_BINST, j = 0, k;

 //   for (i = _I2C_DI21, j = 0; i <= _I2C_DI21; i++, j+= 3)
    {
        cmd[0] = 0b01000010;
        cmd[1] = i;
        pToSendBuff[j] = TXCmdToPIC16 (cmd, 2, 10);
        cmd[0] = 0b01000011;
        for (k = 0; k < 50; k++)
            __delay_ms(2);
        pToSendBuff[j + 1] = RXfromPIC16 (cmd, 2, 10);
        pToSendBuff[j + 2] = cmd[1];
        for (k = 0; k < 50; k++)
            __delay_ms(2);
    }

}

void i2cDoCmd (BYTE* pRecBuff, BYTE* pToSendBuff, unsigned char WaitBtwCmds)
{
    // WaitBtwCmds = 20 - normal
    // WaitBtwCmds = 5 - too little for good writing
    unsigned char cmd[4];
    unsigned char wt;
    memset(cmd, sizeof(cmd), 0);
    cmd[0] = pRecBuff[0];       // Address
    cmd[1] = pRecBuff[1];       // Par 1
    memset(pToSendBuff, 10, 0);

    if (pRecBuff[2] == _I2C_EEPROMRW)
    {
        cmd[2] = pRecBuff[2];
        if (pRecBuff[3] == 0xFF)
        {
            pToSendBuff[0] = TXCmdToPIC16 (cmd, 3, WaitBtwCmds);
            cmd[0] |= 0b00000001;

            for (wt = WaitBtwCmds; wt != 0; wt--)
                __delay_ms(1);

            pToSendBuff[1] = RXfromPIC16 (cmd, 2, WaitBtwCmds);
            pToSendBuff[2] = cmd[1];
        }
        else
        {
            cmd[3] = pRecBuff[3];
            pToSendBuff[0] = TXCmdToPIC16 (cmd, 4, WaitBtwCmds);
/*            if (pRecBuff[4] == 0xFF)
            {
                cmd[0] |= 0b00000001;
                __delay_ms(10);
                pToSendBuff[1] = RXfromPIC16 (cmd, 2, 100);
                pToSendBuff[2] = cmd[1];
            }*/
        }
    }
    else
    {
        if (pRecBuff[2] == 0xFF)    // Get value of Par1
        {
            pToSendBuff[0] = TXCmdToPIC16 (cmd, 2, WaitBtwCmds);
            cmd[0] |= 0b00000001;

            for (wt = WaitBtwCmds; wt != 0; wt--)
                __delay_ms(1);

            pToSendBuff[1] = RXfromPIC16 (cmd, 2, WaitBtwCmds);
            pToSendBuff[2] = cmd[1];
        }
        else
        {
            cmd[2] = pRecBuff[2];
            pToSendBuff[0] = TXCmdToPIC16 (cmd, 3, WaitBtwCmds);
            pToSendBuff[1] = 0;
            if (pRecBuff[3] == 0xFF)
            {
                cmd[0] |= 0b00000001;

                for (wt = WaitBtwCmds; wt != 0; wt--)
                    __delay_ms(1);

                pToSendBuff[1] = RXfromPIC16 (cmd, 2, WaitBtwCmds);
                pToSendBuff[2] = cmd[1];
            }
        }
    }


    SSPCON1bits.SSPEN = 0;
    __delay_ms(10);
    i2c_init(0);

}