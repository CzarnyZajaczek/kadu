; -*- coding: cp-1250 -*-

; Comment out when testing this script. It makes installer generation very slow.
SetCompressor /SOLID /FINAL lzma

!define PRODUCT_NAME "Kadu"
!define PRODUCT_VERSION "0.11.0-beta1a"
!define PRODUCT_BINARY_FILE_NAME "kadu.exe"
!define PRODUCT_UNINSTALLER_FILE_NAME "uninst.exe"
!define PRODUCT_PUBLISHER "Kadu Team"
!define PRODUCT_WEB_SITE "http://www.kadu.im"
!define PRODUCT_REGISTRY_ROOT "HKLM"
!define PRODUCT_REGISTRY_KEY_DIRECTORY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_BINARY_FILE_NAME}"
!define PRODUCT_REGISTRY_KEY_UNINSTALL "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_REGISTRY_STARTMENU_VALUE "NSIS:StartMenuDir"
!define PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR "C:\kadu\git-install"
!define PRODUCT_INSTALLATION_EXTERNAL_PLUGINS_SOURCE_DIR "C:\kadu\external-plugins\install"

; General Settings
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME}-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_DIRECTORY}" ""
RequestExecutionLevel admin
XPStyle on
; Requires unreleased NSIS version
; TargetMinimalOS 5.0

; Modern UI
!include "MUI2.nsh"
!include "LogicLib.nsh"

; MUI Global Settings
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; MUI Language Settings
!define MUI_LANGDLL_REGISTRY_ROOT ${PRODUCT_REGISTRY_ROOT}
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_REGISTRY_KEY_DIRECTORY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "InstallerLanguage"

!insertmacro MUI_RESERVEFILE_LANGDLL

; MUI Installer Settings
!define MUI_ABORTWARNING
!define MUI_ABORTWARNING_CANCEL_DEFAULT

!insertmacro MUI_PAGE_WELCOME

!insertmacro MUI_PAGE_LICENSE "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\COPYING"

!insertmacro MUI_PAGE_COMPONENTS

!insertmacro MUI_PAGE_DIRECTORY

Var START_MENU_FOLDER
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_REGISTRY_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_REGISTRY_KEY_UNINSTALL}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_REGISTRY_STARTMENU_VALUE}"
!insertmacro MUI_PAGE_STARTMENU "Application" $START_MENU_FOLDER

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\${PRODUCT_BINARY_FILE_NAME}"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT
!insertmacro MUI_PAGE_FINISH

; MUI Uninstaller Settings
!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

; MUI Languages
!insertmacro MUI_LANGUAGE "English" ;default
!insertmacro MUI_LANGUAGE "Polish"

; Language Strings
LangString ALREADY_INSTALLED_MESSAGE ${LANG_ENGLISH} "${PRODUCT_NAME} is already installed.$\n$\nClose ${PRODUCT_NAME} and click `OK` to remove the previous version and continue upgrade or `Cancel` to cancel this upgrade."
LangString ALREADY_INSTALLED_MESSAGE ${LANG_POLISH} "Program ${PRODUCT_NAME} jest ju� zainstalowany na tym systemie.$\n$\nZamknij program ${PRODUCT_NAME} i kliknij $\"OK$\", aby usun�� poprzedni� wersj� i kontynuowa� aktualizacj�, lub $\"Anuluj$\", aby anulowa� t� aktualizacj�."
LangString NAME_SectionOfficial ${LANG_ENGLISH} "Official Kadu IM"
LangString NAME_SectionOfficial ${LANG_POLISH} "Oficjalne Kadu"
LangString DESC_SectionOfficial ${LANG_ENGLISH} "Official Kadu IM as prepared by Kadu Team."
LangString DESC_SectionOfficial ${LANG_POLISH} "Oficjalne Kadu przygotowane przez Kadu Team."
LangString NAME_SectionExternalPlugins ${LANG_ENGLISH} "External Plugins"
LangString NAME_SectionExternalPlugins ${LANG_POLISH} "Zewn�trzne wtyczki"
LangString DESC_SectionExternalPlugins ${LANG_ENGLISH} "Additional plugins provided by external developers. They include anonymous_check, import_history, lednotify, mime_tex, networkping, nextinfo, and senthistory."
LangString DESC_SectionExternalPlugins ${LANG_POLISH} "Dodatkowe wtyczki dostarczone przez zewn�trznych tw�rc�w. Zawieraj� anonymous_check, import_history, lednotify, mime_tex, networkping, nextinfo oraz senthistory."
LangString NAME_SectionDesktopShortCut ${LANG_ENGLISH} "Desktop Shortcut"
LangString NAME_SectionDesktopShortCut ${LANG_POLISH} "Skr�t na pulpicie"
; LangString DESC_SectionDesktopShortCut ${LANG_ENGLISH} ""
; LangString DESC_SectionDesktopShortCut ${LANG_POLISH} ""

Function .onInit
  ReadRegStr $R0 ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_UNINSTALL}" "UninstallString"
  StrCmp $R0 "" done

  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION $(ALREADY_INSTALLED_MESSAGE) IDOK uninst
  Abort

;Run the uninstaller
uninst:
  !insertmacro MUI_LANGDLL_DISPLAY

  ClearErrors
  ExecWait '$R0 /S _?=$INSTDIR' ;Do not copy the uninstaller to a temp file

  IfErrors no_remove_uninstaller done
    ;You can either use Delete /REBOOTOK in the uninstaller or add some code
    ;here to remove the uninstaller. Use a registry key to check
    ;whether the user has chosen to uninstall. If you are using an uninstaller
    ;components page, make sure all sections are uninstalled.
  no_remove_uninstaller:

done:
FunctionEnd

Section $(NAME_SectionOfficial) SectionOfficial
; always selected
  SectionIn RO

  SetOverwrite on
; main files
  SetOutPath "$INSTDIR"
  File "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\*.*"
; configuration
  SetOutPath "$INSTDIR\configuration"
  File "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\configuration\*.ui"
; chat scripts
  SetOutPath "$INSTDIR\scripts"
  File "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\scripts\*.js"
; plugins
  SetOutPath "$INSTDIR\plugins"
  File /r "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\plugins\*"
; Qt plugins
  SetOutPath "$INSTDIR\qt-plugins"
  File /r "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\qt-plugins\*"
; syntax
  SetOutPath "$INSTDIR\syntax\chat"
  File  /r "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\syntax\chat\*"
  SetOutPath "$INSTDIR\syntax\infopanel"
  File "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\syntax\infopanel\*.syntax"
; emoticons
  SetOutPath "$INSTDIR\themes\emoticons"
  File /r "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\themes\emoticons\*"
; icons
  SetOutPath "$INSTDIR\themes\icons"
  File /r "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\themes\icons\*"
; sounds
  SetOutPath "$INSTDIR\themes\sounds"
  File /r "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\themes\sounds\*"
; translations
  SetOutPath "$INSTDIR\translations"
  File "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\translations\*.qm"
  File "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\translations\*.language"
; aspell dictionary
  SetOutPath "$INSTDIR\aspell"
  File /r "${PRODUCT_INSTALLATION_OFFICIAL_SOURCE_DIR}\aspell\*"

; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN "Application"
  CreateDirectory "$SMPROGRAMS\$START_MENU_FOLDER"
  CreateShortCut "$SMPROGRAMS\$START_MENU_FOLDER\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_BINARY_FILE_NAME}"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section $(NAME_SectionExternalPlugins) SectionExternalPlugins
  SetOverwrite on
  SetOutPath "$INSTDIR\plugins"
  File /r "${PRODUCT_INSTALLATION_EXTERNAL_PLUGINS_SOURCE_DIR}\*"
SectionEnd

Section $(NAME_SectionDesktopShortCut) SectionDesktopShortCut
  SetOverwrite on
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_BINARY_FILE_NAME}"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SectionOfficial} $(DESC_SectionOfficial)
  !insertmacro MUI_DESCRIPTION_TEXT ${SectionExternalPlugins} $(DESC_SectionExternalPlugins)
;  !insertmacro MUI_DESCRIPTION_TEXT ${SectionDesktopShortCut} $(DESC_SectionDesktopShortCut)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section -AdditionalIcons
  !insertmacro MUI_STARTMENU_WRITE_BEGIN "Application"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$START_MENU_FOLDER\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$START_MENU_FOLDER\Uninstall.lnk" "$INSTDIR\${PRODUCT_UNINSTALLER_FILE_NAME}"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\${PRODUCT_UNINSTALLER_FILE_NAME}"
  WriteRegStr ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_DIRECTORY}" "" "$INSTDIR\${PRODUCT_BINARY_FILE_NAME}"
  WriteRegStr ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_UNINSTALL}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_UNINSTALL}" "UninstallString" "$INSTDIR\${PRODUCT_UNINSTALLER_FILE_NAME}"
  WriteRegStr ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_UNINSTALL}" "DisplayIcon" "$INSTDIR\kadu.ico"
  WriteRegStr ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_UNINSTALL}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_UNINSTALL}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_UNINSTALL}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $START_MENU_FOLDER
  Delete "$INSTDIR\*.*"

  Delete "$SMPROGRAMS\$START_MENU_FOLDER\Uninstall.lnk"
  Delete "$SMPROGRAMS\$START_MENU_FOLDER\Website.lnk"
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\$START_MENU_FOLDER\${PRODUCT_NAME}.lnk"

  RMDir "$SMPROGRAMS\$START_MENU_FOLDER"
  RMDir /r "$INSTDIR\aspell"
  RMDir /r "$INSTDIR\configuration"
  RMDir /r "$INSTDIR\plugins"
  RMDir /r "$INSTDIR\qt-plugins"
  RMDir /r "$INSTDIR\scripts"
  RMDir /r "$INSTDIR\syntax"
  RMDir /r "$INSTDIR\themes"
  RMDir /r "$INSTDIR\translations"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_UNINSTALL}"
  DeleteRegKey ${PRODUCT_REGISTRY_ROOT} "${PRODUCT_REGISTRY_KEY_DIRECTORY}"
  SetAutoClose true
SectionEnd