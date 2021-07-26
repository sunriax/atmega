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

#include "uart0.h"

//  +---------------------------------------------------------------+
//  |                       UART initialization                     |
//  +---------------------------------------------------------------+
void uart0_init(void)
{   
    // Check which bit sampling mode should be activated
    #if UART0_USE2X
        UCSR0A |= (1<<U2X0);                        // Setup 8 samples/bit
    #else
        UCSR0A &= ~(1<<U2X0);                       // Setup 16 samples/bit
    #endif
    
    // Setup baudrate register (HIGH/LOW)
    UBRR0H = (unsigned char)(UART0_UBRR>>8);
    UBRR0L = (unsigned char)(UART0_UBRR);
    
    UCSR0C = (0x06 & ((UART0_DATASIZE - 5)<<1));	    // Setup Datasize
    
    #if UART0_PARITY > 0
        UCSR0C |= (0x30 & ((UART0_PARITY + 1)<<4));  // Parity Mode
    #endif
    
    #if UART0_STOPBITS > 1
        UCSR0C |= (0x08 & (UART0_STOPBITS<<3));      // Setup stop bits
    #endif
    
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);  // Activate UART transmitter and receiver

    // Interrupt control
    
    // Receiver interrupt setup
    #ifdef UART0_RXCIE
        UCSR0B |= (1<<RXCIE0);
    #endif

    // Transmitter interrupt setup
    #if defined(UART0_TXCIE) && !defined(UART0_UDRIE)
        UCSR0B |= (1<<TXCIE0);
    #endif

    // Transmitter interrupt setup
    #if !defined(UART0_TXCIE) && defined(UART0_UDRIE)
        UCSR0B |= (1<<UDRIE0);
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
UART_Error uart0_error_flags(void)
{
    // Frame error
    if(UCSR0A & (1<<FE0))
    {
        UDR0;           // Clear UART data register
        return Frame;   // Return NUL
    }
    // Data Overrun error
    else if(UCSR0A & (1<<DOR0))
    {
        UDR0;           // Clear UART data register
        return Overrun; // Return NUL
    }
    // Parity error
    else if(UCSR0A & (1<<UPE0))
    {
        UDR0;           // Clear UART data register
        return Parity;  // Return NUL
    }
    return None;
}

#if !defined(UART0_TXCIE) && !defined(UART0_UDRIE)

    //  +---------------------------------------------------------------+
    //  |                   UART send character                         |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data    ->  ASCII character/Binary Number       |
    //  |                                                               |
    //  |    Return:    0x00    ->  Transmission complete               |
    //  +---------------------------------------------------------------+
    char uart0_putchar(char data)
    {
        // Wait until last transmission completed
        while(!(UCSR0A & (1<<UDRE0)));
    
        UDR0 = data; // Write data to transmission register
    
        // C99 functions needs an int as a return parameter
        return 0;   // Return that there was no fault
    }
    
    #if (UART_STDMODE == 1 || UART_STDMODE == 2) && UART_HARDWARE == 0
        //  +---------------------------------------------------------------+
        //  |                   UART send character                         |
        //  +---------------------------------------------------------------+
        //  | Parameter:    data         ->  ASCII character                |
        //  |               stream (ptr) ->  Filestream                     |
        //  |                                                               |
        //  |    Return:    0x0000  ->  Transmission complete               |
        //  +---------------------------------------------------------------+
        int uart0_printf(char data, FILE *stream)
        {
            return uart0_putchar(data);
        }
    #endif

#endif

#if !defined(UART0_RXCIE)
    //  +---------------------------------------------------------------+
    //  |               UART receive character (non blocking)           |
    //  +---------------------------------------------------------------+
    //  | Parameter:    0x??        -> Data buffer variable             |
    //  |                                                               |
    //  |    Return:    Empty       -> No data in received              |
    //  |               Received    -> Data received                    |
    //  |               Error       -> Error during transmission        |
    //  +---------------------------------------------------------------+
    UART_Data uart0_scanchar(char *data)
    {
        // If data has been received
        if((UCSR0A & (1<<RXC0)))
        {
            // Check if an Error ocurred
            if(uart0_error_flags() != None)
            {
                UDR0;           // Clear UDR0 Data register
                *data = 0;
                return Error;
            }
        
            *data = UDR0;
        
            #if defined(UART0_RXC_ECHO) && !defined(UART0_TXCIE) && !defined(UART0_UDRIE)
                // Send echo of received data to UART
                uart0_putchar(*data);
            #endif
        
            return Received;
        }
        return Empty;
    }
    
    //  +---------------------------------------------------------------+
    //  |               UART receive character (blocking)               |
    //  +---------------------------------------------------------------+
    //  | Parameter:    status (ptr) -> Pointer to return status        |
    //  |                            -> Empty                           |
    //  |                            -> Received                        |
    //  |                            -> Error                           |
    //  |                                                               |
    //  |    Return:    0x??    ->  data/NUL                            |
    //  +---------------------------------------------------------------+
    char uart0_getchar(UART_Data *status)
    {
        UART_Data temp;
        char data;
        
        // Wait until data has been received
        do 
        {
            temp = uart0_scanchar(&data);
        } while (temp == Empty);
        
        *status = temp;
        return data;
    }

    #if (UART_STDMODE == 1 || UART_STDMODE == 3) && UART_HARDWARE == 0
        //  +---------------------------------------------------------------+
        //  |                   UART receive character                      |
        //  +---------------------------------------------------------------+
        //  | Parameter:    stream (ptr)    ->  Datastream                  |
        //  |                                                               |
        //  |    Return:    0x00??  ->  data/NUL                            |
        //  +---------------------------------------------------------------+
        int uart0_scanf(FILE *stream)
        {
            return (int)uart0_getchar(NULL);
        }
    #endif

#endif