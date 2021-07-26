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

#ifndef UART2_H_
#define UART2_H_

#ifndef UART2_BAUD          // Transmission frequency in bits/s
    #define UART2_BAUD 2400UL

    #ifndef UART2_USE2X     // Double speed mode
                            // 0 = Disabled
                            // 1 = Enabled
        #define UART2_USE2X 1
    #endif
#endif

#ifndef UART2_DATASIZE       // Setup 5 to 8
    #define UART2_DATASIZE 8
#endif


#ifndef UART2_PARITY         // Setup 0 = none | 1 = even | 2 = odd
    #define UART2_PARITY 0
#endif


#ifndef UART2_STOPBITS       // Setup 1 - 2
    #define UART2_STOPBITS 1
#endif

#ifndef UART2_RXC_ECHO       // Setup if echo occurs on data receiving
    #define UART2_RXC_ECHO
#endif

// Definition of UART processing (Enabled = Interrupt/Disabled = Polling)

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Interrupts are not included in this library !!!
// !!! If interrupts are used, disable the polling !!!
// !!! by uncommenting the next lines              !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//#ifndef UART2_RXCIE // Receiver Interrupt based processing
  //#define UART2_RXCIE
//#endif
//
//#ifndef UART2_TXCIE // Transmitter Interrupt based processing
  //#define UART2_TXCIE
//#endif
//
//#ifndef UART2_UDRIE // Transmitter Interrupt based processing
  //#ifndef UART2_TXCIE
      //#define UART2_UDRIE
  //#else
      //#undef UART2_UDRIE
  //#endif
//#endif

#define UART2_UBRR ((F_CPU / ((1<<(4 - UART2_USE2X)) * UART2_BAUD)) - 1UL)

#include "uart.h"

#if defined(UART2_RXCIE) || defined(UART2_TXCIE) || defined(UART2_UDRIE)
    #include <avr/interrupt.h>
#endif

      void uart2_init(void);
UART_Error uart2_error_flags(void);

#if !defined(UART2_TXCIE) && !defined(UART2_UDRIE)
       char uart2_putchar(char data);
       
    #if (UART_STDMODE == 1 || UART_STDMODE == 2) && UART_HARDWARE == 2
        int uart2_printf(char data, FILE *stream);
    #endif
#endif

#if !defined(UART2_RXCIE)
      char uart2_getchar(UART_Data *status);
 UART_Data uart2_scanchar(char *data);
 
    #if (UART_STDMODE == 1 || UART_STDMODE == 3) && UART_HARDWARE == 2
       int uart2_scanf(FILE *stream);
    #endif
#endif

#endif /* UART_H_ */