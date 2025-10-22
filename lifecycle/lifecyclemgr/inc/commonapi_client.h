// @file	commonapi_client.h
// @date	28/04/2020
// @author	oss

#pragma once

#include "C_COMMONAPI_CLIENT_GEN.h"
#include <nre_dlt.h>
#include "C_COMMONAPI_SERVER_LifecycleAPI_IMP.h"

class CommonAPI_Client : public nre::C_COMMONAPI_CLIENT_GEN
{
public:
    CommonAPI_Client(C_CONTROLLER_IMP* p_ctrl);

    void init(C_COMMONAPI_SERVER_LifecycleAPI_IMP*);

private:
    C_CONTROLLER_IMP* m_ctrl; 
    /* This function is called as soon as the server at the opposite side is connected to dbus */
    void SOC_LIFECYCLE_ProxyAvailable() override;

    /* This function is automatically called when the server at the opposite side is disconnected */
    void SOC_LIFECYCLE_ProxyNotAvailable() override;    

    void NetworkManagerAPI_ProxyAvailable() override;
    void NetworkManagerAPI_ProxyNotAvailable() override;
    void SOC_NTW_API_ProxyAvailable() override;
    
    void onPowerModeStatus_MCUAttributeChanged(const uint32_t &p_tempData);
    void onSOC_WAKEUP_REASONAttribute(const uint32_t &p_WakeupReason) ;
    void onLC_BatteryStatus_MCUAttributeChanged(::iMicro::iMicroStructs::Battery_Thermal_Status_t batteryThermalStatus);
    void onLC_VTMstate_MCUAttributeChanged(::iMicro::iMicroStructs::Battery_Thermal_Status_t vtmState) ;
    void onNTW_SyncChanged(uint8_t ntwSync);
     

    bool isDataAvailable = false;
    uint16_t temperatureData = 0;
    C_COMMONAPI_SERVER_LifecycleAPI_IMP* commonApiServer = nullptr;

    typedef enum
    {
        NTW_OK = 0,
        NTW_NOT_OK
    } NTW_Status;

    nre::DLT_Context dlt_ctx;
};
