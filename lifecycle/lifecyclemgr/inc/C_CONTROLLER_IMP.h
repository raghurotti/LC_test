/*
 * C_CONTROLLER_IMP.h
 *
 *  Created on: Dec 28, 2017
 *      Author: tbmnxvmuser
 */

#ifndef C_CONTROLLER_IMP_H_
#define C_CONTROLLER_IMP_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "C_DLT_LOG.h"		
#include "commonapi_client.h"
//#include "LC_CommonAPI_Client.h"
//#include "LC_Message.h"
//#include "LC_Queue.h"
#include "C_OBJ.h"

#include "C_COMMONAPI_SERVER_LifecycleAPI_IMP.h"
#include "LegatoServer.h"
//#include "C_LEGATO_PROXY_le_appCtrl.h"





class C_CONTROLLER_IMP : public C_OBJ{
public:
	C_CONTROLLER_IMP();
	virtual ~C_CONTROLLER_IMP();

	void init();

	//Getters

	C_COMMONAPI_SERVER_LifecycleAPI_IMP & getCommonapiServer() {
		return m_commonapi_server;
	}

	LegatoServer & getLegatoServer() {
		return m_legatoServer;
	}

	CommonAPI_Client & getCommonapiClient() {
		return m_commonapi_client;
	}

	enum WakeupReason : uint32_t {
		SMS = 0,
		POWERKEY = 1,
		MODEM_SPURIOUS = 2,
		RTC = 3,
		MQTT = 4,
		NON_TCP = 5
	};

    static void* ql_recvfrom_kernel(void* arg);
    int ql_lpm_pthread(void);
    void ql_lpm_handler(WakeupReason eWakeUpReason);

private:
	inline C_DLT_LOG& dlt_logger(DltLogLevelType p_loglevel);
    static C_DLT_LOG m_dlt_log;
	

	//void fireNotificationEvent(const lc_int_message_t& pMessage);

	//std::thread m_thread;
	std::atomic_bool m_is_running;
	//lifecycle::queue<lc_message_t> m_queue;

    C_COMMONAPI_SERVER_LifecycleAPI_IMP m_commonapi_server;
	CommonAPI_Client m_commonapi_client;
	LegatoServer m_legatoServer;
	//LegatoProxy_appCtrl m_legatoProxy_le_appCtrl;
};

// === Inline Definition

inline C_DLT_LOG& C_CONTROLLER_IMP::dlt_logger(DltLogLevelType p_loglevel)
{
	return m_dlt_log.Log(p_loglevel, this);
}



#endif /* C_CONTROLLER_IMP_H_ */
