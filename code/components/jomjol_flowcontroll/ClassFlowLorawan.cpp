#ifdef ENABLE_LORAWAN

#include <sstream>
#include <iomanip>
#include "ClassFlowLorawan.h"
#include "ClassLogFile.h"

#include "ClassFlowPostProcessing.h"
#include "ClassFlowControll.h"

#include "i2c_shuttle.h"

#include <time.h>
#include "../../include/defines.h"
#include "battery_adc.h"

static const char *TAG = "LORAWAN CLASS FLOW";

ClassFlowLorawan::ClassFlowLorawan(std::vector<ClassFlow*>* lfc)
{
    SetInitialParameter();

    ListFlowControll = lfc;
    for (int i = 0; i < ListFlowControll->size(); ++i)
    {
        if (((*ListFlowControll)[i])->name().compare("ClassFlowPostProcessing") == 0)
        {
            flowpostprocessing = (ClassFlowPostProcessing*) (*ListFlowControll)[i];
        }
    }
}

ClassFlowLorawan::ClassFlowLorawan(std::vector<ClassFlow*>* lfc, ClassFlow *_prev)
{
    SetInitialParameter();

    previousElement = _prev;
    ListFlowControll = lfc;

    for (int i = 0; i < ListFlowControll->size(); ++i)
    {
        if (((*ListFlowControll)[i])->name().compare("ClassFlowPostProcessing") == 0)
        {
            flowpostprocessing = (ClassFlowPostProcessing*) (*ListFlowControll)[i];
        }
    }
}

static inline int hex_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool ClassFlowLorawan::ReadParameter(FILE* pfile, string& aktparamgraph)
{
    std::vector<string> splitted;

    aktparamgraph = trim(aktparamgraph);

    if (aktparamgraph.size() == 0)
        if (!this->GetNextParagraph(pfile, aktparamgraph))
            return false;

    if (toUpper(aktparamgraph).compare("[LORAWAN]") != 0)       // Paragraph does not fit MQTT
        return false;

    while (this->getNextLine(pfile, &aktparamgraph) && !this->isNewParagraph(aktparamgraph))
    {
        splitted = ZerlegeZeile(aktparamgraph);
        std::string _param = GetParameterName(splitted[0]);
        if ((toUpper(_param) == "METERTYPE") && (splitted.size() > 1)) {
        /* Use meter type for the device class 
           Make sure it is a listed one on https://developers.home-assistant.io/docs/core/entity/sensor/#available-device-classes */
            if (toUpper(splitted[1]) == "WATER_M3") {
                lorawanServer_setMeterType("water", "m³", "h", "m³/h");
            }
            else if (toUpper(splitted[1]) == "WATER_L") {
                lorawanServer_setMeterType("water", "L", "h", "L/h");
            }
            else if (toUpper(splitted[1]) == "WATER_FT3") {
                lorawanServer_setMeterType("water", "ft³", "min", "ft³/min"); // min = Minutes
            }
            else if (toUpper(splitted[1]) == "WATER_GAL") {
                lorawanServer_setMeterType("water", "gal", "h", "gal/h");
            }
            else if (toUpper(splitted[1]) == "GAS_M3") {
                lorawanServer_setMeterType("gas", "m³", "h", "m³/h");
            }
            else if (toUpper(splitted[1]) == "GAS_FT3") {
                lorawanServer_setMeterType("gas", "ft³", "min", "ft³/min"); // min = Minutes
            }
            else if (toUpper(splitted[1]) == "ENERGY_WH") {
                lorawanServer_setMeterType("energy", "Wh", "h", "W");
            }
            else if (toUpper(splitted[1]) == "ENERGY_KWH") {
                lorawanServer_setMeterType("energy", "kWh", "h", "kW");
            }
            else if (toUpper(splitted[1]) == "ENERGY_MWH") {
                lorawanServer_setMeterType("energy", "MWh", "h", "MW");
            }
            else if (toUpper(splitted[1]) == "ENERGY_GJ") {
                lorawanServer_setMeterType("energy", "GJ", "h", "GJ/h");
            }
            else if (toUpper(splitted[1]) == "TEMPERATURE_C") {
                lorawanServer_setMeterType("temperature", "°C", "min", "°C/min"); // min = Minutes
            }
            else if (toUpper(splitted[1]) == "TEMPERATURE_F") {
                lorawanServer_setMeterType("temperature", "°F", "min", "°F/min"); // min = Minutes
            }
            else if (toUpper(splitted[1]) == "TEMPERATURE_K") {
                lorawanServer_setMeterType("temperature", "K", "min", "K/m"); // min = Minutes
            }
        }

        if ((toUpper(_param) == "DEVEUI") && (splitted.size() > 1))
        {
            const std::string& s = splitted[1]; // exactly 16 hex chars: e.g. "0123456789ABCDEF"
            if (s.size() != 16) {
                LogFile.WriteToFile(ESP_LOG_ERROR, TAG, "DEVEUI must be 16 hex characters.");
            } else {
                bool ok = true;
                for (uint8_t i = 0; i < 8; ++i) {
                    int hi = hex_nibble(s[i*2]);
                    int lo = hex_nibble(s[i*2 + 1]);
                    if (hi < 0 || lo < 0) { ok = false; break; }
                    dev_eui[i] = static_cast<uint8_t>((hi << 4) | lo);
                }
                if (!ok) {
                    LogFile.WriteToFile(ESP_LOG_ERROR, TAG, "DEVEUI contains non-hex characters.");
                }
            }
        }

        if ((toUpper(_param) == "APPEUI") && (splitted.size() > 1))
        {
            const std::string& s = splitted[1]; // exactly 16 hex chars: e.g. "0123456789ABCDEF"
            if (s.size() != 16) {
                LogFile.WriteToFile(ESP_LOG_ERROR, TAG, "APPEUI must be 16 hex characters.");
            } else {
                bool ok = true;
                for (uint8_t i = 0; i < 8; ++i) {
                    int hi = hex_nibble(s[i*2]);
                    int lo = hex_nibble(s[i*2 + 1]);
                    if (hi < 0 || lo < 0) { ok = false; break; }
                    app_eui[i] = static_cast<uint8_t>((hi << 4) | lo);
                }
                if (!ok) {
                    LogFile.WriteToFile(ESP_LOG_ERROR, TAG, "APPEUI contains non-hex characters.");
                }
            }
        }

        if ((toUpper(_param) == "APPKEY") && (splitted.size() > 1))
        {
            const std::string& s = splitted[1]; // APPKEY musb be exactly 32 hex chars: e.g. "0123456789ABCDEF"
            if (s.size() != 32) {
                LogFile.WriteToFile(ESP_LOG_ERROR, TAG, "APPKEY must be 32 hex characters.");
            } else {
                bool ok = true;
                for (uint8_t i = 0; i < 16; ++i) {
                    int hi = hex_nibble(s[i*2]);
                    int lo = hex_nibble(s[i*2 + 1]);
                    if (hi < 0 || lo < 0) { ok = false; break; }
                    app_key[i] = static_cast<uint8_t>((hi << 4) | lo);
                }
                if (!ok) {
                    LogFile.WriteToFile(ESP_LOG_ERROR, TAG, "APP_KEY contains non-hex characters.");
                }
            }
        }
        
    }

    return true;
}


bool ClassFlowLorawan::Start(void) 
{
    Lorawan_Enable();
    return (Lorawan_Init() == 0);
}


bool ClassFlowLorawan::doFlow(string zwtime)
{
    bool success = true;
    std::string result;
    std::string resulterror = "";
    std::string resultraw = "";
    std::string resultpre = "";
    std::string resultrate = ""; // Always Unit / Minute
    std::string resultRatePerTimeUnit = ""; // According to selection
    std::string resulttimestamp = "";
    std::string resultchangabs = "";
    string namenumber = "";
    std::string batteryState = "";
    int qos = 1;


    // success = publishSystemData(qos);
    LogFile.WriteToFile(ESP_LOG_INFO, TAG, "Publishing data via Lorawan...");
    batteryState = std::to_string(readBattery());
    
    success |= LorawanPublish("battery_voltage_value_mV", batteryState);


    if (flowpostprocessing)
    {
        std::vector<NumberPost*>* NUMBERS = flowpostprocessing->GetNumbers();

        for (int i = 0; i < (*NUMBERS).size(); ++i)
        {
            result =  (*NUMBERS)[i]->ReturnValue;
            resultraw =  (*NUMBERS)[i]->ReturnRawValue;
            resultpre =  (*NUMBERS)[i]->ReturnPreValue;
            resulterror = (*NUMBERS)[i]->ErrorMessageText;
            resultrate = (*NUMBERS)[i]->ReturnRateValue; // Unit per minutes
            resultchangabs = (*NUMBERS)[i]->ReturnChangeAbsolute; // Units per round
            resulttimestamp = (*NUMBERS)[i]->timeStamp;

            namenumber = (*NUMBERS)[i]->name;

            if (result.length() > 0)
                success |= LorawanPublish(namenumber + "value", result);
            if (resulterror.length() > 0)  
                success |= LorawanPublish(namenumber + "error", resulterror);

            if (resultrate.length() > 0) {
                success |= LorawanPublish(namenumber + "rate", resultrate);
                
                std::string resultRatePerTimeUnit;
                if (lorawanServer_getTimeUnit() == "h") { // Need conversion to be per hour
                    resultRatePerTimeUnit = resultRatePerTimeUnit = to_string((*NUMBERS)[i]->FlowRateAct * 60); // per minutes => per hour
                }
                else { // Keep per minute
                    resultRatePerTimeUnit = resultrate;
                }
                success |= LorawanPublish(namenumber + "rate_per_time_unit", resultRatePerTimeUnit);
            }

            if (resultchangabs.length() > 0) {
                success |= LorawanPublish(namenumber + "changeabsolut", resultchangabs); // Legacy API
                success |= LorawanPublish(namenumber + "rate_per_digitization_round", resultchangabs);
            }

            if (resultraw.length() > 0)   
                success |= LorawanPublish(namenumber + "raw", resultraw);

            if (resulttimestamp.length() > 0)
                success |= LorawanPublish(namenumber + "timestamp", resulttimestamp);

            // std::string json = flowpostprocessing->getJsonFromNumber(i, "\n");
            // success |= LorawanPublish(namenumber + "json", json);
        }
    }
    
    if (!success) {
        LogFile.WriteToFile(ESP_LOG_WARN, TAG, "One or more Lorawan data batches failed to be published!");
    }
    
    return true;
}

#endif //ENABLE_LORAWAN
