/**
 * @file C_APP_IMP.h
 * Library: N/A
 * Package: N/A
 * Module: <module>
 *
 * <description>
 *
 * Copyright (c) 20011-12, Magneti Marelli.
 * All rights reserved.
 * <License Notice>
 */

#ifndef C_APP_IMP_H
#define C_APP_IMP_H

#include <sys/signalfd.h>

#include "C_APP_GEN.h"
#include "C_CONTROLLER_IMP.h"

#define C_APP_NAME "LCMG"
#define C_APP_DESC "Lifecycle Manager"

class C_APP_IMP: public nre::C_APP_GEN
{
public:
    C_APP_IMP();

protected:
    std::error_code Init() override;
    void SignalHandler(signalfd_siginfo& p_signalfd_siginfo) override;

    C_CONTROLLER_IMP controller;
    nre::DLT_Context dlt_app;
};

#endif
