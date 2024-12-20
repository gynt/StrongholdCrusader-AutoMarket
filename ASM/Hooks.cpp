#include "pch.h"

#include "ASM/Hooks.h"
#include "ASM/Writer.h"

namespace ASM
{

bool Hook(LPVOID from, SIZE_T len, void(__cdecl* f)(HookRegisters), LPVOID to)
{
    HANDLE hProcess = GetCurrentProcess();

    // Read original code
    std::vector<char> originalCode(len);
    if (!ReadProcessMemory(hProcess, from, originalCode.data(), len, nullptr))
    {
        return false;
    }

    // Reserve code cave
    SIZE_T caveSize = 1 + 5 + 1 + len + 5;
    LPVOID cavePtr = VirtualAllocEx(hProcess, NULL, caveSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!cavePtr)
    {
        return false;
    }

    if (to == NULL)
    {
        to = (LPVOID)((intptr_t)from + (intptr_t)len);
    }

    // Write code cave
    Writer cave(caveSize, cavePtr);
    cave << '\x60'; // pushad
    cave << '\xE8'; cave.PushAddrLong(f); // call
    cave << '\x61'; // popad
    cave.Push(originalCode.begin(), originalCode.end()); // append original code
    cave << '\xE9'; cave.PushAddrLong(to); // jmp back
    if (WriteProcessMemory(hProcess, cavePtr, cave.GetData(), cave.GetLength(), nullptr))
    {
        // Write hook
        Writer hook(len, from);
        hook << '\xE9'; hook.PushAddrLong(cavePtr);
        if (WriteProcessMemory(hProcess, from, hook.GetData(), hook.GetLength(), nullptr))
        {
            return true;
        }
    }
    VirtualFreeEx(hProcess, cavePtr, 0, MEM_RELEASE);
    return false;
}

}
