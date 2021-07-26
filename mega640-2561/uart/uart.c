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
 *   Function file for uart library
 * -----------------------------------------
 */

#include "uart.h"

#if UART_STDMODE > 0

    // Initialize FILE stream
    #if UART_STDMODE == 1
        static FILE std_uart = FDEV_SETUP_STREAM(uart_printf, uart_scanf, _FDEV_SETUP_RW);
    #elif UART_STDMODE == 2   
        static FILE std_uart = FDEV_SETUP_STREAM(uart_printf, NULL, _FDEV_SETUP_WRITE);
    #elif UART_STDMODE == 3
        static FILE std_uart = FDEV_SETUP_STREAM(NULL, uart_scanf, _FDEV_SETUP_READ);
    #endif

    //  +---------------------------------------------------------------+
    //  |                       UART initialization                     |
    //  +---------------------------------------------------------------+
    void uart_init(void)
    {
        #if UART_HARDWARE == 1
            uart1_init();
        #elif UART_HARDWARE == 2
            uart2_init();
        #elif UART_HARDWARE == 3
            uart3_init();
        #else
            uart0_init();
        #endif
    
        #if UART_STDMODE == 1 || UART_STDMODE == 2
            stdout = &std_uart;
        #endif
        
        #if UART_STDMODE == 1 || UART_STDMODE == 3
            stdin = &std_uart;
        #endif
    }


    int uart_printf(char data, FILE *stream)
    {
        #if UART_HARDWARE == 1
            return uart1_printf(data, stream);
        #elif UART_HARDWARE == 2
            return uart2_printf(data, stream);
        #elif UART_HARDWARE == 3
            return uart3_printf(data, stream);
        #else
            return uart0_printf(data, stream);
        #endif
    }

    int uart_scanf(FILE *stream)
    {
        #if UART_HARDWARE == 1
            return uart1_scanf(stream);
        #elif UART_HARDWARE == 2
            return uart2_scanf(stream);
        #elif UART_HARDWARE == 3
            return uart3_scanf(stream);
        #else
            return uart0_scanf(stream);
        #endif
    }

    //  +---------------------------------------------------------------+
    //  |                   UART error handler                          |
    //  +---------------------------------------------------------------+
    void uart_clear(void)
    {
        clearerr(stdin);    // Clear error on stream
        getchar();          // Remove character from stream
    }

#endif