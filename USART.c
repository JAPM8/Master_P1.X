/* 
 *  USART.h - Source file of PIC MPLABX USART library
 *  Written by Ercan Ersoy.
 *  Modified by Javier A. P�rez Mar�n
 */

#include <xc.h>
#include "USART.h"

#define _XTAL_FREQ 1000000

void USART_set(const unsigned long int baudrate)
{
    
    TXSTAbits.BRGH = 1; //High Speed Baud Rate
    BAUDCTLbits.BRG16 = 1; //Generador de Baud Rate de 16 bits
    SPBRGH = 0; 
    SPBRG = (_XTAL_FREQ - baudrate * 4) / (baudrate * 4); //Se carga Baud rate
    TXSTAbits.SYNC = 0; //Modo as�ncrono
    RCSTAbits.SPEN = 1; //Comunicaci�n Serial habilitada
    
    PIE1bits.RCIE = 1; //Se habilitan interrupciones de recepci�n UART
    
    //Recepci�n de 8 bits
    RCSTAbits.RX9 = 0; 

    RCSTAbits.CREN = 1;//Receptor     S
}

char USART_read(void)
{
    return RCREG;
}




