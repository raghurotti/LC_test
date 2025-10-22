#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "C_APP_IMP.h"



int main(int argc, char *argv[])
{  

    C_APP_IMP application;

    nre::DLT_Context dlt_main("MAIN", "Main Context");
    dlt_main.log() << __PRETTY_FUNCTION__ << " Lifecycle main.cpp" << nre::dlt::endl;
	
	std::this_thread::sleep_for(std::chrono::seconds(10));

    if (nre::NreErr::OK == application.Open())
    {
        std::error_code ret;
        ret = application.Run();
        if (nre::NreErr::ERROR == ret)
        {
            dlt_main.log(nre::level::error) << "impossible to run application, error: "
                << ret.message() << nre::dlt::endl;
        }
    }

    application.Close();
    return 0;
}

/* parasoft-end-suppress SECURITY-19-2 */
