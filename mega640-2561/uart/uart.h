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

#ifndef UART_H_
#define UART_H_

#ifndef F_CPU                       // System clock
    #define F_CPU 16000000UL
#endif

// Definition of UART standard output mode (printf/scanf)
#ifndef UART_STDMODE                // Standard Mode
    #define UART_STDMODE 1          // 0 = None
                                    // 1 = printf/scanf
                                    // 2 = printf
                                    // 3 = scanf
#endif

// Definition of used UART hardware for standard output
#if UART_STDMODE > 0
    #ifndef UART_HARDWARE           // Standard UART Interface
        #define UART_HARDWARE 0     // 0 = UART0 (Standard)
                                    // 1 = UART1
                                    // 2 = UART2
                                    // 3 = UART3
    #endif
#endif

enum UART_Data_t
{
    UART_Empty=0,
    UART_Received,
    UART_Fault
};
typedef enum UART_Data_t UART_Data;

enum UART_Error_t
{
    UART_None=0,
    UART_Frame,
    UART_Overrun,
    UART_Parity
};
typedef enum UART_Error_t UART_Error;

#include <stdio.h>
#include <avr/io.h>

#if UART_STDMODE > 0

    #if UART_HARDWARE == 1
        #include "uart1.h"
    #elif UART_HARDWARE == 2
        #include "uart2.h"
    #elif UART_HARDWARE == 3
        #include "uart3.h"
    #else
        #include "uart0.h"
    #endif
    
    void uart_init(void);

    #if UART_STDMODE == 1 || UART_STDMODE == 2
        int uart_printf(char data, FILE *stream);
    #endif

    #if UART_STDMODE == 1 || UART_STDMODE == 3
         int uart_scanf(FILE *stream);
        void uart_clear(void);
    #endif
#endif

#endif /* UART_H_ */