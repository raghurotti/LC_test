#include "C_APP_IMP.h"

using namespace nre;

C_APP_IMP::C_APP_IMP() : dlt_app("APPL", "User Application") {}

std::error_code C_APP_IMP::Init()
{
    controller.init();
    return nre::NreErr::OK;
}

void C_APP_IMP::SignalHandler(signalfd_siginfo& p_signalfd_siginfo)
{
    if(p_signalfd_siginfo.ssi_signo == SIGUSR1)
    {
        dlt_app.log() << "Application SIGUSR1 received" << dlt::endl;
    }
    else if(p_signalfd_siginfo.ssi_signo == SIGTERM)
    {
        dlt_app.log() << "Application SIGTERM received" << dlt::endl;

        // In case of SIGTERM, the application will be closed after the Application::SignalHandler()
        // return. do not call exit() yourself
    }
    else
    {
        dlt_app.log(level::debug)
            << "Application signal received: " << p_signalfd_siginfo.ssi_signo << dlt::endl;
    }
}
