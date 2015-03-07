/******************** (C) COPYRIGHT 2014 MiaoW Labs ***************************
**	   		| PB8 - I2C1_SCL  |
**			| PB9 - I2C1_SDA  |
**			 -----------------
**********************************************************************************/
#include <DAVE3.h>
//#include "I2C.h"
#define Control_P5_0(Mode, DriveStrength)       PORT5->IOCR0 = (PORT5->IOCR0 & ~0x000000F8) | (Mode << 3); PORT5->PDR0 = (PORT5->PDR0 & ~0x00000007) | (DriveStrength)
#define Control_P5_1(Mode, DriveStrength)       PORT5->IOCR0 = (PORT5->IOCR0 & ~0x0000F800) | (Mode << 11); PORT5->PDR0 = (PORT5->PDR0 & ~0x00000070) | (DriveStrength << 4)
#define Control_P5_2(Mode, DriveStrength)       PORT5->IOCR0 = (PORT5->IOCR0 & ~0x00F80000) | (Mode << 19); PORT5->PDR0 = (PORT5->PDR0 & ~0x00000700) | (DriveStrength << 8)
// Mode Input 0x00

typedef enum INPUT_Type
{
  INPUT          = 0x00,
  INPUT_PD       = 0x01, // Input Pull Down Device
  INPUT_PU       = 0x02, // Input Pull Up Device
  INPUT_PPS      = 0x03,
  INPUT_INV      = 0x04,
  INPUT_INV_PD   = 0x05,
  INPUT_INV_PU   = 0x06,
  INPUT_INV_PPS  = 0x07
}INPUT_Type;
typedef enum OUTPUT_Type
{
  OUTPUT_PP_GP   = 0x10, // Push Pull, GP: General Purpose
  OUTPUT_PP_AF1  = 0x11, // Alternate Function 1
  OUTPUT_PP_AF2  = 0x12,
  OUTPUT_PP_AF3  = 0x13,
  OUTPUT_PP_AF4  = 0x14,
  OUTPUT_OD_GP   = 0x18, // Open Drain, GP: General Purpose
  OUTPUT_OD_AF1  = 0x19,
  OUTPUT_OD_AF2  = 0x1A,
  OUTPUT_OD_AF3  = 0x1B,
  OUTPUT_OD_AF4  = 0X1C
}OUTPUT_Type;
typedef enum PIN_Strength
{
  WEAK           = 0x7,
  MEDIUM         = 0x4,
  STRONG         = 0x2,
  VERYSTRONG     = 0x0
}PIN_Strength;

/*************************************/
#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)

#define  XMC_Soft_IIC
static volatile bool I2C_Received = FALSE;
volatile uint16_t DataReceived = 0x0000;
volatile uint32_t Value;
static volatile uint32_t SDA_IN;
static volatile uint32_t TEMP = 0;
static uint32_t I2C_Status = (uint32_t)0;
/*************************************/
#ifdef XMC_Soft_IIC

/*
 * IO004_Handle4 : SCL
 * IO004_Handle5 : SDA
 * IO004_SetOutputValue(IO004_Handle4,1);
 * IO004_SetOutputValue(IO004_Handle5,1);
 * // End of Soft I2C Pin Initialization
 * IO004_ReadPin(IO004_Handle0);
 * */

#define SCL_H         IO004_SetOutputValue(IO004_Handle0,1) /* GPIO_SetBits(GPIOB , GPIO_Pin_10)   */
#define SCL_L         IO004_SetOutputValue(IO004_Handle0,0) /* GPIO_ResetBits(GPIOB , GPIO_Pin_10) */

#define SDA_H         IO004_SetOutputValue(IO004_Handle1,1) /* GPIO_SetBits(GPIOB , GPIO_Pin_11)   */
#define SDA_L         IO004_SetOutputValue(IO004_Handle1,0) /* GPIO_ResetBits(GPIOB , GPIO_Pin_11) */

#define SCL_read      IO004_ReadPin(IO004_Handle0) /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_10) */
#define SDA_read      ReadSDA()//IO004_ReadPin(IO004_Handle1) /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_11) */


//IO004_EnableOutputDriver(&IO004_Handle0,IO004_OPENDRAIN);


static void I2C_delay(void)
{
    volatile int i = 14; // TODO： 改成12试试 原来值为7
    while (i)
        i--;
}
static uint32_t ReadSDA(void)
{
//  IO口的Strength是在IO004_Init()里面设置的
//	IO004_EnableOutputDriver(&IO004_Handle1,INPUT);
/*	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");*/
	SDA_IN = IO004_ReadPin(IO004_Handle1);
	//IO004_EnableOutputDriver(&IO004_Handle0,IO004_PUSHPULL);
	return SDA_IN;
}
static bool I2C_Start(void)
{
    SDA_H;
    SCL_H;
    I2C_delay();
    I2C_delay();
    if (!SDA_read)
        return FALSE; // 判断SDA是否被成功拉高
  //  IO004_EnableOutputDriver(&IO004_Handle1,OUTPUT_OD_GP);
    SDA_L;            // 拉低SDA，用于产生START信号
    I2C_delay();      // 让SDA保持低

    if (SDA_read)     // 读取SDA确保SDA被成功拉低
         return FALSE;
   // IO004_EnableOutputDriver(&IO004_Handle1,OUTPUT_OD_GP);
    SDA_L;            // 读取SDA后把SDA从输入变为输出,需要重新配置SDA的状态：低
    I2C_delay();      // 延时一段时间
    I2C_delay();
    return TRUE;
}

static void I2C_Stop(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
}

static void I2C_Ack(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

static void I2C_NoAck(void)
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

static bool I2C_WaitAck(void)
{
    SCL_L;
    I2C_delay();
   // SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    I2C_delay();
    I2C_delay();
    I2C_delay();
    if (SDA_read) {
        SCL_L;
        I2C_delay();
        TEMP  = SDA_IN;
       return FALSE;
    }
   // IO004_EnableOutputDriver(&IO004_Handle1,OUTPUT_OD_GP);
    SCL_L;
    //
    I2C_delay();
    return TRUE;
}

static void I2C_SendByte(uint8_t byte)
{
    uint8_t i = 8;
    while (i--) {
        SCL_L;           // 拉低SCL准备给SLAVE传送数据
        I2C_delay();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}

static uint8_t I2C_ReceiveByte(void)
{
    uint8_t i = 8;
    uint8_t byte = 0;

    SDA_H;
    while (i--) {
        byte <<= 1;
        SCL_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        if (SDA_read) {
            byte |= 0x01;
        }
        IO004_EnableOutputDriver(&IO004_Handle1,OUTPUT_OD_GP);
    }
    SCL_L;
    return byte;
}

#if 0
void i2cInit(void)
{
  GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &gpio);
	/*
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

    //PB8,9 SCL and SDA
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // I2C1_SCL on PB08, I2C1_SDA on PB09
    GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);   //

    //I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    //I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    //I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;//100K

    I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStructure);
    //
    I2C_AcknowledgeConfig(I2C1, ENABLE);   */
}
#endif
bool i2cWriteBuffer(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
    int i;
    if (!I2C_Start())
    {
        return FALSE;
    }
    I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);//I2C_Direction_Transmitter:д
    if (!I2C_WaitAck()) {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(reg);
    I2C_WaitAck();
    for (i = 0; i < len; i++) {
        I2C_SendByte(data[i]);
        if (!I2C_WaitAck()) {
            I2C_Stop();
            return FALSE;
        }
    }
    I2C_Stop();
    return TRUE;
}
#endif

#ifdef stm32

#define SCL_H         GPIOB->BSRR = GPIO_Pin_8 /* GPIO_SetBits(GPIOB , GPIO_Pin_10)   */
#define SCL_L         GPIOB->BRR  = GPIO_Pin_8 /* GPIO_ResetBits(GPIOB , GPIO_Pin_10) */

#define SDA_H         GPIOB->BSRR = GPIO_Pin_9 /* GPIO_SetBits(GPIOB , GPIO_Pin_11)   */
#define SDA_L         GPIOB->BRR  = GPIO_Pin_9 /* GPIO_ResetBits(GPIOB , GPIO_Pin_11) */

#define SCL_read      GPIOB->IDR  & GPIO_Pin_8 /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_10) */
#define SDA_read      GPIOB->IDR  & GPIO_Pin_9 /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_11) */


static void I2C_delay(void)
{
    volatile int i = 7;
    while (i)
        i--;
}

static bool I2C_Start(void)
{
    SDA_H;
    SCL_H;
    I2C_delay();
    if (!SDA_read)
        return FALSE;
    SDA_L;
    I2C_delay();
    if (SDA_read)
        return FALSE;
    SDA_L;
    I2C_delay();
    return TRUE;
}

static void I2C_Stop(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
}

static void I2C_Ack(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

static void I2C_NoAck(void)
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

static bool I2C_WaitAck(void)
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    if (SDA_read) {
        SCL_L;
        return FALSE;
    }
    SCL_L;
    return TRUE;
}

static void I2C_SendByte(uint8_t byte)
{
    uint8_t i = 8;
    while (i--) {
        SCL_L;
        I2C_delay();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}

static uint8_t I2C_ReceiveByte(void)
{
    uint8_t i = 8;
    uint8_t byte = 0;

    SDA_H;
    while (i--) {
        byte <<= 1;
        SCL_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        if (SDA_read) {
            byte |= 0x01;
        }
    }
    SCL_L;
    return byte;
}

void i2cInit(void)
{
  GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &gpio);
	/*
    I2C_InitTypeDef  I2C_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

    //PB8,9 SCL and SDA
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // I2C1_SCL on PB08, I2C1_SDA on PB09
    GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);   //

    //I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    //I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    //I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;//100K

    I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStructure);
    //
    I2C_AcknowledgeConfig(I2C1, ENABLE);   */
}

bool i2cWriteBuffer(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
    int i;
    if (!I2C_Start())
        return FALSE;
    I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);//I2C_Direction_Transmitter:д
    if (!I2C_WaitAck()) {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(reg);
    I2C_WaitAck();
    for (i = 0; i < len; i++) {
        I2C_SendByte(data[i]);
        if (!I2C_WaitAck()) {
            I2C_Stop();
            return FALSE;
        }
    }
    I2C_Stop();
    return TRUE;
}
#endif
/////////////////////////////////////////////////////////////////////////////////
#ifdef XMC_Soft_IIC
bool i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    if (!I2C_Start());
        return FALSE;
    I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck()) {
        I2C_Stop();
         return FALSE;
    }
    I2C_SendByte(reg);
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(addr << 1 | I2C_Direction_Receiver);
    I2C_WaitAck();
    while (len) {
        *buf = I2C_ReceiveByte();
        if (len == 1)
            I2C_NoAck();
        else
            I2C_Ack();
        buf++;
        len--;
    }
    I2C_Stop();
    return TRUE;
}
int8_t i2cwrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
	 if(i2cWriteBuffer(addr,reg,len,data))
	/*if(XMCi2cWriteBuffer(addr,reg,len,data))*/
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//return FALSE;
}
int8_t i2cread(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	 if(i2cRead(addr,reg,len,buf))
	/*if(XMCi2cRead(addr,reg,len,buf))*/
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//return FALSE;
}
#else
int8_t i2cwrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
	/* if(i2cWriteBuffer(addr,reg,len,data)) */
	if(XMCi2cWriteBuffer(addr,reg,len,data))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//return FALSE;
}
int8_t i2cread(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	/* if(i2cRead(addr,reg,len,buf)) */
	if(XMCi2cRead(addr,reg,len,buf))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//return FALSE;
}
#endif
#ifdef stm32
//////////////////////////////////////////////////////////////////////////////////
bool i2cWrite(uint8_t addr, uint8_t reg, uint8_t data)
{
    if (!I2C_Start())
        return FALSE;
    I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck()) {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(reg);
    I2C_WaitAck();
    I2C_SendByte(data);
    I2C_WaitAck();
    I2C_Stop();
    return TRUE;
}

bool i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    if (!I2C_Start())
        return FALSE;
    I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck()) {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(reg);
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(addr << 1 | I2C_Direction_Receiver);
    I2C_WaitAck();
    while (len) {
        *buf = I2C_ReceiveByte();
        if (len == 1)
            I2C_NoAck();
        else
            I2C_Ack();
        buf++;
        len--;
    }
    I2C_Stop();
    return TRUE;
}
#endif

uint16_t i2cGetErrorCounter(void)
{
    // TODO maybe fix this, but since this is test code, doesn't matter.
    return 0;
}




int main()
{

	IO004_EnableOutputDriver(&IO004_Handle0,OUTPUT_OD_GP);
	IO004_EnableOutputDriver(&IO004_Handle1,OUTPUT_OD_GP);
	uint8_t test[2] = {0x80,0x45};
	DAVE_Init();
	while(1)
	{
		I2C_Status = i2cwrite(0x68,0x6B,1,&test[0]);
		Value = SCL_read;
/*		SCL_H;
		SCL_L;*/
	}
	return 0;
}
