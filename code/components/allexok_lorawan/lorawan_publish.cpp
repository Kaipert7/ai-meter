#include "i2c_shuttle.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string>
#include "../../include/defines.h"
#include "ClassLogFile.h"
#include <functional>
static const char *TAG = "LORAWAN PUBLISH";

bool lorawan_enabled = false;
bool lorawan_initialized = false;
bool lorawan_i2c_initialized = false;
i2c_port_t lorawan_i2c_port = I2C_NUM_0;
uint8_t i2c_lorawan_addr = I2C_SHUTTLE_ADDRESS;

uint8_t dev_eui[8];
uint8_t app_eui[8];
uint8_t app_key[16];

std::string lorawan_meterType = "";
std::string lorawan_valueUnit = "";
std::string lorawan_timeUnit = "";
std::string lorawan_rateUnit = "Unit/Minute";

void Lorawan_Enable(void){
    lorawan_enabled = true;
}
int Lorawan_Init(void) { 
    if(!lorawan_enabled){
        return -1;
    }
    if (lorawan_initialized) {
        return 0;
    }
    if(!lorawan_i2c_initialized){
        i2c_config_t conf = {}; // Zero-initialize everything    
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = CAM_PIN_SIOD;
        conf.scl_io_num = CAM_PIN_SIOC;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = 100000;

        i2c_param_config(lorawan_i2c_port, &conf);
        i2c_driver_install(lorawan_i2c_port, conf.mode, 0, 0, 0);
        lorawan_i2c_initialized = true;
    }
    esp_err_t err;
    err = i2c_shuttle_set_dev_eui(lorawan_i2c_port, i2c_lorawan_addr, dev_eui);
    if (err != ESP_OK) {
        LogFile.WriteToFile(ESP_LOG_WARN, TAG, "i2c_shuttle_set_dev_eui returned Error");
        return err;
    }
    err = i2c_shuttle_set_app_eui(lorawan_i2c_port, i2c_lorawan_addr, app_eui);
    if (err != ESP_OK) {
        LogFile.WriteToFile(ESP_LOG_WARN, TAG, "i2c_shuttle_set_app_eui returned Error");
        return err;
    }
    err = i2c_shuttle_set_app_key(lorawan_i2c_port, i2c_lorawan_addr, app_key);
    if (err != ESP_OK) {
        LogFile.WriteToFile(ESP_LOG_WARN, TAG, "i2c_shuttle_set_app_key returned Error");
        return err;
    }
    err = i2c_shuttle_start(lorawan_i2c_port, i2c_lorawan_addr);

    if (err != ESP_OK) {
        LogFile.WriteToFile(ESP_LOG_WARN, TAG, "i2c_shuttle_start returned Error");
        return err;
    }

    uint8_t status = 0;
    uint16_t timeoutctr = 0;
    do{
        err = i2c_shuttle_get_status(lorawan_i2c_port,i2c_lorawan_addr,&status);
        if (err != ESP_OK) {
            LogFile.WriteToFile(ESP_LOG_WARN, TAG, "i2c_shuttle_get_status returned Error");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        timeoutctr++;
        if(timeoutctr >= 100){
            LogFile.WriteToFile(ESP_LOG_WARN, TAG, "Lorawan join error...");
            lorawan_initialized = false;
            return -1;
        }
    }while(!(status & I2C_STATUS_JOINED));

    LogFile.WriteToFile(ESP_LOG_INFO, TAG, "Lorawan joined...");
    lorawan_initialized = true;
    return 0;
}

static size_t squish(char *s)
{
    size_t w = 0;
    for (size_t r = 0; s[r] != 0; ++r) {
        char c = s[r];
        if (c != '\n' && c != '\r' && c != '\t')
            s[w++] = c;
    }
    s[w] = 0;
    return w;          // final length
}

bool LorawanPublish(const std::string &key,
                    const std::string &content)
{
    Lorawan_Init();
    if(lorawan_initialized){
        /* 1 ───── build a valid JSON object   {"name":"<key>","msg":<content>}  */
        std::string payload  = "{\"name\":\"" + key + "\",\"msg\":\"" + content + "\"}";
        uint8_t status = 0;
        std::string compact = payload;          // editable copy
        size_t      len     = squish(&compact[0]);
        compact.resize(len);                    // shrink std::string to real size

        LogFile.WriteToFile(ESP_LOG_INFO, TAG, compact.c_str());
        /* 3 ───── transmit (NO trailing NUL)                                   */
        esp_err_t err;
        err = i2c_shuttle_send_payload(
                            lorawan_i2c_port,
                            i2c_lorawan_addr,
                            reinterpret_cast<const uint8_t *>(compact.data()),
                            compact.size());

        if (err != ESP_OK) {
            LogFile.WriteToFile(ESP_LOG_WARN, TAG, "i2c_shuttle_send_payload returned Error");
            return false;
        }

        err = i2c_shuttle_trigger_send(lorawan_i2c_port,i2c_lorawan_addr);
        if (err != ESP_OK) {
            LogFile.WriteToFile(ESP_LOG_WARN, TAG, "i2c_shuttle_trigger_send returned Error");
            return false;
        }

        uint16_t timeoutctr = 0;
        do{
            err = i2c_shuttle_get_status(lorawan_i2c_port,i2c_lorawan_addr,&status);
            if (err != ESP_OK) {
                LogFile.WriteToFile(ESP_LOG_INFO, TAG, "i2c_shuttle_get_status returned Error");
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            timeoutctr++;
            if(timeoutctr > 300){
                LogFile.WriteToFile(ESP_LOG_WARN, TAG, "Lorawan send error...");
                lorawan_initialized = false;
                return 0;
            }

        }while(!(status & I2C_STATUS_SUCCESS));

        err = i2c_shuttle_clear_finished_bit(lorawan_i2c_port,i2c_lorawan_addr);
        if (err != ESP_OK) {
            LogFile.WriteToFile(ESP_LOG_WARN, TAG, "i2c_shuttle_clear_finished_bit returned Error");
            return false;
        }

        return (status & I2C_STATUS_SUCCESS) != 0;
    }
    else{
        return 0;
    }
}

void lorawanServer_setMeterType(std::string _meterType, std::string _valueUnit, std::string _timeUnit,std::string _rateUnit) {
    lorawan_meterType = _meterType;
    lorawan_valueUnit = _valueUnit;
    lorawan_timeUnit = _timeUnit;
    lorawan_rateUnit = _rateUnit;
}

std::string lorawanServer_getTimeUnit(void) {
    return lorawan_timeUnit;
}


