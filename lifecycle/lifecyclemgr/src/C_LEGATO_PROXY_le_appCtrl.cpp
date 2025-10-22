#include "C_LEGATO_PROXY_le_appCtrl.h"
#include "C_CONTROLLER_IMP.h"
//#include "C_TRACE.h"

LegatoProxy_appCtrl::LegatoProxy_appCtrl(C_CONTROLLER_IMP& ctrl) : m_ctrl(ctrl)
{
	LE_INFO("Starting LegatoProxy_appCtrl");
}

LegatoProxy_appCtrl::~LegatoProxy_appCtrl()
{
	LE_INFO("LegatoProxy_appCtrl");
	LE_INFO("Stop loop");

    C_LEGATO_BASE::stop_loop();

    LE_INFO("Joining");
    le_result_t result = le_thread_Join(m_leThread, nullptr);
    if(result != LE_OK)
    {
    	LE_INFO("Joining error");
    }

    LE_INFO("done");
}

void LegatoProxy_appCtrl::init()
{
    m_leThread = le_thread_Create("LegatoProxy_appCtrl", LegatoProxy_appCtrlWrapper, this);
    le_thread_SetJoinable(m_leThread);
    le_thread_Start(m_leThread);
}

void* LegatoProxy_appCtrl::LegatoProxy_appCtrlWrapper(void* context) {
    if(context != nullptr)
    {
        reinterpret_cast<LegatoProxy_appCtrl*>(context)->LegatoProxy_appCtrlThread();
    }
    return context;
}

void LegatoProxy_appCtrl::LegatoProxy_appCtrlThread()
{
	LE_INFO("LegatoProxy_appCtrlThread");

	C_le_appCtrl_ConnectService();

	LE_INFO("Connecting Service ... done");

	C_le_appCtrl_AddAppStartStopEventHandler(AppStartStopHandlerWrapper, this);

	LE_INFO("Registering AppStartStop handler ... done");

	C_LEGATO_BASE::ServiceLoop();

	C_le_appCtrl_DisconnectService();

	LE_INFO("Exit thread");
}

void LegatoProxy_appCtrl::AppStartStopHandlerWrapper(const char* appID, const uint32_t action, void* context)
{
    if(context != nullptr)
    {
        reinterpret_cast<LegatoProxy_appCtrl*>(context)->AppStartStopEventHandler(appID, action);
    }
}

void LegatoProxy_appCtrl::AppStartStopEventHandler(const char* appID, const uint32_t action)
{

	LE_INFO("AppStartStopEventHandler");

	v1::Lifecycle::Interface::LifecycleAPI::LC_Status pStatus;

	if (action == 0)
	{
		LE_INFO("stopped, unregistering it");
		m_ctrl.getCommonapiServer().LC_UnRegisterClient(appID, pStatus);
		LE_INFO("appID unregistered, status: %d", pStatus);
	}
	else
	{
		LE_INFO("started, do nothing");
	}
}
