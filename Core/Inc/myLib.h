#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include <string.h>

#ifndef __MY_LIB
#define __MY_LIB

// Frecuencia a la que funcionará el contador de los timers
#define FRECUENCIA_CNT_LEDS 1e6
#define PERIODO_PWM 20000
#define PASO 2
#define MAXIMA_INTENSIDAD (PERIODO_PWM/PASO)

#define ESP_01 USART1
#define PC USART2

#define COMIENZO_ROJO 0
#define COMIENZO_

static char* intensidadLed;
static uint8_t index_intensidad;

static uint8_t* comandoAT;
static uint8_t index_AT;

static char* caracteres;
static uint8_t index_caracteres;

static uint8_t partyMode = 0;
// Creame una enumeración
enum {
    NARANJA,
    AMARILLO,
    FLUOR,
    ROSA,
    ROJO,
    VIOLETA,
    VERDE,
    CELESTE,
    AZUL,
    BLANCO,
    CELESTE_MAR,
    VIOLETA_OSCURO,
    FUCSIA
};

// Quiero una función con la que pueda ingresar una cadena de caracteres para meter en el buffer de transmisión de USART1
// y que se transmita esa cadena de caracteres
void transmitirCadena(char* cadena)
{
    uint8_t longitud = strlen(cadena);
    for(uint8_t i = 0; i < longitud; i++)
    {
        while(!(USART1->SR & USART_SR_TC));
        USART1->DR = cadena[i];
    }
}

// Sirve para parpadear una sola vez el LED de la placa
void parpadearLedPlaca()
{
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM1->SR = 0;
}

// Sirve para parpadear n veces el LED de la placa
void parpadearLedPlacaVeces(uint64_t n)
{
    for(uint64_t i = 0; i < n; i++)
    {
        parpadearLedPlaca();
        while(TIM1->CR1 & TIM_CR1_CEN);
        while(NVIC->IABR[0] & TIM1_UP_IRQn);
    }
}

// Sirve para configurar el tiempo en el que estará encendido el LED
void msParpadeoLedPlaca(uint16_t milisegundos)
{
    TIM1->ARR = milisegundos;
    TIM1->SR = 0;
}

uint8_t esNumero(uint8_t car)
{
    return car >= 0x30 && car <= 0x39;
}

uint8_t UARTDisponible(USART_TypeDef* pUSART)
{
    if(pUSART->SR & USART_SR_TXE) return 1;
    else return 0;
}

void UARTEscribirByte(USART_TypeDef* pUSART, uint8_t data)
{
    while(!UARTDisponible(pUSART));
    pUSART->DR = data;
}

void UARTEscribirString(USART_TypeDef* pUSART, uint8_t* string)
{
    for(uint8_t* p = string; *p != 0; p++)
        UARTEscribirByte(pUSART, *p);
}

void encenderNaranja()
{
    TIM2->CCR2 = 3000 * PASO;
    TIM3->CCR2 = 500 * PASO;
    TIM4->CCR2 = 0;
}

void encenderAmarillo()
{
    TIM2->CCR2 = 3000 * PASO;
    TIM3->CCR2 = 2100 * PASO;
    TIM4->CCR2 = 0;
}

void encenderFluor()
{
    TIM2->CCR2 = 3000 * PASO;
    TIM3->CCR2 = 3000 * PASO;
    TIM4->CCR2 = 0;
}

void encenderRosa()
{
    TIM2->CCR2 = 2500 * PASO;
    TIM3->CCR2 = 0;
    TIM4->CCR2 = 3000 * PASO;
}

void encenderRojo()
{
    TIM2->CCR2 = 3000 * PASO;
    TIM3->CCR2 = 0;
    TIM4->CCR2 = 0;
}

void encenderVioleta()
{
    TIM2->CCR2 = 800 * PASO;
    TIM3->CCR2 = 0;
    TIM4->CCR2 = 3000 * PASO;
}

void encenderVerde()
{
    TIM2->CCR2 = 0;
    TIM3->CCR2 = 3000 * PASO;
    TIM4->CCR2 = 0;
}

void encenderCeleste()
{
    TIM2->CCR2 = 0;
    TIM3->CCR2 = 1000 * PASO;
    TIM4->CCR2 = 3000 * PASO;
}

void encenderAzul()
{
    TIM2->CCR2 = 0;
    TIM3->CCR2 = 0;
    TIM4->CCR2 = 3000 * PASO;
}

void encenderBlanco()
{
    TIM2->CCR2 = 5000 * PASO;
    TIM3->CCR2 = 5000 * PASO;
    TIM4->CCR2 = 5000 * PASO;
}

void encenderCelesteMar()
{
    TIM2->CCR2 = 0;
    TIM3->CCR2 = 2000 * PASO;
    TIM4->CCR2 = 3000 * PASO;
}

void encenderVioletaOscuro()
{
    TIM2->CCR2 = 500 * PASO;
    TIM3->CCR2 = 0;
    TIM4->CCR2 = 3000 * PASO;
}

void encenderFucsia()
{
    TIM2->CCR2 = 3000 * PASO;
    TIM3->CCR2 = 0;
    TIM4->CCR2 = 1500 * PASO;
}

void apagarLed()
{
    TIM2->CCR2 = 0;
    TIM3->CCR2 = 0;
    TIM4->CCR2 = 0;
}

#endif