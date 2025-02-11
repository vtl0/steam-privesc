#ifndef _PATCHES_H
#define _PATCHES_H

#include <Windows.h>

int patch_near_call(BYTE* address, BYTE* dest, BYTE** original);

#endif // _PATCHES_H