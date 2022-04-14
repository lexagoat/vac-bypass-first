#include "common.h"
#include "hook.h"
#include "util.h"
#include "vac.h"

#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

unsigned int init(void)
{
	while (!GetModuleHandleA("steamservice.dll"))
		Sleep(100);

	char filename[MAX_PATH];
	GetModuleFileName(NULL, filename, MAX_PATH);

	if (strstr(filename, "SteamService"))
		running_as_service = true;

	if (!running_as_service) {
		AllocConsole();
		struct _iobuf* stream;
		freopen_s(&stream, "CONOUT$", "w", stdout);
	}

	vac_funcs.get_entrypoint = (get_entrypoint_t)sigscan("steamservice.dll", "\x55\x8b\xec\x83\xec\x00\x53\x56\x8b\x75\x00\x8b\xd9\x83\x7e", "xxxxx?xxxx?xxxx");
	vac_funcs.unload_module = (unload_module_t)sigscan("steamservice.dll", "\x55\x8b\xec\x81\xec\x00\x00\x00\x00\x53\x56\x8b\x75\x00\x57\x8b\x7e", "xxxxx????xxxx?xxx");

	hooks.get_entrypoint = hook_create(vac_funcs.get_entrypoint, hk_get_entrypoint);
	hook_install(hooks.get_entrypoint);

	if (running_as_service) {
		DWORD ret;
		WTSSendMessageA(WTS_CURRENT_SERVER_HANDLE, 1, "info", 4, "successfully loaded", 19, MB_TOPMOST | MB_SETFOREGROUND, 60000, &ret, false);
	}

	log_debug("loaded");
	return EXIT_SUCCESS;
}

BOOL CALLBACK DllMain(HMODULE module, DWORD reason_for_call, LPVOID _)
{
	if (reason_for_call == DLL_PROCESS_ATTACH)
		CreateThread(0, THREAD_ALL_ACCESS, (LPTHREAD_START_ROUTINE)init, module, 0, 0);

	return TRUE;
}