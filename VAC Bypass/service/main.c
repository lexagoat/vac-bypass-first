#include <Windows.h>
#include <Psapi.h>
#include <Sddl.h>
#include <Wtsapi32.h>
#include <stdbool.h>
#include <stdio.h>
#include <tlhelp32.h>

#pragma comment(lib, "Wtsapi32.lib")

unsigned int get_process_id(const char* process_name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 process_entry;
	process_entry.dwSize = sizeof(PROCESSENTRY32);

	while (Process32Next(snapshot, &process_entry))
		if (!strcmp(process_name, process_entry.szExeFile))
			return process_entry.th32ProcessID;

	return NULL;
}

void send_message(const char* msg)
{
	DWORD ret;
	WTSSendMessageA(WTS_CURRENT_SERVER_HANDLE, 1, (LPSTR)"info", 4, (LPSTR)msg, strlen(msg), MB_TOPMOST | MB_SETFOREGROUND, 60000, &ret, false);
}

int main(void)
{
	char username[64];
	DWORD len = 64;
	GetUserNameA(username, &len);

	if (strcmp(username, "SYSTEM")) {
		send_message("you must run loader.exe");
		return 1;
	}

	char msg[512];

	
	unsigned int pid = get_process_id("SteamService.exe");
	if (!pid) {
		send_message("could not find process\n");
		Sleep(2000);
		return 1;
	}

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!handle) {
		sprintf(msg, "could not open access handle (0x%x)\n", GetLastError());
		send_message(msg);

		Sleep(2000);
		return 1;
	}

	const char* path = "c:\\rape.dll";
	void* string = VirtualAllocEx(handle, NULL, strlen(path), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!WriteProcessMemory(handle, string, path, strlen(path), NULL)) {
		sprintf(msg, "could not write memory (0x%x)\n", GetLastError());
		send_message(msg);

		Sleep(2000);
		return 1;
	}

	HANDLE thread = CreateRemoteThread(handle, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), string, NULL, NULL);
	if (!thread) {
		sprintf(msg, "could not create thread (0x%x)\n", GetLastError());
		send_message(msg);

		Sleep(4000);
		return 1;
	}

	
	return 0;
}
