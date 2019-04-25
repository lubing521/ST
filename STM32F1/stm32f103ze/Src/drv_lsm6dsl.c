#include "user_config.h"
#ifdef USER_USING_LSM6DSL
#include "drv_lsm6dsl.h"

extern SPI_HandleTypeDef hspi1;


extern void DMA_Usart_Send(uint8_t *buf,uint8_t len);//串口发送封装

extern void lsm6dsl_write_reg1(uint8_t reg, uint8_t value);
//
extern uint8_t lsm6dsl_read_reg1(uint8_t reg);
extern char str[100];

uint8_t SPI1SendByte(uint8_t data)
{
    unsigned char writeCommand[1];
    unsigned char readValue[1];

    writeCommand[0] = data;
//	HAL_GPIO_WritePin(BSP_LSM6DSL_CS_PORT, BSP_LSM6DSL_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive_DMA(&hspi1, (uint8_t*)&writeCommand, (uint8_t*)&readValue, 1);
//	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&writeCommand, (uint8_t*)&readValue, 1,10);

//	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)&writeCommand,1);
//    while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_BSY) == SET);
// 	HAL_SPI_Receive_DMA(&hspi1, (uint8_t*)&readValue,1);
    while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_BSY) == SET);

//HAL_GPIO_WritePin(BSP_LSM6DSL_CS_PORT, BSP_LSM6DSL_CS_PIN, GPIO_PIN_SET);

    return readValue[0];
}
void SPI1_WriteReg(uint8_t address, uint8_t value)
{
    HAL_GPIO_WritePin(BSP_LSM6DSL_CS_PORT, BSP_LSM6DSL_CS_PIN, GPIO_PIN_RESET);
    SPI1SendByte(address);
    SPI1SendByte(value);
    HAL_GPIO_WritePin(BSP_LSM6DSL_CS_PORT, BSP_LSM6DSL_CS_PIN, GPIO_PIN_SET);
}

uint8_t SPI1_ReadReg(uint8_t address)
{
    uint8_t	val;

    HAL_GPIO_WritePin(BSP_LSM6DSL_CS_PORT, BSP_LSM6DSL_CS_PIN, GPIO_PIN_RESET);
    SPI1SendByte(address);
    val = SPI1SendByte(0xff);
    HAL_GPIO_WritePin(BSP_LSM6DSL_CS_PORT, BSP_LSM6DSL_CS_PIN, GPIO_PIN_SET);
    return val;
}
uint8_t SPI1_ReadBuffer(uint8_t address,uint8_t* rv_buffer,uint8_t length)
{
    uint8_t i=0;
    HAL_GPIO_WritePin(BSP_LSM6DSL_CS_PORT, BSP_LSM6DSL_CS_PIN, GPIO_PIN_RESET);
    SPI1SendByte(address);
    for(; i<length; i++) {
        rv_buffer[i] = SPI1SendByte(0xff);
    }
    HAL_GPIO_WritePin(BSP_LSM6DSL_CS_PORT, BSP_LSM6DSL_CS_PIN, GPIO_PIN_SET);

    return i;
}
void lsm6dsl_write_reg1(uint8_t reg, uint8_t value)
{
    uint8_t send_buffer[2];
    send_buffer[0] = reg & 0x7f;
    send_buffer[1] = value;
    SPI1_WriteReg(send_buffer[0],send_buffer[1]);


}


uint8_t lsm6dsl_read_reg1(uint8_t reg)
{
    uint8_t rxBuf[2];
    uint8_t txBuf[2];
    txBuf[0] = (reg | 0x80) & 0xbf; // reg|0x80;//SPI2_ReadWriteByte((reg|0x80)&0xbf);

    rxBuf[0]=SPI1_ReadReg(txBuf[0]);

    return rxBuf[0];
}

static uint8_t lsm6dsl_read_buffer1(uint8_t reg, uint8_t *buffer, uint16_t len)
{
    uint8_t txBuf[2];
//    uint8_t rxBuf[20];

    txBuf[0] = (reg | 0x80) & 0xbf; // reg|0x80;
    SPI1_ReadBuffer(txBuf[0],buffer,len);

    return 0;
}

int lsm6dsl_read_alldata1( int16_t *buffer)
{
//	uint8_t buff1[2]={LSM6DSL_OUT_TEMP_L,LSM6DSL_OUT_TEMP_L};
//	 spi_send_then_recv(spi_imu_lsm6dsl->spi_device, buff1, 1, (uint8_t*)buffer, 14);

    lsm6dsl_read_buffer1(LSM6DSL_OUT_TEMP_L,(uint8_t*)buffer,14);
    return    0;
}

uint8_t lsm6dsl_get_accel(int16_t *accel, int16_t *temp)
{
    uint8_t buf[8];
    lsm6dsl_read_buffer1(LSM6DSL_OUTX_L_XL, buf, 6);

    accel[0] = *((int16_t*)(buf + 0))    ; //((int16_t)buf[0]<<8) + buf[1];
    accel[1] = *((int16_t*)(buf + 2))    ; //((int16_t)buf[2]<<8) + buf[3];
    accel[2] = *((int16_t*)(buf + 4))    ; //((int16_t)buf[4]<<8) + buf[5];

    return 0;
}


uint8_t lsm6dsl_get_gyro(int16_t *gyro)
{
    uint8_t buf[8];
    lsm6dsl_read_buffer1(LSM6DSL_OUTX_L_G, buf, 8);

    gyro[0] = *((int16_t*)(buf + 0))    ; // (buf[0]<<8) + buf[1];
    gyro[1] = *((int16_t*)(buf + 2))    ; //(buf[2]<<8) + buf[3];
    gyro[2] = *((int16_t*)(buf + 4))    ; //(buf[4]<<8) + buf[5];

    return 0;
}
uint8_t lsm6dsl_get_temp(int16_t *temp)
{
    uint8_t buf[4];
    lsm6dsl_read_buffer1(LSM6DSL_OUT_TEMP_L, buf, 2);

    temp[0] = *((int16_t*)(buf + 0))    ; // (buf[0]<<8) + buf[1];

    return 0;
}

uint8_t lsm6dsl_get_AllData(int16_t *AllData)
{
    lsm6dsl_read_buffer1(LSM6DSL_OUT_TEMP_L, (uint8_t*)AllData, 14);
    return 0;
}



int lsm6dsl_IMU_init(void)
{
    uint8_t id;
    uint8_t ImuSetBuffer = 0;

    /* init sensor*/


    lsm6dsl_write_reg1(LSM6DSL_CTRL3_C, 0x01);  //复位，

    for(long i=0xfffff; i>0; i--);
//    lsm6dsl_write_reg(LSM6DSL_PWR_MGMT_1, 0x01);      //关闭睡眠，自动选择时钟
//    thread_delay(50);

    id = lsm6dsl_read_reg1(LSM6DSL_WHO_AM_I);//读取ID
    sprintf(str,"lsm6dsl id=0x%x\r\n", id);
    DMA_Usart_Send((uint8_t*)str,30);
    if(id != 0x6a)
    {
        sprintf(str,"lsm6dsl id error !!!\r\n");
        DMA_Usart_Send((uint8_t*)str,30);
        return -1;

    }

//    kprintf("lsm6dsl init pass\r\n");


    //设置量程// ACC scale
    ImuSetBuffer = 0;
#if LSM6DSL_ACC_SCALE == 2
    ImuSetBuffer |= LSM6DSL_FS_XL_2g;
#elif LSM6DSL_ACC_SCALE == 4
    ImuSetBuffer |= LSM6DSL_FS_XL_4g;
#elif LSM6DSL_ACC_SCALE == 8
    ImuSetBuffer |= LSM6DSL_FS_XL_8g;
#else //LSM6DSL_ACC_SCALE == 16
    ImuSetBuffer |= LSM6DSL_FS_XL_16g;
#endif
    //设置acc采样率
#ifdef LSM6DSL_ACC_SAMPLERATE
    ImuSetBuffer |= LSM6DSL_ACC_SAMPLERATE;
#else
    ImuSetBuffer |= LSM6DSL_ODR_XL_1660Hz;
#endif
    lsm6dsl_write_reg1(LSM6DSL_CTRL1_XL, ImuSetBuffer);
    for(long i=0xfffff; i>0; i--);

//acc lpf
    ImuSetBuffer = 0;
    ImuSetBuffer |= 0xa9;//0xc9; //  odr / 9,     400/9 0b1100 1001.odr/50=0b1000 1001.odr/100=0b1010 1001
    lsm6dsl_write_reg1(LSM6DSL_CTRL8_XL, ImuSetBuffer);



    for(long i=0xfffff; i>0; i--);
    // GYO scale

    ImuSetBuffer = 0;
#if LSM6DSL_GYO_SCALE == 250
    ImuSetBuffer |= LSM6DSL_FS_G_250dps;
#elif LSM6DSL_GYO_SCALE == 500
    ImuSetBuffer |= LSM6DSL_FS_G_500dps;
#elif LSM6DSL_GYO_SCALE == 1000
    ImuSetBuffer |= LSM6DSL_FS_G_1000dps;
#else//elif LSM6DSL_GYO_SCALE == 2000
    ImuSetBuffer |= LSM6DSL_FS_G_2000dps;
#endif
    //设置gyo采样率
#ifdef LSM6DSL_GYO_SAMPLERATE
    ImuSetBuffer |= LSM6DSL_GYO_SAMPLERATE;
#else
    ImuSetBuffer |= LSM6DSL_ODR_G_1660Hz;
#endif
    lsm6dsl_write_reg1(LSM6DSL_CTRL2_G, ImuSetBuffer);

    for(long i=0xfffff; i>0; i--);
//gyo lpf
    ImuSetBuffer = 0;
    ImuSetBuffer |= 0x06; //  使能lpf /禁用iic 禁用睡眠
    lsm6dsl_write_reg1(LSM6DSL_CTRL4_C, ImuSetBuffer);
    for(long i=0xfffff; i>0; i--);

    ImuSetBuffer = 0;
    ImuSetBuffer |= 0x02; //  odr=800.lpf=155
    lsm6dsl_write_reg1(LSM6DSL_CTRL6_G, ImuSetBuffer);
    sprintf(str,"configure complete!\r\n");
    DMA_Usart_Send((uint8_t*)str,30);

    return 0;
}








#endif

