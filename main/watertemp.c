#include "include/watertemp.h"


//spinlock
static portMUX_TYPE ds18b20_spinlock = portMUX_INITIALIZER_UNLOCKED;

char tempStr[16];
char *tempStrPtr = NULL;
float tempFloat = 0;
 

// Device Commands
#define READSCRATCH     0xBE  // Read from scratchpad
#define WRITESCRATCH    0x4E  // Write to scratchpad
#define GETTEMP			0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define SKIPROM			0xCC  // Command to address all devices on the bus


// Temperature scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define DS_GPIO         32

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit Default


uint8_t init=0;
uint8_t bitResolution=12;
uint8_t devices=0;


esp_err_t ds18b20_init(int GPIO) {
	esp_err_t ret = gpio_reset_pin(DS_GPIO);
	init = 1;
    return ret;
}



esp_err_t ds18b20_reset(void){
    esp_err_t ret = ESP_OK;
	gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
  taskENTER_CRITICAL(&ds18b20_spinlock);
	gpio_set_level(DS_GPIO, 0);
	esp_rom_delay_us(480);
	gpio_set_level(DS_GPIO, 1);   
	gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);
	esp_rom_delay_us(70);
	if (gpio_get_level(DS_GPIO) != 0)
        {
            ret = ESP_ERR_TIMEOUT;
        }
  taskEXIT_CRITICAL(&ds18b20_spinlock);      
	esp_rom_delay_us(410);
	return (ret);
}


//  https://www.analog.com/en/technical-articles/1wire-communication-through-software.html     Recommended timings for one wire communication 

void ds18b20_write(char bit){
	if (bit & 1) {
		gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
    taskENTER_CRITICAL(&ds18b20_spinlock);
		gpio_set_level(DS_GPIO,0);
		esp_rom_delay_us(6);
		gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);	
    taskEXIT_CRITICAL(&ds18b20_spinlock);
		esp_rom_delay_us(64);
	} else {
		gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
		gpio_set_level(DS_GPIO,0);
		esp_rom_delay_us(60);
		gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);	
		esp_rom_delay_us(10);
	}
}



unsigned char ds18b20_read(void){
    unsigned char result = 0;
	gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
  taskENTER_CRITICAL(&ds18b20_spinlock);
	gpio_set_level(DS_GPIO, 0);
	esp_rom_delay_us(6);
	gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);
	esp_rom_delay_us(9);
	result = gpio_get_level(DS_GPIO);
  taskEXIT_CRITICAL(&ds18b20_spinlock);
	esp_rom_delay_us(55);
	return (result);
}



void ds18b20_write_byte(char data){
  unsigned char i;
  unsigned char x;
  for(i=0;i<8;i++){
    x = data>>i;
    x &= 0x01;
    ds18b20_write(x);
  }
  esp_rom_delay_us(100);
}


unsigned char ds18b20_read_byte(void){
  unsigned char i;
  unsigned char data = 0;
  for (i=0;i<8;i++)
  {
    if(ds18b20_read()) data|=0x01<<i;
    esp_rom_delay_us(15);
  }
  return(data);
}

float ds18b20_get_temp(void) {

    esp_err_t presence;
    char tempLSB=0, tempMSB=0;
      presence=ds18b20_reset();
      if(presence==ESP_OK)
      {
        ds18b20_write_byte(SKIPROM);
        ds18b20_write_byte(GETTEMP);
        vTaskDelay(750 / portTICK_PERIOD_MS); //750ms is 12 bit conversion time
        ds18b20_reset();
        ds18b20_write_byte(SKIPROM);
        ds18b20_write_byte(READSCRATCH);
        tempLSB=ds18b20_read_byte();
        tempMSB=ds18b20_read_byte();
        ds18b20_reset();
        tempFloat=(float)(tempLSB+(tempMSB<<8))/16;
        tempFloat = tempFloat*(1.8) + 32;  //See if there is way to pull farenheit from the device directly 
        return tempFloat;
      }
      else{return 0;}  //Temp is 0 if reset fails so getTempGap wont run. no response is probably a terrible error . maybe it changfes a diff colour on error? maybe i bring the oled back

}




//"Public" Functions. Ptr return prevent the tasks from switching too soon and ruining the data. 

const char* ds18b20_get_temp_s(void){

  float temp = ds18b20_get_temp();
  snprintf(tempStr, sizeof(tempStr), "%.2f", temp);
  tempStrPtr = tempStr;
  return tempStrPtr;          //simplify?
}

void ds18b20_send_mqtt(void){

  ds18b20_get_temp_s();
  send_mqtt_temp(tempStrPtr);

}

float* ds18b20_get_temp_ptr(void){
      ds18b20_get_temp();
      return &tempFloat;

}