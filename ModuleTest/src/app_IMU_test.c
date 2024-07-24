/*
 * app_IMU_test.c
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#define	APP_IMU_TEST_C

#include "main.h"

#include "lsm6dsl_reg.h"
#include "lis2mdl_reg.h"
#include <string.h>
#include <stdio.h>



/*******************************************************************************
* LOCAL DEFINES
********************************************************************************/

#define TX_BUF_DIM          1000
#define BOOT_TIME          15 //ms

/*******************************************************************************
* LOCAL TYPEDEFS 
********************************************************************************/

/*******************************************************************************
* LOCAL VARIABLES
********************************************************************************/
static int16_t data_raw_acceleration[3];
static int16_t data_raw_angular_rate[3];
static int16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;
static uint8_t whoamI, rst;
static uint8_t tx_buffer[TX_BUF_DIM];

static int16_t data_raw_magnetic[3];
static float magnetic_mG[3];


stmdev_ctx_t dev_ctx,dev_ctx_lis2mdl;

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES
********************************************************************************/
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void platform_delay(uint32_t ms);

void lsm6dsl_Init(void);
void lsm6dsl_read_data_polling(void);

// LIS2MDL Magnetometer
static int32_t platform_write_lis2mdl(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read_lis2mdl(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);

void lis2mdl_Init(void);
void lis2mdl_read_data_polling(void);


#define SENSOR_BUS hi2c2

/*******************************************************************************
* FUNCTIONS
********************************************************************************/

/**
* @brief LSM6DSL read data testing..
*  
* @retval None
*/
void app_IMU_Test(void) {

    ULONG currentValue = 0;
     
    if (tx_semaphore_get(&semaphore_buttonpress, TX_NO_WAIT) == TX_SUCCESS)
    {
        tx_semaphore_info_get(&semaphore_buttonpress, NULL, &currentValue, NULL, NULL, NULL);
        printf("\r\n[IMU-] Test starting\r\n");
        HAL_GPIO_WritePin(SA0_LSM6D_GPIO_Port,SA0_LSM6D_Pin,RESET);

        lsm6dsl_Init();
        while (tx_semaphore_get(&semaphore_buttonpress, TX_NO_WAIT) != TX_SUCCESS) {
            tx_thread_sleep(5);
            lsm6dsl_read_data_polling();
        }

        lis2mdl_Init();
        while (tx_semaphore_get(&semaphore_buttonpress, TX_NO_WAIT) != TX_SUCCESS) {
            tx_thread_sleep(5);
            lis2mdl_read_data_polling();
        }

    }
}


//////////////////////////////////////////////////////////////////
/**
* @brief LSM6DSL check & Init sensor..
*  
* @retval None
*/
void lsm6dsl_Init(void)
{
  /* Initialize mems driver interface */
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.mdelay = platform_delay;
  dev_ctx.handle = &SENSOR_BUS;

  /* Wait sensor boot time */
  platform_delay(BOOT_TIME);
  /* Check device ID */
  whoamI = 0;
  lsm6dsl_device_id_get(&dev_ctx, &whoamI);

  if ( whoamI != LSM6DSL_ID ){
    printf("\r\n[IMU-LSM6DSL] (3D Acc-Gyro) sensor not found\r\n");
    Error_Handler();
  }
  else
    printf("\r\n[IMU-LSM6DSL] (3D Acc-Gyro) sensor detected\r\n");

  /* Restore default configuration */
  lsm6dsl_reset_set(&dev_ctx, PROPERTY_ENABLE);

  do {
    lsm6dsl_reset_get(&dev_ctx, &rst);
  } while (rst);

  /* Enable Block Data Update */
  lsm6dsl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set Output Data Rate */
  lsm6dsl_xl_data_rate_set(&dev_ctx, LSM6DSL_XL_ODR_12Hz5);
  lsm6dsl_gy_data_rate_set(&dev_ctx, LSM6DSL_GY_ODR_12Hz5);
  /* Set full scale */
  lsm6dsl_xl_full_scale_set(&dev_ctx, LSM6DSL_2g);
  lsm6dsl_gy_full_scale_set(&dev_ctx, LSM6DSL_2000dps);
  /* Configure filtering chain(No aux interface) */
  /* Accelerometer - analog filter */
  lsm6dsl_xl_filter_analog_set(&dev_ctx, LSM6DSL_XL_ANA_BW_400Hz);
  /* Accelerometer - LPF1 path ( LPF2 not used )*/
  //lsm6dsl_xl_lp1_bandwidth_set(&dev_ctx, LSM6DSL_XL_LP1_ODR_DIV_4);
  /* Accelerometer - LPF1 + LPF2 path */
  lsm6dsl_xl_lp2_bandwidth_set(&dev_ctx,
                               LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);
  /* Accelerometer - High Pass / Slope path */
  //lsm6dsl_xl_reference_mode_set(&dev_ctx, PROPERTY_DISABLE);
  //lsm6dsl_xl_hp_bandwidth_set(&dev_ctx, LSM6DSL_XL_HP_ODR_DIV_100);
  /* Gyroscope - filtering chain */
  lsm6dsl_gy_band_pass_set(&dev_ctx, LSM6DSL_HP_260mHz_LP1_STRONG);
}
/**
* @brief LSM6DSL data polling..
*  
* @retval None
*/
void lsm6dsl_read_data_polling(void)
{
  /* Read samples in polling mode (no int) */
    /* Read output only if new value is available */
    lsm6dsl_reg_t reg;
    lsm6dsl_status_reg_get(&dev_ctx, &reg.status_reg);

    if (reg.status_reg.xlda) {
      /* Read magnetic field data */
      memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
      lsm6dsl_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
      acceleration_mg[0] = lsm6dsl_from_fs2g_to_mg(
                             data_raw_acceleration[0]);
      acceleration_mg[1] = lsm6dsl_from_fs2g_to_mg(
                             data_raw_acceleration[1]);
      acceleration_mg[2] = lsm6dsl_from_fs2g_to_mg(
                             data_raw_acceleration[2]);
      //printf("Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
    }

    if (reg.status_reg.gda) {
      /* Read magnetic field data */
      memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
      lsm6dsl_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
      angular_rate_mdps[0] = lsm6dsl_from_fs2000dps_to_mdps(
                               data_raw_angular_rate[0]);
      angular_rate_mdps[1] = lsm6dsl_from_fs2000dps_to_mdps(
                               data_raw_angular_rate[1]);
      angular_rate_mdps[2] = lsm6dsl_from_fs2000dps_to_mdps(
                               data_raw_angular_rate[2]);
      //printf("Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
    }

    if (reg.status_reg.tda) {
      /* Read temperature data */
      memset(&data_raw_temperature, 0x00, sizeof(int16_t));
      lsm6dsl_temperature_raw_get(&dev_ctx, &data_raw_temperature);
      temperature_degC = lsm6dsl_from_lsb_to_celsius(
                           data_raw_temperature );
//      printf("Temperature [degC]:%2.2f\r\n",temperature_degC);
      //tx_com( tx_buffer, strlen( (char const *)tx_buffer ) );
    }
    printf("Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\t\tAngular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
    acceleration_mg[0], acceleration_mg[1], acceleration_mg[2],angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
  HAL_I2C_Mem_Write(handle, LSM6DSL_I2C_ADD_L, reg,
                    I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);
  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
  HAL_I2C_Mem_Read(handle, LSM6DSL_I2C_ADD_L, reg,
                   I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
  return 0;
}

/**
* @brief LIS2MDL check & Init sensor..
*  
* @retval None
*/
void lis2mdl_Init(void)
{
  /* Initialize magnetometre driver interface */
  dev_ctx_lis2mdl.write_reg = platform_write_lis2mdl;
  dev_ctx_lis2mdl.read_reg = platform_read_lis2mdl;
  dev_ctx_lis2mdl.mdelay = platform_delay;
  dev_ctx_lis2mdl.handle = &SENSOR_BUS;

  /* Check device ID */
  lis2mdl_device_id_get(&dev_ctx_lis2mdl, &whoamI);

  if (whoamI != LIS2MDL_ID){
    printf("\r\n[IMU-LIS2MDL] magnetic sensor not found\r\n");
    Error_Handler();
  }
  else
  {
    printf("\r\n[IMU-LIS2MDL] magnetic sensor detected\r\n");
  }
  /* Restore default configuration */
  lis2mdl_reset_set(&dev_ctx_lis2mdl, PROPERTY_ENABLE);

  do {
    lis2mdl_reset_get(&dev_ctx_lis2mdl, &rst);
  } while (rst);

  /* Enable Block Data Update */
  lis2mdl_block_data_update_set(&dev_ctx_lis2mdl, PROPERTY_ENABLE);
  /* Set Output Data Rate */
  lis2mdl_data_rate_set(&dev_ctx_lis2mdl, LIS2MDL_ODR_10Hz);
  /* Set / Reset sensor mode */
  lis2mdl_set_rst_mode_set(&dev_ctx_lis2mdl, LIS2MDL_SENS_OFF_CANC_EVERY_ODR);
  /* Enable temperature compensation */
  lis2mdl_offset_temp_comp_set(&dev_ctx_lis2mdl, PROPERTY_ENABLE);
  /* Set device in continuous mode */
  lis2mdl_operating_mode_set(&dev_ctx_lis2mdl, LIS2MDL_CONTINUOUS_MODE);


}
/**
* @brief LIS2MDL data polling..
*  
* @retval None
*/
void lis2mdl_read_data_polling(void)
{
    uint8_t reg;
    /* Read output only if new value is available */
    lis2mdl_mag_data_ready_get(&dev_ctx_lis2mdl, &reg);

    if (reg) {
      /* Read magnetic field data */
      memset(data_raw_magnetic, 0x00, 3 * sizeof(int16_t));
      lis2mdl_magnetic_raw_get(&dev_ctx_lis2mdl, data_raw_magnetic);
      magnetic_mG[0] = lis2mdl_from_lsb_to_mgauss(data_raw_magnetic[0]);
      magnetic_mG[1] = lis2mdl_from_lsb_to_mgauss(data_raw_magnetic[1]);
      magnetic_mG[2] = lis2mdl_from_lsb_to_mgauss(data_raw_magnetic[2]);


      /* Read temperature data */
      memset(&data_raw_temperature, 0x00, sizeof(int16_t));
      lis2mdl_temperature_raw_get(&dev_ctx_lis2mdl, &data_raw_temperature);
      temperature_degC = lis2mdl_from_lsb_to_celsius(data_raw_temperature);

      printf("Magnetic field [mG]:%4.2f\t%4.2f\t%4.2f\t\tTemperature [degC]:%2.2f\r\n",
      magnetic_mG[0], magnetic_mG[1], magnetic_mG[2],temperature_degC);
    }


}
/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write_lis2mdl(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
  /* Write multiple command */
  reg |= 0x80;
  HAL_I2C_Mem_Write(handle, LIS2MDL_I2C_ADD, reg,
                    I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);

  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read_lis2mdl(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
  /* Read multiple command */
  reg |= 0x80;
  HAL_I2C_Mem_Read(handle, LIS2MDL_I2C_ADD, reg,
                   I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
  return 0;
}

/*
 * @brief  platform specific delay (platform dependent)
 *
 * @param  ms        delay in ms
 *
 */
static void platform_delay(uint32_t ms)
{
    HAL_Delay(ms);
}