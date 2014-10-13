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

int stopped = 0;
int started = 0;
int was_paused = 0;
int update_sec_timer = 0;
int update_min_timer = 0;
int update_hun_timer = 0;

RtosTimer* timer_hun;
RtosTimer* timer_sec;
RtosTimer* timer_min;

Timer t;

Mutex stopwatch_mutex;
/****************************************************************************
 *                             EXTERNAL DATA                                *
 ****************************************************************************/

/****************************************************************************
 *                     PRIVATE FUNCTION DECLARATIONS                        *
 ****************************************************************************/
bool timeEqual(StopWatchTime * time1, StopWatchTime * time2);
char * getTime(void);

void signal_segment(void const *n);

void hundredths_thread (void const *args);
void seconds_thread (void const *args);
void minutes_thread (void const *args);

/****************************************************************************
 *                     EXPORTED FUNCTION DEFINITIONS                        *
 ****************************************************************************/
//int MAIN(int argc, char *argv[])
int MAIN(void)
{

	//Start each thread here, 
	Thread hun(hundredths_thread);
	Thread sec(seconds_thread);	
	Thread min(minutes_thread);
	
	hun.set_priority(osPriorityNormal);
	sec.set_priority(osPriorityAboveNormal);
	min.set_priority(osPriorityHigh);
	
	
	
	//shared variables: stopwatch (1, 2, 3, 4, 5), currentTime(1,2,3,4)(should just be stopwatch, no? cause it returns the time),
	// or should it just be time, and time gets updated by a separate thread
	
	//========================================================================
	RtosTimer _hun(signal_segment, osTimerPeriodic, (void *) &hun);
	RtosTimer _sec(signal_segment, osTimerPeriodic, (void *) &sec);
	RtosTimer _min(signal_segment, osTimerPeriodic, (void *) &min);
	
	timer_hun = &_hun;
	timer_sec = &_sec;
	timer_min = &_min;
	
		
	//timer_hun.start(10);
	//timer_sec.start(1000);
	//timer_min.start(60000);
	
	
   // StopWatchTime lcdTime;
   // StopWatchTime currentTime;
    Serial pc(USBTX, USBRX);

    //int input = 5;
	//int click = 0;
	
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
	      			//need to know the state of the time 
	      			//if was paused, have to start the timers with a smaller countdown
	      			int diff = 0;
	      			if (was_paused){
	      				//figure out that smaller countdown time	
	      				diff = t.read_ms(); //elapsed time from start in milliseconds
	      				//pc.printf("Elapsed time: %i\n\r", diff);
	      				was_paused = 0;
	      			}
	      			stopWatch->Start();
	      			timer_hun->start(10 - (diff%10));
	      			timer_sec->start(1000 - (diff%1000));
	      			timer_min->start(60000 - (diff%60000));
	      			t.start(); //start counting now
					break;
	      		case 'p' :
	      			timer_hun->stop();
					timer_sec->stop();
					timer_min->stop();
					stopWatch->Stop();
					t.stop(); //pause stopwatch --> stop counting, now t.read has amount of time counted
					was_paused = 1;
					//pc.printf("paused\n\r");
					break;
					
	      		case 'r' :
					stopWatch->Reset();
					//pc.printf("reset\n\r");
					break;
   				}
           }
	}

   return 0; 
}

/****************************************************************************
 *                     PRIVATE FUNCTION DEFINITIONS                         *
 ****************************************************************************/

/*******************************THREADS**************************************/
void signal_segment(void const * tptr)
{
	((Thread *)tptr)->signal_set(0x1);

}

void hundredths_thread (void const *args)
{
	//stopwatch_mutex.lock();
	while (true)
	{
		Thread::signal_wait(0x1);
		
		stopwatch_mutex.lock();
		stopWatch->Tick(0);
		stopwatch_mutex.unlock();
		
		//restart timer correctly
		//if (update_hun_timer)
		//{
			timer_hun->stop();
			timer_hun->start(10);
			//update_hun_timer = 0;
		//}
	}
	
	
	
	//stopwatch_mutex.unlock();	
}

void seconds_thread (void const *args)
{
	while (true)
	{
		//update seconds
		Thread::signal_wait(0x1);
		
		stopwatch_mutex.lock();	
		stopWatch->Tick(1);
		stopwatch_mutex.unlock();
		
		//if(update_sec_timer)
		//{
			timer_sec->stop();
			timer_sec->start(1000);
			//update_sec_timer = 0;	
		//}
	}	
}

void minutes_thread(void const *args)
{
	while(true)
	{
		//update minutes	
		Thread::signal_wait(0x1);
		
		stopwatch_mutex.lock();
		stopWatch->Tick(2);
		stopwatch_mutex.unlock();
		
		
		//if (update_min_timer)
		//{
			timer_min->stop();
			timer_min->start(60000);
			//update_min_timer = 0;	
		//}
	}	
}

//normal functions

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
