/*
 * C_CONTROLLER_IMP.cpp
 *
 *  Created on: Dec 28, 2017
 *      Author: tbmnxvmuser
 */

#include "C_CONTROLLER_IMP.h"
//#include "C_TRACE.h"
//#include "C_Modem.h"

#include <chrono>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <pthread.h>
#include "ql_lpm.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "TBMLib.h"

#ifdef __cplusplus
}
#endif

#define MAX_BUF_SIZE     2048
#define MY_END_CHAR      0x13
#define QL_LPM_NETLINK 28
#define MAX_PAYLOAD 24
#define RESULT_LEN 3

#define VALID_WAKEUP_SOURCE_ID "s2r_wake_lock"

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER; // for g_flag
static char buf[MAX_BUF_SIZE+2];
static int sock_fd = -1;
static pthread_t pth_id = -1;
static struct nlmsghdr *nlh = NULL;
static int g_flag = -1; // 0--sleep;1--wake up

using namespace std;

#define CHECK_BIT(var,pos) ((bool)((var) & (1<<(pos))))


C_DLT_LOG C_CONTROLLER_IMP::m_dlt_log;

C_CONTROLLER_IMP::C_CONTROLLER_IMP() : C_OBJ(), m_is_running(false), m_commonapi_client(this), m_commonapi_server(this), m_legatoServer(this){//, m_commonapi_client(m_queue), m_legatoServer(*this), m_legatoProxy_le_appCtrl(*this){

}

C_CONTROLLER_IMP::~C_CONTROLLER_IMP()
{
    m_commonapi_client.EndInterface();
    m_commonapi_server.EndInterface();

    // Ask the thread to stop and wait for it
    m_is_running.store(false);
    //m_thread.join();

}

void C_CONTROLLER_IMP::init()
{
    C_OBJ::Init();
    // DLT init, which is context_register, must be done here in Application::init because DLT
    // application_register is done in App_Base::open which is calling  App_Base::init which is
    // overload by application class.
    m_dlt_log.init("CTRL", "NRE Skeleton client controller");

	dlt_logger(DLT_LOG_INFO) << " __Lifecycle legato v1.0 __ " << C_DLT_LOG::endl;

    dlt_logger(DLT_LOG_INFO) << "LegatoServer | Initializing legato server interfaces" << C_DLT_LOG::endl;
    m_legatoServer.Init();
    dlt_logger(DLT_LOG_INFO) << "LegatoServer | Initializing...done" << C_DLT_LOG::endl;



    //C_Modem::getInstance()->setClientInstance(&m_commonapi_client);

	dlt_logger(DLT_LOG_INFO) << "LegatoServer | Initializing legato le_appCtrl client interfaces" << C_DLT_LOG::endl;
	//m_legatoProxy_le_appCtrl.init();
	dlt_logger(DLT_LOG_INFO) << "LegatoServer | Initializing...done" << C_DLT_LOG::endl;

    m_is_running.store(true);
    std::thread { &C_COMMONAPI_SERVER_LifecycleAPI_IMP::init, &m_commonapi_server}.detach();
    std::thread { &CommonAPI_Client::init, &m_commonapi_client, &m_commonapi_server}.detach(); //someip rcv thread

    //int qlmret = ql_lpm_pthread();
    //dlt_logger(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << " ql_lpm_pthread " << qlmret << C_DLT_LOG::endl;

    dlt_logger(DLT_LOG_INFO) << __PRETTY_FUNCTION__ << " Init Lifecycle CONTROLLER Ok" << C_DLT_LOG::endl;
}

void C_CONTROLLER_IMP::ql_lpm_handler(WakeupReason eWakeUpReason)
{    
    pthread_mutex_lock(&g_mutex);
    m_commonapi_server.setSocketWakeupSource(eWakeUpReason);
    CommonAPI::CallStatus _internalCallStatus = CommonAPI::CallStatus::NOT_AVAILABLE;

    if((eWakeUpReason == WakeupReason::POWERKEY) || (eWakeUpReason == WakeupReason::SMS) 
        || (eWakeUpReason == WakeupReason::RTC) || (eWakeUpReason == WakeupReason::MQTT))
    {
        dlt_logger(DLT_LOG_INFO) << "WakeupReason" << eWakeUpReason << C_DLT_LOG::endl;
        const bool l_wakeLock_call = TBM_set_wake_lock(VALID_WAKEUP_SOURCE_ID);
    
        if ( true == l_wakeLock_call ) 
        {
            dlt_logger(DLT_LOG_INFO) << "ERROR: LC TCU could NOT acquire wakeup-source!" << C_DLT_LOG::endl;
            
        } 
        else 
        {
            dlt_logger(DLT_LOG_INFO) << "LC TCU successfully acquired wakeup-source" << C_DLT_LOG::endl;
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(4));
    
    switch(eWakeUpReason)
    {
        case WakeupReason::SMS:
        {
            dlt_logger(DLT_LOG_INFO) << "WakeupReason: SMS" << C_DLT_LOG::endl;
            if(m_commonapi_client.SOC_LIFECYCLE_IsAvailable())
            {
                m_commonapi_client.getSOC_LIFECYCLEProxy()->SendEvent_SOC_WakeupReason_TCU(::iMicro::iMicroTypes::SOC_WakeupReason_TCU_enum::sms,_internalCallStatus);
            }
            break;
        }
        /*case WakeupReason::NON_TCP:
        {
            dlt_logger(DLT_LOG_INFO) << "WakeupReason: NON_TCP" << C_DLT_LOG::endl;
            if(m_commonapi_client.SOC_LIFECYCLE_IsAvailable())
            {
                m_commonapi_client.getSOC_LIFECYCLEProxy()->SendEvent_SOC_WakeupReason_TCU(::iMicro::iMicroTypes::SOC_WakeupReason_TCU_enum::spurious_modem_Wakeup,_internalCallStatus);
            }
            break;
        }*/
        case WakeupReason::MODEM_SPURIOUS:            
        {
            dlt_logger(DLT_LOG_INFO) << "WakeupReason: SPURIOUS" << C_DLT_LOG::endl;
            if(m_commonapi_client.SOC_LIFECYCLE_IsAvailable())
            {
                m_commonapi_client.getSOC_LIFECYCLEProxy()->SendEvent_SOC_WakeupReason_TCU(::iMicro::iMicroTypes::SOC_WakeupReason_TCU_enum::spurious_modem_Wakeup,_internalCallStatus);
            }
            break;
        }
        case WakeupReason::RTC:
        {
            dlt_logger(DLT_LOG_INFO) << "WakeupReason: RTC" << C_DLT_LOG::endl;
            break;
        }
        case WakeupReason::MQTT:
        {
            dlt_logger(DLT_LOG_INFO) << "WakeupReason: MQTT" << C_DLT_LOG::endl;
            if(m_commonapi_client.SOC_LIFECYCLE_IsAvailable())
            {
                m_commonapi_client.getSOC_LIFECYCLEProxy()->SendEvent_SOC_WakeupReason_TCU(::iMicro::iMicroTypes::SOC_WakeupReason_TCU_enum::mqtt_wakeup,_internalCallStatus);
            }
            break;
        }
        default:
            break;
    }
    if(_internalCallStatus == CommonAPI::CallStatus::SUCCESS)
    {
        dlt_logger(DLT_LOG_INFO) << "WakeupReason: SendEvent_SOC_WakeupReason_TCU triggerred" << C_DLT_LOG::endl;
    }
    else
    {
        dlt_logger(DLT_LOG_INFO) << "WakeupReason: SendEvent_SOC_WakeupReason_TCU trigger failed" << C_DLT_LOG::endl;
    }    
    pthread_mutex_unlock(&g_mutex);    
}

void* C_CONTROLLER_IMP::ql_recvfrom_kernel(void* arg)
{
    // Convert the argument to an instance pointer if needed
    C_CONTROLLER_IMP* self = static_cast<C_CONTROLLER_IMP*>(arg);
    
    struct sockaddr_nl src_addr, dest_addr;
    struct iovec iov;
    struct msghdr msg;
    char user_msg[MAX_PAYLOAD + 1] = {0};
    WakeupReason eWakeupReason;

    int sock_fd;
    struct nlmsghdr* nlh;

    if (-1 == (sock_fd = socket(AF_NETLINK, SOCK_RAW, QL_LPM_NETLINK)))
    {
        self->dlt_logger(DLT_LOG_INFO) << "ql_recvfrom_kernel socket creation failure" << C_DLT_LOG::endl;
        return NULL;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    src_addr.nl_groups = 0;

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0;

    if (-1 == bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr)))
    {
        self->dlt_logger(DLT_LOG_INFO) << "ql_recvfrom_kernel socket bind failure" << C_DLT_LOG::endl;
        return NULL;
    }

    if (NULL == (nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD + 1))))
    {
        self->dlt_logger(DLT_LOG_INFO) << "ql_recvfrom_kernel socket netlink payload failure" << C_DLT_LOG::endl;
        return NULL;
    }

    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD + 1));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD + 1);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_type = NLMSG_NOOP;
    nlh->nlmsg_flags = 0;

    sprintf(user_msg, "ID %d come in", nlh->nlmsg_pid);
    strcpy((char*)NLMSG_DATA(nlh), user_msg);  // explicit cast

    memset(&iov, 0, sizeof(iov));
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    (void)sendmsg(sock_fd, &msg, 0);

    while (1)
    {
        memset((char*)NLMSG_DATA(nlh), 0, MAX_PAYLOAD + 1);
        (void)recvmsg(sock_fd, &msg, 0);

        if (strncmp((char*)NLMSG_DATA(nlh), "glink_qmi", 9) == 0 ||
           (strncmp((char*)NLMSG_DATA(nlh), "efs_qmi", 7) == 0) ||
           (strncmp((char*)NLMSG_DATA(nlh), "NON_TCP", 7) == 0) ||
           (strncmp((char*)NLMSG_DATA(nlh), "OTHERS", 6) == 0) ||
           (strncmp((char*)NLMSG_DATA(nlh), "ABNORMAL", 8) == 0) ||
           (strncmp((char*)NLMSG_DATA(nlh), "unknown", 7) == 0))
        {
            eWakeupReason = WakeupReason::MODEM_SPURIOUS;
        }
        else if (strncmp((char*)NLMSG_DATA(nlh), "sms", 3) == 0)
        {
            eWakeupReason = WakeupReason::SMS;
        }        
        else if (strncmp((char*)NLMSG_DATA(nlh), "tcp", 3) == 0)            
        {
            eWakeupReason = WakeupReason::MQTT;
            //mqtt wakeup
        }
        else if (strncmp((char*)NLMSG_DATA(nlh), "rtc", 3) == 0)// ||
                //(strncmp((char*)NLMSG_DATA(nlh), "NON_TCP", 7) == 0))            
        {
            eWakeupReason = WakeupReason::RTC;
            //remotewakeup
        }
        else if (strncmp((char*)NLMSG_DATA(nlh), "gp98", 4) == 0)// ||
                //(strncmp((char*)NLMSG_DATA(nlh), "NON_TCP", 7) == 0))            
        {
            eWakeupReason = WakeupReason::POWERKEY;
            //remotewakeup
        }
        /*else if(strncmp((char*)NLMSG_DATA(nlh), "NON_TCP", 7) == 0)
        {
           eWakeupReason = WakeupReason::NON_TCP;
        }*/
        // Use the instance pointer to call the member function
        self->ql_lpm_handler(eWakeupReason);
    }

    close(sock_fd);
    free(nlh);
    nlh = NULL;

    return NULL;
}

int C_CONTROLLER_IMP::ql_lpm_pthread(void)
{
    if (pthread_create(&pth_id, NULL, C_CONTROLLER_IMP::ql_recvfrom_kernel, this) != 0)
    {
        dlt_logger(DLT_LOG_INFO) << "ql_lpm_pthread: ql_recvfrom_kernel thread creation failed" << C_DLT_LOG::endl;
        return -1;
    }

    pthread_detach(pth_id);
    return 0;
}



