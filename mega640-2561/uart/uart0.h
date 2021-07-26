/* -----------------------------------------
 * G.Raf^engineering
 * www.sunriax.at
 * -----------------------------------------
 *    Platform: STK600
 *    Hardware: ATmega2560
 * -----------------------------------------
 *     Version: 1.0 Release
 *      Author: G.Raf
 * Description:
 *   Header file for uart library
 * -----------------------------------------
 */

#ifndef UART0_H_
#define UART0_H_

#ifndef UART0_BAUD          // Transmission frequency in bits/s
    #define UART0_BAUD 9600UL
    
    #ifndef UART0_USE2X     // Double speed mode
                            // 0 = Disabled
                            // 1 = Enabled
        #define UART0_USE2X 1
    #endif
#endif

#ifndef UART0_DATASIZE       // Setup 5 to 8
    #define UART0_DATASIZE 8
#endif


#ifndef UART0_PARITY         // Setup 0 = none | 1 = even | 2 = odd
    #define UART0_PARITY 0
#endif


#ifndef UART0_STOPBITS       // Setup 1 - 2
    #define UART0_STOPBITS 1
#endif

#ifndef UART0_RXC_ECHO       // Setup if echo occurs on data receiving
    #define UART0_RXC_ECHO
#endif

// Definition of UART processing (Enabled = Interrupt/Disabled = Polling)

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Interrupts are not included in this library !!!
// !!! If interrupts are used, disable the polling !!!
// !!! by uncommenting the next lines              !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//#ifndef UART0_RXCIE // Receiver Interrupt based processing
  //#define UART0_RXCIE
//#endif
//
//#ifndef UART0_TXCIE // Transmitter Interrupt based processing
  //#define UART0_TXCIE
//#endif
//
//#ifndef UART0_UDRIE // Transmitter Interrupt based processing
  //#ifndef UART0_TXCIE
      //#define UART0_UDRIE
  //#else
      //#undef UART0_UDRIE
  //#endif
//#endif

#define UART0_UBRR ((F_CPU / ((1<<(4 - UART0_USE2X)) * UART0_BAUD)) - 1UL)

#include "uart.h"

#if defined(UART0_RXCIE) || defined(UART0_TXCIE) || defined(UART0_UDRIE)
    #include <avr/interrupt.h>
#endif

      void uart0_init(void);
UART_Error uart0_error_flags(void);

#if !defined(UART0_TXCIE) && !defined(UART0_UDRIE)
       char uart0_putchar(char data);
       
    #if (UART_STDMODE == 1 || UART_STDMODE == 2) && UART_HARDWARE == 0
        int uart0_printf(char data, FILE *stream);
    #endif
#endif

#if !defined(UART0_RXCIE)
      char uart0_getchar(UART_Data *status);
 UART_Data uart0_scanchar(char *data);
 
    #if (UART_STDMODE == 1 || UART_STDMODE == 3) && UART_HARDWARE == 0
       int uart0_scanf(FILE *stream);
    #endif
#endif

#endif /* UART_H_ */