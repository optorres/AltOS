!addplugindir Instdrv/NSIS/Plugins

Name "Altus Metrum Installer"

OutFile "Altos-Windows.exe"

; Default install directory
InstallDir "$PROGRAMFILES\AltusMetrum"

; Tell the installer where to re-install a new version
InstallDirRegKey HKLM "Software\AltusMetrum" "Install_Dir"

LicenseText "GNU General Public License Version 2"
LicenseData "../../COPYING"

; Need admin privs for Vista or Win7
RequestExecutionLevel admin

ShowInstDetails Show

ComponentText "Altus Metrum Software and Driver Installer"

; Pages to present

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

; And the stuff to install

Section "Install Driver" InstDriver
	InstDrv::InitDriverSetup /NOUNLOAD {4D36E96D-E325-11CE-BFC1-08002BE10318} "Altus Metrum"
	Pop $0
	DetailPrint "InitDriverSetup: $0"

	InstDrv::DeleteOemInfFiles /NOUNLOAD
	InstDrv::CreateDevice /NOUNLOAD
	SetOutPath $TEMP
	File "../../telemetrum.inf"
	InstDrv::InstallDriver /NOUNLOAD "$TEMP\telemetrum.inf"
SectionEnd

Section "AltosUI Application"
	SetOutPath $INSTDIR

	File "windows/AltOS/*.jar"
	File "windows/AltOS/*.dll"

	File "windows/AltOS/*.ico"

	CreateShortCut "$SMPROGRAMS\AltusMetrum.lnk" "$INSTDIR\altosui.jar" "" "$INSTDIR\altus-metrum.ico"
SectionEnd

Section "AltosUI Desktop Shortcut"
	CreateShortCut "$DESKTOP\AltusMetrum.lnk" "$INSTDIR\altosui.jar"  "" "$INSTDIR\altus-metrum.ico"
SectionEnd

Section "TeleMetrum and TeleDongle Firmware"

	SetOutPath $INSTDIR

	File "windows/AltOS/telemetrum-v1.0.ihx"
	File "windows/AltOS/teledongle-v0.2.ihx"

SectionEnd

Section "Uninstaller"

	; Deal with the uninstaller
	
	SetOutPath $INSTDIR

	; Write the install path to the registry
	WriteRegStr HKLM SOFTWARE\AltusMetrum "Install_Dir" "$INSTDIR"
	
	; Write the uninstall keys for windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AltusMetrum" "DisplayName" "Altus Metrum"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AltusMetrum" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AltusMetrum" "NoModify" "1"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AltusMetrum" "NoRepair" "1"

	WriteUninstaller "uninstall.exe"
SectionEnd

Section "Uninstall"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AltusMetrum"
	DeleteRegKey HKLM "Software\AltusMetrum"

	Delete "$INSTDIR\*.*"
	RMDir "$INSTDIR"

	; Remove devices
	InstDrv::InitDriverSetup /NOUNLOAD {4D36E96D-E325-11CE-BFC1-08002BE10318} "Altus Metrum"
	InstDrv::DeleteOemInfFiles /NOUNLOAD
	InstDrv::RemoveAllDevices

	; Remove shortcuts, if any
	Delete "$SMPROGRAMS\AltusMetrum.lnk"
	Delete "$DESKTOP\AltusMetrum.lnk"
SectionEnd
