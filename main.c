#include <stdint.h>
#include "bsp/bsp.h"
#include <stdio.h>
#include <string.h>

/* Variables utilizadas en recepcion de datos a traves de la UART */
#define TAM_RX_BUFFER 7
#define TAM_RX_TRAMA_ENCABEZADO 3
#define ASCII_A_DECIMAL 48

union u_rx {
	struct str_trama{
		uint8_t encabezado[TAM_RX_TRAMA_ENCABEZADO];
		uint8_t led_n;
		uint8_t dos_p;
		uint8_t estado;
		uint8_t fin_trama;
	}trama;

	uint8_t buffer[TAM_RX_BUFFER];
}rx;

static uint8_t rx_dato_disponible = 0;

/**
 * @brief Delay por software
 *
 * @param nCount Numero de ciclos del delay
 */
void delay(volatile uint32_t nCount);

/**
 * @brief Se encarga de prender un led y apagarlo luego de un tiempo
 *
 * @param led    Numero de led a pulsar
 * @param tiempo Numero de ciclos del delay entre prendido y apagado
 */
void pulsoLed(uint8_t led, uint32_t tiempo);

/**
 * @brief Analiza la trama recibida por la UART
 *
 */
void analizar_trama_rx (void);

/**
 * @brief Aplicacion principal
 */
int main(void) {
	bsp_init();

	while (1) {

		if(rx_dato_disponible==1 && rx.trama.fin_trama == 0xD){
				analizar_trama_rx();
		}
	}
}


void pulsoLed(uint8_t led, uint32_t tiempo){
	led_on(led);
	delay(tiempo);
	led_off(led);
}

void delay(volatile uint32_t nCount) {
	while (nCount--) {
	}
}

void APP_ISR_uartrx (void){
	uint8_t data;
	int i;

	/*Levanto bandera de dato disponible en UART RX*/
	rx_dato_disponible = 1;

	/*Cargo dato disponible en "data"*/
	data = uart_rx();

	for (i=0; i<TAM_RX_BUFFER-1; i++) {
		rx.buffer[i] = rx.buffer[i + 1];
	}
	rx.buffer[TAM_RX_BUFFER-1] = data;
}

void analizar_trama_rx (void) {

	if (strncmp("LED",(char *)&rx.trama.encabezado[0],3)==0) {
		if (strncmp(":",(char *)&rx.trama.dos_p,1)==0) {
			if (strncmp("1",(char *)&rx.trama.estado,1)==0){
				led_on(rx.trama.led_n-ASCII_A_DECIMAL);
			} else {
				led_off(rx.trama.led_n-ASCII_A_DECIMAL);
			}
		}
	}
	rx_dato_disponible=0;
}

void APP_ISR_sw (void){
}

void APP_ISR_1ms (void){
}
