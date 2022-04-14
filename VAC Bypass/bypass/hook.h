#include "common.h"

struct detour {
	void* address;
	void* detour;
	bool applied;
	unsigned char* original_bytes;
	void* trampoline;
};

static void safe_copy(void* dest, void* bytes, int size)
{
	unsigned long protect;
	VirtualProtect(dest, 0x05, PAGE_EXECUTE_READWRITE, &protect);
	memcpy(dest, bytes, size);
	VirtualProtect(dest, 0x05, protect, &protect);
}

static void* hook_trampoline_get(void)
{
	static void* trampoline_base;
	if (trampoline_base == NULL)
		trampoline_base = VirtualAlloc(NULL, 0x200, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	static int allocations = 0;
	void* trampoline = (void*)((int)trampoline_base + allocations * 0x5);
	allocations++;

	return trampoline;
}

static struct detour* hook_create(void* address, void* detour)
{
	struct detour* hook = (struct detour*)malloc(sizeof(struct detour));
	hook->address = address;
	hook->detour = detour;
	hook->applied = false;
	hook->trampoline = 0;

	hook->original_bytes = (unsigned char*)malloc(0x5);
	memset(hook->original_bytes, 0, 0x5);

	return hook;
}

static void hook_remove(struct detour* hook)
{
	if (hook == NULL || hook->applied == false)
		return;

	safe_copy(hook->address, hook->original_bytes, 0x5);
	memset(hook->original_bytes, 0, 0x5);

	hook->applied = false;
}

static void hook_install(struct detour* hook)
{
	if (!hook || hook->applied)
		return;

	unsigned char jmp[5] = {
		0xE9, 0x00, 0x00, 0x00, 0x00, 
	};

	if (hook->trampoline == NULL) {
		hook->trampoline = hook_trampoline_get();

		*(int*)(jmp + 0x1) = ((int)hook->detour) - ((int)hook->trampoline) - 0x5;
		safe_copy(hook->trampoline, jmp, 5);
	}

	memcpy(hook->original_bytes, hook->address, 5);

	*(int*)(jmp + 0x1) = ((int)hook->trampoline - (int)hook->address - 0x5);

	safe_copy(hook->address, jmp, 5);
	hook->applied = true;
}

#define hook_restore(__hook, __do) \
	hook_remove(__hook);           \
	__do;                          \
	hook_install(__hook);