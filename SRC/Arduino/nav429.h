//
// nav429.h: library for NAV429 shield from http://naveol.com
//

#include <SPI.h>

// clock divider
#define ClkDiv 0x10 // 16MHz => devide by 16 (12 for NAV429 proto's)

// pins numbers for NAV429 interface
#define SS     10  // SPI Slave Select
#define OE      9  // Output Enable of voltage translator (TXB0104)
#define R1Int   3  // HI-3593 Receiver #1 interrupt
#define R2Int   2  // HI-3593 Receiver #2 interrupt

// SPI settings as required by HI3593
const SPISettings HI3593SpiSettings(10000000, MSBFIRST, SPI_MODE0);

void nav429Setup();
void nav429ReadArincRx(char ch, char* dst); // ch=0 for channel #1, ch=1 for channel #2
void nav429WriteArincTx(char* src);
