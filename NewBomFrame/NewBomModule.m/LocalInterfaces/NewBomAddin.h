// NewBomAddin.h
// CATIA CAA Hello World Plugin - Workbench Addin

#ifndef __NewBomAddin_h__
#define __NewBomAddin_h__

#include "CATBaseUnknown.h"
#include "CATIAfrGeneralWksAddin.h"

class CATCmdContainer;

/**
 * @brief CATIA Workbench Addin implementation
 * 
 * This class implements CATIAfrGeneralWksAddin interface to register
 * our Hello World command into CATIA general workshop.
 */
class NewBomAddin : public CATBaseUnknown
{
    CATDeclareClass;

public:
    NewBomAddin();
    virtual ~NewBomAddin();

    // CATIAfrGeneralWksAddin interface implementation
    virtual void CreateCommands();
    virtual CATCmdContainer* CreateToolbars();
};

#endif // __NewBomAddin_h__
