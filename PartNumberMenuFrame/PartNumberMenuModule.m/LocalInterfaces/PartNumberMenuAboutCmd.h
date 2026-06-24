// PartNumberMenuAboutCmd.h
// CATIA CAA "About" Command - displays plugin version info

#ifndef __PartNumberMenuAboutCmd_h__
#define __PartNumberMenuAboutCmd_h__

#include "CATCommand.h"

/**
 * @brief About Command
 *
 * Lightweight command that pops up an information box with the plugin
 * version info, then self-destructs. No persistent dialog, no shared mode.
 */
class PartNumberMenuAboutCmd : public CATCommand
{
    CATDeclareClass;

public:
    PartNumberMenuAboutCmd();
    virtual ~PartNumberMenuAboutCmd();

    // CATCommand overrides
    virtual CATStatusChangeRC Activate(CATCommand* iCmd,
                                       CATNotification* iNotif);
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd,
                                          CATNotification* iNotif);
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd,
                                     CATNotification* iNotif);
};

#endif // __PartNumberMenuAboutCmd_h__
