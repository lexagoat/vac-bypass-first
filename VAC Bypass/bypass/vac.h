#include <windows.h>

struct vac_mapped_module {
	unsigned int unk;
	unsigned int base_addr;
	struct _IMAGE_NT_HEADERS* nt_header;
};

struct vac_module_info
{
	unsigned int crc;
	unsigned int handle;
	struct vac_mapped_module* mapped_module;
	unsigned int run_func;
	unsigned int result;
	unsigned int size;
	unsigned int raw_module;
};

typedef void(__stdcall* unload_module_t)(struct vac_module_info*);
typedef bool(__stdcall* get_entrypoint_t)(struct vac_module_info*, char);

struct {
	unload_module_t unload_module;
	get_entrypoint_t get_entrypoint;
} vac_funcs;

bool __stdcall hk_get_entrypoint(struct vac_module_info* mi, char flags);

enum {

	ERROR_NONE = 0,
	ERROR_ALREADY_LOADED = 1,
	ERROR_CANT_GET_PATH = 19,
	ERROR_CANT_WRITE_FILE = 21,
	ERROR_CANT_ALLOC = 22,
	ERROR_NO_ENTRYPOINT_LOADLIB = 23,
	ERROR_NO_ENTRYPOINT = 25
};