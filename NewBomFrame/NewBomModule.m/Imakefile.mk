# =============================================================================
# Imakefile.mk - CATIA CAA Module Build Configuration
# =============================================================================
# 
# BUILT_OBJECT_TYPE: Output type (SHARED LIBRARY = DLL)
# LINK_WITH: Libraries to link
#
# BUILT_OBJECT_TYPE indicates the type of build result:
#   - SHARED LIBRARY: Build a DLL that can be dynamically loaded
#   - LIBRARY: Build a static library
#   - PROGRAM: Build an executable
#
# LINK_WITH lists all CATIA frameworks/libraries needed:
#   - CATApplicationFrame: Main application, commands, menus
#   - JS0GROUP: Base object management (CATBaseUnknown)
#   - JS0FM: String handling (CATUnicodeString)
#   - DI0PANV2: Dialog system (CATDlgDialog, CATDlgTree, etc.)
#   - CATProductStructure: Product/Assembly structure (CATIProduct)
#
# =============================================================================

BUILT_OBJECT_TYPE=SHARED LIBRARY

LINK_WITH=CATApplicationFrame JS0GROUP JS0FM DI0PANV2 CATProductStructure