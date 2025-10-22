/*
 * LegatoServer.cpp
 */

#include "LegatoServer.h"
#include "C_CONTROLLER_IMP.h"

lifecycleBroadcastinfo_plus_type LegatoServer::m_LC_NOTIFICATION_BroadcastInfo_plus;

LegatoServer::LegatoServer(C_CONTROLLER_IMP* p_ctrl) :
		m_ctrl(p_ctrl) 
{
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_state = LIFECYCLE_NAD_IDLE;
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_wakeupSource = LIFECYCLE_WAKE_UNKNOWN;
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_canStatus = LIFECYCLE_LC_CAN_STS_OFF;
    m_LC_NOTIFICATION_BroadcastInfo_plus.lc_mainBatteryStatus = LIFECYCLE_LC_MAIN_BATT_STS_OK;
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_vtmState = LIFECYCLE_LC_VTM_T_NORMAL;
}

LegatoServer::~LegatoServer() {

}

void LegatoServer::AdvertiseEvents()
{
	 m_lifecycleStateEvent = le_event_CreateId(EVT_ID_LIFECYCLE, sizeof(lifecycleBroadcastinfo_plus_type));
}

void LegatoServer::lifecycle_GetLifeCycleState_plus(lifecycle_LifeCycleState_t* statePtr, lifecycle_LC_WakeupSource_t* pWakeupSourcePtr, lifecycle_LC_CanStatus_t* pCanStatusPtr, lifecycle_LC_MainBatteryStatus_t* pMainBatteryStatusPtr, lifecycle_LC_VTMstate_t* pVtmStatePtr)
{
	
	LE_INFO("lifecycle_GetLifeCycleState_plus , Power mode : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_state);
	LE_INFO("lifecycle_GetLifeCycleState_plus , Wakeup Source : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_wakeupSource);
	LE_INFO("lifecycle_GetLifeCycleState_plus , CAN Status : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_canStatus);
	LE_INFO("lifecycle_GetLifeCycleState_plus , Main Battery State : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_mainBatteryStatus);
	LE_INFO("lifecycle_GetLifeCycleState_plus , Thermal State : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_vtmState);

	*statePtr = m_LC_NOTIFICATION_BroadcastInfo_plus.lc_state;
	*pWakeupSourcePtr = m_LC_NOTIFICATION_BroadcastInfo_plus.lc_wakeupSource;
	*pCanStatusPtr = m_LC_NOTIFICATION_BroadcastInfo_plus.lc_canStatus;
	*pMainBatteryStatusPtr = m_LC_NOTIFICATION_BroadcastInfo_plus.lc_mainBatteryStatus;
	*pVtmStatePtr = m_LC_NOTIFICATION_BroadcastInfo_plus.lc_vtmState;
}

lifecycle_LC_WakeupSource_t LegatoServer::capiLCWakeupToLeWakeupSource(::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource& lcWakeUpSource)
{
	lifecycle_LC_WakeupSource_t wakeupSource = LIFECYCLE_WAKE_UNKNOWN;

	switch(lcWakeUpSource)
	{
		case ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_LM1_RTC: wakeupSource = LIFECYCLE_WAKE_UP_LM1_RTC; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_MAINBATTERY: wakeupSource = LIFECYCLE_WAKE_UP_MAINBATTERY; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_ACCELEROMETER: wakeupSource = LIFECYCLE_WAKE_UP_ACCELEROMETER; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_CAN: wakeupSource = LIFECYCLE_WAKE_UP_CAN; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_LM3_RTC: wakeupSource = LIFECYCLE_WAKE_UP_LM3_RTC; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_SMS: wakeupSource = LIFECYCLE_WAKE_UP_SMS; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UP_MQTT: wakeupSource = LIFECYCLE_WAKE_UP_MQTT; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource::WAKE_UNKNOWN: wakeupSource = LIFECYCLE_WAKE_UNKNOWN; break;
		default: break;
	}
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_wakeupSource = wakeupSource;
	return wakeupSource;
}

lifecycle_LifeCycleState_t LegatoServer::capiLcStateToLeLcState(const ::Lifecycle::Interface::LifecycleTypes::LC_State& lcstate)
{
	lifecycle_LifeCycleState_t state = LIFECYCLE_NAD_IDLE;

	switch(lcstate)
	{
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_INIT: state = LIFECYCLE_NAD_INIT; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_BACKUP_BATTERY: state = LIFECYCLE_NAD_BACKUP_BATTERY; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_DEEP_SLEEP: state = LIFECYCLE_NAD_DEEP_SLEEP; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_REMOTE_WAKE: state = LIFECYCLE_NAD_REMOTE_WAKE; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON: state = LIFECYCLE_NAD_ON; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_OFF: state = LIFECYCLE_NAD_OFF; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES: state = LIFECYCLE_NAD_STAY_ALIVE_SERVICES; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SHUTDOWN: state = LIFECYCLE_NAD_SHUTDOWN; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SLEEP: state = LIFECYCLE_NAD_SLEEP; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_1: state = LIFECYCLE_NAD_LISTEN_1; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_2: state = LIFECYCLE_NAD_LISTEN_2; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_3: state = LIFECYCLE_NAD_LISTEN_3; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY: state = LIFECYCLE_NAD_EMERGENCY; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_MAINTENANCE: state = LIFECYCLE_NAD_MAINTENANCE; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_TEST_MODE: state = LIFECYCLE_NAD_TEST_MODE; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SW_UPDATE: state = LIFECYCLE_NAD_SW_UPDATE; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LOGISTIC: state = LIFECYCLE_NAD_LOGISTIC; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_RESET: state = LIFECYCLE_NAD_RESET; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW: state = LIFECYCLE_NAD_EMERGENCY_LAW; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_IDLE: state = LIFECYCLE_NAD_IDLE; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP: state = LIFECYCLE_NAD_EMERGENCY_LAW_LP; break;		
		case ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_CONNECTED: state = LIFECYCLE_NAD_STAY_ALIVE_CONNECTED; break;
		default: break;
	}
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_state = state;
	return state;
}

lifecycle_LC_CanStatus_t LegatoServer::capiLCCanStatusToLeCanStatus(::Lifecycle::Interface::LifecycleTypes::LC_CanStatus& lcCanStatus)
{
	lifecycle_LC_CanStatus_t state = LIFECYCLE_LC_CAN_STS_OFF;

	switch(lcCanStatus)
	{
		case ::Lifecycle::Interface::LifecycleTypes::LC_CanStatus::LC_CAN_STS_OFF: state = LIFECYCLE_LC_CAN_STS_ON; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_CanStatus::LC_CAN_STS_ON: state = LIFECYCLE_LC_CAN_STS_OFF; break;
		default: break;
	}
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_canStatus = state;
	return state;
}

lifecycle_LC_MainBatteryStatus_t LegatoServer::capiLCBatteryStateToLeBatteryStatus(::Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus& lcBatteryStatus)
{
	lifecycle_LC_MainBatteryStatus_t state = LIFECYCLE_LC_MAIN_BATT_STS_NOK;

	switch(lcBatteryStatus)
	{
		case ::Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus::LC_MAIN_BATT_STS_NOK: state = LIFECYCLE_LC_MAIN_BATT_STS_NOK; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus::LC_MAIN_BATT_STS_OK: state = LIFECYCLE_LC_MAIN_BATT_STS_OK; break;
		default: break;
	}
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_mainBatteryStatus = state;
	return state;

}

lifecycle_LC_VTMstate_t LegatoServer::capiLCThermalStateToLeThermalState(::Lifecycle::Interface::LifecycleTypes::LC_VTMstate& lcThermalState)
{
	lifecycle_LC_VTMstate_t state = LIFECYCLE_LC_VTM_T_NORMAL;
	
	switch(lcThermalState)
	{
		case ::Lifecycle::Interface::LifecycleTypes::LC_VTMstate::LC_VTM_T_NORMAL: state = LIFECYCLE_LC_VTM_T_NORMAL; break;
		case ::Lifecycle::Interface::LifecycleTypes::LC_VTMstate::LC_VTM_T_OVERTEMP: state = LIFECYCLE_LC_VTM_T_OVERTEMP; break;
		default: break;
	}
	m_LC_NOTIFICATION_BroadcastInfo_plus.lc_vtmState = state;
	return state;

}

void LegatoServer::ReportLcStateEvent_plus(::Lifecycle::Interface::LifecycleTypes::LC_State lcState, additionalLifecycleStateInfo_t lcAdditionalInfo)
{
	LE_INFO("ReportLcStateEvent_plus , Dbus Power mode : < %d > ",lcState);
	LE_INFO("ReportLcStateEvent_plus , Dbus Wakeup Source : < %d > ",lcAdditionalInfo.NAD_WakeupSource);
	LE_INFO("ReportLcStateEvent_plus , Dbus CAN Status : < %d > ",lcAdditionalInfo.NAD_CanStatus);
	LE_INFO("ReportLcStateEvent_plus , Dbus Main Battery State : < %d > ",lcAdditionalInfo.NAD_MainBatteryStatus);
	LE_INFO("ReportLcStateEvent_plus , Dbus Thermal State : < %d > ",lcAdditionalInfo.NAD_VTMstate);

	if(m_lifecycleStateEvent == nullptr)
	{
		LE_INFO("ReportLcStateEvent_plus m_lifecycleStateEvent is NULL");
		return;
	}	
	capiLcStateToLeLcState(lcState);
	capiLCWakeupToLeWakeupSource(lcAdditionalInfo.NAD_WakeupSource);
	capiLCCanStatusToLeCanStatus(lcAdditionalInfo.NAD_CanStatus);
	capiLCBatteryStateToLeBatteryStatus(lcAdditionalInfo.NAD_MainBatteryStatus);
	capiLCThermalStateToLeThermalState(lcAdditionalInfo.NAD_VTMstate);

	LE_INFO("ReportLcStateEvent_plus , LE Power mode : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_state);
	LE_INFO("ReportLcStateEvent_plus , LE Wakeup Source : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_wakeupSource);
	LE_INFO("ReportLcStateEvent_plus , LE CAN Status : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_canStatus);
	LE_INFO("ReportLcStateEvent_plus , LE Main Battery State : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_mainBatteryStatus);
	LE_INFO("ReportLcStateEvent_plus , LE Thermal State : < %d > ",m_LC_NOTIFICATION_BroadcastInfo_plus.lc_vtmState);

    le_event_Report(m_lifecycleStateEvent, &m_LC_NOTIFICATION_BroadcastInfo_plus, sizeof(m_LC_NOTIFICATION_BroadcastInfo_plus));
}

void LegatoServer::lifecycle_LC_Emergency(const char* pClientName, lifecycle_LifeCycleState_t pleState, lifecycle_LC_Status_t* statePtr)
{   

	Lifecycle::Interface::LifecycleTypes::LC_Status pStatus {};
	Lifecycle::Interface::LifecycleTypes::LC_State pState {};

	pState = leLcStateToCapiLcState(pleState);
		
	//m_ctrl->getCommonapiServer().evaluateLC_Emergency(pClientName, pState, pStatus);

	m_ctrl->getCommonapiServer().triggerLC_Emergency(pState, pStatus);

	*statePtr = capiLcStatusToLeLcStatus(pStatus);
}

void LegatoServer::lifecycle_RegisterClient(const char* pClientName, lifecycle_LC_Status_t* statePtr, lifecycle_LifeCycleState_t* PowerStatePtr)
{   

	::Lifecycle::Interface::LifecycleTypes::LC_Status pStatus {};
	::Lifecycle::Interface::LifecycleTypes::LC_State pState {};

	

	m_ctrl->getCommonapiServer().evaluate_RegisterClient(pClientName, pState, pStatus);
	
	* PowerStatePtr = capiLcStateToLeLcState(pState);
	* statePtr = capiLcStatusToLeLcStatus(pStatus);

	//*statePtr = capiLcStatusToLeLcStatus(pStatus);
}



Lifecycle::Interface::LifecycleTypes::LC_State LegatoServer::leLcStateToCapiLcState(lifecycle_LifeCycleState_t state)
{
	Lifecycle::Interface::LifecycleTypes::LC_State returnState = Lifecycle::Interface::LifecycleTypes::LC_State::NAD_IDLE;

    switch(state)
    {
		case LIFECYCLE_NAD_INIT: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_INIT;break;
		case LIFECYCLE_NAD_BACKUP_BATTERY: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_BACKUP_BATTERY;break;
		case LIFECYCLE_NAD_ON: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_ON; break;
		case LIFECYCLE_NAD_OFF: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_OFF; break;
		case LIFECYCLE_NAD_STAY_ALIVE_SERVICES: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_SERVICES; break;
		case LIFECYCLE_NAD_SHUTDOWN: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SHUTDOWN; break;
		case LIFECYCLE_NAD_SLEEP: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SLEEP; break;
		case LIFECYCLE_NAD_DEEP_SLEEP: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_DEEP_SLEEP; break;
		case LIFECYCLE_NAD_LISTEN_1: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_1; break;
		case LIFECYCLE_NAD_LISTEN_2: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_2; break;
		case LIFECYCLE_NAD_LISTEN_3: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LISTEN_3; break;
		case LIFECYCLE_NAD_EMERGENCY: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY; break;
		case LIFECYCLE_NAD_MAINTENANCE: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_MAINTENANCE; break;
		case LIFECYCLE_NAD_TEST_MODE: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_TEST_MODE; break;
		case LIFECYCLE_NAD_SW_UPDATE: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_SW_UPDATE; break;
		case LIFECYCLE_NAD_LOGISTIC: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_LOGISTIC; break;
		case LIFECYCLE_NAD_RESET: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_RESET; break;
		case LIFECYCLE_NAD_EMERGENCY_LAW: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW; break;
		case LIFECYCLE_NAD_IDLE: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_IDLE; break;
		case LIFECYCLE_NAD_EMERGENCY_LAW_LP: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_EMERGENCY_LAW_LP; break;
		case LIFECYCLE_NAD_REMOTE_WAKE: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_REMOTE_WAKE; break;
		case LIFECYCLE_NAD_STAY_ALIVE_CONNECTED: returnState=Lifecycle::Interface::LifecycleTypes::LC_State::NAD_STAY_ALIVE_CONNECTED; break;
		default: break;
    }

    return returnState;
}


lifecycle_LC_Status_t LegatoServer::capiLcStatusToLeLcStatus(const Lifecycle::Interface::LifecycleTypes::LC_Status& status)
{
    lifecycle_LC_Status_t le_state = lifecycle_LC_Status_t::LIFECYCLE_LC_STS_NOT_OK;

    switch(status)
    {
		case Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK: le_state = LIFECYCLE_LC_STS_NOT_OK; break;
		case Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_NOT_OK_CLIENT_NOT_REGISTERED: le_state = LIFECYCLE_LC_STS_NOT_OK_CLIENT_NOT_REGISTERED; break;
		case Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK: le_state = LIFECYCLE_LC_STS_OK; break;
		case Lifecycle::Interface::LifecycleTypes::LC_Status::LC_STS_OK_CLIENT_ALREADY_REGISTERED: le_state = LIFECYCLE_LC_STS_OK_CLIENT_ALREADY_REGISTERED; break;
		default: break;
    }
    return (le_state);
}


lifecycle_ELifeCycleHandlerRef_t LegatoServer::lifecycle_AddELifeCycleHandler(lifecycle_LifeCycleHandlerFunc_t handlerPtr, void* contextPtr)
{
    if(handlerPtr != nullptr)
    {
    	le_event_HandlerRef_t EventHandler = le_event_AddLayeredHandler(EVT_ID_LIFECYCLE, m_lifecycleStateEvent, OnLifeCycleEvent, (void*) handlerPtr);

		if (contextPtr != nullptr)
		{
			le_event_SetContextPtr(EventHandler, contextPtr);
		}
		else
		{
			// Think of this
		}

		return reinterpret_cast<lifecycle_ELifeCycleHandlerRef_t>(EventHandler);
    }

    //Handler is nullptr so nothing will be registered
    return {};
}

void LegatoServer::OnLifeCycleEvent(void* pData, void* functionHdler)
{

	lifecycle_LifeCycleHandlerFunc_t l_functionHdler = (lifecycle_LifeCycleHandlerFunc_t) functionHdler;

    if( pData != nullptr && l_functionHdler != nullptr)
    {
    	l_functionHdler(m_LC_NOTIFICATION_BroadcastInfo_plus.lc_state,
    			        m_LC_NOTIFICATION_BroadcastInfo_plus.lc_wakeupSource,
    			        m_LC_NOTIFICATION_BroadcastInfo_plus.lc_canStatus,
    			        m_LC_NOTIFICATION_BroadcastInfo_plus.lc_mainBatteryStatus,
    			        m_LC_NOTIFICATION_BroadcastInfo_plus.lc_vtmState,
    				    le_event_GetContextPtr());
    }
    else
    {
    	// No handler will be registered
    	return;
    }
}


void LegatoServer::lifecycle_RemoveELifeCycleHandler(lifecycle_ELifeCycleHandlerRef_t handlerRef)
{
    le_event_RemoveHandler(reinterpret_cast<le_event_HandlerRef_t>(handlerRef));
}

void LegatoServer::lifecycle_LC_StayActive(const char* pClientName, lifecycle_LC_stayActiveEnum_t pStayActive, lifecycle_LC_Status_t* statePtr)
{
	//ClassLog(DLT_LOG_INFO) << __func__<< "START"<<  pClientName << C_DLT_LOG::endl;

	
	::Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum plcStayActive = Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_RELEASE;
	::Lifecycle::Interface::LifecycleTypes::LC_Status pStatus {};

	leLcStayActiveToCapiLcStayActive(pStayActive, plcStayActive);	
	m_ctrl->getCommonapiServer().evaluateLC_StayActive(pClientName,plcStayActive,pStatus);

	*statePtr = capiLcStatusToLeLcStatus(pStatus);

	//ClassLog(DLT_LOG_INFO) << __func__<< " END " <<  C_DLT_LOG::endl;

}
void LegatoServer::leLcStayActiveToCapiLcStayActive(lifecycle_LC_stayActiveEnum_t lcStayActive, Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum& lelcStayActive)
{
    switch(lcStayActive)
    {
		case LIFECYCLE_LC_RELEASE: lelcStayActive=Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_RELEASE; break;
		case LIFECYCLE_LC_LOCK: lelcStayActive=Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_LOCK; break;
		case LIFECYCLE_LC_BUB: lelcStayActive=Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_BUB; break;
		default: lelcStayActive=Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_RELEASE; break;
    }
}

