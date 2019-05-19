//
// nav429.cpp: library for nav429 shield from http://naveol.com
//

#include "nav429.h"

//...............................................................................
// nav429 shield initialization
void nav429Setup() {
   // in/out pins configuration + outputs default states
   pinMode(SS,   OUTPUT); digitalWrite(SS, HIGH);
   pinMode(OE,   OUTPUT); digitalWrite(OE, HIGH);
   pinMode(R1Int, INPUT);
   pinMode(R2Int, INPUT);

   // initialize SPI
   SPI.begin();
   SPI.beginTransaction(HI3593SpiSettings);

   // initialize HI-3593
   digitalWrite(SS, LOW); // ACLK
   SPI.transfer(0x38);
   SPI.transfer(ClkDiv);
   digitalWrite(SS, HIGH);
    
   digitalWrite(SS, LOW); // TMODE
   SPI.transfer(0x08);
   SPI.transfer(0x20);
   digitalWrite(SS, HIGH);
}
//...............................................................................
// reads an ARINC word from nav429 shield channel #1 or #2 (resp.: ch=0 or ch=1)
void nav429ReadArincRx(char ch, char* dest) {
   digitalWrite(SS, LOW);
   SPI.transfer(ch==0 ? 0xA0 : 0xC0);
   for (int i=0; i<4; i++) dest[i] = SPI.transfer(0);
   digitalWrite(SS, HIGH);
}
//...............................................................................
// writes an ARINC word to nav429 shield for emission
void nav429WriteArincTx(char* src) {
   digitalWrite(SS, LOW);
   SPI.transfer(0x0C);
   for (int i=0; i<4; i++) SPI.transfer(src[i]);
   digitalWrite(SS, HIGH);
}
//...............................................................................
