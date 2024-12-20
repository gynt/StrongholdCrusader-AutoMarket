#pragma once

#include <minwindef.h>

namespace ASM
{

struct HookRegisters
{
    int edi;
    int esi;
    int ebp;
    int esp;
    int ebx;
    int edx;
    int ecx;
    int eax;
};

bool Hook(LPVOID from, SIZE_T len, void(__cdecl* f)(HookRegisters), LPVOID to = NULL);

}