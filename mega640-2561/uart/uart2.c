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

#include "uart2.h"

//  +---------------------------------------------------------------+
//  |                       UART initialization                     |
//  +---------------------------------------------------------------+
void uart2_init(void)
{   
    // Check which bit sampling mode should be activated
    #if UART2_USE2X
        UCSR2A |= (1<<U2X2);                        // Setup 8 samples/bit
    #else
        UCSR2A &= ~(1<<U2X2);                       // Setup 16 samples/bit
    #endif
    
    // Setup baudrate register (HIGH/LOW)
    UBRR2H = (unsigned char)(UART2_UBRR>>8);
    UBRR2L = (unsigned char)(UART2_UBRR);
    
    UCSR2C = (0x06 & ((UART2_DATASIZE - 5)<<1));	    // Setup Datasize
    
    #if UART2_PARITY > 0
        UCSR2C |= (0x30 & ((UART2_PARITY + 1)<<4));  // UART_Parity Mode
    #endif
    
    #if UART2_STOPBITS > 1
        UCSR2C |= (0x08 & (UART2_STOPBITS<<3));      // Setup stop bits
    #endif
    
    UCSR2B = (1<<RXEN2) | (1<<TXEN2);  // Activate UART transmitter and receiver

    // Interrupt control
    
    // Receiver interrupt setup
    #ifdef UART2_RXCIE
        UCSR2B |= (1<<RXCIE2);
    #endif

    // Transmitter interrupt setup
    #if defined(UART2_TXCIE) && !defined(UART2_UDRIE)
        UCSR2B |= (1<<TXCIE2);
    #endif

    // Transmitter interrupt setup
    #if !defined(UART2_TXCIE) && defined(UART2_UDRIE)
        UCSR2B |= (1<<UDRIE2);
    #endif
}

//  +---------------------------------------------------------------+
//  |                   UART send character                         |
//  +---------------------------------------------------------------+
//  |    Return:    UART_None=0  -> No error ocurred                |
//  |               UART_Frame   -> Error in frame                  |
//  |               UART_Overrun -> Data overrun @ transmission     |
//  |               UART_Parity  -> Parity error                    |
//  +---------------------------------------------------------------+
UART_Error uart2_error_flags(void)
{
    // UART_Frame error
    if(UCSR2A & (1<<FE2))
    {
        UDR2;           // Clear UART data register
        return UART_Frame;   // Return NUL
    }
    // Data UART_Overrun error
    else if(UCSR2A & (1<<DOR2))
    {
        UDR2;           // Clear UART data register
        return UART_Overrun; // Return NUL
    }
    // UART_Parity error
    else if(UCSR2A & (1<<UPE2))
    {
        UDR2;           // Clear UART data register
        return UART_Parity;  // Return NUL
    }
    return UART_None;
}

#if !defined(UART2_TXCIE) && !defined(UART2_UDRIE)

    //  +---------------------------------------------------------------+
    //  |                   UART send character                         |
    //  +---------------------------------------------------------------+
    //  | Parameter:    data    ->  ASCII character/Binary Number       |
    //  |                                                               |
    //  |    Return:    0x00    ->  Transmission complete               |
    //  +---------------------------------------------------------------+
    char uart2_putchar(char data)
    {
        // Wait until last transmission completed
        while(!(UCSR2A & (1<<UDRE2)));
    
        UDR2 = data; // Write data to transmission register
    
        // C99 functions needs an int as a return parameter
        return 0;   // Return that there was no fault
    }
    
    #if (UART_STDMODE == 1 || UART_STDMODE == 2) && UART_HARDWARE == 2
        //  +---------------------------------------------------------------+
        //  |                   UART send character                         |
        //  +---------------------------------------------------------------+
        //  | Parameter:    data         ->  ASCII character                |
        //  |               stream (ptr) ->  Filestream                     |
        //  |                                                               |
        //  |    Return:    0x0000  ->  Transmission complete               |
        //  +---------------------------------------------------------------+
        int uart2_printf(char data, FILE *stream)
        {
            return uart2_putchar(data);
        }
    #endif

#endif

#if !defined(UART2_RXCIE)
    //  +---------------------------------------------------------------+
    //  |               UART receive character (non blocking)           |
    //  +---------------------------------------------------------------+
    //  | Parameter:    0x??        -> Data buffer variable             |
    //  |                                                               |
    //  |    Return:    UART_Empty       -> No data in received              |
    //  |               UART_Received    -> Data received                    |
    //  |               UART_Fault       -> UART_Fault during transmission        |
    //  +---------------------------------------------------------------+
    UART_Data uart2_scanchar(char *data)
    {
        // If data has been received
        if((UCSR2A & (1<<RXC2)))
        {
            // Check if an UART_Fault ocurred
            if(uart2_error_flags() != UART_None)
            {
                UDR2;           // Clear UDR2 Data register
                *data = 0;
                return UART_Fault;
            }
        
            *data = UDR2;
        
            #if defined(UART2_RXC_ECHO) && !defined(UART2_TXCIE) && !defined(UART2_UDRIE)
                // Send echo of received data to UART
                uart2_putchar(*data);
            #endif
        
            return UART_Received;
        }
        return UART_Empty;
    }
    
    //  +---------------------------------------------------------------+
    //  |                   UART receive character                      |
    //  +---------------------------------------------------------------+
    //  | Parameter:    status (ptr) -> Pointer to return status        |
    //  |                            -> UART_Empty                      |
    //  |                            -> UART_Received                   |
    //  |                            -> UART_Error                      |
    //  |                                                               |
    //  |    Return:    0x??    ->  data/NUL                            |
    //  +---------------------------------------------------------------+
    char uart2_getchar(UART_Data *status)
    {
        UART_Data temp;
        char data;
        
        // Wait until data has been received
        do 
        {
            temp = uart2_scanchar(&data);
        } while (temp == UART_Empty);
        
        *status = temp;
        return data;
    }

    #if (UART_STDMODE == 1 || UART_STDMODE == 3) && UART_HARDWARE == 2
        //  +---------------------------------------------------------------+
        //  |                   UART receive character                      |
        //  +---------------------------------------------------------------+
        //  | Parameter:    stream (ptr)    ->  Datastream                  |
        //  |                                                               |
        //  |    Return:    0x00??  ->  data/NUL                            |
        //  +---------------------------------------------------------------+
        int uart2_scanf(FILE *stream)
        {
            return (int)uart2_getchar(NULL);
        }
    #endif

#endif