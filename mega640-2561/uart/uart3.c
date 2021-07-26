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

#include "uart3.h"

//  +---------------------------------------------------------------+
//  |                       UART initialization                     |
//  +---------------------------------------------------------------+
void uart3_init(void)
{   
    // Check which bit sampling mode should be activated
    #if UART3_USE2X
        UCSR3A |= (1<<U2X3);                        // Setup 8 samples/bit
    #else
        UCSR3A &= ~(1<<U2X3);                       // Setup 16 samples/bit
    #endif
    
    // Setup baudrate register (HIGH/LOW)
    UBRR3H = (unsigned char)(UART3_UBRR>>8);
    UBRR3L = (unsigned char)(UART3_UBRR);
    
    UCSR3C = (0x06 & ((UART3_DATASIZE - 5)<<1));	    // Setup Datasize
    
    #if UART3_PARITY > 0
        UCSR3C |= (0x30 & ((UART3_PARITY + 1)<<4));  // Parity Mode
    #endif
    
    #if UART3_STOPBITS > 1
        UCSR3C |= (0x08 & (UART3_STOPBITS<<3));      // Setup stop bits
    #endif
    
    UCSR3B = (1<<RXEN1) | (1<<TXEN1);  // Activate UART transmitter and receiver

    // Interrupt control
    
    // Receiver interrupt setup
    #ifdef UART3_RXCIE
        UCSR3B |= (1<<RXCIE1);
    #endif

    // Transmitter interrupt setup
    #if defined(UART3_TXCIE) && !defined(UART3_UDRIE)
        UCSR3B |= (1<<TXCIE1);
    #endif

    // Transmitter interrupt setup
    #if !defined(UART3_TXCIE) && defined(UART3_UDRIE)
        UCSR3B |= (1<<UDRIE1);
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
UART_Error uart3_error_flags(void)
{
    // Frame error
    if(UCSR3A & (1<<FE3))
    {
        UDR3;           // Clear UART data register
        return Frame;   // Return NUL
    }
    // Data Overrun error
    else if(UCSR3A & (1<<DOR3))
    {
        UDR3;           // Clear UART data register
        return Overrun; // Return NUL
    }
    // Parity error
    else if(UCSR3A & (1<<UPE3))
    {
        UDR3;           // Clear UART data register
        return Parity;  // Return NUL
    }
    return None;
}

#if !defined(UART3_TXCIE) && !defined(UART3_UDRIE)

    //  +---------------------------------------------------------------+
    //  |                   UART send character                         |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data    ->  ASCII character/Binary Number       |
    //  |                                                               |
    //  |    Return:    0x00    ->  Transmission complete               |
    //  +---------------------------------------------------------------+
    char uart3_putchar(char data)
    {
        // Wait until last transmission completed
        while(!(UCSR3A & (1<<UDRE3)));
    
        UDR3 = data; // Write data to transmission register
    
        // C99 functions needs an int as a return parameter
        return 0;   // Return that there was no fault
    }
    
    #if (UART_STDMODE == 1 || UART_STDMODE == 2) && UART_HARDWARE == 3
        //  +---------------------------------------------------------------+
        //  |                   UART send character                         |
        //  +---------------------------------------------------------------+
        //  | Parameter:    data         ->  ASCII character                |
        //  |               stream (ptr) ->  Filestream                     |
        //  |                                                               |
        //  |    Return:    0x0000  ->  Transmission complete               |
        //  +---------------------------------------------------------------+
        int uart3_printf(char data, FILE *stream)
        {
            return uart3_putchar(data);
        }
    #endif

#endif

#if !defined(UART3_RXCIE)
    //  +---------------------------------------------------------------+
    //  |               UART receive character (non blocking)           |
    //  +---------------------------------------------------------------+
    //  | Parameter:    0x??        -> Data buffer variable             |
    //  |                                                               |
    //  |    Return:    Empty       -> No data in received              |
    //  |               Received    -> Data received                    |
    //  |               Error       -> Error during transmission        |
    //  +---------------------------------------------------------------+
    UART_Data uart3_scanchar(char *data)
    {
        // If data has been received
        if((UCSR3A & (1<<RXC3)))
        {
            // Check if an Error ocurred
            if(uart3_error_flags() != None)
            {
                UDR3;           // Clear UDR3 Data register
                *data = 0;
                return Error;
            }
        
            *data = UDR3;
        
            #if defined(UART3_RXC_ECHO) && !defined(UART3_TXCIE) && !defined(UART3_UDRIE)
                // Send echo of received data to UART
                uart3_putchar(*data);
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
    char uart3_getchar(UART_Data *status)
    {
        UART_Data temp;
        char data;
        
        // Wait until data has been received
        do 
        {
            temp = uart3_scanchar(&data);
        } while (temp == Empty);
        
        *status = temp;
        return data;
    }

    #if (UART_STDMODE == 1 || UART_STDMODE == 3) && UART_HARDWARE == 3
        //  +---------------------------------------------------------------+
        //  |                   UART receive character                      |
        //  +---------------------------------------------------------------+
        //  | Parameter:    stream (ptr)    ->  Datastream                  |
        //  |                                                               |
        //  |    Return:    0x00??  ->  data/NUL                            |
        //  +---------------------------------------------------------------+
        int uart3_scanf(FILE *stream)
        {
            return (int)uart3_getchar(NULL);
        }
    #endif

#endif