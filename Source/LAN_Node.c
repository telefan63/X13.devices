/*
Copyright (c) 2011-2013 <comparator@gmx.de>

This file is part of the X13.Home project.
http://X13home.github.com

BSD New License
See LICENSE.txt file for license details.
*/

#include "config.h"

volatile uint8_t iPool;
#define IPOOL_USR       0x01
#define IPOOL_LED_ONL   0x10
#define IPOOL_LED_CONN  0x20

int main(void) __attribute__((naked));
int main(void)
{
  // Watchdog Stop
  cli();
  wdt_reset();
  wdt_disable();
  // Configure Power Reduction
  CONFIG_PRR();
  // Initialize Memory manager
  mqInit();
  // Initialize Object's Dictionary
  InitOD();
  // Init MQTTS
  MQTTS_Init();
  // Init Interconnection Interface & Load Configuration
  PHY_Init();

  // Initialise  variables
  iPool = 0;

  MQ_t * pRBuf = NULL;        // LAN Buffer
  MQ_t * pMBuf = NULL;        // MQTTS Buffer
  uint8_t * pPBuf = NULL;     // Publish Buffer
    
  uint8_t bTmp;
  uint16_t poolIdx = 0xFFFF;

  s_Addr sAddr;

  // Initialize Task Planer
  InitTimer();
  // configure Sleep controller & enable interrupts
  set_sleep_mode(SLEEP_MODE_IDLE);    // Standby, Idle
  sei();                              // Enable global interrupts

  // 
  PHY_Start();

  iPool |= IPOOL_LED_ONL;

  while(1)
  {
    if(((pRBuf = PHY_GetBuf(&sAddr)) != NULL) && (MQTTS_Parser(pRBuf, &sAddr) == 0))
      mqRelease(pRBuf);

    if((pMBuf = MQTTS_Get(&sAddr)) != NULL)
      PHY_Send(pMBuf, &sAddr);

    if(iPool & IPOOL_USR)
    {
      iPool &= ~IPOOL_USR;

      if(MQTTS_GetStatus() == MQTTS_STATUS_CONNECT)
      {
        iPool |= IPOOL_LED_CONN;
        if(poolIdx == 0xFFFF)
          poolIdx = PoolOD();

        if(poolIdx != 0xFFFF)
        {
          // Publish
          pPBuf = (uint8_t *)mqAssert();
          if(pPBuf != NULL)
          {
            bTmp = (MQTTS_MSG_SIZE - MQTTS_SIZEOF_MSG_PUBLISH);
              
            ReadOD(poolIdx, MQTTS_FL_TOPICID_NORM | 0x80, &bTmp, pPBuf);
            MQTTS_Publish(poolIdx, MQTTS_FL_QOS1, bTmp, pPBuf);
            mqRelease((MQ_t *)pPBuf);
            poolIdx = 0xFFFF;
          }
        }
      }
      else
        iPool &= ~IPOOL_LED_CONN;

      MQTTS_Pool(poolIdx != 0xFFFF);
    }
  }
}

ISR(TIMER_ISR)
{
  static uint8_t led_cnt = 0;

  iPool |= IPOOL_USR;

  if(iPool & IPOOL_LED_ONL)
  {
    if(led_cnt)
      led_cnt--;
    else
    {
      LED_TGL();
      if(iPool & IPOOL_LED_CONN)
        led_cnt = (POOL_TMR_FREQ/16);  // 125mS Period
      else
        led_cnt = (POOL_TMR_FREQ/2);  // 1S Period
    }
  }
  else
    LED_OFF();

  PHY_Pool();
}
