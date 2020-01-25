/* 
 * File:   i2c.h
 * Author: emelif
 *
 * Created on 16 Июнь 2014 г., 16:28
 */
#include <system.h>
#include <string.h>

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

void i2cTestInOut (BYTE *, BYTE*);
void i2cDoCmd (BYTE *, BYTE *, unsigned char);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

