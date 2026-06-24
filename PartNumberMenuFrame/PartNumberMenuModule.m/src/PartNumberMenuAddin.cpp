// PartNumberMenuAddin.cpp
// CATIA CAA Part Number Menu Plugin - Workbench Addin Implementation

#include "PartNumberMenuAddin.h"
#include "PartNumberMenuAboutCmd.h"
#include "PartNumberMenuLog.h"
#include "TIE_CATIAfrGeneralWksAddin.h"
#include "CATCommandHeader.h"
#include "CATCmdContainer.h"
#include "CATCreateWorkshop.h"
#include "CATUnicodeString.h"
#include "CATI18NTypes.h"  // For CATUC2Bytes

// Implement the class
// General workshop add-ins are data extensions of a late type registered in
// the interface dictionary. They are not standalone implementation classes.
CATImplementClass(PartNumberMenuAddin,
                  DataExtension,
                  CATBaseUnknown,
                  PartNumberMenuGeneralWksAddin);

// Expose CATIAfrGeneralWksAddin through the CAA TIE mechanism.  The previous
// custom factory was not queried by the CATIA component runtime.
TIE_CATIAfrGeneralWksAddin(PartNumberMenuAddin);

MacDeclareHeader(PartNumberMenuHeader);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
PartNumberMenuAddin::PartNumberMenuAddin()
{
    PartNumberMenuLog("ADDIN", "constructor this=%p", this);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
PartNumberMenuAddin::~PartNumberMenuAddin()
{
    PartNumberMenuLog("ADDIN", "destructor this=%p", this);
}

//-----------------------------------------------------------------------------
// CreateCommands - Instantiate command headers
//-----------------------------------------------------------------------------
void PartNumberMenuAddin::CreateCommands()
{
    PartNumberMenuLog("ADDIN", "CreateCommands entered");
    // Create the command header
    // Parameters: HeaderID, LoadName (DLL), ClassName, Argument
    CATCommandHeader* header = new PartNumberMenuHeader("PartNumberMenuCmd",
                                                    "PartNumberMenuModule",
                                                    "PartNumberMenuCmd",
                                                    (void*) NULL);
    CATCommandHeader* aboutHeader = new PartNumberMenuHeader("PartNumberMenuAboutCmd",
                                                    "PartNumberMenuModule",
                                                    "PartNumberMenuAboutCmd",
                                                    (void*) NULL);
    PartNumberMenuLog("ADDIN", "CreateCommands completed header=%p aboutheader=%p",
                      header, aboutHeader);
}

CATCmdContainer* PartNumberMenuAddin::CreateToolbars()
{
    PartNumberMenuLog("ADDIN", "CreateToolbars/CreateMenus entered");

    // Addin root container and the addin's menu bar container.
    NewAccess(CATCmdContainer, pAddinRoot, PartNumberMenuAddinRoot);
    NewAccess(CATCmdContainer, pMenuBar, PartNumberMenuMenuBar);
    PartNumberMenuLog("ADDIN", "Step1: Created root=%p menubar=%p", pAddinRoot, pMenuBar);

    // ---- Top-level menu (NewBOM) ----
    NewAccess(CATCmdContainer, pTopMenu, PartNumberMenuTopMnu);
    SetAccessChild(pMenuBar, pTopMenu);
    {
        CATUnicodeString title;
        title = "NewBOM";
        pTopMenu->SetTitle(title);
        PartNumberMenuLog("ADDIN", "TopMenu title set: length=%d", title.GetLengthInChar());
    }
    PartNumberMenuLog("ADDIN", "Step2: Created TopMenu container=%p", pTopMenu);

    // ---- Submenu (编号管理) ----
    // UCS-2 code points: 编=0x7F16, 号=0x53F7, 管=0x7BA1, 理=0x7406
    NewAccess(CATCmdContainer, pNumberMgtMenu, PartNumberMenuNumberMgtMnu);
    SetAccessChild(pTopMenu, pNumberMgtMenu);
    {
        // Use BuildFromUCChar for reliable Unicode string construction
        static const CATUC2Bytes titleChars[] = {0x7F16, 0x53F7, 0x7BA1, 0x7406, 0};  // "编号管理" + null
        CATUnicodeString title;
        title.BuildFromUCChar(titleChars, 4);
        pNumberMgtMenu->SetTitle(title);
        PartNumberMenuLog("ADDIN", "NumberMgtMenu title set: length=%d", title.GetLengthInChar());
    }
    PartNumberMenuLog("ADDIN", "Step3: Created NumberMgtMenu container=%p", pNumberMgtMenu);

    //      Command item (取新编号) -> reuses existing PartNumberMenuCmd
    NewAccess(CATCmdStarter, pGetNumberStarter, PartNumberMenuGetNumberStr);
    SetAccessCommand(pGetNumberStarter, "PartNumberMenuCmd");
    SetAccessChild(pNumberMgtMenu, pGetNumberStarter);
    PartNumberMenuLog("ADDIN", "Step4: Created GetNumberStarter=%p command=PartNumberMenuCmd", pGetNumberStarter);

    // ---- Submenu (工具) ----
    // UCS-2 code points: 工=0x5DE5, 具=0x5177
    NewAccess(CATCmdContainer, pToolsMenu, PartNumberMenuToolsMnu);
    SetAccessNext(pNumberMgtMenu, pToolsMenu);
    {
        // Use BuildFromUCChar for reliable Unicode string construction
        static const CATUC2Bytes titleChars[] = {0x5DE5, 0x5177, 0};  // "工具" + null
        CATUnicodeString title;
        title.BuildFromUCChar(titleChars, 2);
        pToolsMenu->SetTitle(title);
        PartNumberMenuLog("ADDIN", "ToolsMenu title set: length=%d", title.GetLengthInChar());
    }
    PartNumberMenuLog("ADDIN", "Step5: Created ToolsMenu container=%p", pToolsMenu);

    //      Command item (关于本插件) -> PartNumberMenuAboutCmd
    NewAccess(CATCmdStarter, pAboutStarter, PartNumberMenuAboutStr);
    SetAccessCommand(pAboutStarter, "PartNumberMenuAboutCmd");
    SetAccessChild(pToolsMenu, pAboutStarter);
    PartNumberMenuLog("ADDIN", "Step6: Created AboutStarter=%p command=PartNumberMenuAboutCmd", pAboutStarter);

    // Attach the menu bar to the CATIA frame.
    SetAddinMenu(pAddinRoot, pMenuBar);
    PartNumberMenuLog("ADDIN", "Step7: SetAddinMenu called root=%p menubar=%p", pAddinRoot, pMenuBar);

    PartNumberMenuLog("ADDIN",
                      "CreateToolbars completed root=%p menubar=%p top=%p numbermgt=%p tools=%p",
                      pAddinRoot, pMenuBar, pTopMenu, pNumberMgtMenu, pToolsMenu);
    return pAddinRoot;
}
