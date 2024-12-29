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
  local _, isIngame = utils.AOBExtract("89 ? I(? ? ? ?) 89 ? ? ? ? ? 89 ? ? ? ? ? E8 ? ? ? ? 56 6A FF")
  local _, isPaused = utils.AOBExtract("83 ? I(? ? ? ?) ? 75 CC 5D")
  local _, ingameTime = utils.AOBExtract("8B ? I(? ? ? ?) 66 8B 54 24 1C")
  local _, ctrlModifier = utils.AOBExtract("39 ? I(? ? ? ?) 74 08 39 ? ? ? ? ? 75 2B")
  local shiftModifier = ctrlModifier + 4
  local altModifier = shiftModifier + 4
  local _, u0 = utils.AOBExtract("B9 I(? ? ? ?) E8 ? ? ? ? E9 ? ? ? ? 8B 54 24 14 6A 00")
  local _, u1 = utils.AOBExtract("B9 I(? ? ? ?) E8 ? ? ? ? A1 ? ? ? ? 8B 54 24 20")
  local _, hWindow = utils.AOBExtract("89 ? I(? ? ? ?) FF ? ? ? ? ? E9 ? ? ? ? 8B ? ? ? ? ? 83 FA 29")
   addresses = {
    -- Data
    playerIndex = playerIndex,
    playerData = playerData,
    isIngame = isIngame,
    isPaused = isPaused,
    ingameTime = ingameTime,
    ctrlModifier = ctrlModifier,
    shiftModifier = shiftModifier,
    altModifier = altModifier,
    u0 = u0,
    u1 = u1,
    hWindow = hWindow,
    -- Functions
    Escape = { address = core.AOBScan("A1 ? ? ? ? 3B C5 74 C1"), size = 5, to = nil},
    Update = { address = core.AOBScan("39 ? ? ? ? ? 74 15 FF D7"), size = 6, to = nil},
    SetIngameStatus = {address = 13 + core.AOBScan("6A 64 57 B9 ? ? ? ? E8 ? ? ? ? 89 ? ? ? ? ?"), size = 6, to = nil},
    Mouse = {address = core.AOBScan("66 8B 44 24 04 66 8B 54 24 08"), size = 5, to = nil},
    SystemKey = {address = core.AOBScan("8D 46 F3 3D D1 00 00 00"), size = 8, to = nil},
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
        isIngame = addresses.isIngame,
        isPaused = addresses.isPaused,
        ingameTime = addresses.ingameTime,
        ctrlModifier = addresses.ctrlModifier,
        shiftModifier = addresses.shiftModifier,
        altModifier = addresses.altModifier,
        u0 = addresses.u0,
        u1 = addresses.u1,
      })
      -- Do hooks now here
      local dllAddresses = dll.getAddresses()

      hook(addresses.Escape.address, addresses.Escape.size, dllAddresses.EscapeCallback, addresses.Escape.to)
      hook(addresses.Update.address, addresses.Update.size, dllAddresses.UpdateCallback, addresses.Update.to)
      hook(addresses.SetIngameStatus.address, addresses.SetIngameStatus.size, dllAddresses.SetIngameStatusCallback, addresses.SetIngameStatus.to)
      hook(addresses.Mouse.address, addresses.Mouse.size, dllAddresses.MouseCallback, addresses.Mouse.to)
      hook(addresses.SystemKey.address, addresses.SystemKey.size, dllAddresses.SystemKeyCallback, addresses.SystemKey.to)
      core.writeCode(core.AOBScan("0F ? ? ? ? ? 0F BF 86 E6 00 00 00"), {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, })

      if dll.initialize() == false then
        log(ERROR, 'could not initialize auto market dll')
      end


    end,

    disable = function(self, config)

    end,
}
