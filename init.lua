-- bool Hook(LPVOID from, SIZE_T len, void(__cdecl* f)(HookRegisters), LPVOID to)
local function hook(from, len, f, to)
    -- {
    --     HANDLE hProcess = GetCurrentProcess();

    --     // Read original code
    --     std::vector<char> originalCode(len);
    --     if (!ReadProcessMemory(hProcess, from, originalCode.data(), len, nullptr))
    --     {
    --         return false;
    --     }
    local originalCode = core.readBytes(from, len)

    --     // Reserve code cave
    --     SIZE_T caveSize = 1 + 5 + 1 + len + 5;
    local caveSize = 1 + 5 + 1 + len + 5
    --     LPVOID cavePtr = VirtualAllocEx(hProcess, NULL, caveSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    local cavePtr = core.allocateCode(caveSize)
    --     if (!cavePtr)
    --     {
    --         return false;
    --     }

    --     if (to == NULL)
    --     {
    --         to = (LPVOID)((intptr_t)from + (intptr_t)len);
    --     }
    if to == nil then
        to = from + len
    end

    --     // Write code cave
    --     Writer cave(caveSize, cavePtr);
    --     cave << '\x60'; // pushad
    --     cave << '\xE8'; cave.PushAddrLong(f); // call
    --     cave << '\x61'; // popad
    --     cave.Push(originalCode.begin(), originalCode.end()); // append original code
    --     cave << '\xE9'; cave.PushAddrLong(to); // jmp back
    local cave = {
      core.AssemblyLambda([[
        pushad
        call f
        popad
      ]], {
        f = f,
      }),
      originalCode,
      core.AssemblyLambda([[
        jmp to
      ]], {
        to = to,
      })
    }
    --     if (WriteProcessMemory(hProcess, cavePtr, cave.GetData(), cave.GetLength(), nullptr))
    --     {
    core.writeCode(cavePtr, cave, true)
    --         // Write hook
    --         Writer hook(len, from);
    --         hook << '\xE9'; hook.PushAddrLong(cavePtr);
    local hook = {
      core.AssemblyLambda([[
        jmp cavePtr
      ]], {
        cavePtr = cavePtr,
      })
    }
    --         if (WriteProcessMemory(hProcess, from, hook.GetData(), hook.GetLength(), nullptr))
    --         {
    core.writeCode(from, hook, true)
    --             return true;
    --         }
    --     }
    --     VirtualFreeEx(hProcess, cavePtr, 0, MEM_RELEASE);
    --     return false;
    -- }
end

return {

    enable = function(self, config)
        core.writeCode()
    end,

    disable = function(self, config)

    end,
}
