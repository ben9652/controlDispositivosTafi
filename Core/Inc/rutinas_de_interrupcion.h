#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include "myLib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void TIM1_UP_IRQHandler()
{
  TIM1->SR = 0;
  
  GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void USART1_IRQHandler()
{
  uint32_t estado = USART1->SR;
  
  if(estado & USART_SR_FE)
  {
    msParpadeoLedPlaca(50);
    parpadearLedPlacaVeces(4);
  }
  else if(estado & USART_SR_PE)
  {
    msParpadeoLedPlaca(150);
    parpadearLedPlacaVeces(3);
  }
  else if(estado & USART_SR_NE)
  {
    msParpadeoLedPlaca(250);
    parpadearLedPlaca();
  }

  uint8_t dato = USART1->DR;

  if(dato == 0xFE)
  {
    USART1->DR = 0xFF;
    while(!(USART1->SR & USART_SR_TC));
    return;
  }
  
  if(esNumero(dato))
  {
    if(strlen(intensidadLed) < 5)
    {
        intensidadLed[index_intensidad++] = dato;
        intensidadLed[index_intensidad] = 0;
    }
  }
  else
  {
    if(index_caracteres < 20)
                  caracteres[index_caracteres++] = dato;

    uint16_t numero_intensidad = atoi(intensidadLed);
    if(numero_intensidad >= MAXIMA_INTENSIDAD)
        numero_intensidad = MAXIMA_INTENSIDAD - PASO;

    if(caracteres[index_caracteres - 1] == '\n')
    {
        // Aquí hago el equivalente a lo que hace la función tolower() a la cadena 'caracteres'
        for(uint8_t i = 0; i < index_caracteres; i++)
        {
            if(caracteres[i] >= 'A' && caracteres[i] <= 'Z')
                caracteres[i] += 0x20;
        }

        memset(intensidadLed, 0, 6);
        index_intensidad = 0;

        if(strcmp(caracteres, "party\r\n"))
          partyMode = 0;

        if(!strcmp(caracteres, "off\r\n"))
        {
          TIM2->CCR2 = 0;
          TIM3->CCR2 = 0;
          TIM4->CCR2 = 0;
        }
        else if(!strcmp(caracteres, "r\r\n"))
            TIM2->CCR2 = numero_intensidad * PASO;
        else if(!strcmp(caracteres, "g\r\n"))
            TIM3->CCR2 = numero_intensidad * PASO;
        else if(!strcmp(caracteres, "b\r\n"))
            TIM4->CCR2 = numero_intensidad * PASO;
        else if(!strcmp(caracteres, "orange\r\n")) encenderNaranja();
        else if(!strcmp(caracteres, "yellow\r\n")) encenderAmarillo();
        else if(!strcmp(caracteres, "fluor\r\n")) encenderFluor();
        else if(!strcmp(caracteres, "pink\r\n")) encenderRosa();
        else if(!strcmp(caracteres, "fuchsia\r\n")) encenderFucsia();
        else if(!strcmp(caracteres, "purple\r\n") || !strcmp(caracteres, "violet\r\n")) encenderVioleta();
        else if(!strcmp(caracteres, "dark purple\r\n")) encenderVioletaOscuro();
        else if(!strcmp(caracteres, "cyan\r\n")) encenderCeleste();
        else if(!strcmp(caracteres, "sea cyan\r\n")) encenderCelesteMar();
        else if(!strcmp(caracteres, "red\r\n")) encenderRojo();
        else if(!strcmp(caracteres, "green\r\n")) encenderVerde();
        else if(!strcmp(caracteres, "blue\r\n")) encenderAzul();
        else if(!strcmp(caracteres, "white\r\n")) encenderBlanco();
        else if(!strcmp(caracteres, "party\r\n"))
        {
            msParpadeoLedPlaca(500);
            partyMode = 1;
            parpadearLedPlaca();
        }
        else
        {
          msParpadeoLedPlaca(100);
          parpadearLedPlacaVeces(2);
        }

        memset(caracteres, 0, 20);
        index_caracteres = 0;
    }
  }
}