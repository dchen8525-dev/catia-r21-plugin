// CATIA CAA Hello World Plugin - Module Entry Point
// This file contains the module initialization code

#include "PartNumberMenuLog.h"

class PartNumberMenuModuleTrace
{
public:
    PartNumberMenuModuleTrace()
    {
        PartNumberMenuLog("MODULE", "PartNumberMenuModule.dll loaded");
    }

    ~PartNumberMenuModuleTrace()
    {
        PartNumberMenuLog("MODULE", "PartNumberMenuModule.dll unloaded");
    }
};

static PartNumberMenuModuleTrace g_PartNumberMenuModuleTrace;
