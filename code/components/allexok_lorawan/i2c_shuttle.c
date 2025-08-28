#include "i2c_shuttle.h"
#include <string.h>

#define I2C_REG_DEVEUI      0x01
#define I2C_REG_APPEUI      0x02
#define I2C_REG_APPKEY      0x03
#define I2C_REG_REGION      0x04
#define I2C_REG_CHANMSK     0x05
#define I2C_REG_DEF_DR      0x06
#define I2C_REG_ADR         0x07
#define I2C_REG_CONFIRM     0x08
#define I2C_REG_START       0x10
#define I2C_REG_PAYLOAD     0x11
#define I2C_REG_STATUS      0x12
#define I2C_REG_TRANSMIT    0x13
#define I2C_REG_CTRANSMIT   0x14

static esp_err_t write_reg(i2c_port_t port, uint8_t addr, uint8_t reg, const uint8_t *data, size_t len)
{
    uint8_t buf[1 + 222];
    if (len > 222) {
        len = 222; // constrain to buffer size
    }
    buf[0] = reg;
    if (data && len) {
        memcpy(&buf[1], data, len);
    }
    return i2c_master_write_to_device(port, addr, buf, len + 1, pdMS_TO_TICKS(1000));
}

esp_err_t i2c_shuttle_send_payload(i2c_port_t port, uint8_t addr, const uint8_t *data, size_t len)
{
    return write_reg(port, addr, I2C_REG_PAYLOAD, data, len);
}

esp_err_t i2c_shuttle_set_dev_eui(i2c_port_t port, uint8_t addr, const uint8_t eui[8])
{
    return write_reg(port, addr, I2C_REG_DEVEUI, eui, 8);
}

esp_err_t i2c_shuttle_set_app_eui(i2c_port_t port, uint8_t addr, const uint8_t eui[8])
{
    return write_reg(port, addr, I2C_REG_APPEUI, eui, 8);
}

esp_err_t i2c_shuttle_set_app_key(i2c_port_t port, uint8_t addr, const uint8_t key[16])
{
    return write_reg(port, addr, I2C_REG_APPKEY, key, 16);
}

esp_err_t i2c_shuttle_set_region(i2c_port_t port, uint8_t addr, uint8_t region)
{
    return write_reg(port, addr, I2C_REG_REGION, &region, 1);
}

esp_err_t i2c_shuttle_set_channel_mask(i2c_port_t port, uint8_t addr, const uint16_t mask[6])
{
    uint8_t buf[12];
    for (size_t i = 0; i < 6; ++i) {
        buf[i * 2] = mask[i] & 0xFF;
        buf[i * 2 + 1] = mask[i] >> 8;
    }
    return write_reg(port, addr, I2C_REG_CHANMSK, buf, sizeof(buf));
}

esp_err_t i2c_shuttle_set_default_dr(i2c_port_t port, uint8_t addr, uint8_t dr)
{
    return write_reg(port, addr, I2C_REG_DEF_DR, &dr, 1);
}

esp_err_t i2c_shuttle_set_adr(i2c_port_t port, uint8_t addr, bool adr)
{
    uint8_t val = adr ? 1 : 0;
    return write_reg(port, addr, I2C_REG_ADR, &val, 1);
}

esp_err_t i2c_shuttle_set_confirmed(i2c_port_t port, uint8_t addr, bool confirmed, uint8_t retries)
{
    uint8_t buf[2] = { confirmed ? 1 : 0, retries };
    return write_reg(port, addr, I2C_REG_CONFIRM, buf, 2);
}

esp_err_t i2c_shuttle_start(i2c_port_t port, uint8_t addr)
{
    return write_reg(port, addr, I2C_REG_START, NULL, 0);
}

esp_err_t i2c_shuttle_get_status(i2c_port_t port, uint8_t addr, uint8_t *status)
{
    uint8_t reg = I2C_REG_STATUS;
    return i2c_master_write_read_device(port, addr, &reg, 1, status, 1, pdMS_TO_TICKS(1000));
}

esp_err_t i2c_shuttle_trigger_send(i2c_port_t port, uint8_t addr)
{
    return write_reg(port, addr, I2C_REG_TRANSMIT, NULL, 0);
}

esp_err_t i2c_shuttle_clear_finished_bit(i2c_port_t port, uint8_t addr)
{
    return write_reg(port, addr, I2C_REG_CTRANSMIT, NULL, 0);
}

