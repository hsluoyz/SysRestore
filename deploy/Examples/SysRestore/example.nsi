Name "System Restore Example"
OutFile "Example.exe"
InstallDir "$PROGRAMFILES\$(^Name)\"

ShowInstDetails Show
ShowUninstDetails Show

Page Directory
Page InstFiles
UninstPage UninstConfirm
UninstPage InstFiles

Var Error

Section "install"
  SetOverWrite try
  StrCpy $Error 0

  DetailPrint "Setting System Restore point..."
  SysRestore::StartRestorePoint "Installed $(^Name)"
  Pop $0
  StrCmp $0 0 next
  StrCpy $Error $0
  DetailPrint "Error occured setting restore point, return value=|$0|"
next:
  SetOutPath "$INSTDIR"
  File "${NSISDIR}\docs\makensisw\license.txt"
  WriteUninstaller "$INSTDIR\uninst.exe"
  
  ExecShell open "$INSTDIR"

  StrCmp $Error 0 0 +4
  SysRestore::FinishRestorePoint
  Pop $0
  DetailPrint "FinishRestorePoint return value=|$0|"
  SetAutoClose False
SectionEnd

Section "un.install"
  SetOverWrite try
  StrCpy $Error 0
  
  DetailPrint "Setting System Restore point..."
  SysRestore::StartUnRestorePoint "Uninstalled $(^Name)"
  Pop $0
  StrCmp $0 0 next
  StrCpy $Error $0
  DetailPrint "Error occured setting restore point, return value=|$0|"
next:
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\uninst.exe"
  RMDir "$INSTDIR"
  
  StrCmp $Error 0 0 +4
  SysRestore::FinishRestorePoint
  Pop $0
  DetailPrint "FinishRestorePoint return value=|$0|"
  SetAutoClose False
SectionEnd