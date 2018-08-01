#include <windows.h>
#include <stdio.h>

#include "dllInjector.h"
#include "objbase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _BOOL {
	false, true
} bool;

#define FALSE 0
#define TRUE  1

void Wait(DWORD dwSeconds)
{
	DWORD dwStart = GetTickCount();
	while ((GetTickCount()-dwStart) <= dwSeconds);
}

void testfunc(void)
{
	bool bShow = false;
	MessageBox(NULL, "This was a test!", "It works!", MB_OK | MB_ICONINFORMATION);
	while (!GetAsyncKeyState(VK_F2)) {
		if (GetAsyncKeyState(VK_F3)) {
			ShowWindow(GetConsoleWindow(), bShow);
			bShow = !bShow;
		}
		Wait(500);
	}
}

void PRS_Init(void)
{
	DWORD dwProcessId;

	printf("DLL: This is the custom PRS_init function!\n");
	dwProcessId = GetCurrentProcessId();

	testfunc();
}

object InitFuncProto = {
	.InitFunc = PRS_Init
};

DWORD WINAPI DoStuff(LPVOID lpParam)
{
	initFunc = NEW(InitFunc);
	if(initFunc == NULL)
		return E_FAIL;
	initFunc->_(InitFunc)();
	return S_OK;
}

__declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			DWORD dwThreadId;
			DisableThreadLibraryCalls(hDLL);
			OutputDebugString("DLL: Process_Attach()\n");
			CreateThread(NULL, 0, DoStuff, NULL, 0, &dwThreadId);
		break;

		case DLL_PROCESS_DETACH:
			if(initFunc != NULL) initFunc->_(Destroy)(initFunc);
			OutputDebugString("DLL: Process_Detach()\n");
			Sleep(2500);
		break;
	}

	return TRUE;
}

#ifdef __cplusplus
}
#endif