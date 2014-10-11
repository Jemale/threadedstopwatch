/************************************************************************//**
 *
 * \file main.c
 *
 * \addtogroup main main
 * \{
 *
 * \brief
 *
 * \note
 *
 * \author kjshepherd & jlockett
 * \date 2014-09-09
 *
 ****************************************************************************/

/****************************************************************************
 *                              INCLUDE FILES                               *
 ****************************************************************************/
#include "main.h"
#include "mbed.h"
#include "CommonDefines.h"
#include "StopWatch.h"
#include "SysTimer.h"
#include "TextLCD.h"
#include "rtos.h"

/****************************************************************************
 *                      PRIVATE TYPES and DEFINITIONS                       *
 ****************************************************************************/
#define TIME_ARRAY_LEN 8
//error message
void printTime(int, char);

/****************************************************************************
 *                              PRIVATE DATA                                *
 ****************************************************************************/
SystemTimerDevice * sysTimer = SystemTimer_Init();
StopWatch * stopWatch = StopWatch_Init(sysTimer, printTime);
TextLCD lcd(p15, p16, p17, p18, p19, p20);
    
char timeBuf[16];
/****************************************************************************
 *                             EXTERNAL DATA                                *
 ****************************************************************************/

/****************************************************************************
 *                     PRIVATE FUNCTION DECLARATIONS                        *
 ****************************************************************************/
bool timeEqual(StopWatchTime * time1, StopWatchTime * time2);
char * getTime(void);

/****************************************************************************
 *                     EXPORTED FUNCTION DEFINITIONS                        *
 ****************************************************************************/
//int MAIN(int argc, char *argv[])
int MAIN(void)
{

    StopWatchTime lcdTime;
    StopWatchTime currentTime;
    Serial pc(USBTX, USBRX);

    int input = 5;
	int click = 0;
	
    lcd.printf("Timer Starting");
    lcd.cls();
    lcd.locate(0, 0);
    lcd.putc('0');
    lcd.putc('0');
    
	lcd.locate(2, 0);
	lcd.putc(':');
	lcd.putc('0');
	lcd.putc('0');
	
	lcd.locate(5, 0);
	lcd.putc(':');
	lcd.putc('0');
	lcd.putc('0');
	
	lcd.locate(8, 0);
	lcd.putc('.');
	lcd.putc('0');
	
    while(1)
    {
		if (pc.readable())
		{
			
			
	   		char c = pc.getc();
           
            
	   		switch (c){
	      		case 's':
					stopWatch->Start();
					break;
	      		case 'p' :
					stopWatch->Stop();
					break;
	      		case 'r' :
					stopWatch->Reset();
					break;
   				}
           }
	}

   return 0; 
}

/****************************************************************************
 *                     PRIVATE FUNCTION DEFINITIONS                         *
 ****************************************************************************/
bool timeEqual(StopWatchTime * time1, StopWatchTime * time2)
{
   int * time1array = (int *)time1;
   int * time2array = (int *)time2;
    
   return (memcmp(time1array, time2array, sizeof(int)*TIME_ARRAY_LEN) ? 1 : 0);

}

char * getTime(void)
{       
		StopWatchTime * time = stopWatch->GetTime();
		
		sprintf(timeBuf, "%d%d:%d%d:%d%d.%d%d", time->hour_high,
			time->hour_low, time->min_high, time->min_low,
			time->sec_high, time->sec_low, time->tenth, time->hundredth);
        
		return timeBuf;
}

void ScreenTest(void)
{
	static int wtf = 0;
	lcd.cls();
	lcd.printf("wtf %d", wtf++);
}

void printTime(int segment, char c)
{
	lcd.locate(segment, 0);
	lcd.putc(c + 48);
		
}


/************************************************************************//**
 * \brief * \param
 * \return
 ****************************************************************************/

/** \}*/
