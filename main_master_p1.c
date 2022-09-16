/*
 * File:   main_master_p1.c
 * Author: Javier Alejandro Pérez Marín y Jorge Ricardo Cerón Cheley
 * 
 * Programación PIC maestro, este maneja la comunicación entre pics como con el
 * ESP e imprime en la LCD el estado de los sensores.
 *
 * Created on 9 de septiembre de 2022, 02:18 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR21V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*
 * Librerias
 */
#include <xc.h>
#include <pic16f887.h>
#include <stdint.h>
#include <stdio.h> 
#include "osc.h"
#include "USART.h"
#include "LCD.h"

/*
 * Constantes
 */
#define _XTAL_FREQ 1000000 //Osc a 1 MHz

 //Pines LCD
#define RS RA0
#define EN RA1
#define D4 RA2
#define D5 RA3
#define D6 RA4
#define D7 RA5

/*
 * Variables
 */
char val, frow[20], srow[20];
int mov = 0, temp = 24;
/*
 * Prototipos de Función
 */
void setup(void); //Función para configuraciones
void read_red(void);
/*
 * Interrupciones
 */
void __interrupt() master(void){
    //Interrupción UART
    if(PIR1bits.RCIF){ //Se verifica si hay un nuevo dato en el serial
        val = USART_read();
    }
    return;
}

/*
 * Loop principal
 */
void main(void) {
    setup();
    Lcd_Clear();
    while(1){
        read_red();
        sprintf(frow, "%d%d:%d%d    Luz: %2d", 2,3,4,2,95);
        sprintf(srow, "Mov: %d  Temp: %2d",mov,temp);
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String(frow);
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(srow);
    }
    return;
}

/*
 * Funciones
 */
void read_red(void){
    if ((val>>1)== 'M'){
        mov = val & 0x01;
    }
    else{
        temp = val;
    }
    return;
}

/*
 * Configuraciones
 */
void setup(void){
    int_osc_MHz(1); //OSC a 1 MHz
    
    //I/O DIGITALES
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0; //PORTA -> Output
    TRISB = 0;
    
    Lcd_Init(); //Inicialización LCD modo 4 bits
    USART_set(9600); 
    
    //Interrupciones
    INTCONbits.GIE = 1; //Int. Globales
    INTCONbits.PEIE = 1; //Int. de periféricos
    return;
}