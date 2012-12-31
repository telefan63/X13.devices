/*
Copyright (c) 2011-2012 <comparator@gmx.de>

This file is part of the X13.Home project.
https://github.com/X13home

BSD License
See LICENSE.txt file for license details.
*/

// TWI Driver Silicon Image Si7005, Temperature & Humidity

// Outs
// TW(addr) Temperature 0,1�C
// TW(add + 1)    Humidity uncompensated  0,1%  RH(%)
// RH_lin = RH - (RH^2*-0,00393 + RH*0,4008 - 4,7844)
// RH_C = RH_lin + (Temp(�C) - 30)*(RH_lin*0,0237 + 0,1973)

#define SI7005_ADDR                 0x40

// Si7005 Registers
#define SI7005_REG_STATUS           0x00
#define SI7005_REG_DATA             0x01
#define SI7005_REG_CONFIG           0x03
#define SI7005_REG_ID               0x11

// Status Register
#define SI7005_STATUS_NOT_READY     0x01

// Config Register
#define SI7005_CONFIG_START         0x01
#define SI7005_CONFIG_HEAT          0x02
#define SI7005_CONFIG_HUMIDITY      0x00
#define SI7005_CONFIG_TEMPERATURE   0x10
#define SI7005_CONFIG_FAST          0x20

// ID Register
#define SI7005_ID_SI7005            0x50

// Process variables
static uint8_t  si7005_stat;
static uint16_t si7005_oldTemp;
static uint16_t si7005_oldHumi;

static uint8_t twi_SI7005_Read(subidx_t * pSubidx, uint8_t *pLen, uint8_t *pBuf)
{
    
    *pLen = 2;
    if(pSubidx->Base & 1)   // Read Humidity
        *(uint16_t *)pBuf = si7005_oldHumi;
    else                    // Read Temperature
        *(uint16_t *)pBuf = si7005_oldTemp;
    return MQTTS_RET_ACCEPTED;
}

static uint8_t twi_SI7005_Pool1(subidx_t * pSubidx)
{
    switch(si7005_stat)
    {
        case 0:
            if(twi_bus_busy)
                return 0;
            si7005_stat = 1;
            twi_bus_busy = 1;
        case 1:             // Start Conversion, Temperature
        case 7:
            twi_trv_buf[0] = SI7005_REG_CONFIG;
            twi_trv_buf[1] = (SI7005_CONFIG_START | SI7005_CONFIG_TEMPERATURE);
            twiExch_ISR(SI7005_ADDR, TW_WRITE, 2);
            break;
        // !! Conversion Time 35mS - Normal / 18 mS - Fast
        case 3:     // Read Busy Flag
        case 9:
        case 16:
            twi_trv_buf[0] = SI7005_REG_STATUS;
            twiExch_ISR(SI7005_ADDR, TW_WRITE | TW_SEQUENTIAL, 1);
            break;
        case 4:
        case 10:
        case 17:
            twiExch_ISR(SI7005_ADDR, TW_READ, 1);
            break;
        case 5:
        case 11:
        case 18:
            if(twi_trv_buf[0] & SI7005_STATUS_NOT_READY)           // Busy
            {
                si7005_stat -= 2;
                return 0;
            }
            twi_trv_buf[0] = SI7005_REG_DATA;
            twiExch_ISR(SI7005_ADDR, TW_WRITE | TW_SEQUENTIAL, 1);
            break;
        case 6:
        case 12:
        case 19:
            twiExch_ISR(SI7005_ADDR, TW_READ, 2);
            break;
        case 13:
            {
            uint16_t val = ((uint16_t)twi_trv_buf[0]<<5) | (twi_trv_buf[1]>>3);
            val *= 5;
            val >>=3;
            val -= 500;
            if(val != si7005_oldTemp)
            {
                si7005_oldTemp = val;
                si7005_stat++;
                return 1;
            }
            }
            break;
        case 14:        // Start Conversion, Humidity
            twi_trv_buf[0] = SI7005_REG_CONFIG;
            twi_trv_buf[1] = (SI7005_CONFIG_START | SI7005_CONFIG_HUMIDITY);
            twiExch_ISR(SI7005_ADDR, TW_WRITE, 2);
            break;
    }
    si7005_stat++;

    return 0;
}

static uint8_t twi_SI7005_Pool2(subidx_t * pSubidx)
{
    if(si7005_stat == 21)
    {
        si7005_stat++;
        twi_bus_busy = 0;
        uint16_t val = ((uint16_t)twi_trv_buf[0]<<4) | (twi_trv_buf[1]>>4);
        val *= 5;
        val >>=3;
        val -= 240;
        if(val != si7005_oldHumi)
        {
            si7005_oldHumi = val;
            return 1;
        }
    }
    return 0;
}

static uint8_t twi_SI7005_Config(void)
{
    uint8_t reg = SI7005_REG_ID;
    if(twiExch(SI7005_ADDR, TW_WRITE | TW_SEQUENTIAL, 1, &reg) != TW_SUCCESS)
    {
        twiSendStop();
        return 0;
    }
    if((twiExch(SI7005_ADDR, TW_READ, 1, &reg) != TW_SUCCESS) ||        // Communication error
       (reg != SI7005_ID_SI7005))                                       // Bad device
        return 0;

    si7005_stat = 0;
    si7005_oldTemp = 0;
    si7005_oldHumi = 0;

    indextable_t index;
    
    // Register variable 1, Temperature 0,1�C
    index.cbRead  =  &twi_SI7005_Read;
    index.cbWrite =  NULL;
    index.cbPool  =  &twi_SI7005_Pool1;

    index.sidx.Place = objTWI;                   // Object TWI
    index.sidx.Type =  objInt16;                 // Variables Type -  Int16
    index.sidx.Base = (SI7005_ADDR<<8);          // Device addr
    
    if(RegistIntOD(&index) != MQTTS_RET_ACCEPTED)
        return 0;

    // Register variable 2, Humidity - uncompensated
    index.cbPool  =  &twi_SI7005_Pool2;
    index.sidx.Type =  objUInt16;                // Variables Type -  UInt32
    index.sidx.Base++;
    
    if(RegistIntOD(&index) != MQTTS_RET_ACCEPTED)
        return 0;

    return 2;
}
