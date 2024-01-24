#include "hal_data.h"
#include "r_adc_api.h"
#include "r_iic_master.h"
#include "display.h"
#include "adc.h"
#include <stdio.h>
#include <stdint.h>
volatile bool pulsador = false;
// Comandos para configurar líneas en el LCD
uint8_t f1[0x02] = {0x00, 0x80};
uint8_t f2[0x02] = {0x00, 0xC0};
volatile bool system_on = false;  // Variable global para controlar el estado del sistema
fsp_err_t icu_init(void);
fsp_err_t icu_enable(void);
void icu_deinit(void);
#define BUZZER_PINBSP_IO_PORT_01_PIN_13
bool buzzerActivated = false;// comentar esto
// Matrices para almacenar datos a escribir en el LCD
uint8_t m1[0x08] = {0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
uint8_t m2[0x08] = {0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

#define FILTRO_TAMANO 5
uint16_t filtro[FILTRO_TAMANO];
uint8_t indiceFiltro = 0;

void actualizarFiltro(uint16_t nuevaLectura) {
    filtro[indiceFiltro] = nuevaLectura;
    indiceFiltro = (indiceFiltro + 1) % FILTRO_TAMANO;
}


// Función para calcular la media del filtro
uint16_t calcularMediana() {
    uint16_t valoresOrdenados[FILTRO_TAMANO];
    memcpy(valoresOrdenados, filtro, sizeof(filtro));

    // Ordenar el array
    for (int i = 0; i < FILTRO_TAMANO - 1; i++) {
        for (int j = i + 1; j < FILTRO_TAMANO; j++) {
            if (valoresOrdenados[j] < valoresOrdenados[i]) {
                uint16_t temp = valoresOrdenados[i];
                valoresOrdenados[i] = valoresOrdenados[j];
                valoresOrdenados[j] = temp;
            }
        }
    }

    // Devolver el valor medio
    return valoresOrdenados[FILTRO_TAMANO / 2];
}

void init_led(void) {
    // Inicializar el módulo IOPORT y configurar los pines
    fsp_err_t err = R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
        assert(FSP_SUCCESS == err);
        // Configurar el pin del LED como salida
        err = R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06, IOPORT_CFG_PORT_DIRECTION_OUTPUT);
        assert(FSP_SUCCESS == err);

}



void blink_led(void) {
    fsp_err_t err;
    // Encender el LED
     err = R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_HIGH);
     assert(FSP_SUCCESS == err);

     // Delay para mantener el LED encendido
     R_BSP_SoftwareDelay(2000, BSP_DELAY_UNITS_MILLISECONDS);

     // Apagar el LED
     err = R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_LOW);
     assert(FSP_SUCCESS == err);

     // Delay antes de volver a llamar a blink_led
     R_BSP_SoftwareDelay(2000, BSP_DELAY_UNITS_MILLISECONDS);
}
void init_zumbador(void) {
    // Inicializar el módulo IOPORT y configurar los pines
    fsp_err_t err = R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
        assert(FSP_SUCCESS == err);
        // Configurar el pin del LED como salida
        err = R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_13, IOPORT_CFG_PORT_DIRECTION_OUTPUT);
        assert(FSP_SUCCESS == err);

}
void zumbador(void){

    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_13, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_13, BSP_IO_LEVEL_LOW);
    buzzerActivated = true;  // Establecer la bandera(comentar esto)
}
void init_pulsador(void) {
    fsp_err_t err;

    // Inicializar la interrupción externa (pulsador)
    err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    assert(FSP_SUCCESS == err);

    // Establecer la función de callback para la interrupción externa
    err = R_ICU_ExternalIrqCallbackSet(&g_external_irq0_ctrl, Btn_interruption, NULL, NULL);
    assert(FSP_SUCCESS == err);
}
void Btn_interruption(external_irq_callback_args_t *p_args) {
    if (p_args->channel == g_external_irq0_cfg.channel) {
        system_on = true;  // Activar el sistema
    }
}


// Función para mostrar "SYSTEM ON" en el LCD
void mostrarSystemOn() {
    buzzerActivated=0;
    m1[0x00] = 0x40;
    m1[0x01] = 0x53;
    m1[0x02] = 0x59;
    m1[0x03] = 0x53;
    m1[0x04] = 0x54;
    m1[0x05] = 0x45;
    m1[0x06] = 0x4D;
    m1[0x07] = 0x80;

    m2[0x00] = 0x40;
    m2[0x01] = 0x4F;
    m2[0x02] = 0x4E;
    m2[0x03] = 0x80;
    m2[0x04] = 0x80;
    m2[0x05] = 0x80;
    m2[0x06] = 0x80;
    m2[0x07] = 0x80;
    write_LCD(f1, m1);  // Escribe "SYSTEM ON" en la primera línea del LCD
    write_LCD(f2, m2);
}
// Función para actualizar el LCD con un valor numérico
void DisplayLCD(uint16_t value, uint8_t m1[]) {
    char str[100];
    char numero_char;
    unsigned char ValorAsci1, ValorAsci2, ValorAsci3, ValorAsci4, ValorAsci5;

    // Convertir el valor numérico a una cadena de caracteres
    sprintf(str, "%i", value);
    uart_write(str);
    // Convertir cada dígito a su valor ASCII correspondiente
    numero_char = str[0];
    ValorAsci1 = (unsigned char)numero_char;

    numero_char = str[1];
    ValorAsci2 = (unsigned char)numero_char;

    numero_char = str[2];
    ValorAsci3 = (unsigned char)numero_char;

    numero_char = str[3];
    ValorAsci4 = (unsigned char)numero_char;

    numero_char = str[4];
    ValorAsci5 = (unsigned char)numero_char;

    // Actualizar la matriz de datos con la configuración para el LCD
    m1[0x00] = 0x40;
    m1[0x01] = ValorAsci1;
    m1[0x02] = ValorAsci2;
    m1[0x03] = ValorAsci3;
    m1[0x04] = ValorAsci4;
    m1[0x05] = ValorAsci5;
    m1[0x06] = 0x80;
    m1[0x07] = 0x80;
}

// Función para mostrar una alerta en el LCD
void DisplayLCDAlerta(int nivel, uint8_t m1[]) {

    if (nivel == 4) {
        blink_led();
        if (buzzerActivated==0){
        zumbador();
        }
        // Configuración para mostrar "peligro" en el LCD
        m1[0x00] = 0x40;
        m1[0x01] = 0x50; // P
        m1[0x02] = 0x45; // E
        m1[0x03] = 0x4C; // L
        m1[0x04] = 0x49; // I
        m1[0x05] = 0x47; // G
        m1[0x06] = 0x52; // R
        m1[0x07] = 0x4F; // O


    } else {
        m1[0x00] = 0x40;
        m1[0x01] = 0x80;
        m1[0x02] = 0x80;
        m1[0x03] = 0x80;
        m1[0x04] = 0x80;
        m1[0x05] = 0x80;
        m1[0x06] = 0x80;
        m1[0x07] = 0x80;
    }
}

// Función para determinar el nivel de velocidad
int nivel_velocidad(uint16_t mediana) {
    const uint16_t menor1 = 1000;// Valores creados por mi estos pueden ser modificados
    const uint16_t menor2 = 2600;
    const uint16_t menor3 = 10000;

    if (mediana < menor1) {
        return 1; // Nivel de velocidad menor a  1000
    } else if (mediana < menor2) {
        return 2; // Nivel de velocidad menor a 2600
    } else if (mediana < menor3) {
        return 3; // Nivel de velocidad menor a 4000
    } else {
        return 4; // Nivel de velocidad 4 mayor a 10000
    }
}

// Función de callback del temporizador
void timer_callback(timer_callback_args_t *p_args) {
    if (TIMER_EVENT_CYCLE_END == p_args->event) {
        // Iniciar la conversión ADC y esperar a que finalice
        ADCStartScan();
        ADCWaitConversion();

        // Obtener el valor de la lectura del ADC
        uint16_t adc_result = ReadADC(ADC_CHANNEL_4);

        // Actualizar el filtro y calcular la media
        actualizarFiltro(adc_result);
        // Calcular la mediana del filtro
        uint16_t mediana = calcularMediana();
        int nivel = nivel_velocidad(adc_result);
            if (nivel == 4) {
                DisplayLCD(mediana, m1);

                // Determinar el nivel de velocidad y mostrar alerta en el segundo LCD
                DisplayLCDAlerta(nivel, m2);
            } else {
                // No hay peligro: Asegúrate de que el LED esté apagado y limpiar mensaje de LCD
                mostrarSystemOn();
                // Restablece el mensaje del LCD a un estado normal si es necesario
            }
    }
}

// Función de entrada del HAL
void hal_entry(void) {
    fsp_err_t err = FSP_SUCCESS;
    err = R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    assert(FSP_SUCCESS == err);
    (void) R_GPT_Start(&g_timer0_ctrl);

    // Inicializar el ADC, I2C y LCD
    ADCInit();
    fsp_err_t i2c = init_i2c();
    clear_i2c();
    init_uart();
    initialice_LCD();
    /* TODO: add your own code here */





    /* 1. Inicializar el driver IRQ*/
    err = icu_init();
    if (FSP_SUCCESS != err)
    {
        // Manejar error de inicialización
        // Por ejemplo: Hacer un loop infinito o reiniciar.
        //while(1); // Loop infinito (deberías decidir qué hacer en caso de error aquí).
        printf("Error en la inicializacion del driver");
    }



    // Habilitar el IRQ driver
    err = icu_enable();
    if (FSP_SUCCESS != err)
    {
        // Si habilitar falla, desinicializar el controlador de IRQ para no dejar recursos colgados.
        icu_deinit();
        // Manejar el error
        while(1); // Loop infinito (deberías decidir qué hacer en caso de error aquí).
    }

    init_led();
    init_zumbador();
    init_pulsador();

    while (!system_on) {

    }

    mostrarSystemOn();
    while (1) {
        // Escribir las líneas en el LCD
        write_LCD(f1, m1);
        write_LCD(f2, m2);
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}



fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Open failure message */
        //APP_ERR_PRINT ("\r\n**R_ICU_ExternalIrqOpen API FAILED**\r\n");
    }
    return err;
}

fsp_err_t icu_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq0_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Enable failure message */
       // APP_ERR_PRINT ("\r\n**R_ICU_ExternalIrqEnable API FAILED**\r\n");
    }
    return err;
}

void icu_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ICU module */
    err = R_ICU_ExternalIrqClose(&g_external_irq0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Close failure message */
        //APP_ERR_PRINT("\r\n**R_ICU_ExternalIrqClose API FAILED**\r\n");
    }
}
// función para comprobar el bit de paridad PAR


/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
