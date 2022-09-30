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
#include "SPI.h"

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
#define D7 RE0

/*
 * Variables
 */
char val, frow[20], srow[20];
int req = 10, mov = 0, temp = 24,hrs = 15, mins = 15, lux = 0;
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
        read_red();
    }
    if (PIR1bits.SSPIF){
        req = spiRead(); //Se lee requerimiento del Master
        switch (req){
            case 0: // Al recbir un 0 -> Se carga temperatura
                spiWrite(temp);
                break;
            case 1: // Al recbir un 1 -> Se carga movimiento
                spiWrite(mov);
                break;
            case 2: // Al recbir un 2 -> Se carga minutos
                spiWrite(mins); 
                break;
            case 3: // Al recbir un 3 -> Se carga horas
                spiWrite(hrs);
                break;
            case 4: // Al recbir un 4 -> Se carga lux
                spiWrite(lux);
                break;
            default:
                break;
        }
        
        PIR1bits.SSPIF = 0;
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
        sprintf(frow, "%02d:%02d    Luz: %02d", hrs,mins,lux);
        sprintf(srow, "Mov: %d  Temp: %02d",mov,temp);
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String(frow);
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(srow);
        
        if(lux <= 6)
            PORTB = 0xFF;
        else
            PORTB = 0x00;
        
    }
    return;
}

/*
 * Funciones
 */
void read_red(void){
    switch(val & 224){
        case 128:
            mov = val & 31;
            break;
        case 160:
            temp = val & 31;
            break;
        case 192:
            lux = val & 31;
            break;
        case 224:
            hrs = val & 31;
            break;
        default:
            mins = val & 31;
            break;
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
    
    TRISAbits.TRISA0 = 0; //RA0 -> Output
    TRISAbits.TRISA1 = 0; //RA1 -> Output
    TRISAbits.TRISA2 = 0; //RA2 -> Output
    TRISAbits.TRISA3 = 0; //RA3 -> Output
    TRISAbits.TRISA4 = 0; //RA4 -> Output
    TRISAbits.TRISA5 = 1; //RA5 -> Input
    PORTAbits.RA5 = 0;
    
    TRISB = 0; //PORTB -> Output 
    
    TRISEbits.TRISE0 = 0; //RE0 -> Output
    
    Lcd_Init(); //Inicialización LCD modo 4 bits
    USART_set(9600); //Baudaje de ~9600
    
    // SS activo, SMP a la mitad del tiempo, CKP en low, CKE en flanco positivo
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
        
    //Interrupciones
    INTCONbits.GIE = 1; //Int. Globales
    INTCONbits.PEIE = 1; //Int. de periféricos
    PIR1bits.SSPIF = 0;  // Clear -> SPI Int. Flag
    PIE1bits.SSPIE = 1;  // Enable -> Int. SPI
    
    return;
}