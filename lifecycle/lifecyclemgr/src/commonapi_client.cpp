// @file	commonapi_client.cpp
// @date	20/04/2020
// @author	oss

#include "commonapi_client.h"
#include "C_CONTROLLER_IMP.h"
//#include "helper.h"

using namespace nre;
using namespace v0::iMicro;

CommonAPI_Client::CommonAPI_Client(C_CONTROLLER_IMP* p_ctrl) :m_ctrl(p_ctrl),
    dlt_ctx("CLT", "CommonAPI Client") {}

void CommonAPI_Client::init(C_COMMONAPI_SERVER_LifecycleAPI_IMP* p_commonApiServer) {
    commonApiServer = p_commonApiServer; 
    InitInterface();    
}


void CommonAPI_Client::SOC_LIFECYCLE_ProxyAvailable()
{
    dlt_ctx.log() << __PRETTY_FUNCTION__ << dlt::endl;

    // Subscribe
    getSOC_LIFECYCLEProxy()->getSOC_WAKEUP_REASONAttribute().getChangedEvent().subscribe(
            [this](const uint16_t& wakeupReason)
            {this->onSOC_WAKEUP_REASONAttribute(wakeupReason);}
            );
    getSOC_LIFECYCLEProxy()->getBattery_Thermal_StatusAttribute().getChangedEvent().subscribe(
            [this](::iMicro::iMicroStructs::Battery_Thermal_Status_t batteryThermalStatus)
            {this->onLC_BatteryStatus_MCUAttributeChanged(batteryThermalStatus);}
            );

    getSOC_LIFECYCLEProxy()->getPowerModeStatus_MCUAttribute().getChangedEvent().subscribe(
            [this](const uint16_t& powerMode)
            {this->onPowerModeStatus_MCUAttributeChanged(powerMode);}
            );

    
    int qlmret = m_ctrl->ql_lpm_pthread();
    dlt_ctx.log() << " ql_lpm_pthread status" << qlmret << dlt::endl;            
}

void CommonAPI_Client::SOC_LIFECYCLE_ProxyNotAvailable()
{
    dlt_ctx.log() << __PRETTY_FUNCTION__ << dlt::endl;
}


void CommonAPI_Client::NetworkManagerAPI_ProxyAvailable()
{
    dlt_ctx.log() << __PRETTY_FUNCTION__ << dlt::endl;            
}

void CommonAPI_Client::NetworkManagerAPI_ProxyNotAvailable()
{
    dlt_ctx.log() << __PRETTY_FUNCTION__ << dlt::endl;
}


void CommonAPI_Client::onPowerModeStatus_MCUAttributeChanged(const uint32_t &p_PowerMode) {
    
    dlt_ctx.log() << "The onPowerModeStatus_MCUAttributeChanged has changed. New value: " << p_PowerMode << dlt::endl;
   
    if (nullptr != commonApiServer){

        dlt_ctx.log() << __PRETTY_FUNCTION__ << "sending to capiserver"<< dlt::endl;

        int powerMode_t = p_PowerMode;

        commonApiServer->setPowerMode(powerMode_t);
    }
    else
    {
        dlt_ctx.log() << __PRETTY_FUNCTION__ << "commonApiServer is null"<< dlt::endl;
    }
}


void CommonAPI_Client::onSOC_WAKEUP_REASONAttribute(const uint32_t &p_WakeupReason) 
{
    dlt_ctx.log() << "The onSOC_WAKEUP_REASONAttribute has changed. New value: " << p_WakeupReason << dlt::endl;
    if (nullptr != commonApiServer){
        dlt_ctx.log() << __PRETTY_FUNCTION__ << "sending to capiserver"<< dlt::endl;
        commonApiServer->setPMWakeupSource(p_WakeupReason);
    }
    else
    {
        dlt_ctx.log() << __PRETTY_FUNCTION__ << "commonApiServer is null"<< dlt::endl;
    }
}


void CommonAPI_Client::onLC_BatteryStatus_MCUAttributeChanged(::iMicro::iMicroStructs::Battery_Thermal_Status_t batteryThermalStatus) 
{
    ::iMicro::iMicroTypes::Status_type_enum statusType = batteryThermalStatus.getParam();
    dlt_ctx.log() << "The onLC_BatteryStatus_MCUAttributeChanged has changed. Enum value " << statusType << dlt::endl;
    dlt_ctx.log() << "The onLC_BatteryStatus_MCUAttributeChanged has changed. Status value " << batteryThermalStatus.getStatus() << dlt::endl;
    bool serverBatteryStatus;    
    bool serverThermalStatus;

    if (statusType == ::iMicro::iMicroTypes::Status_type_enum::Thermal_status)
    {
        if(batteryThermalStatus.getStatus() == 0)
            serverThermalStatus = true;
        else
            serverThermalStatus = false;
        
        if (nullptr != commonApiServer)
        {
            dlt_ctx.log() << __PRETTY_FUNCTION__ << "Thermal_status changed"<< dlt::endl;
            commonApiServer->set_LC_VTMstate(serverThermalStatus);    
        }
        else
        {
            dlt_ctx.log() << __PRETTY_FUNCTION__ << "commonApiServer is null"<< dlt::endl;
        }
    }

    else
    {
        if(statusType ==::iMicro::iMicroTypes::Status_type_enum::MainBattery_Status)
        {		
            if(batteryThermalStatus.getStatus() == 0)
                serverBatteryStatus = false;
            else	
                serverBatteryStatus = true;
        }
        else if(statusType == ::iMicro::iMicroTypes::Status_type_enum::Backup_Battery_Status)
        {	
            serverBatteryStatus = false;	
        }
        if (nullptr != commonApiServer){
            dlt_ctx.log() << __PRETTY_FUNCTION__ << "battery state changed"<< dlt::endl;
            commonApiServer->set_LC_MainBattery(serverBatteryStatus);
        }
        else
        {
            dlt_ctx.log() << __PRETTY_FUNCTION__ << "commonApiServer is null"<< dlt::endl;
        }
    }
}

void CommonAPI_Client::SOC_NTW_API_ProxyAvailable()
{
	getSOC_NTW_APIProxy()->getNTW_ComSyncAttribute().getChangedEvent().subscribe(
		[this](const uint8_t &ntwSync)
		{ this->onNTW_SyncChanged(ntwSync); });
	ClassLog(DLT_LOG_INFO) << "SOC_NTW_API_ProxyAvailable" << C_DLT_LOG::endl;
}

void CommonAPI_Client::onNTW_SyncChanged(uint8_t ntwSync)
{
	ClassLog(DLT_LOG_INFO) << "Lifecycle : onNTW_SyncChanged networkSync: '" << ntwSync << C_DLT_LOG::endl;
     bool _pIsSynchronized= false;

	if (ntwSync == NTW_Status::NTW_NOT_OK)
	{
		_pIsSynchronized = false;
        
	}
	else
	{
		_pIsSynchronized = true;
	}
    commonApiServer->setNWSync(_pIsSynchronized);
}


