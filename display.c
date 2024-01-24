#include "hal_data.h"
#include <stdint.h>

/* Reading I2C callback event through i2c_Master callback */
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;

/* Global variable for error handling */
fsp_err_t err = FSP_SUCCESS;

/*
 * @brief       Initialize IIC master module and set up PMOD ACL sensor
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 */
fsp_err_t init_i2c(void)
{
    uint32_t direccion = 0x3C;
    err = R_IIC_MASTER_SlaveAddressSet(&g_i2c_master_ctrl, direccion, I2C_MASTER_ADDR_MODE_7BIT);
    /* Opening IIC master module */
    err = R_IIC_MASTER_Open(&g_i2c_master_ctrl, &g_i2c_master_cfg);
    return err;
}

/* Initialization commands */
uint8_t init1[0x02] = {0x00, 0x38}; // Function Set: DL=1: 8 bits; N=1: 2 lines; F=0: 5 x 8dot
uint8_t init2[0x02] = {0x00, 0x0C}; // Display ON: D=1, display on; C=B=0; cursor off; blinking off;
uint8_t init3[0x02] = {0x00, 0x06}; // Entry Mode: I/D=1: Increment by 1; S=0: No shift

/*
 * @brief       Initialize the LCD with specific configuration commands
 * @param[IN]   None
 * @retval      None
 */
void initialice_LCD(void)
{
    // Set the first configuration - Function Set
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, init1, 0x02, false);
    // Delay
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    // Display ON: with active display (D), cursor (C), and blinking cursor (B) off.
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, init2, 0x02, false);
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    // Entry Mode: I/D=1, SD=0
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, init3, 0x02, false);
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
}

uint8_t clear[0x02] = {0x00, 0x01}; // Clear the LCD

/*
 * @brief       Clear the LCD display
 * @param[IN]   None
 * @retval      None
 */
void clear_i2c(void)
{
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, clear, 0x02, false);
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
}

/*
 * @brief       Write data to the LCD
 * @param[IN]   line: Array of data for the line
 *              msg: Array of data to be written
 * @retval      FSP_SUCCESS                  Upon successful write
 *              Any Other Error code apart from FSP_SUCCESS is  Unsuccessful write
 */
fsp_err_t write_LCD(uint8_t line[], uint8_t msg[])
{
    // Write the line data
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, line, 0x02, false);
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    // Write the message data
    err = R_IIC_MASTER_Write(&g_i2c_master_ctrl, msg, 0x09, false);
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    return err;
}

/*
 * @brief       Callback function for handling I2C master events
 * @param[IN]   p_args: Pointer to callback arguments
 * @retval      None
 */
void g_i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* Capture callback event for validating the I2C transfer event */
        i2c_event = p_args->event;
    }
}

