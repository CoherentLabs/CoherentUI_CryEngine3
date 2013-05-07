; CoherentUI Plugin Installer

!include "MUI2.nsh"
!include "Sections.nsh"

##################################
XPStyle on

!define VERSION "1.0.0.0"
Name "CoherentUI Plugin ${VERSION} for CryEngine"

; The file to write
OutFile "Plugin_CoherentUI_${VERSION}.exe"

; Default Installdir
InstallDir "C:\CryENGINE_PC\"

; Request application privileges for Windows Vista/7
RequestExecutionLevel user

!define FILES_ROOT ".."

; MUI Settings
!define MUI_ABORTWARNING

!define MUI_ICON "${FILES_ROOT}\..\Plugin_SDK\images\logos\PluginWizard.ico"
!define MUI_UNICON "${FILES_ROOT}\..\Plugin_SDK\images\logos\PluginWizard.ico"

;!define MUI_HEADERIMAGE
;!define MUI_HEADERIMAGE_BITMAP ""
;!define MUI_HEADERIMAGE_BITMAP_NOSTRETCH

###################################

; License page
!insertmacro MUI_PAGE_LICENSE "${FILES_ROOT}\license.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!define MUI_DIRECTORYPAGE_VARIABLE $INSTDIR
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "IsValidCEInstallation"
!insertmacro MUI_PAGE_DIRECTORY

; Install files page
!insertmacro MUI_PAGE_INSTFILES

###################################

; Sections
Section "CoherentUI" SEC_PLUGIN
    SectionIn RO
        
    SetOutPath "$INSTDIR\Bin32\Plugins\CoherentUI"
    ;File "${FILES_ROOT}\..\..\Bin32\Plugins\CoherentUI\Plugin_CoherentUI.dll"
	File "${FILES_ROOT}\..\..\Bin32\Plugins\CoherentUI\*.dll"

    SetOutPath "$INSTDIR\Bin64\Plugins\CoherentUI"
    ;File "${FILES_ROOT}\..\..\Bin64\Plugins\CoherentUI\Plugin_CoherentUI.dll"
    File "${FILES_ROOT}\..\..\Bin64\Plugins\CoherentUI\*.dll"

	; Standard Code directory also used in git repo
    SetOutPath "$INSTDIR\Code\Plugin_CoherentUI"
    File /nonfatal "${FILES_ROOT}\*.txt"
    File /nonfatal "${FILES_ROOT}\*.md"
    File /nonfatal "${FILES_ROOT}\*.markdown"

	Call ShowChangelog
SectionEnd

Section "Interface Headers" SEC_INTERFACE
    SetOutPath "$INSTDIR\Code\Plugin_CoherentUI\inc"
    File /r "${FILES_ROOT}\inc\"
SectionEnd

####################################

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_PLUGIN} "Redistributables loaded by the plugin manager."
    !insertmacro MUI_DESCRIPTION_TEXT ${SEC_INTERFACE} "Interfaces required to use the plugin from C++."
!insertmacro MUI_FUNCTION_DESCRIPTION_END


!insertmacro MUI_LANGUAGE "English"

; Set Fileinfos
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "CoherentUI Plugin"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (c) 2013, The authors of the CoherentUI Plugin project"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "CoherentUI Plugin for CryEngine"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}"
VIProductVersion "${VERSION}"

####################################

; Show changelog
Function "ShowChangelog"
	Exec 'notepad "$INSTDIR\Code\Plugin_CoherentUI\changelog.md"'
FunctionEnd

; Custom functions
Function "IsValidCEInstallation"
	IfFileExists "$INSTDIR\Bin32\Plugins\Plugin_Manager.dll" cont
		MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND \
			"The path is not a CryEngine installation or the Plugin SDK is not installed:$\n\
			$INSTDIR $\n$\n\
			The Plugin will NOT work,$\ncontinue anyways?" \
			IDOK cont1
		Abort
    cont:
	cont1:
FunctionEnd
