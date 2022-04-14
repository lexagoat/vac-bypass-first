#include "common.h"
#include "hook.h"
#include "util.h"
#include "vac.h"

bool __stdcall hk_get_entrypoint(struct vac_module_info* mi, char flags)
{
	hook_restore(hooks.get_entrypoint, vac_funcs.get_entrypoint(mi, flags));

	static unsigned short whitelist[] = { 0x5c00, 0x7e00, 0x4600, 0x6a00 };
	unsigned int identifier = mi->mapped_module->nt_header->OptionalHeader.SizeOfCode;

	for (int i = 0; i < sizeof(whitelist) / sizeof(unsigned short); i++) {
		if (whitelist[i] == identifier) {
			log_debug("ran 0x%x (%x)", mi->crc, identifier);
			return true;
		}
	}

	log_debug("blocked 0x%x (%x)", mi->crc, identifier);
	vac_funcs.unload_module(mi);
	mi->run_func = 0;
	mi->result = 11;;
	return false;
}
