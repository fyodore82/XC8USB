#include <htc.h>
#include "BEAN.h"
#if defined (__18F4550)
#include "app_device_custom_hid.h"
#endif
#include <string.h>

void Timer0ONOFF(unsigned char ON)
{
#if defined (__18F4550)
    if (ON)
    {
        T0CON = TMR0_CONFIG;
        TMR0H = 0;
        TMR0L = TMR0L_INIT;
        INTCONbits.TMR0IF = 0;
        INTCONbits.TMR0IE = 1;
        INTCON2bits.TMR0IP = 1;
        T0CONbits.TMR0ON = 1;
    }
    else
        T0CONbits.TMR0ON = 0;
#endif

#if defined (__16F819)
    if (ON)
    {
#asm
        CLRWDT
#endasm
/*BANKSEL OPTION_REG ; Select Bank of OPTION_REG
MOVLW b'xxxx0xxx' ; Select TMR0, new prescale
MOVWF OPTION_REG ; value and clock source*/
OPTION_REG = TMR0_CONFIG;
        TMR0 = TMR0L_INIT;
        INTCONbits.TMR0IF = 0;
        INTCONbits.TMR0IE = 1;
    }
    else
        INTCONbits.TMR0IE = 0;      // No option to turn off Tmr0, so turn off int
#endif
}

void Timer2ONOFF(unsigned char ON, unsigned char cPR2)
{
#if defined (__18F4550)
    if (ON)
    {
        T2CON = T2CON_CONFIG;
        PR2 = cPR2;
        TMR2 = 0;
        Tmr2Sync = 0;
        PIR1bits.TMR2IF = 0;
        PIE1bits.TMR2IE = 1;
        IPR1bits.TMR2IP = 1;
        T2CONbits.TMR2ON = 1;
    }
    else
        T2CONbits.TMR2ON = 0;
#endif

#if defined (__16F819)
    if (ON)
    {
        T2CON = T2CON_CONFIG;
        PR2 = cPR2;
        TMR2 = 0;
        PIR1bits.TMR2IF = 0;
        PIE1bits.TMR2IE = 1;
        T2CONbits.TMR2ON = 1;
    }
    else
        T2CONbits.TMR2ON = 0;
#endif
}

void IOCONOFF (char ON)
{
#if defined (__18F4550)
    INTCON2bits.RBIP = 1;       // RB IOC High Priority
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = (ON ? 1:0);        // RB IOC enable
#endif

#if defined (__16F819)
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = (ON ? 1:0);        // RB IOC enable
#endif
}

void InitBEANVars(char NextFrame)
{
//    CmdInPr = CMD_BEAN_CODE;
    if (!NextFrame)
    {
        // Buffers
/*        memset((void *)BEANSndBuff1, 0, sizeof(BEANSndBuff1));
        memset((void *)BEANSndBuff2, 0, sizeof(BEANSndBuff2));
        memset((void *)BEANSndBuff3, 0, sizeof(BEANSndBuff3));
        memset((void *)BEANSndBuff4, 0, sizeof(BEANSndBuff4));
        ;
        pBEANSndBuff[0] = 1;
*/
        BEANBuffInUse = 4;  // Will use buff 1
        BEANBuffSent = 1;
        pBEANSndBuff = BEANSndBuff1;
        pBEANSndBuff[0] = 0;
        pBEANSndBuff[1] = 1;    // Make buffer contain some data, so we'll set to Buff=1

        BEANTrSt = BEAN_NO_TR;

        // BEAN pin
        BeanInPrevSt = 0;
    }

    BEANbitPos = 7;     // First bit = SOF, put it in first byte
//    BEANBytesReceived = 0;


    // Transfer vars
    BEANByteCountInTr = 0;
    BEANBitStaffing = BEAN_TR_NO_STAFFING;
//    TransferInProgress = 0;
//    ZeroBitCounter = 0;
//    TmrPrevBeanSt = PREV_BEAN_NOTTMR;
//    CountOfSameBits = 0;

    if (pBEANSndBuff[0] != 1 && pBEANSndBuff[1] != 0)   // Only Increment if Buffer is containing data
    {
        //if (pBEANSndBuff[(pBEANSndBuff[1] & 0x0F) + 1] != Crc8(&pBEANSndBuff[1], (pBEANSndBuff[1] & 0x0F) + 1))
        //    pBEANSndBuff[0] |= 0x80;    // CRC incorrect
        switch (BEANBuffInUse)
        {
            case 1:
                memset((void *)BEANSndBuff2, 0, sizeof(BEANSndBuff2));
                pBEANSndBuff = BEANSndBuff2;
                break;
            case 2:
                memset((void *)BEANSndBuff3, 0, sizeof(BEANSndBuff3));
                pBEANSndBuff = BEANSndBuff3;
                break;
            case 3:
                memset((void *)BEANSndBuff4, 0, sizeof(BEANSndBuff4));
                pBEANSndBuff = BEANSndBuff4;
                break;
            case 4:
                memset((void *)BEANSndBuff1, 0, sizeof(BEANSndBuff1));
                pBEANSndBuff = BEANSndBuff1;
                break;
        }
        pBEANSndBuff[0] = 1;  // As we should start writing from the pBEANSndBuff[1] (pBEANSndBuff[0] contains BEANBuffPos
        pBEANSndBuff[1] = 0;

        BEANBuffInUse++;
        if (BEANBuffInUse > 4)
            BEANBuffInUse = 1;
    }

}

void TmrInt (char bean)
{
    Timer0ONOFF(0);
    BEANTrSt = (bean ? BEAN_TR_ERR : BEAN_NO_TR);

    InitBEANVars(1);
    if (BEANTrSt == BEAN_NO_TR && BEANCmdToSend[0] != 0)      // Nothing to send
    {
        BEANStartSend();
    }
//    else
//    {
//        IOCONOFF(1);       // Turn ON IOC as we turned off it for Sending
//    }
}

void IOCInt (char bean)
{

    if (bean == BeanInPrevSt || pBEANSndBuff[0] == RECBUFFSIZE - 1)
        return;

            
    // When we got 0 it means, that before were 1's, as we get only edges of bits
    BeanInPrevSt = bean;
    bean = (bean ? 0 : 1);  // We need bean value, that was BEFORE we got rising or falling edge

#if defined (__18F4550)
    unsigned char cnt = TMR0L;
#endif

#if defined (__16F819)
    unsigned char cnt = TMR0;
#endif

    Timer0ONOFF(1);//0xFF - (TMR0_1TICK * TRM0_NOTR_CONDITION));  // Reset timer

    cnt /= TMR0_1TICK;
 //                  pBEANSndBuff[pBEANSndBuff[0]] = (bean ? 0x80 : 0) | cnt;
 //               pBEANSndBuff[0]++;
 //               return;
    if (cnt >= TRM0_NOTR_CONDITION)
    {
        BEANTrSt = (bean ? BEAN_TR_ERR : BEAN_NO_TR);
//        BEANTrSt = BEAN_NO_TR;
        // We shouldn't stop timer as we could already got SOF rising Edge
//                    Timer0ONOFF(0, 0, 0);
//        InitBEANVars(1);
        return;
    }

/*    if (BEANTrSt == BEAN_TR_ERR)
    {
        if (bean == 0 && tmrlCurr > TRM0_NOTR_CONDITION)
        {
            BEANTrSt == BEAN_NO_TR;
//                        Timer0ONOFF(0, 0, 0);
//                        InitBEANVars(1);
        }
        return;
    }
*/
    if (BEANTrSt == BEAN_NO_TR)       
    {
       //             Timer0ONOFF(0, 0, 0);  // Stop timer as we got only rising edge of SOF bit
        if (bean == 1)   // This is end of SOF. Init buffer
        {
            BEANTrSt = BEAN_TR_SOF;
            InitBEANVars(1);
        }
    }

    if (cnt == 0)
        return;
                
    if (BEANTrSt == BEAN_TR_SOF)
    {
        cnt--;      // We won't write SOF
        BEANTrSt = BEAN_TR_MLINPR;
    }

    // Bit Staffing processing
    if (BEANBitStaffing == BEAN_TR_NEXT_STAFF_BIT)
    {
        if (cnt < 6)
        {
            cnt--;
        }
        BEANBitStaffing = BEAN_TR_NO_STAFFING;        // Clear bit staffing
    }
    if (cnt == 5)       // 85 = 17*5,  100 = 20*5
    {
        BEANBitStaffing = BEAN_TR_NEXT_STAFF_BIT;
    }



    for (; cnt; cnt--)
    {
        pBEANSndBuff[pBEANSndBuff[0]] |= (bean << BEANbitPos);

        if (BEANbitPos == 0)
        {

            //BEANBytesReceived++;
            if (pBEANSndBuff[0] == 1)     // We just got first byte. Set ByteCountInTr
            {
                BEANByteCountInTr = (pBEANSndBuff[pBEANSndBuff[0]] & 0b00001111);
                BEANTrSt = BEAN_TR_ML;
            }
/*            if (pBEANSndBuff[0] == 2)     // We just got second byte.
            {
                BEANTrSt = BEAN_TR_DSTID;
            }
            if (pBEANSndBuff[0] == 3)     // We just got third byte.
            {
                BEANTrSt = BEAN_TR_MESID;
            }
            if (pBEANSndBuff[0] == BEANByteCountInTr + 1) // Got Data
            {
                BEANTrSt = BEAN_TR_DATA; // We can start calculating CRC
            }*/
            if (pBEANSndBuff[0] == BEANByteCountInTr + 2) // Plus PRI-ML and CRC bytes
            {
                BEANTrSt = BEAN_TR_CRC8; // We can start calculating CRC
            }
            if (pBEANSndBuff[0] == BEANByteCountInTr + 3)
            {
                if (pBEANSndBuff[pBEANSndBuff[0]] == 0b01111110)     // We wrote response. Stop receiving
                {
                    //pBEANSndBuff[pBEANSndBuff[0]] = 0;
                    //pBEANSndBuff[0]--;      // We don't need EOM in result
                    BEANTrSt = BEAN_TR_EOM;
                }
                else
                {
                    //BEANTrSt = BEAN_TR_ERR;
                }
            }

            BEANbitPos = 7;
            pBEANSndBuff[0]++;
        }
        else
        {
            if (BEANbitPos == 6 && BEANTrSt == BEAN_TR_EOM)
            {
                BEANTrSt = BEAN_TR_RSP;
            }

            BEANbitPos--;
        }
    }
}

char BEANStartSend(void)
{
    if (BEANCmdToSend[0] == 0)      // Nothing to send
        return 1;
#if defined (__18F4550)
    if (BEANTrSt != BEAN_NO_TR && TMR0L < TMR0_1TICK)
        return 1;
#endif

#if defined (__16F819)
    if (BEANTrSt != BEAN_NO_TR && TMR0 < TMR0_1TICK)
        return 1;
#endif
    IOCONOFF (0);      // Turn OFF IOC for sending

    BEANTrSt = BEAN_NO_TR;
    Timer0ONOFF(0);

    BEANNextBitCount = 0;
    BEANSndSt = BEAN_TR_SOF;
    BEANSndBitPos = 7;
    BEANSndByteCount = 0;

/*    while ((BEANCmdToSend[BEANSndByteCount] & (1 << BEANSndBitPos)) && BEANSndBitPos > 3)
        BEANSndBitPos--;

    if (BEANSndBitPos == 3)
        BEANSndStaffing = BEAN_TR_NEXT_STAFF_BIT;
    else
        BEANSndStaffing = BEAN_TR_NO_STAFFING;
*/
    BEANSndStaffing = BEAN_TR_NEXT_STAFF_BIT;    // Simulate Staffing (it will be Start bit)
    BEANGetNextBitToSend();

    Timer2ONOFF (1, BEANNextBitCount);
//    Timer2ONOFF (1, TMR2_1TICK * (8 - BEANSndBitPos));
    BEAN_OUT = 1;       // Finally set BEAN_OUT to 1 as we get BEAN in interrupt
    IOCInt(BEANNextBit);


    BEANNextBitCount = 0;
    BEANGetNextBitToSend();

    return 0;
}

void Tmr2Int()
{
/*            unsigned char d[6];
        d[0] = 2;
        d[1] = BEAN_OUT;
        d[2] = BEANNextBitCount;
        d[3] = BEANSndSt;
        d[4] = BEANSndBitPos;
        d[5]=  BEANSndByteCount;
//__delay_ms (10);
        USBDebug(d, 6);
*/
    if (BEANSndSt != BEAN_NO_TR)
    {
        Timer2ONOFF (1, BEANNextBitCount);
        BEAN_OUT = BEANNextBit;
        IOCInt(BEANNextBit);
        BEANNextBitCount = 0;
        BEANGetNextBitToSend();
    }
    else
    {
        Timer2ONOFF (0, 0);
        BEAN_OUT = BEANNextBit;
        IOCInt(BEANNextBit);
    }
}

void BEANGetNextBitToSend()
{
    if (BEANNextBitCount != 0)      // Not send prev bit
        return;
    
    if (BEANSndStaffing == BEAN_TR_NEXT_STAFF_BIT)
    {
        BEANNextBit = !BEAN_OUT;
        BEANNextBitCount = TMR2_1TICK;
        BEANSndStaffing = BEAN_TR_NO_STAFFING;
    }

    if ((BEANCmdToSend[0] & 0b00001111) + 2 == BEANSndByteCount)    // Data & CRC8 had been sent
    {
        if (BEANNextBitCount == 0)
        {
            BEANSndSt = BEAN_TR_EOM;
            switch (BEANSndBitPos)
            {
                case 0:
                    BEANSndBitPos = 6;
                    BEANSndByteCount++;
                    BEANNextBit = 0;
                    BEANNextBitCount = TMR2_1TICK * 2;  // Send EOM and RSP bits
                    break;
                case 6:
                    BEANSndBitPos = 0;
                    BEANNextBit = 1;
                    BEANNextBitCount = TMR2_1TICK * 6;
                    break;
                case 7:
                    BEANSndBitPos--;
                    BEANNextBit = 0;
                    BEANNextBitCount = TMR2_1TICK;
                    break;
            }
        }
        return;
    }
    if ((BEANCmdToSend[0] & 0b00001111) + 3 == BEANSndByteCount)    // EOM sent, send RSP
    {
        if (BEANNextBitCount == 0)
        {
            BEANSndSt = BEAN_TR_RSP;
            switch (BEANSndBitPos)
            {
                case 5:
                    BEANSndSt = BEAN_NO_TR;
                    BEANNextBit = 0;
                    BEANNextBitCount = 0;
                    BEANCmdToSend[0] = 0;   // Set first byte to zero to mark that we already sent everything
                    break;
                case 6:
                    BEANSndBitPos--;
                    BEANNextBit = 1;
                    BEANNextBitCount = TMR2_1TICK;
            }
/*            if (BEANSndBitPos == 7)
            {
                BEANSndBitPos--;
                BEANNextBit = 0;
                BEANNextBitCount = TMR2_1TICK;
            }*/
        }
        return;
    }

    if (BEANNextBitCount == 0)
        BEANNextBit = !!(BEANCmdToSend[BEANSndByteCount] & (1 << BEANSndBitPos));

    while (((BEANCmdToSend[0] & 0b00001111) + 2 != BEANSndByteCount) &&
            (BEANNextBit == (!!(BEANCmdToSend[BEANSndByteCount] & (1 << BEANSndBitPos)))) &&
            BEANNextBitCount != TMR2_1TICK * 5)
    {
        if (BEANSndBitPos == 0)
        {
            BEANSndBitPos = 7;
            BEANSndByteCount++;
//    BEANSndBuff1[0]++;
//    BEANSndBuff1[BEANSndBuff1[0]] = BEANSndByteCount;
        }
        else
        {
            BEANSndBitPos--;
        }
        BEANNextBitCount += TMR2_1TICK;
    }

    if (BEANNextBitCount == (TMR2_1TICK * 5))
        BEANSndStaffing = BEAN_TR_NEXT_STAFF_BIT;
}

