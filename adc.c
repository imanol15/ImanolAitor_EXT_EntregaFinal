/*
 * adc.c
 *
 *  Created on: 7 dic 2023
 *      Author: Imanol
 */
#include "hal_data.h"

// Función para inicializar el ADC
void ADCInit(void) {
    fsp_err_t err = FSP_SUCCESS;

    // Inicializar ADC
    err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    assert(FSP_SUCCESS == err);
    err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    assert(FSP_SUCCESS == err);
}

// Función para iniciar la conversión del ADC
void ADCStartScan(void) {
    // En modo de activación por software, iniciar una exploración llamando a R_ADC_ScanStart().
    // En otros modos, habilitar activadores externos llamando a R_ADC_ScanStart().
    R_ADC_ScanStart(&g_adc_ctrl);
}

// Función para esperar la finalización de la conversión del ADC
int ADCWaitConversion(void) {
    // Esperar a que la conversión se complete.
    adc_status_t status;
    R_ADC_StatusGet(&g_adc_ctrl, &status);

    if (status.state == ADC_STATE_SCAN_IN_PROGRESS) {
        return 0;  // Devuelve 0 si la conversión está en progreso.
    }
    return 1;  // Devuelve 1 si la conversión ha terminado.
}

// Función para leer el valor convertido del ADC
uint16_t ReadADC(adc_channel_t Achan) {
    fsp_err_t err = FSP_SUCCESS;
    uint16_t channel4_conversion_result;

    // Lectura del valor del potenciómetro conectado en el canal especificado.
    err = R_ADC_Read(&g_adc_ctrl, Achan, &channel4_conversion_result);
    assert(FSP_SUCCESS == err);

    return channel4_conversion_result;
}

// Función de inicialización del ADC (llamada por ADC_Initialize)
static fsp_err_t ADC_Initialize(void) {
    fsp_err_t fsp_err = FSP_SUCCESS;

    // Abrir el controlador ADC
    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

    // Configurar el escaneo del ADC
    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

    // Iniciar el escaneo del ADC
    fsp_err = R_ADC_ScanStart(&g_adc_ctrl);
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;

    return fsp_err;
}


