/******************************************************************************
 * cobs.h : (Consistant Overhead Byte Stuffing) transcodage d'une chaine.     *
 *      https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing       *
 * Permet d'utiliser le caractere NULL comme delimiteur de trames pour une    *
 * transmission sur RS232.                                                    *
 ******************************************************************************/
 
// encodage
void cobs_cod(char* src, unsigned char len, char* dst);

// decodage
void cobs_dec(char* src, unsigned char len, char* dst);
