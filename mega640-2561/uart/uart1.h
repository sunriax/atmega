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

#ifndef UART1_H_
#define UART1_H_

#ifndef UART1_BAUD          // Transmission frequency in bits/s
    #define UART1_BAUD 19200UL

    #ifndef UART1_USE2X     // Double speed mode
                            // 0 = Disabled
                            // 1 = Enabled
        #define UART1_USE2X 1
    #endif
#endif

#ifndef UART1_DATASIZE       // Setup 5 to 8
    #define UART1_DATASIZE 8
#endif


#ifndef UART1_PARITY         // Setup 0 = none | 1 = even | 2 = odd
    #define UART1_PARITY 0
#endif


#ifndef UART1_STOPBITS       // Setup 1 - 2
    #define UART1_STOPBITS 1
#endif

#ifndef UART1_RXC_ECHO       // Setup if echo occurs on data receiving
    #define UART1_RXC_ECHO
#endif

// Definition of UART processing (Enabled = Interrupt/Disabled = Polling)

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Interrupts are not included in this library !!!
// !!! If interrupts are used, disable the polling !!!
// !!! by uncommenting the next lines              !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//#ifndef UART1_RXCIE // Receiver Interrupt based processing
  //#define UART1_RXCIE
//#endif
//
//#ifndef UART1_TXCIE // Transmitter Interrupt based processing
  //#define UART1_TXCIE
//#endif
//
//#ifndef UART1_UDRIE // Transmitter Interrupt based processing
  //#ifndef UART1_TXCIE
      //#define UART1_UDRIE
  //#else
      //#undef UART1_UDRIE
  //#endif
//#endif

#define UART1_UBRR ((F_CPU / ((1<<(4 - UART1_USE2X)) * UART1_BAUD)) - 1UL)

#include "uart.h"

#if defined(UART1_RXCIE) || defined(UART1_TXCIE) || defined(UART1_UDRIE)
    #include <avr/interrupt.h>
#endif

      void uart1_init(void);
UART_Error uart1_error_flags(void);

#if !defined(UART1_TXCIE) && !defined(UART1_UDRIE)
       char uart1_putchar(char data);
       
    #if (UART_STDMODE == 1 || UART_STDMODE == 2) && UART_HARDWARE == 1
        int uart1_printf(char data, FILE *stream);
    #endif
#endif

#if !defined(UART1_RXCIE)
      char uart1_getchar(UART_Data *status);
 UART_Data uart1_scanchar(char *data);
 
    #if (UART_STDMODE == 1 || UART_STDMODE == 3) && UART_HARDWARE == 1
       int uart1_scanf(FILE *stream);
    #endif
#endif

#endif /* UART_H_ */