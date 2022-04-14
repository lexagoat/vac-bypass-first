#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <Psapi.h>
#include <winternl.h>
#include <stdbool.h>

#pragma once 
struct {
	struct detour* get_entrypoint;
} hooks;