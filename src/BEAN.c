
#include "BEAN.h"
#if defined (__18F4550)
#include "app_device_custom_hid.h"
#endif

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") Timer0ONOFF(unsigned char ON, unsigned char ForceReset)
#else
void Timer0ONOFF(unsigned char ON, unsigned char ForceReset)
#endif
// ForceReset - if set to 1 Timer0 will be reset to 0 even if already ON
{

    if (ON)
    {
#if defined (__18F4550)
        if (!T0CONbits.TMR0ON || ForceReset == TMR0_FORCERESET)
        {

            T0CON = TMR0_CONFIG;
            TMR0H = 0;
            TMR0L = TMR0L_INIT;
            INTCONbits.TMR0IF = 0;
            INTCONbits.TMR0IE = 1;
            INTCON2bits.TMR0IP = 1;
            T0CONbits.TMR0ON = 1;
        }
#endif
#if defined (__16F819)
        if (!INTCONbits.TMR0IE || ForceReset == TMR0_FORCERESET)
        {
        
            #asm
            CLRWDT
            #endasm

            OPTION_REG = TMR0_CONFIG;
            TMR0 = TMR0L_INIT;
            INTCONbits.TMR0IF = 0;
            INTCONbits.TMR0IE = 1;
        }
#endif
    }
    else
#if defined (__18F4550)
        T0CONbits.TMR0ON = 0;
#endif
#if defined (__16F819)
        INTCONbits.TMR0IE = 0;      // No option to turn off Tmr0, so turn off int
#endif
/*
#if defined (__16F819)
    if (ON)
    {
#asm
        CLRWDT
#endasm

        OPTION_REG = TMR0_CONFIG;
        TMR0 = TMR0L_INIT;
        INTCONbits.TMR0IF = 0;
        INTCONbits.TMR0IE = 1;
    }
    else
        INTCONbits.TMR0IE = 0;      // No option to turn off Tmr0, so turn off int
#endif*/
}

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") Timer2ONOFF(unsigned char ON, unsigned char cPR2)
#else
void Timer2ONOFF(unsigned char ON, unsigned char cPR2)
#endif
{

    if (ON)
    {
        T2CON = T2CON_CONFIG;
        PR2 = cPR2;
        TMR2 = 0;
//        Tmr2Sync = 0;
        PIR1bits.TMR2IF = 0;
        PIE1bits.TMR2IE = 1;
#if defined (__18F4550)
        IPR1bits.TMR2IP = 1;
#endif
        T2CONbits.TMR2ON = 1;
    }
    else
        T2CONbits.TMR2ON = 0;

/*#if defined (__16F819)
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
#endif*/
}

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") IOCONOFF (char ON)
#else
void IOCONOFF (char ON)
#endif
{
#if defined (__18F4550)
    INTCON2bits.RBIP = 1;       // RB IOC High Priority
#endif
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = (ON ? 1:0);        // RB IOC enable

/*
#if defined (__16F819)
    INTCONbits.RBIF = 0;
    INTCONbits.RBIE = (ON ? 1:0);        // RB IOC enable
#endif*/
}

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") InitBEANVars(char NextFrame)
#else
void InitBEANVars(char NextFrame)
#endif
{
//    CmdInPr = CMD_BEAN_CODE;
    if (!NextFrame)
    {

        // Buffers
        BEANCmdToSend[0] = 0;
        BEANBuffInUse = 4;  // Will use buff 1
        BEANBuffProcessed = 1;
        pBEANRecBuff = BEANRecBuff1;
        pBEANRecBuff[0] = 0;
        pBEANRecBuff[1] = 1;    // Make buffer contain some data, so we'll set to Buff=1

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

    if (pBEANRecBuff[0] != 1 && pBEANRecBuff[1] != 0)   // Only Increment if Buffer is containing data
    {
//        if (pBEANRecBuff[(pBEANRecBuff[1] & 0x0F) + 2] != Crc8(&pBEANRecBuff[1], (pBEANRecBuff[1] & 0x0F) + 1))
//            pBEANRecBuff[0] |= 0x80;    // CRC incorrect

        switch (BEANBuffInUse)
        {
            case 2:
                memset((void *)BEANRecBuff3, 0, sizeof(BEANRecBuff3));
                pBEANRecBuff = BEANRecBuff3;
                break;
            case 3:
                memset((void *)BEANRecBuff4, 0, sizeof(BEANRecBuff4));
                pBEANRecBuff = BEANRecBuff4;
                break;
            case 4:
                memset((void *)BEANRecBuff1, 0, sizeof(BEANRecBuff1));
                pBEANRecBuff = BEANRecBuff1;
                break;
            default:      // case 1:
                memset((void *)BEANRecBuff2, 0, sizeof(BEANRecBuff2));
                pBEANRecBuff = BEANRecBuff2;
                break;
        }
        pBEANRecBuff[0] = 1;  // As we should start writing from the pBEANRecBuff[1] (pBEANRecBuff[0] contains BEANBuffPos)
        pBEANRecBuff[1] = 0;

        BEANBuffInUse++;
        if (BEANBuffInUse > 4)
            BEANBuffInUse = 1;
    }

}

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") TmrInt (char bean)
#else
void TmrInt (char bean)
#endif
{
    Timer0ONOFF(0, TMR0_NOTRESET);
    BEANTrSt = (bean ? BEAN_TR_ERR : BEAN_NO_TR);

    InitBEANVars(1);
    if (BEANTrSt == BEAN_NO_TR && BEANCmdToSend[0] != 0)      // Nothing to send
    {
        IOCONOFF (0);      // Turn OFF IOC for sending
        BEANStartSend();
    }
   /* else
    {
        IOCBEANIN(1);       // Turn ON IOC as we turned off it for Sending
    }*/
}

/*
  Name  : CRC-8
  Poly  : 0x31    x^8 + x^5 + x^4 + 1
  Init  : 0xFF
  Revert: false
  XorOut: 0x00
  Check : 0xF7 ("123456789")
  MaxLen: 15 ????(127 ???) - ???????????
    ?????????, ???????, ??????? ? ???? ???????? ??????
*/

#if defined (_IMMRELAYPRODUCTION)
unsigned char __section("bloader") Crc8(unsigned char *pcBlock, unsigned char len)
#else
unsigned char Crc8(unsigned char *pcBlock, unsigned char len)
#endif
{
    unsigned char crc = 0x00;
    unsigned char i;

    while (len--)
    {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x13 : crc << 1;
    }

    return crc;
}

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") IOCInt (char bean)
#else
void IOCInt (char bean)
#endif
{

    if (bean == BeanInPrevSt || pBEANRecBuff[0] == RECBUFFSIZE - 1)
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

    Timer0ONOFF(1, TMR0_FORCERESET);  // Reset timer

//    unsigned char cnt1 = cnt;
    cnt /= TMR0_1TICK;
    //cnt -= TMR0L_INIT;

    if (cnt >= TRM0_NOTR_COND /*&& BEANSndSt == BEAN_NO_TR*/) //TRM0_NOTR_COND)
    {
        BEANTrSt = (bean ? BEAN_TR_ERR : BEAN_NO_TR);
//        BEANTrSt = BEAN_NO_TR;

        return;
    }


/*    if (BEANTrSt == BEAN_TR_ERR)
    {
        if (bean == 0 && cnt > TRM0_NOTR_CONDITION)
        {
            BEANTrSt == BEAN_NO_TR;
        }
        return;
    }*/

    if (BEANTrSt == BEAN_NO_TR)
    {
       //             Timer0ONOFF(0, 0, 0);  // Stop timer as we got only rising edge of SOF bit
        if (bean == 1)   // This is end of SOF. Init buffer
        {
            InitBEANVars(1);
            BEANTrSt = BEAN_TR_SOF;
        }
        else
        {
            return;
        }
//        pBEANRecBuff[pBEANRecBuff[0]] = ((bean ? 0x80 : 0) | 0x3F);
//        pBEANRecBuff[0]++;
    }

 //   pBEANRecBuff[pBEANRecBuff[0]] = ((bean ? 0x80 : 0) | cnt);
 //   pBEANRecBuff[pBEANRecBuff[0] + 1] = cnt1;
    
//    pBEANRecBuff[0]+=2;
  //  return;
    
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
        pBEANRecBuff[pBEANRecBuff[0]] |= (bean << BEANbitPos);

        if (BEANbitPos == 0)
        {
            //BEANBytesReceived++;
            if (pBEANRecBuff[0] == 1)     // We just got first byte. Set ByteCountInTr
            {
                BEANByteCountInTr = (pBEANRecBuff[pBEANRecBuff[0]] & 0b00001111);
                BEANTrSt = BEAN_TR_ML;
            }
/*            if (pBEANRecBuff[0] == 2)     // We just got second byte.
            {
                BEANTrSt = BEAN_TR_DSTID;
            }
            if (pBEANRecBuff[0] == 3)     // We just got third byte.
            {
                BEANTrSt = BEAN_TR_MESID;
            }
            if (pBEANRecBuff[0] == BEANByteCountInTr + 1) // Got Data
            {
                BEANTrSt = BEAN_TR_DATA; // We can start calculating CRC
            }*/
//            if (pBEANRecBuff[0] == BEANByteCountInTr + 2) // Plus PRI-ML and CRC bytes
//            {
                // CRC here
//                BEANTrSt = BEAN_TR_CRC8;    // We need CRC state as we use it to send RSP
//            }
  /*          if (pBEANRecBuff[0] == BEANByteCountInTr + 3)
            {
                if (pBEANRecBuff[pBEANRecBuff[0]] == 0b01111110)     // We wrote response. Stop receiving
                {
                    BEANTrSt = BEAN_TR_EOM;
                }
                else
                {
                    //BEANTrSt = BEAN_TR_ERR;
                }
            }
*/
            BEANbitPos = 7;
            pBEANRecBuff[0]++;
        }
        else
        {
  /*          if (BEANbitPos == 6 && BEANTrSt == BEAN_TR_EOM)
            {
                BEANTrSt = BEAN_TR_RSP;
            }*/

            BEANbitPos--;
        }
    }
}

#if defined (_IMMRELAYPRODUCTION)
char __section("bloader") BEANStartSend(void)
#else
char BEANStartSend(void)
#endif
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

    BEANTrSt = BEAN_NO_TR;
    Timer0ONOFF(0, TMR0_NOTRESET);

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

    BEAN_OUT = 1;       // Finally set BEAN_OUT to 1 as we get BEAN in interrupt
  //  IOCInt(BEANNextBit);

    BEANNextBitCount = 0;
    BEANGetNextBitToSend();

    // Calculate CRC8

    BEANCmdToSend[(BEANCmdToSend[0] & 0b00001111) + 1] = Crc8(BEANCmdToSend, (BEANCmdToSend[0] & 0b00001111) + 1);
    InitBEANVars(1);
    return 0;
}

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") Tmr2Int()
#else
void Tmr2Int()
#endif
{
    // Colligions check for Previous bit, as rise time may be too large for check immediately
/*    if (BEAN_IN != BEAN_OUT)
    {
        BEAN_OUT = 0;
        BEANTrSt = BEAN_NO_TR;  // Set to NO TRANSFER
        Timer2ONOFF (0, 0);     // Turn OFF Timer2
        IOCONOFF (1);           // Turn On IOC
    }
    else*/
    {
        if (BEANSndSt != BEAN_NO_TR)
        {
            Timer2ONOFF (1, BEANNextBitCount);
            BEAN_OUT = BEANNextBit;
            //IOCInt(BEANNextBit);
            BEANNextBitCount = 0;
            BEANGetNextBitToSend();
        }
        else
        {
            Timer2ONOFF (0, 0);
            BEAN_OUT = BEANNextBit;
            //IOCInt(BEANNextBit);
            memcpy(&pBEANRecBuff[1], BEANCmdToSend, (BEANCmdToSend[0] & 0b00001111) + 2);
            pBEANRecBuff[(BEANCmdToSend[0] & 0b00001111) + 3] = 0x7e;
            pBEANRecBuff[(BEANCmdToSend[0] & 0b00001111) + 4] = 0x40;
            pBEANRecBuff[0] = (BEANCmdToSend[0] & 0b00001111) + 4;
            BEANCmdToSend[0] = 0;   // Set first byte to zero to mark that we already sent everything
            InitBEANVars(1);
            IOCONOFF (1);           // Turn On IOC
        }
    }

}

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") BEANGetNextBitToSend()
#else
void BEANGetNextBitToSend()
#endif
{
    if (BEANNextBitCount != 0)      // Still not sent prev bit
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
//    BEANRecBuff1[0]++;
//    BEANRecBuff1[BEANRecBuff1[0]] = BEANSndByteCount;
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

