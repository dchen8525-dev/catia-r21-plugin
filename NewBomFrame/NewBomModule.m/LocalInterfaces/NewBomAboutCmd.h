// NewBomAboutCmd.h
// CATIA CAA "About" Command - displays plugin version info

#ifndef __NewBomAboutCmd_h__
#define __NewBomAboutCmd_h__

#include "CATCommand.h"

/**
 * @brief About Command
 *
 * Lightweight command that pops up an information box with the plugin
 * version info, then self-destructs. No persistent dialog, no shared mode.
 */
class NewBomAboutCmd : public CATCommand
{
    CATDeclareClass;

public:
    NewBomAboutCmd();
    virtual ~NewBomAboutCmd();

    // CATCommand overrides
    virtual CATStatusChangeRC Activate(CATCommand* iCmd,
                                       CATNotification* iNotif);
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd,
                                          CATNotification* iNotif);
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd,
                                     CATNotification* iNotif);
};

#endif // __NewBomAboutCmd_h__
