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

local dll

local addresses = {
  Escape = { address = core.AOBScan("A1 ? ? ? ? 3B C5 74 C1"), size = 5, to = nil},
  Update = { address = core.AOBScan("39 ? ? ? ? ? 74 15 FF D7"), size = 6, to = nil},
  -- TODO: all of them
  SetIngameStatus = {},
  Mouse = {},
  SystemKey = {},
}

return {

    enable = function(self, config)
      dll = require('crusaderautomarket.dll')

      dll.setConfig({
        file = config.file or "ucp-automarket.json",
      })
      dll.setAddresses({
        SellResource = core.AOBScan("53 8B 5C 24 0C 56 8B 74 24 0C 57"),
        BuyResource = core.AOBScan("53 8B 5C 24 10 55 8B 6C 24 10 56 8B 74 24 10"),
        GetResourceCost = core.AOBScan("8B 44 24 08 8B 8C C1 1C 1F 05 00"),
        GetResourceValue = core.AOBScan("8B 44 24 08 8B 8C C1 20 1F 05 00"),
        GetResourceSpace = core.AOBScan("83 EC 0C 55 8B 6C 24 18 56"),
      })
      if dll.initialize() == false then
        log(ERROR, 'could not initialize auto market dll')
      end

      -- Do hooks now here
      local dllAddresses = dll.getAddresses()

      hook(addresses.Escape.address, addresses.Escape.size, dllAddresses.EscapeCallback, addresses.Escape.to)
      hook(addresses.Update.address, addresses.Update.size, dllAddresses.UpdateCallback, addresses.Update.to)
      -- TODO: all of them

      core.writeCode(core.AOBScan("0F ? ? ? ? ? 0F BF 86 E6 00 00 00"), {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, })
    end,

    disable = function(self, config)

    end,
}
