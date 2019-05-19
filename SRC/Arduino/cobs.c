/******************************************************************************
 * cobs.c : (Consistant Overhead Byte Stuffing) transcodage d'une chaine.     *
 *      https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing       *
 * Permet d'utiliser le caractere NULL comme delimiteur de trames pour une    *
 * transmission sur RS232.                                                    *
 ******************************************************************************/

#include <string.h>

//------------------------------------------------------------------------------
// requires that src[len] == '\0' otherwise it will crash
void cobs_cod(char* src, unsigned char len, char* dst) {
   char *s = src, *end = src + len;
   do {
      while (*src != '\0') src++;
      unsigned char len = (unsigned char)(src - s);
      *dst++ = (unsigned char)(len + 1);
      strcpy(dst, s);
      dst += len;
     s = ++src;
   } while (src <= end);
}
//------------------------------------------------------------------------------
void cobs_dec(char* src, unsigned char len, char* dst) {
   char* end = src + len;
   while (src < end) {
      unsigned char i, c = *src++;
      for (i = 1; i < c; i++) *dst++ = *src++;
      if (c < 0xff) *dst++ = 0;
   }
}
//------------------------------------------------------------------------------
