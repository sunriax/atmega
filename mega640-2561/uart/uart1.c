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

#include "uart1.h"

//  +---------------------------------------------------------------+
//  |                       UART initialization                     |
//  +---------------------------------------------------------------+
void uart1_init(void)
{   
    // Check which bit sampling mode should be activated
    #if UART1_USE2X
        UCSR1A |= (1<<U2X1);                        // Setup 8 samples/bit
    #else
        UCSR1A &= ~(1<<U2X1);                       // Setup 16 samples/bit
    #endif
    
    // Setup baudrate register (HIGH/LOW)
    UBRR1H = (unsigned char)(UART1_UBRR>>8);
    UBRR1L = (unsigned char)(UART1_UBRR);
    
    UCSR1C = (0x06 & ((UART1_DATASIZE - 5)<<1));	    // Setup Datasize
    
    #if UART1_PARITY > 0
        UCSR1C |= (0x30 & ((UART1_PARITY + 1)<<4));  // Parity Mode
    #endif
    
    #if UART1_STOPBITS > 1
        UCSR1C |= (0x08 & (UART1_STOPBITS<<3));      // Setup stop bits
    #endif
    
    UCSR1B = (1<<RXEN1) | (1<<TXEN1);  // Activate UART transmitter and receiver

    // Interrupt control
    
    // Receiver interrupt setup
    #ifdef UART1_RXCIE
        UCSR1B |= (1<<RXCIE1);
    #endif

    // Transmitter interrupt setup
    #if defined(UART1_TXCIE) && !defined(UART1_UDRIE)
        UCSR1B |= (1<<TXCIE1);
    #endif

    // Transmitter interrupt setup
    #if !defined(UART1_TXCIE) && defined(UART1_UDRIE)
        UCSR1B |= (1<<UDRIE1);
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
UART_Error uart1_error_flags(void)
{
    // Frame error
    if(UCSR1A & (1<<FE1))
    {
        UDR1;           // Clear UART data register
        return Frame;   // Return NUL
    }
    // Data Overrun error
    else if(UCSR1A & (1<<DOR1))
    {
        UDR1;           // Clear UART data register
        return Overrun; // Return NUL
    }
    // Parity error
    else if(UCSR1A & (1<<UPE1))
    {
        UDR1;           // Clear UART data register
        return Parity;  // Return NUL
    }
    return None;
}

#if !defined(UART1_TXCIE) && !defined(UART1_UDRIE)

    //  +---------------------------------------------------------------+
    //  |                   UART send character                         |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data    ->  ASCII character/Binary Number       |
    //  |                                                               |
    //  |    Return:    0x00    ->  Transmission complete               |
    //  +---------------------------------------------------------------+
    char uart1_putchar(char data)
    {
        // Wait until last transmission completed
        while(!(UCSR1A & (1<<UDRE1)));
    
        UDR1 = data; // Write data to transmission register
    
        // C99 functions needs an int as a return parameter
        return 0;   // Return that there was no fault
    }
    
    #if (UART_STDMODE == 1 || UART_STDMODE == 2) && UART_HARDWARE == 1
        //  +---------------------------------------------------------------+
        //  |                   UART send character                         |
        //  +---------------------------------------------------------------+
        //  | Parameter:    data         ->  ASCII character                |
        //  |               stream (ptr) ->  Filestream                     |
        //  |                                                               |
        //  |    Return:    0x0000  ->  Transmission complete               |
        //  +---------------------------------------------------------------+
        int uart1_printf(char data, FILE *stream)
        {
            return uart1_putchar(data);
        }
    #endif

#endif

#if !defined(UART1_RXCIE)
    //  +---------------------------------------------------------------+
    //  |               UART receive character (non blocking)           |
    //  +---------------------------------------------------------------+
    //  | Parameter:    0x??        -> Data buffer variable             |
    //  |                                                               |
    //  |    Return:    Empty       -> No data in received              |
    //  |               Received    -> Data received                    |
    //  |               Error       -> Error during transmission        |
    //  +---------------------------------------------------------------+
    UART_Data uart1_scanchar(char *data)
    {
        // If data has been received
        if((UCSR1A & (1<<RXC1)))
        {
            // Check if an Error ocurred
            if(uart1_error_flags() != None)
            {
                UDR1;           // Clear UDR1 Data register
                *data = 0;
                return Error;
            }
        
            *data = UDR1;
        
            #if defined(UART1_RXC_ECHO) && !defined(UART1_TXCIE) && !defined(UART1_UDRIE)
                // Send echo of received data to UART
                uart1_putchar(*data);
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
    char uart1_getchar(UART_Data *status)
    {
        UART_Data temp;
        char data;
        
        // Wait until data has been received
        do 
        {
            temp = uart1_scanchar(&data);
        } while (temp == Empty);
        
        *status = temp;
        return data;
    }

    #if (UART_STDMODE == 1 || UART_STDMODE == 3) && UART_HARDWARE == 1
        //  +---------------------------------------------------------------+
        //  |                   UART receive character                      |
        //  +---------------------------------------------------------------+
        //  | Parameter:    stream (ptr)    ->  Datastream                  |
        //  |                                                               |
        //  |    Return:    0x00??  ->  data/NUL                            |
        //  +---------------------------------------------------------------+
        int uart1_scanf(FILE *stream)
        {
            return (int)uart1_getchar(NULL);
        }
    #endif

#endif