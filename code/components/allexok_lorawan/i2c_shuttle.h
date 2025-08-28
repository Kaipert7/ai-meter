#ifndef I2C_SHUTTLE_H
#define I2C_SHUTTLE_H

#include <stdint.h>
#include <stddef.h>
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_SHUTTLE_ADDRESS 0x55

#define I2C_REG_STATUS 0x12
#define I2C_STATUS_DONE     0x01
#define I2C_STATUS_SUCCESS  0x02
#define I2C_STATUS_JOINED   0x04
#define I2C_STATUS_SEND     0x08

/** Write a LoRaWAN payload */
esp_err_t i2c_shuttle_send_payload(i2c_port_t port, uint8_t addr, const uint8_t *data, size_t len);

/** Set the device EUI */
esp_err_t i2c_shuttle_set_dev_eui(i2c_port_t port, uint8_t addr, const uint8_t eui[8]);

/** Set the application EUI */
esp_err_t i2c_shuttle_set_app_eui(i2c_port_t port, uint8_t addr, const uint8_t eui[8]);

/** Set the application key */
esp_err_t i2c_shuttle_set_app_key(i2c_port_t port, uint8_t addr, const uint8_t key[16]);

/** Set the LoRaWAN region */
esp_err_t i2c_shuttle_set_region(i2c_port_t port, uint8_t addr, uint8_t region);

/** Set the channel mask */
esp_err_t i2c_shuttle_set_channel_mask(i2c_port_t port, uint8_t addr, const uint16_t mask[6]);

/** Set the default data rate */
esp_err_t i2c_shuttle_set_default_dr(i2c_port_t port, uint8_t addr, uint8_t dr);

/** Enable or disable ADR */
esp_err_t i2c_shuttle_set_adr(i2c_port_t port, uint8_t addr, bool adr);

/** Set confirmed/unconfirmed mode and retry count */
esp_err_t i2c_shuttle_set_confirmed(i2c_port_t port, uint8_t addr, bool confirmed, uint8_t retries);

/** Start LoRaWAN operation */
esp_err_t i2c_shuttle_start(i2c_port_t port, uint8_t addr);

/** Get the last TX status */
esp_err_t i2c_shuttle_get_status(i2c_port_t port, uint8_t addr, uint8_t *status);

/** Trigger send transaction */
esp_err_t i2c_shuttle_trigger_send(i2c_port_t port, uint8_t addr);

/** Clear transaction finished bit */
esp_err_t i2c_shuttle_clear_finished_bit(i2c_port_t port, uint8_t addr);

#ifdef __cplusplus
}

#include <string>
#include <functional>
/**
 * Convenience helper that mimics an MQTT publish call and forwards the
 * payload to the I2C shuttle for LoRaWAN transmission. The arguments
 * are embedded in a JSON payload so the remote side can decode the
 * metadata.
 */
bool LorawanPublish(const std::string &key,
                    const std::string &content);

void Lorawan_Enable(void);

int Lorawan_Init(void);

void lorawanServer_setMeterType(std::string _meterType, std::string _valueUnit, std::string _timeUnit,std::string _rateUnit);

std::string lorawanServer_getTimeUnit(void);
#endif

#endif // I2C_SHUTTLE_H
