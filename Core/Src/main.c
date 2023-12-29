#include "configuraciones.h"
#include "rutinas_de_interrupcion.h"
#include "gestorComandosAT.h"
#include "myLib.h"
#include "stdlib.h"

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  setTo28MHzFromHSE();

  comandoAT = (uint8_t*)malloc(50);
  *comandoAT = 0;
  index_AT = 0;

  intensidadLed = (char*)malloc(6);
  memset(intensidadLed, 0, 6);
  index_intensidad = 0;

  caracteres = (char*)malloc(20);
  memset(caracteres, 0, 20);
  index_caracteres = 0;
  
  configurarPuertos();
  configurarTIM1();
  configurarTimerParaLedRgb();
  // configurarUSART1();
  configurarUSART2();
  
  while(1);
}