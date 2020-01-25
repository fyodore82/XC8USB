/* 
 * File:   BEAN.h
 * Author: fedor
 *
 * Created on 6 Июнь 2014 г., 16:43
 */

// How to use
// To SEND - set data to BEANCmdToSend and call
//  Timer0ONOFF(1, TMR0_NOTRESET);   // To check if no transfer in progress
//  IOCONOFF(1);   // To get any transfers
//      After transfer will be finished, we will start to send immedeately

// To receive, call IOCONOFF(1)

// After receiving, main prog must check crc8 by
//   if (pCurrBuff[(pCurrBuff[1] & 0x0F) + 2] == Crc8(&pCurrBuff[1], (pCurrBuff[1] & 0x0F) + 1))


#ifndef BEAN_H
#define	BEAN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <htc.h>
#include <string.h>

// ----------------------------------------
// COMMON DEFINITIONS
// ----------------------------------------

#define TRM0_NOTR_COND          7
#define TMR0L_INIT              0 //0xFF - 119
//#define TRM0_NOTR_CONDITION     9      // TMR0 value for no transfer condition
                                        // 18,75 * 7 = 131,25, 19 * 7 = 133
                                        // 18 * 7 = 126
                                        // 17 * 7 = 119
                                        // 19 * 6 = 114
                                // 98 ticks mean no transfer condition
                                 // 15,125 * 7 = 105,875
                                 // 14 * 7 = 98
                                 // 15 * 7 = 105
                                 // 16 * 6 = 96

// -------------------------------------------
// PROCESSOR SPECIFIC DEFINITIONS
// -------------------------------------------

#if defined(__18F4550)

#define _XTAL_FREQ 48000000.0

#define BEAN_IN PORTBbits.RB4 //PORTDbits.RD2
#define BEAN_IN_DIG PORTBbits.RB4
#define BEAN_OUT LATBbits.LB5

#define TMR0_CONFIG             0b01000101  //ReceivedDataBuffer[2];
                                            // 1:64 prescaler
                                            // 8 bit timer
                                            // 10 ms for 18,75 ticks
#define TMR0_1TICK              17
#define T2CON_CONFIG    0b00100011          // 100 - 1:5 Postscaler
                                            // 1:16 prescaler
                                            // 10 ms = 15 ticks
#define TMR2_1TICK      15
#define RECBUFFSIZE     63  // Incl CRC, EOM, RSP. First byte is length

#endif

#if defined (__16F819)

#define _XTAL_FREQ 20000000.0

#define BEAN_IN PORTBbits.RB7
#define BEAN_OUT PORTAbits.RA4

#define TMR0_CONFIG 0b11000100   // Prescaler 1:32 = 20000000/(4*32*10000) = 15.125 tiks on 1 BEAN bit
                                 // Really it is OPTION_REG. We don't have TMR0_CONFIG register

#define TMR0_1TICK      14
                                        
#define T2CON_CONFIG    0b00100001      // Prescaler = 5, postscaler = 4. 20000000/(4*4*5*10000) = 25 ticks on 1 BEAN bit
#define TMR2_1TICK      25

#define RECBUFFSIZE     20  // Incl CRC, EOM, RSP. First byte is length

#endif

#if defined (_IMMRELAYPRODUCTION)
void __section("bloader") InitBEANVars(char);
void __section("bloader") Timer0ONOFF(unsigned char ON, unsigned char ForceReset);
void __section("bloader") IOCONOFF (char);
void __section("bloader") TmrInt (char);
void __section("bloader") IOCInt (char);
char __section("bloader") BEANStartSend(void);       // Start sending: send START bit + 0-4 more bits by starting timer
                                                // 0 - Send started
                                                // 1 - Line busy
void __section("bloader") Timer2ONOFF(unsigned char ON, unsigned char);
void __section("bloader") Tmr2Int(void);             // Continue sending
void __section("bloader") BEANGetNextBitToSend(void);
unsigned char __section("bloader") Crc8(unsigned char *, unsigned char);
#else
void InitBEANVars(char);
void Timer0ONOFF(unsigned char ON, unsigned char ForceReset);
void IOCONOFF (char);
void TmrInt (char);
void IOCInt (char);
char BEANStartSend(void);       // Start sending: send START bit + 0-4 more bits by starting timer
                                                // 0 - Send started
                                                // 1 - Line busy
void Timer2ONOFF(unsigned char ON, unsigned char);
void Tmr2Int(void);             // Continue sending
void BEANGetNextBitToSend(void);
unsigned char Crc8(unsigned char *, unsigned char);
#endif

#if defined (__16F819)

#define RECBUFF1_ADDRESS 0x120
#define RECBUFF2_ADDRESS RECBUFF1_ADDRESS + RECBUFFSIZE
#define RECBUFF3_ADDRESS RECBUFF2_ADDRESS + RECBUFFSIZE
#define RECBUFF4_ADDRESS RECBUFF3_ADDRESS + RECBUFFSIZE

#define BEANBUFFINUSE_ADDRESS 0x20; //RECBUFF4_ADDRESS + RECBUFFSIZE
#define BEANBUFFSENT_ADDRESS 0x21; //BEANBUFFINUSE_ADDRESS + 1

#define SNDBUFF_ADDRESS 0xA0

unsigned char BEANRecBuff1[RECBUFFSIZE] @ RECBUFF1_ADDRESS;
unsigned char BEANRecBuff2[RECBUFFSIZE] @ RECBUFF2_ADDRESS;
unsigned char BEANRecBuff3[RECBUFFSIZE] @ RECBUFF3_ADDRESS;
unsigned char BEANRecBuff4[RECBUFFSIZE] @ RECBUFF4_ADDRESS;

unsigned char BEANCmdToSend[14] @ SNDBUFF_ADDRESS;        // ML-PRI till end of Data bit
unsigned char BEANBuffInUse @ BEANBUFFINUSE_ADDRESS;
unsigned char BEANBuffProcessed @ BEANBUFFSENT_ADDRESS; // BEANBuffProcessed = Buffer + 1, that already had been sent
#endif
#if defined(__18F4550)
unsigned char BEANRecBuff1[RECBUFFSIZE];
unsigned char BEANRecBuff2[RECBUFFSIZE];
unsigned char BEANRecBuff3[RECBUFFSIZE];
unsigned char BEANRecBuff4[RECBUFFSIZE];

unsigned char BEANCmdToSend[14];        // ML-PRI till end of Data bit
unsigned char BEANBuffInUse, BEANBuffProcessed; // BEANBuffProcessed = Buffer + 1, that already had been sent
#endif

unsigned char *pBEANRecBuff;

unsigned char /*BEANBuffPos,*/ BEANbitPos, BEANByteCountInTr;//, BEANBytesReceived;
unsigned char BeanInPrevSt;//, TransferInProgress;
//unsigned char tmrl; /*tmrlCurr, Tmr0Con, ZeroBitCounter;*/
unsigned char BEANTrSt, BEANBitStaffing;

//unsigned char Tmr2Con;
unsigned char BEANSndSt, BEANSndStaffing, BEANSndBitPos, BEANSndByteCount;
unsigned char BEANNextBit, BEANNextBitCount;
//unsigned char Tmr2Sync;

#define TMR0_FORCERESET         1
#define TMR0_NOTRESET           0

#define BEAN_NO_TR              0

#define BEAN_TR_SOF             1       // We are just got SOF and some part of ML.
//#define BEAN_ERR_NO_SOF         0x11
#define BEAN_TR_MLINPR          2
#define BEAN_TR_ML              3       // We are getting ML
#define BEAN_TR_DSTID           4
#define BEAN_TR_MESID           5
#define BEAN_TR_DATA            6
#define BEAN_TR_CRC8            7
#define BEAN_TR_EOM             8
#define BEAN_TR_RSP             9

#define BEAN_TR_ERR             0x81
#define BEAN_TR_CRCERR          0x82

#define BEAN_TR_NEXT_STAFF_BIT  0x80   // 5 same bits received
#define BEAN_TR_NO_STAFFING     0




//unsigned char TmrPrevBeanSt, CountOfSameBits;

#ifdef	__cplusplus
}
#endif

#endif	/* BEAN_H */

