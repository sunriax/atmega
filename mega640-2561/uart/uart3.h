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

#ifndef UART3_H_
#define UART3_H_

#ifndef UART3_BAUD          // Transmission frequency in bits/s
    #define UART3_BAUD 4800UL

    #ifndef UART3_USE2X     // Double speed mode
                            // 0 = Disabled
                            // 1 = Enabled
        #define UART3_USE2X 1
    #endif
#endif

#ifndef UART3_DATASIZE       // Setup 5 to 8
    #define UART3_DATASIZE 8
#endif


#ifndef UART3_PARITY         // Setup 0 = none | 1 = even | 2 = odd
    #define UART3_PARITY 0
#endif


#ifndef UART3_STOPBITS       // Setup 1 - 2
    #define UART3_STOPBITS 1
#endif

#ifndef UART3_RXC_ECHO       // Setup if echo occurs on data receiving
    #define UART3_RXC_ECHO
#endif

// Definition of UART processing (Enabled = Interrupt/Disabled = Polling)

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Interrupts are not included in this library !!!
// !!! If interrupts are used, disable the polling !!!
// !!! by uncommenting the next lines              !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//#ifndef UART3_RXCIE // Receiver Interrupt based processing
  //#define UART3_RXCIE
//#endif
//
//#ifndef UART3_TXCIE // Transmitter Interrupt based processing
  //#define UART3_TXCIE
//#endif
//
//#ifndef UART3_UDRIE // Transmitter Interrupt based processing
  //#ifndef UART3_TXCIE
      //#define UART3_UDRIE
  //#else
      //#undef UART3_UDRIE
  //#endif
//#endif

#define UART3_UBRR ((F_CPU / ((1<<(4 - UART3_USE2X)) * UART3_BAUD)) - 1UL)

#include "uart.h"

#if defined(UART3_RXCIE) || defined(UART3_TXCIE) || defined(UART3_UDRIE)
    #include <avr/interrupt.h>
#endif

      void uart3_init(void);
UART_Error uart3_error_flags(void);

#if !defined(UART3_TXCIE) && !defined(UART3_UDRIE)
       char uart3_putchar(char data);
       
    #if (UART_STDMODE == 1 || UART_STDMODE == 2) && UART_HARDWARE == 3
        int uart3_printf(char data, FILE *stream);
    #endif
#endif

#if !defined(UART3_RXCIE)
      char uart3_getchar(UART_Data *status);
 UART_Data uart3_scanchar(char *data);
 
    #if (UART_STDMODE == 1 || UART_STDMODE == 3) && UART_HARDWARE == 3
       int uart3_scanf(FILE *stream);
    #endif
#endif

#endif /* UART_H_ */