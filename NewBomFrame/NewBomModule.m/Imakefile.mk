# =============================================================================
# Imakefile.mk
# CATIA CAA Module Build Configuration - Define Compile and Link Parameters
# =============================================================================
#
# What is Imakefile?
# Imakefile.mk is CATIA RADE build system config file, similar to Makefile.
# It tells RADE how to compile and link our module.
#
# Key Configuration Items:
# BUILT_OBJECT_TYPE: Define output type (library, DLL, executable, etc.)
# LINK_WITH: Define libraries to link
#
# Build Process:
# RADE build system will:
#   1. Read IdentityCard.h for framework dependencies
#   2. Read Imakefile.mk for module config
#   3. Compile all .cpp files
#   4. Link to generate DLL
#
# =============================================================================

# -----------------------------------------------------------------------------
# BUILT_OBJECT_TYPE - Output Type Definition
# -----------------------------------------------------------------------------
# Define what type of output this module generates.
#
# Possible Values:
#   SHARED LIBRARY: Shared library (DLL) - most common
#   STATIC LIBRARY: Static library (.lib)
#   EXECUTABLE:     Executable (.exe)
#   HEADERLIBRARY:  Header-only library
#
# SHARED LIBRARY Explanation:
# We choose SHARED LIBRARY because:
#   - CATIA plugins must be DLLs (dynamic loading)
#   - DLL can be loaded/unloaded by CATIA at runtime
#   - DLL allows multiple CATIA processes to share code
#
# Output File Name:
# Module name NewBomModule.m determines DLL name:
#   Output: win_b64/code/bin/NewBomModule.dll
#   Library: win_b64/code/lib/NewBomModule.lib
#
BUILT_OBJECT_TYPE=SHARED LIBRARY

# -----------------------------------------------------------------------------
# LINK_WITH - Link Dependencies
# -----------------------------------------------------------------------------
# Define CATIA libraries needed for compile and link.
# These correspond to framework dependencies in IdentityCard.h.
#
# Format:
# LINK_WITH=Lib1 \
#           Lib2 \
#           Lib3
#
# Each Library's Purpose:
#
# CATApplicationFrame - Application Frame Library
#   Function:
#     - Main window management
#     - Command system (CATCommand, CATCommandHeader)
#     - Menu system (CATCmdContainer, CATCmdStarter)
#     - Workshop addin (CATIAfrGeneralWksAddin)
#   We need:
#     - NewBomAddin uses this for menu implementation
#     - NewBomCmd and NewBomAboutCmd use this as command base
#
LINK_WITH=CATApplicationFrame \

# JS0GROUP - CATIA System Core Library
#   Function:
#     - Reference counting and lifecycle management
#     - Object creation and destruction
#     - Basic type definitions
#   We need:
#     - CATBaseUnknown base class implementation
#     - CATImplementClass macro implementation
          JS0GROUP            \

# JS0FM - CATIA String and Collection Library
#   Function:
#     - CATUnicodeString implementation
#     - CATString implementation
#     - String manipulation functions
#   We need:
#     - Menu title setting (BuildFromUCChar)
#     - Text display in dialogs
          JS0FM               \

# DI0PANV2 - CATIA Dialog Library
#   Function:
#     - CATDlgDialog implementation
#     - Various controls (Label, Editor, Combo, PushButton)
#     - CATDlgNotify info box
#     - Dialog layout system
#   We need:
#     - NewBomDlg dialog
#     - CATDlgNotify about dialog
#     - Dialog controls and layout
          DI0PANV2

# =============================================================================
# Build Output Explanation
# =============================================================================
#
# After successful compilation, RADE generates:
#
# win_b64/
#   |- code/
#   |   |- bin/
#   |   |   |- NewBomModule.dll  <- Our plugin DLL
#   |   |- lib/
#   |   |   |- NewBomModule.lib  <- Link library (for other modules)
#   |   |- dictionary/
#   |   |   |- NewBomFrame.dico  <- Dictionary file (copied from CNext/)
#   |- resources/
#       |- msgcatalog/
#           |- NewBomModule.CATNls <- Menu title resources
#           |- ...
#
# When CATIA loads plugin:
#   1. Read dictionary .dico files
#   2. Find interface registration info
#   3. Load DLL in code/bin
#   4. Create Addin instance
#   5. Call CreateCommands and CreateToolbars
#
# =============================================================================