# CATIA V5R21 Hello World Add-in

Clean CAA RADE workspace for the CATIA V5R21 64-bit plug-in.

## Layout

- `PartNumberMenuFrame/IdentityCard`: framework prerequisites
- `PartNumberMenuFrame/PartNumberMenuModule.m`: CAA shared-library module
- `PartNumberMenuFrame/CNext/code/dictionary`: interface dictionary
- `PartNumberMenuFrame/CNext/resources/msgcatalog`: command and toolbar resources

Build and runtime scripts will be maintained in this workspace. The previous
`D:\work\catia_plugin` directory is retained only as migration history.

## Build And Deploy

Run `build.bat` to build the `win_b64` module and generate the official RADE
runtime view. Then run `deploy.bat` from an Administrator command prompt to
register that runtime view in the CATIA V5R21 environment.

Start CATIA with `launch_catia.bat`. This preserves the normal P3 resource
paths while enabling the same RADE runtime-view discovery used by `mkrun`.
