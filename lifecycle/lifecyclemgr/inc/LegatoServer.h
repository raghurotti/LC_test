/*
 * C_LEGATO_IMP.h
 */

#pragma once

#include <cstdint>
#include "C_LEGATO_SERVER_GEN.h"
#include "legato.h"
#include "C_COMMONAPI_SERVER_LifecycleAPI_IMP.h"

using namespace v1::Lifecycle::Interface;

class C_CONTROLLER_IMP;

constexpr auto EVT_ID_LIFECYCLE = "LC_NOTIFICATION_State";

typedef struct
{
   lifecycle_LifeCycleState_t lc_state;
   lifecycle_LC_WakeupSource_t lc_wakeupSource;
   lifecycle_LC_CanStatus_t lc_canStatus;
   lifecycle_LC_MainBatteryStatus_t lc_mainBatteryStatus;
   lifecycle_LC_VTMstate_t lc_vtmState;
} lifecycleBroadcastinfo_plus_type;


class LegatoServer: public C_LEGATO_SERVER_GEN {

public:
	LegatoServer(C_CONTROLLER_IMP* p_ctrl);
	virtual ~LegatoServer();

	virtual void AdvertiseEvents() override;
	lifecycle_LC_Status_t capiLcStatusToLeLcStatus(const Lifecycle::Interface::LifecycleTypes::LC_Status& status);
	Lifecycle::Interface::LifecycleTypes::LC_State leLcStateToCapiLcState(lifecycle_LifeCycleState_t state);
	virtual void lifecycle_GetLifeCycleState_plus(lifecycle_LifeCycleState_t* statePtr, lifecycle_LC_WakeupSource_t* pWakeupSourcePtr, lifecycle_LC_CanStatus_t* pCanStatusPtr, lifecycle_LC_MainBatteryStatus_t* pMainBatteryStatusPtr, lifecycle_LC_VTMstate_t* pVtmStatePtr) override;
	void ReportLcStateEvent_plus(::Lifecycle::Interface::LifecycleTypes::LC_State PWState, additionalLifecycleStateInfo_t lcAdditionalInfo);
	virtual void lifecycle_LC_Emergency(const char* clientName,lifecycle_LifeCycleState_t powerState,lifecycle_LC_Status_t* statePtr) override;
	virtual void lifecycle_LC_StayActive(const char* pClientName, lifecycle_LC_stayActiveEnum_t pStayActive, lifecycle_LC_Status_t* statePtr) override;
	virtual void lifecycle_RemoveELifeCycleHandler(lifecycle_ELifeCycleHandlerRef_t handlerRef) override;
	virtual lifecycle_ELifeCycleHandlerRef_t lifecycle_AddELifeCycleHandler(lifecycle_LifeCycleHandlerFunc_t handlerPtr, void* contextPtr) override;
	void leLcStayActiveToCapiLcStayActive(lifecycle_LC_stayActiveEnum_t lcStayActive, Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum& lelcStayActive);
	lifecycle_LifeCycleState_t capiLcStateToLeLcState(const ::Lifecycle::Interface::LifecycleTypes::LC_State& lcstate);
	lifecycle_LC_WakeupSource_t capiLCWakeupToLeWakeupSource(::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource& lcWakeUpSource);
	lifecycle_LC_CanStatus_t capiLCCanStatusToLeCanStatus(::Lifecycle::Interface::LifecycleTypes::LC_CanStatus& lcCanStatus);
	lifecycle_LC_MainBatteryStatus_t capiLCBatteryStateToLeBatteryStatus(::Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus& lcBatteryStatus);
	lifecycle_LC_VTMstate_t capiLCThermalStateToLeThermalState(::Lifecycle::Interface::LifecycleTypes::LC_VTMstate& lcThermalState);
	virtual void lifecycle_RegisterClient(const char* pClientName, lifecycle_LC_Status_t* statePtr, lifecycle_LifeCycleState_t* PowerStatePtr) override;
protected:
	/*virtual void NSLCTEST_Print(const char* message);
	int32_t srvS2_Fct2(const char* sIn, char* sOut, size_t sOutNumElements);
	void msg_SrvCliIdGet(le_msg_SessionRef_t Session, uid_t* pUserId);
	srvS2_Event2HandlerRef_t srvS2_AddEvent2Handler(const char* sIn2,
			srvS2_Handler2Func_t Handfunc2, void* pCtx);
	void srvS2_RemoveEvent2Handler(srvS2_Event2HandlerRef_t SrvHdl2);
	void AdvertiseEvents();*/
private:
	C_CONTROLLER_IMP* m_ctrl;
	static lifecycleBroadcastinfo_plus_type m_LC_NOTIFICATION_BroadcastInfo_plus;
	le_event_Id_t m_lifecycleStateEvent     = nullptr;
	static void OnLifeCycleEvent(void* Data, void* functionHdler);	
};

