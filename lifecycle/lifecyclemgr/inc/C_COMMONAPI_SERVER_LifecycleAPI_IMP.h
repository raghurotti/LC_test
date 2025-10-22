#ifndef C_COMMONAPI_SERVER_NRE_test_H_
#define C_COMMONAPI_SERVER_NRE_test_H_
#include <string.h>
#include <thread>
#include "C_COMMONAPI_SERVER_LifecycleAPI_GEN.h"
#include <mutex>
#include "ql_lpm.h"
#include <cstdlib>    // For std::system()
#include <sys/stat.h> // For chmod()


using namespace v1::Lifecycle::Interface;
class C_CONTROLLER_IMP;
struct additionalLifecycleStateInfo_t
{
	::Lifecycle::Interface::LifecycleTypes::LC_WakeupSource NAD_WakeupSource;
	::Lifecycle::Interface::LifecycleTypes::LC_CanStatus NAD_CanStatus;
	::Lifecycle::Interface::LifecycleTypes::LC_MainBatteryStatus NAD_MainBatteryStatus;
	::Lifecycle::Interface::LifecycleTypes::LC_VTMstate NAD_VTMstate;
};

class C_COMMONAPI_SERVER_LifecycleAPI_IMP: public nre::C_COMMONAPI_SERVER_LifecycleAPI_GEN {

public:
	C_COMMONAPI_SERVER_LifecycleAPI_IMP(C_CONTROLLER_IMP* p_ctrl);
	virtual ~C_COMMONAPI_SERVER_LifecycleAPI_IMP();
    void init(void);
    void setPowerMode(int powerMode);
	void run_Modem(uint32_t state, uint32_t powerMode);
	void evaluateAndBroadcastPowerMode(::Lifecycle::Interface::LifecycleTypes::LC_State& currPowMode);
	void NotifyAllRegisteredClientsNadStateInfo(Lifecycle::Interface::LifecycleTypes::LC_State l_NAD_LifecycleStateToAPI);							  

    void LC_GetPowerState_plus(const std::shared_ptr<CommonAPI::ClientId> _client, LC_GetPowerState_plusReply_t _reply) override;	
    void LC_RegisterClient(const std::shared_ptr<CommonAPI::ClientId> _client, std::string l_pClientName, LC_RegisterClientReply_t _reply) override;
	void LC_UnRegisterClient(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _pClientName, LC_UnRegisterClientReply_t _reply) override;
	void LC_Emergency(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _pClientName, ::Lifecycle::Interface::LifecycleTypes::LC_State _pState, LC_EmergencyReply_t _reply) override;
	void LC_Wakeup(const std::shared_ptr<CommonAPI::ClientId> _client, LC_WakeupReply_t _reply) override;
	void LC_StayActive(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _pClientName, ::Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum _pStayActive, LC_StayActiveReply_t _reply) override;
	void LC_SetRTC(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _pTimeout, LC_SetRTCReply_t _reply) override;
	void LC_JumpToListen2(const std::shared_ptr<CommonAPI::ClientId> _client, LC_JumpToListen2Reply_t _reply) override;
	void setAndroidState(ANDROID_STATUS_E status);
	void evaluateLC_Emergency(std::string pClientName, ::Lifecycle::Interface::LifecycleTypes::LC_State &_pState, ::Lifecycle::Interface::LifecycleTypes::LC_Status &_pStatus);
	void evaluate_RegisterClient(std::string p_ClientName, ::Lifecycle::Interface::LifecycleTypes::LC_State &_pState, ::Lifecycle::Interface::LifecycleTypes::LC_Status &_pStatus);
	void evaluateLC_StayActive(std::string pClientName, ::Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum pStayActive, ::Lifecycle::Interface::LifecycleTypes::LC_Status &_pStatus);
	void triggerLC_Emergency(::Lifecycle::Interface::LifecycleTypes::LC_State &_pState, ::Lifecycle::Interface::LifecycleTypes::LC_Status &_pStatus);
	void setNWSync(bool nwSync);
	void setSocketWakeupSource(uint32_t wakeupReason);
	void setPMWakeupSource(uint32_t wakeupReason);
	void set_LC_MainBattery(bool batteryStatus);
	void set_LC_VTMstate(bool vtmState);
	void startTCUOnlyTimer(uint32_t timerVal);


	enum PowerModeStates{
			
		SYSTEM_LC_OFF = 0U,
		SYSTEM_LC_BACKUP_BATTERY,		
		SYSTEM_LC_LOGISTICS,
		SYSTEM_LC_MAINTENANCE,
	    SYSTEM_LC_RESET,	
		SYSTEM_LC_BENCH,
		SYSTEM_LC_FULL_OPERATION,
		SYSTEM_LC_IDLE,		
		SYSTEM_LC_TIMED,	
		SYSTEM_LC_STANDBY,	
        SYSTEM_LC_PARTIAL_OPERATION,
        SYSTEM_LC_INIT_PHASE_1,		
		SYSTEM_LC_INIT_PHASE_2,	
		SYSTEM_LC_TCU_ONLY,		
		SYSTEM_LC_SUSPEND_TO_RAM,
		SYSTEM_LC_LISTEN_MODE_1,			
		SYSTEM_LC_LISTEN_MODE_2,		
		SYSTEM_LC_LISTEN_MODE_3,		
		SYSTEM_LC_DEEP_SLEEP,
		SYSTEM_LC_STAY_ALIVE_SERVICES,
		SYSTEM_LC_CAN_NETWORK_SLEEP_STATUS,
		SYSTEM_LC_GO_TO_SLEEP_AGAIN,                     
		SYSTEM_LC_NAD_EMERGENCY_LAW, 		      
		SYSTEM_LC_NAD_EMERGENCY,		      
		SYSTEM_LC_NAD_EMERGENCY_LAW_LP,
		SYSTEM_LC_NAD_ON                                 
	};

	enum pStayActive_Status{
		STAY_INACTIVE=0,
		STAY_ACTIVE=1
	};
	enum pConnection_Status{
		INACTIVE=0,
		ACTIVE=1
	};
	enum WAKEUP_REASON_enum : uint8_t {
        None = 0,
        CAN_Activity = 1,
        Speed = 2,
        SMS = 3,
        IGN = 4,
        NAD = 5
    };

private:
	C_CONTROLLER_IMP* m_ctrl;   

    ::Lifecycle::Interface::LifecycleTypes::LC_State NAD_LifecycleState;   
    additionalLifecycleStateInfo_t NAD_additionalLifecycleStateInfo;
	std::mutex m_clients_mtx;
	

	struct client_t
    {
		std::string name;
    	::Lifecycle::Interface::LifecycleTypes::LC_State lcState = ::Lifecycle::Interface::LifecycleTypes::LC_State::NAD_IDLE;
    	::Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum stayActive = ::Lifecycle::Interface::LifecycleTypes::LC_stayActiveEnum::LC_RELEASE;
        bool eCall = false;
        bool eCallLaw = false;
        bool testMode = false;
    };
	void PrintRegisteredClients(void);
	std::vector<client_t> m_clients;
	uint32_t m_modemState;
	bool bBuBMode;	
	bool noTCUPowerMode;
};

#endif
