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

}