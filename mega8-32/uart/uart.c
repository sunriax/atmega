/* -----------------------------------------
 * G.Raf^engineering
 * www.sunriax.at
 * -----------------------------------------
 *    Platform: Megacard
 *    Hardware: ATmega16
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
    #if !defined(UART_TXCIE) && !defined(UART_UDRIE) && !defined(UART_RXCIE) && UART_STDMODE == 1
        static FILE std_uart = FDEV_SETUP_STREAM(uart_printf, uart_scanf, _FDEV_SETUP_RW);
    #elif !defined(UART_TXCIE) && !defined(UART_UDRIE) && UART_STDMODE == 2
        static FILE std_uart = FDEV_SETUP_STREAM(uart_printf, NULL, _FDEV_SETUP_WRITE);
    #elif !defined(UART_RXCIE) && UART_STDMODE == 3
        static FILE std_uart = FDEV_SETUP_STREAM(NULL, uart_scanf, _FDEV_SETUP_READ);
    #endif

#endif

//  +---------------------------------------------------------------+
//  |                       UART initialization                     |
//  +---------------------------------------------------------------+
void uart_init(void)
{   
    // Check which bit sampling mode should be activated
    #if USE_2X
        UCSRA |= (1<<U2X);          // Setup 8 samples/bit
    #else
        UCSRA &= ~(1<<U2X);         // Setup 16 samples/bit
    #endif

    UBRRH = UBRRH_VALUE;            // Calculated through setbaud.h
    UBRRL = UBRRL_VALUE;            // Calculated through setbaud.h

    unsigned char SETREG = (1<<URSEL);  // Activate URSEL (normally in register UCSRC)
    
    SETREG |= (0x06 & ((UART_DATASIZE - 5)<<1));    // Setup data size
    
    #if UART_PARITY > 0
        SETREG |= (0x30 & ((UART_PARITY + 1)<<4));  // Parity Mode
    #endif
    
    #if UART_STOPBITS > 1
        SETREG |= (0x08 & (UART_STOPBITS<<3));      // Setup stop bits
    #endif
    
    UCSRC = SETREG;                 // Write SETREG settings to UCSRC
    UCSRB = (1<<RXEN) | (1<<TXEN);  // Activate UART transmitter and receiver

    // Interrupt control
    
    // Receiver interrupt setup
    #ifdef UART_RXCIE
        UCSRB |= (1<<RXCIE);
    #endif

    // Transmitter interrupt setup
    #if defined(UART_TXCIE) && !defined(UART_UDRIE)
        UCSRB |= (1<<TXCIE);
    #endif

    // Transmitter interrupt setup
    #if !defined(UART_TXCIE) && defined(UART_UDRIE)
        UCSRB |= (1<<UDRIE);
    #endif

    #if !defined(UART_TXCIE) && !defined(UART_UDRIE) && (UART_STDMODE == 1 || UART_STDMODE == 2)
        stdout = &std_uart;
    #endif
    
    #if !defined(UART_RXCIE) && UART_STDMODE == 1 || UART_STDMODE == 3
        stdin = &std_uart;
    #endif
}

//  +---------------------------------------------------------------+
//  |                   UART send character                         |
//  +---------------------------------------------------------------+
//  |    Return:    None=0  -> No error ocurred                     |
//  |               Frame   -> Error in frame                       |
//  |               Overrun -> Data overrun during transmission     |
//  |               Parity  -> Parity error                         |
//  +---------------------------------------------------------------+
UART_Error uart_error_flags(void)
{
    // Frame error
    if(UCSRA & (1<<FE))
    {
        UDR;           // Clear UART data register
        return Frame;   // Return NUL
    }
    // Data Overrun error
    else if(UCSRA & (1<<DOR))
    {
        UDR;           // Clear UART data register
        return Overrun; // Return NUL
    }
    // Parity error
    else if(UCSRA & (1<<UPE))
    {
        UDR;           // Clear UART data register
        return Parity;  // Return NUL
    }
    return None;
}

#if !defined(UART_TXCIE) && !defined(UART_UDRIE)

    //  +---------------------------------------------------------------+
    //  |                   UART send character                         |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data    ->  ASCII character/Binary Number       |
    //  |                                                               |
    //  |    Return:    0x00    ->  Transmission complete               |
    //  +---------------------------------------------------------------+
    char uart_putchar(char data)
    {
        // Wait until last transmission completed
        while(!(UCSRA & (1<<UDRE)));
        
        UDR = data; // Write data to transmission register
        
        // C99 functions needs an int as a return parameter
        return 0;   // Return that there was no fault
    }
    
    #if (UART_STDMODE == 1 || UART_STDMODE == 2)
        //  +---------------------------------------------------------------+
        //  |                   UART send character                         |
        //  +---------------------------------------------------------------+
        //  | Parameter:    data         ->  ASCII character                |
        //  |               stream (ptr) ->  Filestream                     |
        //  |                                                               |
        //  |    Return:    0x00    ->  Transmission complete               |
        //  +---------------------------------------------------------------+
        int uart_printf(char data, FILE *stream)
        {
            return uart_putchar(data);
        }
    #endif

#endif

#if !defined(UART_RXCIE)

    UART_Data uart_scanchar(char *data)
    {
        // If data has been received
        if((UCSRA & (1<<RXC)))
        {
            // Check if an Error ocurred
            if(uart_error_flags() != None)
            {
                UDR;           // Clear UDR0 Data register
                *data = 0;
                return Error;
            }
            
            *data = UDR;
            
            #if defined(UART_RXC_ECHO) && !defined(UART_TXCIE) && !defined(UART_UDRIE)
                // Send echo of received data to UART
                uart_putchar(*data);
            #endif
            
            return Received;
        }
        return Empty;
    }
    
    //  +---------------------------------------------------------------+
    //  |                   UART receive character                      |
    //  +---------------------------------------------------------------+
    //  | Parameter:    status (ptr) -> Pointer to return status        |
    //  |                            -> Empty                           |
    //  |                            -> Received                        |
    //  |                            -> Error                           |
    //  |                                                               |
    //  |    Return:    0x??    ->  data/NUL                            |
    //  +---------------------------------------------------------------+
    char uart_getchar(UART_Data *status)
    {
        UART_Data temp;
        char data;
        
        // Wait until data has been received
        do
        {
            temp = uart_scanchar(&data);
        } while (temp == Empty);
        
        *status = temp;
        return data;
    }
    
    #if (UART_STDMODE == 1 || UART_STDMODE == 3)
        //  +---------------------------------------------------------------+
        //  |                   UART receive character                      |
        //  +---------------------------------------------------------------+
        //  | Parameter:    stream (ptr)    ->  Datastream                  |
        //  |                                                               |
        //  |    Return:    0x00??  ->  data/NUL                            |
        //  +---------------------------------------------------------------+
        int uart_scanf(FILE *stream)
        {
            return (int)uart_getchar(NULL);
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

#endif