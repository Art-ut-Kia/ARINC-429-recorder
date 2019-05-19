//////////////////////////////////////////////////////////////////////////////////////////
//                                     Arinc2Uart.ino                                   //
//////////////////////////////////////////////////////////////////////////////////////////
// - Acquisition of 2 ARINC429 lines via a NAV429 shield (http://naveol.com)            //
// - Each received ARINC word is transmitted over the arduino's UART-TX.                //
//   The following UART protocol is implemented:                                        //
//      o 2Mbit/s, 1 start, 8 data, no parity, 1 stop                                   //
//      o Each ARINC word is translated into an 8-byte payload packet:                  //
//        . 1st 4 bytes contain a 31-bit time stamp in µs (rollover each 2^31µs = 36')  //
//        . 32nd bit (MSB) is the ARINC channel from which the A429 word was received   //
//        . last 4 bytes contain the A429 word                                          //
//        . byte order convention is little endian (LSByte transmitted 1st)             //
//      o Byte stuffing is applied according to COBS algorithm:                         //
//            https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing           //
//        Total packet size is 10 bytes = 100 bits. At 2Mb/s, may achieve 20k packet/s  //
//        This is enough for transferring 2xA429 lines (i.e. 200kbps/36 = 5.55k word/s) //
// - A test funcion is implemented. When enabled, an ARINC word is cyclically emitted.  //
//   NAV429 ARINC output can then be looped back to both ARINC inputs for self-testing. //
//////////////////////////////////////////////////////////////////////////////////////////

#define selfTest // uncomment to enable cyclic transmission of an ARINC word on NAV429 TX
 
#include "nav429.h"
extern "C" {
#include "cobs.h" // COBS library is pure C
}

// interrupt service routines for A429 receive channel #1 and #2
typedef struct { 
  unsigned long int rxTime; // channel ID + time stamp
  char arincWord[4];        // 4-byte buffer for A429 word
  char trailingZero;        // trailing zero to prepare COBS encoding
} packet; // payload of the message to be transmitted over UART (unstuffed)

packet pkt1, pkt2; 

volatile bool rxd1; // semaphore
void isr1() {
  pkt1.rxTime = micros() & 0x7FFFFFFF; // MSB (channel ID) forced to 0
  rxd1 = true;
}

volatile bool rxd2; // semaphore
void isr2() {
  pkt2.rxTime = micros() | 0x80000000; // MSB (channel ID) forced to 1
  rxd2 = true;
}

// interrupt service routine for transmission event (from TIMER #2 overflow)
volatile bool txReq; // semaphore
ISR(TIMER2_OVF_vect) {
   txReq = true;
}

// initializations
void setup() {
   Serial.begin(2000000); // open serial port

   nav429Setup(); // initialize nav429 shield

   // configure timer #2 for 16.384ms
   TIMSK2 &= 0x01; // enable overflow interrupt (set TOIE bit)
   TCCR2B = TCCR2B | 0x07; // CS2 = 7 => 16MHz/256 / 8 /2^7 = 61Hz

   // configure interrupts and semaphores
   attachInterrupt(digitalPinToInterrupt(R1Int), isr1, RISING);
   attachInterrupt(digitalPinToInterrupt(R2Int), isr2, RISING);
   rxd1 = false; rxd2 = false; txReq = false;

   // prepares COBS transcoding
   pkt1.trailingZero = 0; pkt2.trailingZero = 0;
}

char txbf[10] = {0,0,0,0,0,0,0,0,0,0}; // TX buffer (COBS result)

void loop() {
   #ifdef selfTest
   if (txReq) {                               // emission process
      char arincWord[4] = {1, 2, 3, 4};
      nav429WriteArincTx(arincWord);
      txReq = false;
   }
   #endif

   if (rxd1) {                           // channel #1 RX process
      nav429ReadArincRx(1, pkt1.arincWord);
      cobs_cod((char*)&pkt1, 8, txbf);
      Serial.write(txbf, 10);
      rxd1 = false;
   }

   if (rxd2) {                           // channel #2 RX process
      nav429ReadArincRx(2, pkt2.arincWord);
      cobs_cod((char*)&pkt2, 8, txbf);
      Serial.write(txbf, 10);
      rxd2 = false;
   }
}
