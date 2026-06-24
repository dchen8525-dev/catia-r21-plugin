// CATIA CAA Hello World Plugin - Module Entry Point
// This file contains the module initialization code

#include "NewBomLog.h"

class NewBomModuleTrace
{
public:
    NewBomModuleTrace()
    {
        NewBomLog("MODULE", "NewBomModule.dll loaded");
    }

    ~NewBomModuleTrace()
    {
        NewBomLog("MODULE", "NewBomModule.dll unloaded");
    }
};

static NewBomModuleTrace g_NewBomModuleTrace;
