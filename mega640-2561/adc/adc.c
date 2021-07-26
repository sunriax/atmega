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
 *   Function file for adc library
 * -----------------------------------------
 */

#include "adc.h"

//  +---------------------------------------------------------------+
//  |                   ADC initialization                          |
//  +---------------------------------------------------------------+
void adc_init(void)
{
    // Setup ADC parameters
    ADMUX = (((0x03 & ADC_REF)<<6) | ((0x01 & ADC_DATA_SHIFT)<<5));
    
    // Setup ADC prescaler
    // F_ADC = F_CPU / ADC_PRESCALER
    ADCSRA = ((0x07 & ADC_PRESCALER));
    
    #ifdef ADC_POWER_SAVE_CH0_7
        DIDR0 = ADC_POWER_SAVE_CH0_7;
    #endif
    
    #ifdef ADC_POWER_SAVE_CH0_7
        DIDR2 = ADC_POWER_SAVE_CH8_15;
    #endif
    
    // Setup ADC permanent transformation and trigger source
    #if ADC_MODE == 0x01
        ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE);
        
        ADCSRB &= ~(0x07);
        ADCSRB |= (0x07 & ADC_TRIGGER_SOURCE);
    #endif
    
    // Check if ADC interrupt handler is active
    #ifdef ADC_ADIE
        ADCSRA |= (1<<ADIE);    // Enable ADC interrupt
        sei();                  // Enable global interrupt
    #endif
}

//  +---------------------------------------------------------------+
//  |                   ADC disable function                        |
//  +---------------------------------------------------------------+
void adc_disable(void)
{
    ADCSRA &= ~(1<<ADEN);   // Disable ADC
}

//  +---------------------------------------------------------------+
//  |                   ADC mode selection                          |
//  +---------------------------------------------------------------+
//  | Parameter:    mode    ->  0x00 = Singled ended input          |
//  |                                  ADC0 - ADC7                  |
//  |                                  ADC8 - ADC15                 |
//  |                           0x01 = Differential input   | Gain  |
//  |                                  ADC0 - ADC0 (CH0)    |  10x  |
//  |                                  ADC1 - ADC0 (CH1)    |  10x  |
//  |                                  ADC0 - ADC0 (CH0)    | 200x  |
//  |                                  ADC1 - ADC0 (CH1)    | 200x  |
//  |                                  ADC2 - ADC2 (CH2)    |  10x  |
//  |                                  ADC3 - ADC2 (CH3)    |  10x  |
//  |                                  ADC2 - ADC2 (CH2)    | 200x  |
//  |                                  ADC3 - ADC2 (CH3)    | 200x  |
//  |                                  ADC8 - ADC8 (CH8)    |  10x  |
//  |                                  ADC9 - ADC8 (CH9)    |  10x  |
//  |                                  ADC8 - ADC8 (CH8)    | 200x  |
//  |                                  ADC9 - ADC9 (CH9)    | 200x  |
//  |                                  ADC10 - ADC10 (CH10) |  10x  |
//  |                                  ADC11 - ADC11 (CH11) |  10x  |
//  |                                  ADC10 - ADC10 (CH10) | 200x  |
//  |                                  ADC11 - ADC11 (CH11) | 200x  |
//  |                           0x02 = Differential input   | Gain  |
//  |                                  ADC0 - ADC1 (CH0)    |   1x  |
//  |                                  ADC1 - ADC1 (CH1)    |   1x  |
//  |                                  ADC2 - ADC1 (CH2)    |   1x  |
//  |                                  ADC3 - ADC1 (CH3)    |   1x  |
//  |                                  ADC4 - ADC1 (CH4)    |   1x  |
//  |                                  ADC5 - ADC1 (CH5)    |   1x  |
//  |                                  ADC6 - ADC1 (CH6)    |   1x  |
//  |                                  ADC7 - ADC1 (CH7)    |   1x  |
//  |                                  ADC8 - ADC9 (CH8)    |   1x  |
//  |                                  ADC9 - ADC9 (CH9)    |   1x  |
//  |                                  ADC10 - ADC9 (CH10)  |   1x  |
//  |                                  ADC11 - ADC9 (CH11)  |   1x  |
//  |                                  ADC12 - ADC9 (CH12)  |   1x  |
//  |                                  ADC13 - ADC9 (CH13)  |   1x  |
//  |                                  ADC14 - ADC9 (CH14)  |   1x  |
//  |                                  ADC15 - ADC9 (CH15)  |   1x  |
//  |                           0x03 = Differential input   | Gain  |
//  |                                  ADC8 - ADC10 (CH8)   |   1x  |
//  |                                  ADC9 - ADC10 (CH9)   |   1x  |
//  |                                  ADC10 - ADC10 (CH10) |   1x  |
//  |                                  ADC11 - ADC10 (CH11) |   1x  |
//  |                                  ADC12 - ADC10 (CH12) |   1x  |
//  |                                  ADC13 - ADC10 (CH13) |   1x  |
//  |                                          --- Special ---      |
//  |                                  Reserved                     |
//  |                                  Reserved                     |
//  +---------------------------------------------------------------+
void adc_mode(ADC_Mode mode)
{
    ADMUX = (ADMUX & 0xE7) | ((0x03 & mode)<<3);    // Setup ADC Mode
}

//  +---------------------------------------------------------------+
//  |                   ADC channel selection                       |
//  +---------------------------------------------------------------+
//  | Parameter:    channel ->  0x00 = CH0 (Standard after init)    |
//  |                           0x0? = ...                          |
//  |                           0x0F = CH15                         |
//  +---------------------------------------------------------------+
void adc_channel(ADC_Channel channel)
{
    if(channel > CH7)
    {
        ADCSRB |= (1<<MUX5);
        ADMUX = (ADMUX & ~(0x07)) | (0x07 & (0x08 - channel));   // Select ADC Channel
    }        
    else
    {
        ADCSRB &= ~(1<<MUX5);
        ADMUX = (ADMUX & ~(0x07)) | (0x07 & channel);   // Select ADC Channel
    }
}

#ifndef ADC_ADIE

    //  +---------------------------------------------------------------+
    //  |                   ADC read value                              |
    //  +---------------------------------------------------------------+
    //  | Return:   ->  RIGHT shift of ADC data register                |
    //  +-------------------------------+-------------------------------+
    //  |             ADCH              |             ADCL              |
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //  | - | - | - | - | - | - | ? | ? | ? | ? | ? | ? | ? | ? | ? | ? |
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //  |                                                               |
    //  |           ->  LEFT shift of ADC data register                 |
    //  +-------------------------------+-------------------------------+
    //  |             ADCH              |             ADCL              |
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //  | ? | ? | ? | ? | ? | ? | ? | ? | ? | ? | - | - | - | - | - | - |
    //  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    unsigned int adc_read(void)
    {
        #ifdef ADC_NOISE_REDUCTION
            // Enter ADC noise reduction mode (stop the CPU)
            MCUCR &= ~((1<<SM2) | (1<<SM1));
            MCUCR |= (1<<SM0);
        #endif
        
        #if ADC_MODE == 0x01
            
            // Wait until ADC interrupt flag is zero
            while(!(ADCSRA & (1<<ADIF)))
                asm volatile("NOP");
            
            unsigned char ADC_LOW = ADCL;   // Save ADC LOW value to variable
            unsigned char ADC_HIGH = ADCH;  // Save ADC HIGH value to variable
            
            ADCSRA |= (1<<ADIF);            // Reset ADC interrupt flag
            
            // Return ADC high + low value
            return ((ADC_HIGH<<8) | ADC_LOW);
        
        #else
            
            // Start single conversion
            ADCSRA |= (1<<ADEN) | (1<<ADSC);
            
            // Wait until conversion is in progress
            while(ADCSRA & (1<<ADSC))
                asm volatile("NOP");
            
            unsigned char ADC_LOW = ADCL;   // Save ADC LOW value to variable
            unsigned char ADC_HIGH = ADCH;  // Save ADC HIGH value to variable
            
            // Return ADC high + low value
            return ((ADC_HIGH<<8) | ADC_LOW);
            
        #endif
        
        #ifdef ADC_NOISE_REDUCTION
            // Exit ADC noise reduction mode (start the CPU)
            MCUCR &= ~((1<<SM2) | (1<<SM1)) | (1<<SM0));
        #endif
        
    }

    //  +---------------------------------------------------------------+
    //  |                   ADC read average from values                |
    //  +---------------------------------------------------------------+
    //  | Parameter:    size    ->  quantity of conversions             |
    //  |                                                               |
    //  |    Return:    0x????  ->  Arithmetic average                  |
    //  |                           of converted data                   |
    //  +---------------------------------------------------------------+
    unsigned int adc_average(unsigned char size)
    {
        unsigned long average = 0; // average buffer
        
        // Read and add ADC values
        for(unsigned char i=0; i < size; i++)
        {
            #if ADC_DATA_SHIFT == 1
                average += (adc_read()>>6);
            #else
                average += adc_read();
            #endif
        }
        
        average /= size;    // Build average
        
        // Return average as unsigned int
        return (unsigned int)(average);
    }

#endif