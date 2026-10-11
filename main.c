
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"


#define I2C_SLAVE_ADDR  0x50U        // 읽을 슬레이브의 7비트 주소 (실제 장치에 맞게 수정)
#define I2C_READ_BYTES  2U           // 한 번에 읽을 바이트 수
#define I2C_STANDARDMODE_BITRATE     100000U      // 100kHz (Standard-mode)
#define I2C_FASTMODE_BITRATE     400000U      // 400kHz (Fast-mode)


void i2c_gpio_init(void);
void i2ca_init(void);
Uint16 i2ca_read(Uint16 slaveAddr, Uint16 *rxBuf, Uint16 numBytes);

Uint16 rxdata[2];

void main(void)
{
    Uint16 rxData[I2C_READ_BYTES];
    Uint16 status;

    InitSysCtrl();

    i2ca_init();

    while(1)
    {
        status = i2ca_read(I2C_SLAVE_ADDR, rxData, I2C_READ_BYTES);

        if(status == 0)
        {
            // rxData[0], rxData[1] ... 에 슬레이브로부터 읽은 값이 들어있음
        }
        else
        {
            // NACK 등 에러 발생 (슬레이브 응답 없음, 주소 틀림 등)
        }

        DEVICE_DELAY_US(1000000);   // 2s 주기로 반복 (테스트용)
    }
}


/*
I2C-A
SDAA : GPIO104
SCLKA : GPIO105
*/
void i2c_gpio_init(void){
    GPIO_setMasterCore(104, GPIO_CORE_CPU1);          
    GPIO_setPinConfig(GPIO_104_SDAA);                 
    // GPIO_setPadConfig(104, GPIO_PIN_TYPE_PULLUP);     
    GPIO_setPadConfig(104, GPIO_PIN_TYPE_STD);               
    GPIO_setQualificationMode(104, GPIO_QUAL_ASYNC);   


    GPIO_setMasterCore(105, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_105_SCLA);
    // GPIO_setPadConfig(105, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(105, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(105, GPIO_QUAL_ASYNC);
}


// i2ca_init - GPIO mux, I2C-A 클록, 마스터 모드 통신 설정 (driverlib)
void i2ca_init(void)
{
    i2c_gpio_init();

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_I2CA);


    I2C_disableModule(I2CA_BASE);                                
    I2C_initController(I2CA_BASE, DEVICE_SYSCLK_FREQ,
                        I2C_FASTMODE_BITRATE, I2C_DUTYCYCLE_33);             
    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_RECEIVE_MODE);          
    I2C_setAddressMode(I2CA_BASE, I2C_ADDR_MODE_7BITS);              
    I2C_disableLoopback(I2CA_BASE);
    I2C_enableModule(I2CA_BASE);                                  
}


// i2ca_read - 슬레이브로부터 numBytes 바이트를 읽어서 rxBuf에 저장 (driverlib)
// 반환값: 0 = 성공, 1 = NACK/에러
Uint16 i2ca_read(Uint16 slaveAddr, Uint16 *rxBuf, Uint16 numBytes)
{
    Uint16 i;

    while(I2C_isBusBusy(I2CA_BASE) == true);

    I2C_setTargetAddress(I2CA_BASE, slaveAddr);   // 슬레이브 주소
    I2C_setDataCount(I2CA_BASE, numBytes);        // 읽을 바이트 수

    I2C_setConfig(I2CA_BASE, I2C_CONTROLLER_RECEIVE_MODE);
    I2C_sendStartCondition(I2CA_BASE);            // START 조건 발생
    I2C_sendStopCondition(I2CA_BASE);             // numBytes 만큼 받으면 자동으로 STOP

    for(i = 0; i < numBytes; i++)
    {
        while(((I2C_getStatus(I2CA_BASE) & I2C_STS_RX_DATA_RDY) == 0) &&
              ((I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK) == 0))
        {
            // 데이터 준비되거나(RX_DATA_RDY) 슬레이브 응답 없음(NACK) 상태까지 대기
        }

        if((I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK) != 0)
        {
            I2C_clearStatus(I2CA_BASE, I2C_STS_NO_ACK);  // NACK 플래그 클리어
            I2C_sendStopCondition(I2CA_BASE);             // 버스 정리를 위해 STOP 발생
            return 1;                                      // 에러로 리턴
        }

        rxdata[i]=rxBuf[i] = I2C_getData(I2CA_BASE);   // 수신 데이터 읽기 (읽으면 플래그 자동 클리어)
    }

    while(I2C_getStopConditionStatus(I2CA_BASE) == true);

    return 0;
}

