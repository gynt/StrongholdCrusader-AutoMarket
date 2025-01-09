#include "pch.h"
#include "AutoMarket/Manager.h"

#include "Game/Data.h"
#include "Game/Resources.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <memory>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace AutoMarket
{
namespace Defaults
{
    static constexpr bool   ResEnabled = true;
    static constexpr size_t ResMin     = 0;
    static constexpr size_t ResMax     = 999;

    static constexpr size_t Interval = 150;
    static constexpr size_t Fee      = 10; // 10%
    static constexpr size_t Limit    = 50;
}

Manager::ResParams::ResParams()
    : enabled(Defaults::ResEnabled)
    , amountMin(Defaults::ResMin)
    , amountMax(Defaults::ResMax)
{
}

Manager::Manager()
    : m_resParams()
    , m_interval(Defaults::Interval)
    , m_fee(Defaults::Fee)
    , m_amountLimit(Defaults::Limit)
    , m_enabled(false)
    , m_nextTime(0)
    , m_curRes(0)
{
    Load();
}

Manager::~Manager()
{
}

void Manager::Reset()
{
    m_enabled  = false;
    m_nextTime = 0;
    m_curRes   = 0;
}

void Manager::Update(size_t time)
{
    if (!m_enabled)
    {
        return;
    }

    if (time < m_nextTime)
    {
        return;
    }
    m_nextTime = time + m_interval;

    for (size_t i = 0; i < Game::Resource::Max; ++i)
    {
        size_t resource = m_curRes++;
        if (m_curRes == Game::Resource::Max)
        {
            m_curRes = 0;
        }

        if (!Game::Resource::IsValid(resource))
        {
            continue;
        }

        if (IsResourceEnabled(resource))
        {
            size_t minValue = GetResourceMin(resource);
            size_t maxValue = GetResourceMax(resource);

            // Adjust min/max values to not resell resources that were auto-bought.
            minValue = std::min(minValue, maxValue);
            maxValue = std::max(minValue, maxValue);

            size_t const current = Game::playerData[*Game::playerIndex].resources[resource];
            if (current < minValue && TryBuy(resource, minValue - current))
            {
                break;
            }
            else if (current > maxValue && TrySell(resource, current - maxValue))
            {
                break;
            }
        }
    }
}

void Manager::SetEnabled(bool enabled)
{
    m_enabled = enabled;
    m_nextTime = Game::status->ingameTime + m_interval;
}

void Manager::SetResourceEnabled(size_t resource, bool enabled)
{
    assert(resource < Game::Resource::Max);
    m_resParams[resource].enabled = enabled;
}

void Manager::SetResourceMin(size_t resource, size_t amountMin)
{
    assert(resource < Game::Resource::Max);
    m_resParams[resource].amountMin = amountMin;
}

void Manager::SetResourceMax(size_t resource, size_t amountMax)
{
    assert(resource < Game::Resource::Max);
    m_resParams[resource].amountMax = amountMax;
}

bool Manager::IsEnabled() const
{
    return m_enabled;
}

bool Manager::IsResourceEnabled(size_t resource) const
{
    assert(resource < Game::Resource::Max);
    return m_resParams[resource].enabled;
}

size_t Manager::GetResourceMin(size_t resource) const
{
    assert(resource < Game::Resource::Max);
    return m_resParams[resource].amountMin;
}

size_t Manager::GetResourceMax(size_t resource) const
{
    assert(resource < Game::Resource::Max);
    return m_resParams[resource].amountMax;
}

size_t Manager::CalcFee(size_t value) const
{
    return (size_t)std::ceil(value * m_fee / 100.0f);
}

bool Manager::TryBuy(size_t resource, size_t amount)
{
    if (amount <= 0)
    {
        return true;
    }
    size_t const player = *Game::playerIndex;
    size_t&      gold = Game::playerData[player].resources[Game::Resource::Gold];

    // Limit amount by settings.
    if (m_amountLimit <= 0)
    {
        return false;
    }
    amount = std::min(amount, m_amountLimit);

    // Limit amount by available space.
    size_t const availableSpace = Game::Invoke::GetResourceSpace(Game::u1, player, resource);
    if (availableSpace <= 0)
    {
        return false;
    }
    amount = std::min(amount, availableSpace);

    // Limit amount by available gold.
    size_t const costFive = Game::Invoke::GetResourceCost(Game::u0, player, resource, 5);  // Crusader stores the cost in amounts of five, multiply by five to not get rounding errors.
    if (costFive > 0)
    {
        // What amount could we buy with the current gold?
        size_t const maxPurchasableAmount = (gold * 5 * 100) / (costFive * (100 + m_fee));
        if (maxPurchasableAmount <= 0)
        {
            return false;
        }
        amount = std::min(amount, maxPurchasableAmount);
    }

    size_t const cost = Game::Invoke::GetResourceCost(Game::u0, player, resource, amount);
    size_t const fee  = CalcFee(cost);

    if (gold < cost + fee)
    {
        return false; // Should not happen, but just for security.
    }

    if (!Game::Invoke::BuyResource(player, resource, amount))
    {
        return false;
    }
    gold -= fee;
    return true;
}

bool Manager::TrySell(size_t resource, size_t amount)
{
    if (amount <= 0)
    {
        return true;
    }
    amount = std::min(m_amountLimit, amount);

    size_t const player = *Game::playerIndex;

    size_t const value = Game::Invoke::GetResourceValue(Game::u0, player, resource, amount);
    size_t const fee   = CalcFee(value);

    Game::Invoke::SellResource(player, resource, amount);
    Game::playerData[player].resources[Game::Resource::Gold] -= fee;
    return true;
}

void Manager::Save()
{
    json j;
    j["Interval"] = m_interval;
    j["Fee"]      = m_fee;
    j["Limit"]    = m_amountLimit;
    
    json resources;
    for (size_t i = 0; i < Game::Resource::Max; ++i)
    {
        if (Game::Resource::IsValid(i))
        {
            json resParams;
            resParams["Enabled"] = m_resParams[i].enabled;
            resParams["Min"]     = m_resParams[i].amountMin;
            resParams["Max"]     = m_resParams[i].amountMax;
            resources[Game::Resource::names[i]] = std::move(resParams);
        }
    }
    j["Resources"] = std::move(resources);

    std::ofstream f("automarket.json");
    f << std::setw(3) << j << std::endl;
}

template<typename T, typename TDefault>
static bool TryLoadJson(T& outValue, const json& j, char const* key, TDefault&& defaultValue)
{
    auto const it = j.find(key);
    if (it == j.end())
    {
        outValue = std::forward<TDefault>(defaultValue);
        return false;
    }
    outValue = *it;
    return true;
}

void Manager::Load()
{
    json j;
    {
        std::ifstream f("automarket.json");
        f >> j;
    }

    TryLoadJson(m_interval, j, "Interval", Defaults::Interval);
    TryLoadJson(m_fee, j, "Fee", Defaults::Fee);
    TryLoadJson(m_amountLimit, j, "Limit", Defaults::Limit);
    auto it = j.find("Resources");
    if (it != j.end())
    {
        json resNode = *it;
        for (size_t i = 0; i < Game::Resource::Max; ++i)
        {
            if (Game::Resource::IsValid(i))
            {
                it = resNode.find(Game::Resource::names[i]);
                if (it != resNode.end())
                {
                    TryLoadJson(m_resParams[i].enabled, *it, "Enabled", Defaults::ResEnabled);
                    TryLoadJson(m_resParams[i].amountMin, *it, "Min", Defaults::ResMin);
                    TryLoadJson(m_resParams[i].amountMax, *it, "Max", Defaults::ResMax);

                    // Adjust in case the input is nonse.
                    m_resParams[i].amountMin = std::min(m_resParams[i].amountMin, m_resParams[i].amountMax);
                }
            }
        }
    }
}

}
