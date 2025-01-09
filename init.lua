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

local addresses = {}
local function findAddresses()
  local _, playerIndex = utils.AOBExtract("A1 I(? ? ? ?) 83 ? ? ? ? ? ? ? 75 61")
  local _, playerData = utils.AOBExtract("83 ? I(? ? ? ?) ? C7 ? ? ? ? ? ? ? ? ? 74 0A C7 ? ? ? ? ? ? ? ? ? 83 ? ? ? ? ? ?")
  local _, ingameTime = utils.AOBExtract("8B ? I(? ? ? ?) 66 8B 54 24 1C")
  local _, ctrlModifier = utils.AOBExtract("39 ? I(? ? ? ?) 74 08 39 ? ? ? ? ? 75 2B")
  local _, u0 = utils.AOBExtract("B9 I(? ? ? ?) E8 ? ? ? ? E9 ? ? ? ? 8B 54 24 14 6A 00")
  local _, u1 = utils.AOBExtract("B9 I(? ? ? ?) E8 ? ? ? ? A1 ? ? ? ? 8B 54 24 20")
  local _, hWindow = utils.AOBExtract("89 ? I(? ? ? ?) FF ? ? ? ? ? E9 ? ? ? ? 8B ? ? ? ? ? 83 FA 29")
   addresses = {
    -- Data
    playerIndex = playerIndex,
    playerData = playerData,
    ingameTime = ingameTime,
    ctrlModifier = ctrlModifier,
    u0 = u0,
    u1 = u1,
    hWindow = hWindow,
    -- Functions
    -- Hopefully this Leave Game is a better place to do this code.
    LeaveGame = { address = core.AOBScan("8B 44 24 04 83 E8 16"), size = 7, to = nil},
    EnterGame = { address = core.AOBScan("55 8B EC 83 E4 F8 81 EC 84 0D 00 00"), size = 6, to = nil},
    UpdateGame = { address = core.AOBScan("39 ? ? ? ? ? 74 15 FF D7"), size = 6, to = nil},
  }
end

return {

    enable = function(self, config)
      dll = require('crusaderautomarket.dll')

      findAddresses()

      local cf = config.file or "ucp-automarket.json"
      cf = ucp.internal.resolveAliasedPath(cf)
      dll.setConfig({
        file = cf,
      })
      local _, getResourceValue = utils.AOBExtract("@(E8 ? ? ? ?) 01 ? ? ? ? ? ? 8B CE 69 C9 F4 39 00 00 01 ? ? ? ? ? 01 ? ? ? ? ? 6A 00 57 ")
      local _, getResourceCost = utils.AOBExtract("@(E8 ? ? ? ?) 53 55 56 B9 ? ? ? ? ")
      dll.setAddresses({
        SellResource = core.AOBScan("53 8B 5C 24 0C 56 8B 74 24 0C 57"),
        BuyResource = core.AOBScan("53 8B 5C 24 10 55 8B 6C 24 10 56 8B 74 24 10"),
        GetResourceCost = getResourceCost,
        GetResourceValue = getResourceValue,
        GetResourceSpace = core.AOBScan("83 EC 0C 55 8B 6C 24 18 56"),
        playerIndex = addresses.playerIndex,
        playerData = addresses.playerData,
        ingameTime = addresses.ingameTime,
        ctrlModifier = addresses.ctrlModifier,
        u0 = addresses.u0,
        u1 = addresses.u1,
        hWindow = addresses.hWindow,
      })
      -- Do hooks now here
      local dllAddresses = dll.getAddresses()

      hook(addresses.UpdateGame.address, addresses.UpdateGame.size, dllAddresses.UpdateGameCallback, addresses.UpdateGame.to)
      hook(addresses.LeaveGame.address, addresses.LeaveGame.size, dllAddresses.EnterLeaveGameCallback, addresses.LeaveGame.to)
      hook(addresses.EnterGame.address, addresses.EnterGame.size, dllAddresses.EnterLeaveGameCallback, addresses.EnterGame.to)
      
      -- TODO: check same with master
      core.writeCode(core.AOBScan("0F ? ? ? ? ? 0F BF 86 E6 00 00 00"), {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, })

      if dll.initialize() == false then
        log(ERROR, 'could not initialize auto market dll')
      end


    end,

    disable = function(self, config)

    end,
}
