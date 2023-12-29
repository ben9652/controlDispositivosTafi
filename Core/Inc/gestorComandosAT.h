#include "myLib.h"

void USART2_IRQHandler()
{
    uint32_t estado = PC->SR;

    // Para cuando se recibe un dato de la computadora
    if(estado & USART_SR_RXNE)
    {
        char dato = PC->DR;
        
        *(comandoAT + index_AT++) = dato;

        if(dato == '\b')
        {
            if(index_AT > 0)
            {
                *(comandoAT + --index_AT) = 0;
                *(comandoAT + --index_AT) = 0;
            }
            
            UARTEscribirByte(PC, 0x08);
            UARTEscribirByte(PC, 0x20);
        }
        if(dato == '\r')
        {
            UARTEscribirByte(PC, '\n');
            
            *(comandoAT + index_AT++) = '\n';
            *(comandoAT + index_AT) = 0;
            
            memset(comandoAT, 0, 50);
            index_AT = 0;

            UARTEscribirString(ESP_01, comandoAT);
        }

        UARTEscribirByte(PC, dato);
    }
}