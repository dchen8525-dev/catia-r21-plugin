// PartNumberMenuAddin.h
// CATIA CAA Hello World Plugin - Workbench Addin

#ifndef __PartNumberMenuAddin_h__
#define __PartNumberMenuAddin_h__

#include "CATBaseUnknown.h"
#include "CATIAfrGeneralWksAddin.h"

class CATCmdContainer;

/**
 * @brief CATIA Workbench Addin implementation
 * 
 * This class implements CATIAfrGeneralWksAddin interface to register
 * our Hello World command into CATIA general workshop.
 */
class PartNumberMenuAddin : public CATBaseUnknown
{
    CATDeclareClass;

public:
    PartNumberMenuAddin();
    virtual ~PartNumberMenuAddin();

    // CATIAfrGeneralWksAddin interface implementation
    virtual void CreateCommands();
    virtual CATCmdContainer* CreateToolbars();
};

#endif // __PartNumberMenuAddin_h__
