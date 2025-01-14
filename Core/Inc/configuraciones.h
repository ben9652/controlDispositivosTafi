#ifndef __CONFIGURACIONES
#define __CONFIGURACIONES
#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include "myLib.h"

// Configuro la frecuencia del sistema a 28 MHz
void setTo28MHzFromHSE(void)
{
  // Para seleccionar, como frecuencia externa de entrada al micro la del cristal de 8 MHz, llamada HSE
  RCC->CR |= RCC_CR_HSEON;

  // Hay que esperar a que se estabilice la señal seleccionada
  while(!(RCC->CR & RCC_CR_HSERDY));

  // Activa un pre-buffer para la próxima instrucción, así se hace más rápida la ejecución
  FLASH->ACR |= FLASH_ACR_PRFTBE;
  
  // Esta latencia hay que especificarla para que no haya diferencias entre la
  // velocidad que espera el micro para ejecutar cada instrucción y la que es realmente.
  // Aquí se especifica particularmente la velocidad que esperamos configurar del SYSCLK.
  // 0:      0 < SYSCLK < 24 MHz
  // 1: 24 MHz < SYSCLK < 48 MHz
  // 2: 48 MHz < SYSCLK <= 72 MHz
  FLASH->ACR |= FLASH_ACR_LATENCY_1;
  // ¿Por qué? Porque esperamos configurar una frecuencia de 28 MHz para el SYSCLK

  // Con esto selecciono si la frecuencia HSE será dividida entre 2 o no.
  // Aquí particularmente decido que se dividirá. Por lo que se tornará en 4 MHz.
  // Esta frecuencia de salida se llama PREDIV1
  RCC->CFGR |= RCC_CFGR_PLLXTPRE;

  // Con este bit selecciono si la frecuencia de salida PLLCLK será HSI o HSE.
  // Como venimos configurando el HSE, elegimos esa, que en el multiplexor sería un 1.
  RCC->CFGR |= RCC_CFGR_PLLSRC;

  // Aquí multiplico el reloj seleccionado por 7 así la señal termina siendo de 28 MHz
  RCC->CFGR |= RCC_CFGR_PLLMULL7;

  // Con esto dejo que la señal AHB que sale a los periféricos siga siendo de 28 MHz
  RCC->CFGR &= ~RCC_CFGR_HPRE;
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  // Con esto hago que la señal APB1 sea de 28 MHz
  RCC->CFGR &= ~RCC_CFGR_PPRE1;
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;

  // Con esto hago que la señal APB2 sea de 28 MHz
  RCC->CFGR &= ~RCC_CFGR_PPRE2;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

  // Enciendo el PLL
  RCC->CR |= RCC_CR_PLLON;

  // Espero a que se estabilice la señal del PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));

  // Selecciono como señal SYSCLK a la señal PLLCLK
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));

  // Actualizo finalmente la señal de reloj del sistema (SYSCLK)
  SystemCoreClockUpdate();
}

void configurarTimerParaLedRgb()
{
  
}

// Timer usado para notificar un error con el LED de la placa
void configurarTIM1(void)
{
  __asm__ volatile ("cpsid i");

  // Habilito la entrada de reloj para el Timer 1
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  uint32_t frecuencia_CNT = 1000;

  // Con esto consigo una frecuencia de 1 kHz para el contador
  // Así, si la frecuencia de entrada al timer es de 28 MHz, el PSC tendrá el valor 1000
  TIM1->PSC = SystemCoreClock / frecuencia_CNT - 1;
  // Con esto consigo un período de 0.5 s
  TIM1->ARR = 500;

  // Hago que los eventos como el desborde generen pedido de interrupción
  TIM1->DIER = TIM_DIER_UIE;

  TIM1->CR1 = 0;

  NVIC->ISER[0] |= 1 << TIM1_UP_IRQn;
  NVIC->ICPR[0] |= 1 << TIM1_UP_IRQn;
  
  TIM1->CR1 = TIM_CR1_CEN;
  TIM1->SR = 0;

  __asm__ volatile ("cpsie i");
}

// Configurar la USART usada para el módulo Bluetooth
void configurarUSART1()
{
  // Esto hace que se divida en 45.57 la frecuencia de 28 MHz.
  // De esta manera obtendría una velocidad de transmisión/recepción de 38400 bps.
  USART1->BRR = 0x02D9;

  USART1->CR2 = 0;
  // Habilito las interrupciones por error
  USART1->CR3 = USART_CR3_EIE;
  // Habilito la USART, el recibimiento de datos y la transmisión de datos.
  // Implícitamente además, estoy haciendo que lo que se mande sea:
  //      - 1 bit de start
  //      - 8 bits de datos
  //      - 1 bit de stop
  //      - sin paridad
  USART1->CR1 = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE;

  NVIC->ISER[1] |= 1 << (USART1_IRQn - 32);
  NVIC->ICPR[1] |= 1 << (USART1_IRQn - 32);
}

void configurarUSART2()
{
  // Esto hace que se divida en 45.57 la frecuencia de 28 MHz.
  // De esta manera obtendría una velocidad de transmisión/recepción de 38400 bps.
  USART2->BRR = 0x02D9;

  USART2->CR2 = 0;
  // Habilito las interrupciones por error
  USART2->CR3 = USART_CR3_EIE;
  // Habilito la USART, el recibimiento de datos y la transmisión de datos.
  // Implícitamente además, estoy haciendo que lo que se mande sea:
  //      - 1 bit de start
  //      - 8 bits de datos
  //      - 1 bit de stop
  //      - sin paridad
  USART2->CR1 = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE;

  NVIC->ISER[1] |= 1 << (USART2_IRQn - 32);
  NVIC->ICPR[1] |= 1 << (USART2_IRQn - 32);
}

void configurarPuertos()
{
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  // Se configura como salida TX el puerto A9 y como RX el puerto A10
  GPIOA->CRH = GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_1 |
                GPIO_CRH_CNF10_1;

  // Se configura como salida TX el puerto A2 y como RX el puerto A3
  GPIOA->CRL = GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2_1 |
                GPIO_CRL_CNF3_1;

  GPIOC->CRH = GPIO_CRH_MODE13_1;
  GPIOC->ODR = GPIO_ODR_ODR13;
}

#endif