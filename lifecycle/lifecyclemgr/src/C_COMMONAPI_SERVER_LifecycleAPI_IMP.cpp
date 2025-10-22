#include "C_CONTROLLER_IMP.h"
#include "C_COMMONAPI_SERVER_LifecycleAPI_IMP.h"
#include "ql_lpm.h"

using namespace nre;

#define MODEM_ON 1
#define MODEM_OFF 0


/*
Supported Wakeup Sources from PM module:
None = 0,
CAN_Activity = 1,
Speed = 2,
SMS = 3,
IGN = 4,
NAD = 5,
SANITY_RESET = 6,
LM3_RTCWakeUp = 7,
LM3_to_DeepSleepPrepare = 8,
IMU_Wakeup = 9,
PowerONReset = 10,
MCU_Watchdog = 11,
Wakeup_MainBattery = 12,
SOC_RTC_WakeUp = 13,
Reserved1 = 14,
Reserved2 = 15
*/

#define PM_SMS  3
#define	PM_IGN  4
#define	PM_NAD  5
#define	PM_LM3_RTC 7
#define	PM_IMU_WAKE 9
#define PM_CAN_ACTIVITY 1
#define PM_MAINBAT_WAKEUP 12
#define PM_SOC_RTC_WAKE 13

#define SOCKET_SMS  0
#define SOCKET_POWERKEY  1
#define SOCKET_MODEM_SPURIOUS  2
#define SOCKET_RTC  3
#define SOCKET_MQTT 4
#define SOCKET_NON_TCP  5




C_COMMONAPI_SERVER_LifecycleAPI_IMP::C_COMMONAPI_SERVER_LifecycleAPI_IMP(
		C_CONTROLLER_IMP* p_ctrl) :
		C_COMMONAPI_SERVER_LifecycleAPI_GEN(), m_ctrl(p_ctrl) 
{
    m_modemState = MODEM_ON;
    bBuBMode = false;
    noTCUPowerMode = true;
}

C_COMMONAPI_SERVER_LifecycleAPI_IMP::~C_COMMONAPI_SERVER_LifecycleAPI_IMP()
{
    
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::init(void) { 
    InitInterface(); 
}

const char* GetAndroidStatusString(ANDROID_STATUS_E status) {
	    switch (status) {
	            case ANDROID_STATUS_WORKING:
	                return "WORKING";
		    case ANDROID_STATUS_SLEEP:
	                return "SLEEP";
	            case ANDROID_STATUS_UNKNOWN:
	            default:
	                return "UNKNOWN";
									    }
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::setAndroidState(ANDROID_STATUS_E status)
{    
    ANDROID_STATUS_E tmp_status = ql_get_android_status();
    ClassLog(DLT_LOG_INFO) << "Current Android status before setting from TCU" << GetAndroidStatusString(tmp_status)<< C_DLT_LOG::endl;
    

    if(NAD_LifecycleState != ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_BACKUP_BATTERY)
    {
        if(bBuBMode == true)
        {
            if(tmp_status != status)
            {
                ql_set_android_status(status);
            }
            bBuBMode = false;
        }    
    }
    else
    {
        if(tmp_status != status)
        {
            ql_set_android_status(status);
        }
    }    
    tmp_status = ql_get_android_status();
    ClassLog(DLT_LOG_INFO) << "Current Android status:" << GetAndroidStatusString(tmp_status)<< C_DLT_LOG::endl;
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::run_Modem(uint32_t state, uint32_t powerMode)
{
    if(m_modemState == state)
    {
        ClassLog(DLT_LOG_INFO) << "Script not run since Power Mode :" << powerMode << C_DLT_LOG::endl; 
        return;
    }

    const char* scriptPath = "/etc/mm_data/modem_manager/atc_control.sh";
    if (chmod(scriptPath, 0755) != 0)
    {  
        ClassLog(DLT_LOG_INFO) << "Failed to change file permissions :"  << C_DLT_LOG::endl;            
    }
 
    //  int result = std::system(command.c_str());
        //int result = std::system("bash /home/connect_apn3.sh\n");
        //int result = std::system("/bin/bash /home/connect_apn3.sh&");
    int result;
    if(state == MODEM_ON)
    {
        ClassLog(DLT_LOG_INFO) << "Script run since Power Mode :" << powerMode << C_DLT_LOG::endl; 
        result = std::system("/bin/bash /etc/mm_data/modem_manager/atc_control.sh 1 > /tmp/atc_control.out 2>&1 &");        
    }
    else
    {
        ClassLog(DLT_LOG_INFO) << "Script run since Power Mode :" << powerMode << C_DLT_LOG::endl; 
        result = std::system("/bin/bash /etc/mm_data/modem_manager/atc_control.sh 0 > /tmp/atc_control.out 2>&1 &");
    }
    
 
    if (result == 0)
    {
        ClassLog(DLT_LOG_INFO) << "LAUNCH successful :"  << C_DLT_LOG::endl;
        m_modemState = state;
    }
    else
    {
        ClassLog(DLT_LOG_INFO) << "LAUNCH failed :"  << C_DLT_LOG::endl;
    }
 
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::setNWSync(bool nwSync)
{ 
    if(nwSync== true ){
        NAD_additionalLifecycleStateInfo.NAD_CanStatus = ::Lifecycle::Interface::LifecycleTypes::LC_CanStatus::LC_CAN_STS_ON;
    }
    else {
         NAD_additionalLifecycleStateInfo.NAD_CanStatus = ::Lifecycle::Interface::LifecycleTypes::LC_CanStatus::LC_CAN_STS_OFF;
    }
    //NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::setPMWakeupSource(uint32_t wakeupReason)
{
    switch(wakeupReason)
    {  
        case PM_CAN_ACTIVITY:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_CAN;
            break;

        case PM_MAINBAT_WAKEUP:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_MAINBATTERY;
            break;
        
        case PM_SMS:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_SMS;
            break;

        case PM_IMU_WAKE:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_ACCELEROMETER;
            break;
            
        case PM_SOC_RTC_WAKE:
            if((NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_1) || 
               (NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SHUTDOWN))
            {
                NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_LM1_RTC;                
            }
            else if(NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_3)
            {
                NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_LM3_RTC;
            }            
            break;
        case PM_LM3_RTC:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_LM3_RTC;
            break;

        default:            
            break;
    }
    //NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::setSocketWakeupSource(uint32_t wakeupReason)
{
    switch(wakeupReason)
    {        
        case SOCKET_MQTT:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_MQTT;
            break;
        
        case SOCKET_SMS:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_SMS;
            break;

        case SOCKET_MODEM_SPURIOUS:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UNKNOWN;
            break;
            
        case SOCKET_RTC:
            if((NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_1) || 
               (NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SHUTDOWN))
            {
                NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_LM1_RTC;                
            }
            else if(NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_3)
            {
                NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_LM3_RTC;
            }            
            break;

        default:
            NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UNKNOWN;
            break;
    }
    //NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
}


void C_COMMONAPI_SERVER_LifecycleAPI_IMP::set_LC_MainBattery(bool batteryStatus)
{
    if(batteryStatus == true)
    {
        NAD_additionalLifecycleStateInfo.NAD_MainBatteryStatus = ::Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus::LC_MAIN_BATT_STS_OK;
    }
    else
    {
        NAD_additionalLifecycleStateInfo.NAD_MainBatteryStatus = ::Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus::LC_MAIN_BATT_STS_NOK;
    }
    NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
}


void C_COMMONAPI_SERVER_LifecycleAPI_IMP::set_LC_VTMstate(bool vtmState)
{
    if(vtmState == true)
    {
        NAD_additionalLifecycleStateInfo.NAD_VTMstate = ::Lifecycle::Interface::LifecycleTypes::LC_VTMstate::LC_VTM_T_OVERTEMP;
    }
    else
    {
        NAD_additionalLifecycleStateInfo.NAD_VTMstate = ::Lifecycle::Interface::LifecycleTypes::LC_VTMstate::LC_VTM_T_NORMAL;
    }
    NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
}


void C_COMMONAPI_SERVER_LifecycleAPI_IMP::setPowerMode(int powerMode) {

    ::Lifecycle::Interface::LifecycleTypes::LC_State dbusPowerMode;
    switch (powerMode)
    {
        case PowerModeStates::SYSTEM_LC_INIT_PHASE_2:
        case PowerModeStates::SYSTEM_LC_INIT_PHASE_1:
        {
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_INIT;
            //run_Modem(MODEM_ON, powerMode);
            break;
        }
        case PowerModeStates::SYSTEM_LC_OFF:
        {
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_OFF;            
            break;
        }
        case PowerModeStates::SYSTEM_LC_BACKUP_BATTERY:
        {
            bBuBMode = true;
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_BACKUP_BATTERY;
            ANDROID_STATUS_E status = ANDROID_STATUS_SLEEP;            
           // setAndroidState(status);            
            break;
        }
        case PowerModeStates::SYSTEM_LC_STAY_ALIVE_SERVICES:
        {           
            // TODO: check this later.. blpmTCUOnly = true;
            if(NAD_LifecycleState != ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES)
            {
                ::iMicro::pmStructs::tcu_parameters_type _tcu_param;
                CommonAPI::CallStatus _internalCallStatus;            
                _tcu_param.setPENDING_TCU_ACTION(pStayActive_Status::STAY_ACTIVE);
                _tcu_param.setCONNECTION_UNLOCK_STATUS(1);
                m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->update_TCU_Parameters(_tcu_param, _internalCallStatus);
                ClassLog(DLT_LOG_INFO) << "Pending TCU actions trigerred" << C_DLT_LOG::endl; 
            }
            break;
        }
        case PowerModeStates::SYSTEM_LC_TIMED:
        {
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_CONNECTED;  
            //run_Modem(MODEM_ON, powerMode);
            ANDROID_STATUS_E status = ANDROID_STATUS_WORKING;
            //setAndroidState(status);          
            break;
        }
        case PowerModeStates::SYSTEM_LC_STANDBY:
        {
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SLEEP; 
           // run_Modem(MODEM_ON, powerMode);
            ANDROID_STATUS_E status = ANDROID_STATUS_WORKING;
            //setAndroidState(status);                      
            break;
        }
        case PowerModeStates::SYSTEM_LC_DEEP_SLEEP:
        {
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_DEEP_SLEEP;            
            break;
        }
        case PowerModeStates::SYSTEM_LC_LISTEN_MODE_1:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_1;            
            break;
        }
        case PowerModeStates::SYSTEM_LC_LISTEN_MODE_2:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_2;            
            break;
        }
        case PowerModeStates::SYSTEM_LC_LISTEN_MODE_3:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_3;            
            break;
        }
        case PowerModeStates::SYSTEM_LC_PARTIAL_OPERATION:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_REMOTE_WAKE;
            //run_Modem(MODEM_ON, powerMode);
            ANDROID_STATUS_E status = ANDROID_STATUS_WORKING;
           // setAndroidState(status);
            break;
        }
        case PowerModeStates::SYSTEM_LC_FULL_OPERATION:
        case PowerModeStates::SYSTEM_LC_NAD_ON:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON;
            //run_Modem(MODEM_ON, powerMode);
            ANDROID_STATUS_E status = ANDROID_STATUS_WORKING;
            //setAndroidState(status);
            break;
        }
        /*case PowerModeFIDLStates::NAD_EMERGENCY:
        {
            CommonAPI_Server::getInstance()->setValue(0);
            break;
        }*/
        case PowerModeStates::SYSTEM_LC_MAINTENANCE:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_MAINTENANCE;
            break;
        }
        case PowerModeStates::SYSTEM_LC_BENCH:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_TEST_MODE;
            ANDROID_STATUS_E status = ANDROID_STATUS_WORKING;
           // setAndroidState(status);            
            break;
        }
        case PowerModeStates::SYSTEM_LC_LOGISTICS:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LOGISTIC;
            break;
        }
        case PowerModeStates::SYSTEM_LC_RESET:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_RESET;
            break;
        }
        case PowerModeStates::SYSTEM_LC_IDLE:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_IDLE;
            ANDROID_STATUS_E status = ANDROID_STATUS_WORKING;
            //setAndroidState(status);            
            break;
        }
        case PowerModeStates::SYSTEM_LC_SUSPEND_TO_RAM:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SHUTDOWN;            
            //ANDROID_STATUS_E status = ANDROID_STATUS_WORKING;
            //blpmTCUOnly = false;
            //setAndroidState(status);
            /*if(m_qlmthreadStatus)
            {
                m_ctrl->ql_lpm_pthread();
                m_qlmthreadStatus = false;
            }*/
            
            //run_Modem(MODEM_OFF, powerMode);
            break;
        }
        case PowerModeStates::SYSTEM_LC_TCU_ONLY:
        {
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES;            
            break;
        }
        case PowerModeStates::SYSTEM_LC_NAD_EMERGENCY_LAW:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW;
            break;
        }
        case PowerModeStates::SYSTEM_LC_NAD_EMERGENCY:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY;
            break;
        }
        case PowerModeStates::SYSTEM_LC_NAD_EMERGENCY_LAW_LP:
        {            
            dbusPowerMode = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP;
            break;
        }        
        default:
        {       
            noTCUPowerMode = false;    
            ClassLog(DLT_LOG_INFO) << "No mapping available for this powerstate in TCU" << C_DLT_LOG::endl;
            break;
        }
    }
    if((powerMode != PowerModeStates::SYSTEM_LC_STAY_ALIVE_SERVICES) && (noTCUPowerMode == true)) 
    {
        evaluateAndBroadcastPowerMode(dbusPowerMode);
    }
    else if(noTCUPowerMode == false)
    {
        noTCUPowerMode = true;
    }    
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::startTCUOnlyTimer(uint32_t timerVal)
{
    ClassLog(DLT_LOG_INFO) << "startTCUOnlyTimer" << C_DLT_LOG::endl;
    std::this_thread::sleep_for(std::chrono::seconds(timerVal));
    ::iMicro::pmStructs::tcu_parameters_type _tcu_param;
    CommonAPI::CallStatus _internalCallStatus;
    _tcu_param.setPENDING_TCU_ACTION(pStayActive_Status::STAY_INACTIVE);
    _tcu_param.setCONNECTION_UNLOCK_STATUS(1);
    m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->update_TCU_Parameters(_tcu_param, _internalCallStatus);
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::evaluateAndBroadcastPowerMode(::Lifecycle::Interface::LifecycleTypes::LC_State& currPowMode)
{
    ClassLog(DLT_LOG_INFO) << "evaluateAndBroadcastPowerMode, Current Power Mode" << currPowMode << C_DLT_LOG::endl;
    ClassLog(DLT_LOG_INFO) << "evaluateAndBroadcastPowerMode, Previous Power Mode" << NAD_LifecycleState << C_DLT_LOG::endl;
    if(currPowMode == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES)
    {
        if((NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_1) || 
           (NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_2) ||
           (NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_3) || 
           (NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SHUTDOWN))
        {
            NAD_LifecycleState = currPowMode;
            NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
            startTCUOnlyTimer(200); //200 sec            
        }
        else
        {
            NAD_LifecycleState = currPowMode;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if(NAD_additionalLifecycleStateInfo.NAD_WakeupSource == ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_ACCELEROMETER)
            {
                ClassLog(DLT_LOG_INFO) << "evaluateAndBroadcastPowerMode, IMU wakeup" << C_DLT_LOG::endl;
                NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
                startTCUOnlyTimer(120); //100 sec
                //Resetting wakeup source recheck if really required
                //NAD_additionalLifecycleStateInfo.NAD_WakeupSource = ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UNKNOWN;
            }
            else
            {    
                ClassLog(DLT_LOG_INFO) << "evaluateAndBroadcastPowerMode, STANDBY-->S2R" << C_DLT_LOG::endl;
                NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
                startTCUOnlyTimer(21); //21 sec
            }
        }        
    }
    else
    {
        NAD_LifecycleState = currPowMode;
        NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);                
    }
}



void C_COMMONAPI_SERVER_LifecycleAPI_IMP:: LC_GetPowerState_plus(const std::shared_ptr<CommonAPI::ClientId> _client, LC_GetPowerState_plusReply_t _reply)
{
    ::Lifecycle::Interface::LifecycleTypes::LC_State pPowerState = NAD_LifecycleState;
    ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource pWakeupSource = NAD_additionalLifecycleStateInfo.NAD_WakeupSource;
    ::Lifecycle::Interface::LifecycleTypes::LC_CanStatus pCanStatus = NAD_additionalLifecycleStateInfo.NAD_CanStatus;
    ::Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus pMainBatteryStatus = NAD_additionalLifecycleStateInfo.NAD_MainBatteryStatus;
    ::Lifecycle::Interface::LifecycleTypes::LC_VTMstate pVtmState = NAD_additionalLifecycleStateInfo.NAD_VTMstate;

    _reply(pPowerState, pWakeupSource, pCanStatus, pMainBatteryStatus, pVtmState);

}


void C_COMMONAPI_SERVER_LifecycleAPI_IMP::LC_JumpToListen2(const std::shared_ptr<CommonAPI::ClientId> _client, LC_JumpToListen2Reply_t _reply) {
    (void)_client;
    ::Lifecycle::Interface::LifecycleTypes::LC_Status pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;

    ::iMicro::pmStructs::tcu_parameters_type _tcu_param;
    CommonAPI::CallStatus _internalCallStatus;
    _tcu_param.setPENDING_TCU_ACTION(pStayActive_Status::STAY_INACTIVE);  
    _tcu_param.setSERVICE_PROVIDER_CONNECTION_STATUS(pConnection_Status::INACTIVE);
    _tcu_param.setCONNECTION_UNLOCK_STATUS(1);
    m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->update_TCU_Parameters(_tcu_param, _internalCallStatus); 
    ClassLog(DLT_LOG_INFO) << "Jump to ListenMode 2 as there is no network connection" << C_DLT_LOG::endl;

    _reply(pStatus);
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::NotifyAllRegisteredClientsNadStateInfo(::Lifecycle::Interface::LifecycleTypes::LC_State l_NAD_LifecycleStateToAPI)
{
    ClassLog(DLT_LOG_INFO) << "fireLC_NOTIFICATION_State_plusEvent, Power Mode" << NAD_LifecycleState << C_DLT_LOG::endl;
    ClassLog(DLT_LOG_INFO) << "fireLC_NOTIFICATION_State_plusEvent, Wakeup Source" << NAD_additionalLifecycleStateInfo.NAD_WakeupSource << C_DLT_LOG::endl;
    ClassLog(DLT_LOG_INFO) << "fireLC_NOTIFICATION_State_plusEvent, CAN Status" << NAD_additionalLifecycleStateInfo.NAD_CanStatus << C_DLT_LOG::endl;
    ClassLog(DLT_LOG_INFO) << "fireLC_NOTIFICATION_State_plusEvent, Main Battery State" << NAD_additionalLifecycleStateInfo.NAD_MainBatteryStatus << C_DLT_LOG::endl;
    ClassLog(DLT_LOG_INFO) << "fireLC_NOTIFICATION_State_plusEvent, VTM State" << NAD_additionalLifecycleStateInfo.NAD_VTMstate << C_DLT_LOG::endl;
    
    this->fireLC_NOTIFICATION_State_plusEvent(NAD_LifecycleState,
                                              NAD_additionalLifecycleStateInfo.NAD_WakeupSource,
                                              NAD_additionalLifecycleStateInfo.NAD_CanStatus,
                                              NAD_additionalLifecycleStateInfo.NAD_MainBatteryStatus,
                                              NAD_additionalLifecycleStateInfo.NAD_VTMstate);

    m_ctrl->getLegatoServer().ReportLcStateEvent_plus(NAD_LifecycleState, NAD_additionalLifecycleStateInfo);
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::LC_RegisterClient(const std::shared_ptr<CommonAPI::ClientId> _client, std::string l_pClientName, LC_RegisterClientReply_t _reply)
{

    ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "Start" << C_DLT_LOG::endl;
    (void)_client;
    std::vector<client_t>::iterator itClients;
    ::Lifecycle::Interface::LifecycleTypes::LC_State pState = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_INIT;
    ::Lifecycle::Interface::LifecycleTypes::LC_Status pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK_CLIENT_NOT_REGISTERED;

    std::string pClientName = l_pClientName;

    evaluate_RegisterClient(pClientName,pState,pStatus);
    
     _reply(pStatus, pState);

}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP:: LC_UnRegisterClient(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _pClientName, LC_UnRegisterClientReply_t _reply)
{
    ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "Start" << C_DLT_LOG::endl;
    (void)_client;
    std::vector<client_t>::iterator itClients;
    ::Lifecycle::Interface::LifecycleTypes::LC_Status pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK;

    std::string pClientName = _pClientName;

    // clean the input string : start from the end of the string
    for (int cnt = (int)pClientName.length() - 1; cnt > -1; cnt--)
    {
        if (pClientName[cnt] <= 32)
        {
            pClientName.erase(cnt, 1);
        }
        else
        {
            break;
        }
    }

    // clean the input string : start from the begining of the string
    std::string r_pClientName = "";
    for (int cnt = 0; cnt < (int)pClientName.length(); cnt++)
    {
        if (pClientName[cnt] > 32)
        {
            r_pClientName = pClientName.substr(cnt);
            break;
        }
    }

    pClientName = r_pClientName;

    if (pClientName.size() < 1)
    {               
        ClassLog(DLT_LOG_INFO) << "LC_UnRegisterClient() : ClientName" << pClientName.c_str() << C_DLT_LOG::endl;
        PrintRegisteredClients();

        return;
    }

    m_clients_mtx.lock();

    ClassLog(DLT_LOG_INFO) << "LC_UnRegisterClient() : ClientName" << pClientName.c_str() << C_DLT_LOG::endl;
    ClassLog(DLT_LOG_INFO) << "Begin . LC_UnRegisterClient() : Num of clients" << m_clients.size() << C_DLT_LOG::endl;                                

    for (itClients = m_clients.begin(); itClients != m_clients.end(); itClients++)
    {
        if (itClients->name == pClientName)
        {
            break;
        }
    }

    if (itClients == m_clients.end())
    {
        pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK;
        ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : ClientName" << pClientName.c_str() << C_DLT_LOG::endl;
        ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : pState" << (int)pStatus << C_DLT_LOG::endl;
        ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : Num of clients" << m_clients.size() << C_DLT_LOG::endl;                                    
    }
    else
    {
        m_clients.erase(itClients);

        pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;

        // When Lifecycle can switch off (StayActive false), then we send ACK to VuC
        /*if(isStayActive(false) != Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_LOCK)
        {
        	if(this->NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES)
        	{
        		this->SendAcknowledgeToVuC(true);
        	}
        }*/
        ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : ClientName unregistered correctly" << pClientName.c_str() << C_DLT_LOG::endl;
        ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : pState" << (int)pStatus << C_DLT_LOG::endl;
        ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : Num of clients" << m_clients.size() << C_DLT_LOG::endl; 
    }

    m_clients_mtx.unlock();

    PrintRegisteredClients();
    ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "End" << C_DLT_LOG::endl;
    _reply(pStatus);
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::PrintRegisteredClients()
{
    int cnt = 0;

    m_clients_mtx.lock();

    if (m_clients.size() == 0)
    {        
        ClassLog(DLT_LOG_INFO) << "Number of registered Clients = 0" << C_DLT_LOG::endl;
    }

    for (std::vector<client_t>::iterator itClients = m_clients.begin(); itClients != m_clients.end(); itClients++, cnt++)
    {
        ClassLog(DLT_LOG_INFO) << "Registered Client count" << cnt << C_DLT_LOG::endl;
        ClassLog(DLT_LOG_INFO) << "Registered Client Name" << itClients->name.c_str() << C_DLT_LOG::endl;
        ClassLog(DLT_LOG_INFO) << "Registered Client State" << (int)itClients->lcState << C_DLT_LOG::endl;
        ClassLog(DLT_LOG_INFO) << "Registered Client stay active" << (int)itClients->stayActive<< C_DLT_LOG::endl;
    }

    m_clients_mtx.unlock();

    return;
}


void C_COMMONAPI_SERVER_LifecycleAPI_IMP::LC_Wakeup(const std::shared_ptr<CommonAPI::ClientId> _client, LC_WakeupReply_t _reply)
{
    ::Lifecycle::Interface::LifecycleTypes::LC_Status pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
                
    if((NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_1) ||
	 (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_2) ||
	 (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_3) ||
	 (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP))
    {
        ::iMicro::pmStructs::tcu_parameters_type _tcu_param;    
        CommonAPI::CallStatus _internalCallStatus = CommonAPI::CallStatus::SUCCESS;
        if(NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP)
        {
            NAD_LifecycleState = Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW;
        }
        else
        {
            if(	(NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_2) ||
                (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_3))
            {
               // m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->SendEvent_SOC_WakeupReason_TCU(WAKEUP_REASON_enum::SMS, _internalCallStatus);
            }
            _tcu_param.setPENDING_TCU_ACTION(pStayActive_Status::STAY_ACTIVE);
            _tcu_param.setCONNECTION_UNLOCK_STATUS(1);
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->update_TCU_Parameters(_tcu_param, _internalCallStatus);
            ClassLog(DLT_LOG_INFO) << "Pending TCU actions trigerred" << C_DLT_LOG::endl;
            NAD_LifecycleState = Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES;
        }
        this->NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);

        
        

		/* Ack Listen not sent yet to VuC, so VuC must be active
		 * In case there is a Listen Mode procedure in progress, send ACK to VuC with DELAYED_SHUTDOWN answer
		 */
    	/* if(!l_AckListenAlreadySent)
    	{
    		//this->SendAcknowledgeToVuC(false);


    	}
    	else
		{
			if(!m_thread_is_running.load())
			{
				if(m_thread)
				{
					m_thread->join();
					delete m_thread;
					m_thread = NULL;
				}
				else
				{
					// do nothing 
				}

				m_thread = new std::thread(&C_COMMONAPI_SERVER_LifecycleAPI_IMP::LC_ExitListen_Management, this);

				if(!m_thread)
				{
					pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK;
				}
				else
				{
					// do nothing 
				}
			}
			else
			{
				// do nothing, thread already running 
			}
		}*/
    }
    else
    {
		pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK;
    }
    _reply(pStatus);
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::LC_Emergency(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _pClientName, ::Lifecycle::Interface::LifecycleTypes::LC_State _pState, LC_EmergencyReply_t _reply)
{
    (void)_client;
    ::Lifecycle::Interface::LifecycleTypes::LC_Status _pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK;
    //evaluateLC_Emergency(_pClientName, _pState, _pStatus);
    triggerLC_Emergency(_pState, _pStatus);
    _reply(_pStatus);
}


void C_COMMONAPI_SERVER_LifecycleAPI_IMP::triggerLC_Emergency(::Lifecycle::Interface::LifecycleTypes::LC_State &_pState, ::Lifecycle::Interface::LifecycleTypes::LC_Status &_pStatus)
{
    ::iMicro::iMicroTypes::call_status_enum _eCallState = ::iMicro::iMicroTypes::call_status_enum::NAD_ON; 
    CommonAPI::CallStatus _internalCallStatus;
    switch(_pState)
    {
        case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY:
        {
            _eCallState = ::iMicro::iMicroTypes::call_status_enum::NAD_EMERGENCY;
            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : NAD_EMERGENCY" << C_DLT_LOG::endl;
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->Update_Emergency_Call_status(_eCallState, _internalCallStatus);
            break;
        }
        case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW:
        {
            _eCallState = ::iMicro::iMicroTypes::call_status_enum::NAD_EMERGENCY_LAW;
            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : NAD_EMERGENCY_LAW" << C_DLT_LOG::endl;
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->Update_Emergency_Call_status(_eCallState, _internalCallStatus);
            break;
        }
        case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP:
        {
            _eCallState = ::iMicro::iMicroTypes::call_status_enum::NAD_EMERGENCY_LAW_LP;
            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : NAD_EMERGENCY_LAW_LP" << C_DLT_LOG::endl;
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->Update_Emergency_Call_status(_eCallState, _internalCallStatus);
            break;
        }
        case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON:
        {
            _eCallState = ::iMicro::iMicroTypes::call_status_enum::NAD_ON;
            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : NAD_ON" << C_DLT_LOG::endl;
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->Update_Emergency_Call_status(_eCallState, _internalCallStatus);
            break;
        }
        default:
        {
            break;
        }
    }
    if(_internalCallStatus == CommonAPI::CallStatus::SUCCESS)
    {
        _pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
        ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : Update_Emergency_Call_status triggerred" << C_DLT_LOG::endl;
    }
    else
    {
        _pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK;
        ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : Update_Emergency_Call_status trigger failed" << C_DLT_LOG::endl;
    }
    
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::evaluateLC_Emergency(std::string pClientName, ::Lifecycle::Interface::LifecycleTypes::LC_State &_pState, ::Lifecycle::Interface::LifecycleTypes::LC_Status &_pStatus)
{
    std::vector<client_t>::iterator itClients;
    
    bool alreadyInEmergency = false;
    bool clean_eCall = false;

    for (itClients = m_clients.begin(); itClients != m_clients.end(); itClients++)
    {
    	if((_pState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON) && (0==itClients->name.compare(pClientName)))
    	{
    		if(itClients->eCall == true)
    		{
        		itClients->eCall = false;
        		_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
    		}
    		else if(itClients->eCallLaw == true)
    		{
    			itClients->eCallLaw = false;
    			//this->g_isEmergencyLaw = false;
    			_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
    		}
    		else if (itClients->testMode == true)
    		{
    			itClients->testMode = false;
    			_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
    		}
    		else
    		{
    			/* do nothing */
    		}
             ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : ask to exit from Emergency" << C_DLT_LOG::endl;
		}
		else if((_pState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON) && (itClients->eCall) &&
		        (0!=itClients->name.compare(pClientName)))
		{
			alreadyInEmergency = true;
            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : Not possible to change status" << C_DLT_LOG::endl;
		}
		else if((_pState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY) &&
		       ((NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON) ||
		        (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY)) &&
		       (0==itClients->name.compare(pClientName)))
		{
			_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
			itClients->eCall = true;
            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : Change status to NAD_EMERGENCY" << C_DLT_LOG::endl;
		}
		else if((_pState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW) &&
		        ((NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON) ||
		        (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY) ||
				(NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES) ||
				(NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP)) &&
		        (0==pClientName.compare("tlmapp_mw_legislativecall")) &&
		       (0==itClients->name.compare(pClientName)))
		{
			_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
			clean_eCall = true;
			itClients->eCallLaw = true;
			//this->g_isEmergencyLaw = true;

            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : Change status to NAD_EMERGENCY" << C_DLT_LOG::endl;
        }
        else if ((_pState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW) &&
                 ((NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON) ||
                  (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY)) &&
                 (0 == itClients->name.compare(pClientName)))
        {
            _pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
            clean_eCall = true;
            itClients->eCallLaw = true;
            //g_isEmergencyLaw = true;

            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : Change status to NAD_EMERGENCY_LAW" << C_DLT_LOG::endl;
            
        }
        else if ((_pState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_TEST_MODE) &&
                 ((NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON) ||
                  (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_MAINTENANCE)) &&
                 (0 == itClients->name.compare(pClientName)))
        {
            _pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
            itClients->testMode = true;
		}
		else if((_pState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP) &&
		        (NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW) &&
		        //(NAD_additionalLifecycleStateInfo.NAD_MainBatteryStatus == LC_MAIN_BATT_STS_NOK) &&
		        //(NAD_additionalLifecycleStateInfo.NAD_CanStatus == LC_CAN_STS_OFF) &&
		        (0==pClientName.compare("tlmapp_mw_legislativecall")) &&
		        (0==itClients->name.compare(pClientName)))
		{
			_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;

		}
		else
		{
		/* do nothing */
		}
    }

    if (clean_eCall)
    {
        for (itClients = m_clients.begin(); itClients != m_clients.end(); itClients++)
        {
            if (itClients->eCall)
            {
                itClients->eCall = false;
            }
        }
    }
    else
    {
        /* do nothing */
    }

    if(_pStatus == Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK)
    {
	  if(!alreadyInEmergency)
	  {
		  if(_pState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP)
		  {
			  /* in this particular case, send directly the new state to APPs and ACK to VC. This is to avoid the creation of a new state VC side */
			  NAD_LifecycleState = Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP;
              
              NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
			  
              /*bool b_fireLC_NOTIFICATION_StateEvent_ToBeCalled = is_fireLC_NOTIFICATION_StateEvent_ToBeCalled(true);

			  if (b_fireLC_NOTIFICATION_StateEvent_ToBeCalled)
			  {
				  NotifyAllRegisteredClientsNadStateInfo(NAD_LifecycleState);
			  }
			  else
			  {
				 
              }

			  this->SendAcknowledgeToVuC(true);*/
		  }
          /*
		  else
		  {
			bool lValidAck = false;
			t_lifecycle_state vcState = LIFECYCLE_SM_IDLE;

            ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : send change status " << C_DLT_LOG::endl;
			vcState = resolveLC_PowerState(_pState, lValidAck);

			this->triggerVcPowerStateChange(vcState);
		  }*/
	  }
	  else
	  {
        ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() : others client wants to stay in emergency, cannot exit " << C_DLT_LOG::endl;
	  }
    }
    else
    {
        ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_Emergency() Error " << C_DLT_LOG::endl;                            
    }

    ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "End" << C_DLT_LOG::endl;
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::evaluate_RegisterClient(std::string p_ClientName, ::Lifecycle::Interface::LifecycleTypes::LC_State &_pState, ::Lifecycle::Interface::LifecycleTypes::LC_Status &_pStatus)
{
    std::vector<client_t>::iterator itClients;

    std::string pClientName = p_ClientName;

    // clean the input string : start from the end of the string
    for (int cnt = (int)pClientName.length() - 1; cnt > -1; cnt--)
    {
        if (pClientName[cnt] <= 32)
        {
            pClientName.erase(cnt, 1);
        }
        else
        {
            break;
        }
    }

    // clean the input string : start from the begining of the string
    std::string r_pClientName = "";
    for (int cnt = 0; cnt < (int)pClientName.length(); cnt++)
    {
        if (pClientName[cnt] > 32)
        {
            r_pClientName = pClientName.substr(cnt);
            break;
        }
    }

    pClientName = r_pClientName;

    if (pClientName.size() < 1)
    { 
        PrintRegisteredClients();
        return;
    }

    client_t tempClient;
    tempClient.name = pClientName;
    tempClient.lcState = _pState = NAD_LifecycleState;

    tempClient.stayActive = ::Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_RELEASE;

    m_clients_mtx.lock();

    ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : ClientName" << pClientName.c_str() << C_DLT_LOG::endl;
    ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : pState" << (int)_pState << C_DLT_LOG::endl;
    ClassLog(DLT_LOG_INFO) << "Begin . LC_RegisterClient() : Num of clients" << m_clients.size() << C_DLT_LOG::endl;


    int cnt = 0;
    for (itClients = m_clients.begin(); itClients != m_clients.end(); itClients++, cnt++)
    {
        if (itClients->name == pClientName)
        {
            _pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK_CLIENT_ALREADY_REGISTERED;

            ClassLog(DLT_LOG_INFO) << "LC_RegisterClient() : ClientName" << itClients->name.c_str() << C_DLT_LOG::endl;
            ClassLog(DLT_LOG_INFO) << "LC_RegisterClient() : lcState" << (int)itClients->lcState << C_DLT_LOG::endl;
            break;
        }
    }

    if (_pStatus != ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK_CLIENT_ALREADY_REGISTERED)
    {
        // insert the new client
        m_clients.push_back(tempClient);

        _pStatus = ::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;

        ClassLog(DLT_LOG_INFO) << "LC_RegisterClient() : ClientName, Registered successfully" << tempClient.name.c_str() << C_DLT_LOG::endl;
        ClassLog(DLT_LOG_INFO) << "LC_RegisterClient() : lcState, Registered successfully" << tempClient.name.c_str() << C_DLT_LOG::endl;
    }
    else
    {
    	/* do nothing, keep the client status as it was*/
    }

    m_clients_mtx.unlock();

    PrintRegisteredClients();
    ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "End" << C_DLT_LOG::endl;
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::evaluateLC_StayActive(std::string pClientName, ::Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum pStayActive, ::Lifecycle::Interface::LifecycleTypes::LC_Status &_pStatus)
{
    ::iMicro::pmStructs::tcu_parameters_type _tcu_param;    
    CommonAPI::CallStatus _internalCallStatus = CommonAPI::CallStatus::SUCCESS;
    //ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "Start" << C_DLT_LOG::endl;
    m_clients_mtx.lock();

    std::vector<client_t>::iterator l_count = m_clients.begin();
    while (l_count != m_clients.end())
    {
        if (l_count->name == pClientName)
        {
            break;
        }
        else
        {
            l_count++;
        }
    }

    if (l_count == m_clients.end())
    {
        _pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK;
        ClassLog(DLT_LOG_INFO) << "LC_StayActive() : " << pClientName << " error: client not found " << C_DLT_LOG::endl;
    }
    else
    {
    	if((pStayActive == Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_LOCK) &&
    			((this->NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES)||
    			(this->NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_CONNECTED)))
    	{
    		_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
            _tcu_param.setPENDING_TCU_ACTION(pStayActive_Status::STAY_ACTIVE);
		//ClassLog(DLT_LOG_INFO) << "LC_StayActive() : " << pClientName << " wants to set stay active " << pStayActive <<  C_DLT_LOG::endl;
    		l_count->stayActive = pStayActive;
            _tcu_param.setCONNECTION_UNLOCK_STATUS(1);
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->update_TCU_Parameters(_tcu_param, _internalCallStatus);
            ClassLog(DLT_LOG_INFO) << "Pending TCU actions trigerred" << C_DLT_LOG::endl;
    	}
    	else if(pStayActive == Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_RELEASE)
    	{
    		_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
            _tcu_param.setPENDING_TCU_ACTION(pStayActive_Status::STAY_INACTIVE);
		//ClassLog(DLT_LOG_INFO) << "LC_StayActive() : " << pClientName << " wants to set stay active " << pStayActive <<  C_DLT_LOG::endl;
    		l_count->stayActive = pStayActive;
            _tcu_param.setCONNECTION_UNLOCK_STATUS(1);
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->update_TCU_Parameters(_tcu_param, _internalCallStatus);
            ClassLog(DLT_LOG_INFO) << "No more Pending TCU actions trigerred" << C_DLT_LOG::endl;
    	}
    	else if((pStayActive == Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_BUB) && ((this->NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES)||(this->NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_CONNECTED))
    			                                                                                   && (this->NAD_additionalLifecycleStateInfo.NAD_CanStatus ==Lifecycle::Interface::LifecycleTypes::LC_CanStatus::LC_CAN_STS_OFF)
    			                                                                                   && (this->NAD_additionalLifecycleStateInfo.NAD_MainBatteryStatus == Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus::LC_MAIN_BATT_STS_NOK))
    	{
    		_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK;
            _tcu_param.setPENDING_TCU_ACTION(pStayActive_Status::STAY_INACTIVE);
            _tcu_param.setCONNECTION_UNLOCK_STATUS(1);
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->update_TCU_Parameters(_tcu_param, _internalCallStatus);
            ClassLog(DLT_LOG_INFO) << "No more Pending TCU actions trigerred/BUB mode" << C_DLT_LOG::endl;

		//ClassLog(DLT_LOG_INFO) << "LC_StayActive() : " << pClientName << " wants to set stay active " << pStayActive <<  C_DLT_LOG::endl;
    		l_count->stayActive = pStayActive;
    	}
    	else
    	{
    		_pStatus = Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK;
            _tcu_param.setPENDING_TCU_ACTION(pStayActive_Status::STAY_INACTIVE);
            _tcu_param.setCONNECTION_UNLOCK_STATUS(1);
            m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->update_TCU_Parameters(_tcu_param, _internalCallStatus);
		//ClassLog(DLT_LOG_INFO) << "LC_StayActive() : " << pClientName << " error: "<< pStayActive << " not permitted  " << C_DLT_LOG::endl;
    	}

        // When Lifecycle can switch off (StayActive false), then we send ACK to VuC
       /* if(isStayActive(false) != Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_LOCK)
        {
        	if((this->NAD_LifecycleState == ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES)||(this->NAD_LifecycleState == Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_CONNECTED))
        	{
        		ClassLog(DLT_LOG_INFO) << "LC_StayActive() :  this->SendAcknowledgeToVuC();" << C_DLT_LOG::endl;
                this->SendAcknowledgeToVuC(true);
        	}
        }*/
    }

    m_clients_mtx.unlock();
    PrintRegisteredClients();
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::LC_StayActive(const std::shared_ptr<CommonAPI::ClientId> _client, std::string pClientName, ::Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum pStayActive, LC_StayActiveReply_t _reply)
{
    (void)_client;
    ::Lifecycle::Interface::LifecycleTypes::LC_Status _pStatus;
    evaluateLC_StayActive(pClientName, pStayActive, _pStatus);
    ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_StayActive , StayActiveStatus" << _pStatus << C_DLT_LOG::endl; 
    _reply(_pStatus);     
}

void C_COMMONAPI_SERVER_LifecycleAPI_IMP::LC_SetRTC(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _pTimeout, LC_SetRTCReply_t _reply)
{
    ClassLog(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << "LC_SetRTC : RTC Config Time in min" << _pTimeout << C_DLT_LOG::endl; 
    ::iMicro::iMicroStructs::WakeUpTimer_t _timer_param; 
    CommonAPI::CallStatus _internalCallStatus;    

    _timer_param.setWakeUpTimerVal(_pTimeout);
    _timer_param.setPowerModeVal(PowerModeStates::SYSTEM_LC_LISTEN_MODE_1);
    m_ctrl->getCommonapiClient().getSOC_LIFECYCLEProxy()->SetSOC_Periodic_WakeUpTimer(_timer_param,_internalCallStatus);

    if(_internalCallStatus == CommonAPI::CallStatus::SUCCESS)
    {
        ClassLog(DLT_LOG_INFO) << "LC_SetRTC triggerred" << C_DLT_LOG::endl;
    }
    else
    {
        ClassLog(DLT_LOG_INFO) << "LC_SetRTC trigger failed" << C_DLT_LOG::endl;
    }
    _reply(::Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK);
}
