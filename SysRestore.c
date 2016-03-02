/*
* SysRestore plug-in for NSIS
*
* Written by Jason Ross aka JasonFriday13 on the forums.
*
* SysRestore.c
*/

/*  Include the windows.h file, and the NSIS plugin header. */
#include <windows.h>
#include <srrestoreptapi.h>
#include "nsis\pluginapi.h" /* This means NSIS 2.42 or higher is required. */

/*  Handle for the plugin. */
HANDLE hInstance;

/*  Variables for the System Restore API */
PRESTOREPOINTINFO pRestPtInfo;
STATEMGRSTATUS SMgrStatus;
INT64 SequenceNumber = 0;

/*  Internal variables. */
BOOL RestorePointStarted = FALSE, Initialized = FALSE;

void Destroy(void)
{
  if (pRestPtInfo) GlobalFree(pRestPtInfo);
}

/*  Our callback function so that our dll stays loaded. */
UINT_PTR __cdecl NSISPluginCallback(enum NSPIM Event) 
{
  if (Event == NSPIM_GUIUNLOAD)
    Destroy();

  return 0;
}

void Initialize(extra_parameters* xp)
{
  if (!Initialized)
  {
    xp->RegisterPluginCallback(hInstance, NSISPluginCallback);
    Initialized = TRUE;
  }
}

/*  This function is common to all functions. To reduce size,
having one copy instead of two saves space (I think). */
int SetRestorePoint(void)
{
  TCHAR *pTemp;

  pTemp = GlobalAlloc(GPTR, sizeof(TCHAR)*64);
  SRSetRestorePoint(pRestPtInfo, &SMgrStatus);
  SequenceNumber = SMgrStatus.llSequenceNumber;
  wsprintf(pTemp, TEXT("%u"), SMgrStatus.nStatus);
  pushstring(pTemp);
  GlobalFree(pTemp);

  return SMgrStatus.nStatus;
}

/*  These functions must be internal to the dll,
because they use a global variable. It does
not work if these functions are exported. */
int Begin(BOOL Uninstaller)
{
  SecureZeroMemory(pRestPtInfo, sizeof(RESTOREPOINTINFO));
  SecureZeroMemory(&SMgrStatus, sizeof(STATEMGRSTATUS));

  /*  Initialize the RESTOREPOINTINFO structure. */
  pRestPtInfo->dwEventType = BEGIN_SYSTEM_CHANGE;

  /*  Notify the system that changes are about to be made.
  An application is to be installed (or uninstalled). */
  if (Uninstaller)
    pRestPtInfo->dwRestorePtType = APPLICATION_UNINSTALL;
  else
    pRestPtInfo->dwRestorePtType = APPLICATION_INSTALL;

  /*  Set RestPtInfo.llSequenceNumber. */
  pRestPtInfo->llSequenceNumber = 0;

  /*  String to be displayed by System Restore for this restore point. */
  popstring(pRestPtInfo->szDescription);

  /*  Notify the system that changes are to be made and that
  the beginning of the restore point should be marked. */
  return SetRestorePoint();
}

int Finish(void)
{
  /*  Re-initialize the RESTOREPOINTINFO structure to notify the 
  system that the operation is finished. */
  pRestPtInfo->dwEventType = END_SYSTEM_CHANGE;

  /*  End the system change by returning the sequence number 
  received from the first call to SRSetRestorePoint. */
  pRestPtInfo->llSequenceNumber = SequenceNumber;

  /*  Notify the system that the operation is done and that this
  is the end of the restore point. */
  return SetRestorePoint();
}

int Remove(void)
{
  int result;

  result = SRRemoveRestorePoint((DWORD)SequenceNumber);

  if (result == ERROR_SUCCESS)
    return 1;
  else
    return 0;
}

__declspec(dllexport) void StartRestorePoint(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  EXDLL_INIT();
  Initialize(xp);

  /*  This is to prevent multiple calls to SRSetRestorePoint. */
  if (!RestorePointStarted)
  {
    if (Begin(FALSE) == ERROR_SUCCESS)
      RestorePointStarted = TRUE;
  }
  else
    pushint(1);
}

__declspec(dllexport) void StartUnRestorePoint(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  EXDLL_INIT();
  Initialize(xp);

  /*  This is to prevent multiple calls to SRSetRestorePoint. */
  if (!RestorePointStarted)
  {
    if (Begin(TRUE) == ERROR_SUCCESS)
      RestorePointStarted = TRUE;
  }
  else
    pushint(1);
}

__declspec(dllexport) void FinishRestorePoint(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  EXDLL_INIT();
  Initialize(xp);

  /*  This is to prevent multiple calls to SRSetRestorePoint. */
  if (RestorePointStarted)
  {
    if (Finish() == ERROR_SUCCESS)
      RestorePointStarted = FALSE;
  }
  else
    pushint(2);
}

void __declspec(dllexport) RemoveRestorePoint(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  EXDLL_INIT();
  Initialize(xp);

  /*  This is to prevent multiple calls to SRSetRestorePoint. */
  if (SequenceNumber)
  {	
    if (Remove())
      SequenceNumber = 0;
  }
  else
    pushint(3);
}

BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
  hInstance = hInst;

  if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    pRestPtInfo = GlobalAlloc(GPTR, sizeof(RESTOREPOINTINFO));

  if (ul_reason_for_call == DLL_PROCESS_DETACH)
    Destroy();

  return TRUE;
}