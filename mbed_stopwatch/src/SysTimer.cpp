/************************************************************************//**
 *
 * \file SysTimer.c
 *
 * \addtogroup SysTimer SysTimer
 * \{
 *
 * \brief
 *
 * \note
 *
 * \author kjshepherd ()
 * \date 2014-09-10
 *
 ****************************************************************************/

/****************************************************************************
 *                              INCLUDE FILES                               *
 ****************************************************************************/
#include "SysTimer.h"
//#include "types.h"
#include "CommonDefines.h"
#include "LPC17xx.h"


/****************************************************************************
 *                      PRIVATE TYPES and DEFINITIONS                       *
 ****************************************************************************/

/****************************************************************************
 *                              PRIVATE DATA                                *
 ****************************************************************************/
static SystemTimerDevice Timer;
static void (* timerCallback)();

/****************************************************************************
 *                             EXTERNAL DATA                                *
 ****************************************************************************/

/****************************************************************************
 *                     PRIVATE FUNCTION DECLARATIONS                        *
 ****************************************************************************/
//void set_timer(long long millisecs);
void register_callback(void (* cb)(void));
void execute_timer_callback(void);
void timer0_init(void);
/****************************************************************************
 *                     EXPORTED FUNCTION DEFINITIONS                        *
 ****************************************************************************/
SystemTimerDevice * SystemTimer_Init(void)
{
    Timer.SetTimer = set_timer;
    Timer.RegisterInterruptCallback = register_callback;
    return &Timer;
}

/****************************************************************************
 *                     PRIVATE FUNCTION DEFINITIONS                         *
 ****************************************************************************/
void set_timer(long long millisecs)
{
    //if millisecs is 0 want to disable timer
    if(millisecs == 0)
    {
        LPC_TIM0->TCR = 0; //disable timer
    }
    else
    {
    	timer0_init();
    }
}

void register_callback(void (* cb)(void))
{
    
    timerCallback = cb;
}

void execute_timer_callback(void)
{
    //
    if (timerCallback != NULL)
    {
        timerCallback();
    }
}


//
extern "C" void TIMER0_IRQHandler (void)
{
    LPC_TIM0->IR |= 1 << 0;         // Clear MR0 interrupt flag
    execute_timer_callback();
    
}

void timer0_init(void)
{
    LPC_SC->PCONP |=1<1;            //timer0 power on
    LPC_TIM0->MR0 = 239800;         //10 msec
    LPC_TIM0->MCR = 3;              //interrupt and reset control
                                    //3 = Interrupt & reset timer0 on match
                                    //1 = Interrupt only, no reset of timer0
    NVIC_EnableIRQ(TIMER0_IRQn);    //enable timer0 interrupt
    LPC_TIM0->TCR = 1;              //enable Timer0
    //pc.printf("Done timer_init\n\r");
}


/************************************************************************//**
 * \brief
 * \param
 * \return
 ****************************************************************************/

/** \}*/
