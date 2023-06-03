#ifndef __avrcommon_h
  #define __avrcommon_h
  
  #include <stdint.h>
  
  #define INPUT 0
  #define OUTPUT 1
  
  #define Min(X,Y) ((X) < (Y) ? (X) : (Y))
  #define Max(X,Y) ((X) > (Y) ? (X) : (Y))

  #define SetBit(BIT, PORT)    (PORT |= (1<<BIT))
  #define ClearBit(BIT, PORT)  (PORT &= ~(1<<BIT))
  
  #define AssignBit(BIT, PORT, VALUE) ((VALUE == 0)?ClearBit(BIT, PORT):SetBit(BIT, PORT))
  
  #define IsHigh(BIT, PORT)    (PORT & (1<<BIT)) != 0
  #define IsLow(BIT, PORT)     (PORT & (1<<BIT)) == 0
  #define NOP()                 asm volatile ("nop"::)
  #define ABS(a)                ((a) < 0 ? -(a) : (a))
  #define SIGN(x)               ((x)==0?0:(x)>0?1:-1)
  #define limit(v, l, h)        (((v) > (h)) ? (h) : ((v) < (l)) ? (l) : (v))
  #define inBounds(v, l, h)        (((v) > (h)) ? (0) : ((v) < (l)) ? (0) : (1))
  #define inBoundsI(v, l, h)        (((v) >= (h)) ? (0) : ((v) <= (l)) ? (0) : (1))
  
  #define pulsePin(BIT, PORT)  SetBit(BIT,PORT);NOP();ClearBit(BIT,PORT)
  
  // for linear remapping of number ranges, see arduino map()
  // think of a line, between Il,Ol and Ih,Oh, this solves the y for given x position
  #define RangeRemap(v,Il,Ih,Ol,Oh) (((((v)-(Il))*((Oh)-(Ol)))/((Ih)-(Il)))+(Ol))
  
  #define IsDigit(C)  (((C)>='0') && ((C)<='9'))
  
   // for bounded ramping
  #define RampUp(k,top)  (((k)<(top))?(k++):(k+=0))
  #define RampDown(k,bot) (((k)>(bot))?(k--):(k+=0))


  #define CharIsHex(A) (( ((A)>='A')&&((A)<='F') )||( ((A)>='0')&&((A)<='9') ))?1:0
  #define NibToBin(A)  (((A)>'9')?((A)-'7'):((A)-'0'))

  #define OUTPUT             1
  #define INPUT              0

  typedef struct IOPin_s {  // use like: IOPin_t foo = { 5, PINA }; 
      uint8_t bit;
      volatile uint8_t * portPIN;
  } IOPin_t;

  #define SetPin(P, L)  ((L)?SetBit(P.bit,*((P.portPIN)+2)):ClearBit(P.bit, *((P.portPIN)+2)))
  #define GetPin(P)     (IsHigh(P.bit,*(P.portPIN)))
  #define DirPin(P, D) ((D)?SetBit(P.bit,*((P.portPIN)+1)):ClearBit(P.bit,*((P.portPIN)+1)))
    
  #define PulseHPin(P) SetPin(P, 1); NOP(); SetPin(P, 0)
  #define PulseLPin(P) SetPin(P, 0); NOP(); SetPin(P, 1)
    
#endif
