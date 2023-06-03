/******************************************************************************
 Title:    dir/pwm x analog in, analog feedback  servo loop controller.
 
 
 Author:   rue_mohr
 Date:     May 2023
 Software: AVR-GCC 3.3 
 Hardware: attiny13 @ 9.6Mhz
 
    

PB0 STEP
PB1 DIR
PB2 
PB3 
PB4 Vctrl

PB5 (RESET)   


                            +-----U-----+    
               RESET    PB5 | o         | VCC
               ADC3     PB3 |           | PB2 ADC1 
               ADC2     PB4 |   Tiny13  | PB1 OC0B
                        GND |           | PB0 OC0A
                            +-----------+    


Motor step driver set to 8 microstep.
Max step rate is  20kHz

timer 0 is used for diving the system clock to generate the step timings



    
*******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrcommon.h"
#include "stdint.h"
 
#define OUTPUT                   1
#define INPUT                    0

#define STEPBIT  0
#define DIRBIT   1

#define PWM0PORT PORTB


volatile uint8_t  waitCount;
volatile int16_t ctrl;
volatile int16_t fb;

int8_t  dir;
uint16_t rate;

void ADCInit() ;
void TimerInit() ;
void waitCalcLoop() ;
void setSpeed( int v ) ;


int main (void)  {

  int  t; // target velocity
  int  v; // current velocity
  
  // Set clock prescaler: 0 gives full 9.6 MHz from internal oscillator.
  CLKPR = (1 << CLKPCE);
  CLKPR = 0;  
  
  dir   = 0;
  rate  = 255;
         
  DDRB = (OUTPUT << DDB0 | OUTPUT << DDB1 | INPUT << DDB2 | INPUT << DDB3 | INPUT << DDB4 | INPUT << DDB5 ); 
  
  TimerInit();
  ADCInit();  
  sei();         // turn on interrupts    
  
  waitCalcLoop(); // wait for ADC to settle.
  

  
  fb = ctrl;
  v = 0;            
              
  while(1) {
    t = fb;
    t -= ctrl;    
    t /= 2;                          
    t = limit(t, -255, 255); // limit target velocity
                       
    v += limit((t - v), -1, 1); // step velocity, acceleration determined by waitCalcLoop    
    
    setSpeed( v );         

    waitCalcLoop();             
             
   }
}

    
//------------------------| FUNCTIONS |------------------------

/* OK */
void waitCalcLoop() {
  waitCount = 2;
  while (waitCount);
}


/*
  OK

  Max freq 20kHz
  Max start 9.5kHz

 v input range is -255 to 255
 
 output values are 
    dir (-1, 0, 1)
    rate (480 -> 10000) inverse for a clock divider to timer 1 (16 bit)
*/
void setSpeed( int v ) {

  dir = 1;
  if (v < 0) {
    dir = -1;
    v = -v;
  }  

  if (v == 0) {
    dir = 0;
    return;
  } 
  
  v *= 49; // not blowing the 16 bit math yet!
  v = 16335 - v;
  v /= 64;
  rate = v; 
 
}

/* OK */
void ADCInit() {

  // ADC, 9.6Mhz / 128
  
  ADMUX  = ( (1 << MUX1) ); 
  
  ADCSRA = ( (1 << ADEN) | (1 << ADIF) | (1 << ADIE) | (5 << ADPS0)  | (1 << ADATE) | (1 << ADSC)  ) ; 

} 

/*
  OK
  
  Timer 0 as a rate generator
  
*/
void TimerInit() {
  
  OCR0A  = 255;      
  
  TCCR0B = (2<<CS00); // timer 1 using /8 clock
  TCCR0A = (1<<WGM01);
  SetBit( OCIE0A, TIMSK0 ); // enable timer
    
}


// ----------------------| ISR |------------------------------

/* ok */
ISR(  ADC_vect) {    

  ctrl = ADC+512;
  ADMUX  = ( (1 << MUX1) ); 
    
  if (waitCount) waitCount--; 
}

/* OK */
// timer 1 for stepping the motor
ISR( TIM0_COMPA_vect ) { 
 
 // update timer
 OCR0A = rate;
 
 // pulse step line. 
 if (dir == 0)  return;
  
 // set up direction 
 if (dir == 1)  SetBit   ( DIRBIT, PORTB );
 else           ClearBit ( DIRBIT, PORTB );
  
 // do step
 SetBit   ( STEPBIT, PORTB );
 NOP();
 ClearBit ( STEPBIT, PORTB );
  
 // feedback
 fb -= dir; 
  
}





















































