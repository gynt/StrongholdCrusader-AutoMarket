#pragma once

#include "Game/Resources.h"

#include <stdint.h>

namespace AutoMarket
{

class Manager
{
    struct ResParams
    {
        ResParams();

        bool   enabled;
        size_t amountMin;
        size_t amountMax;
    };

public:
    Manager();
    ~Manager();

    void   Reset();
    void   Update(size_t time);

    void   Open();
    void   Close();
    bool   IsOpened() const;

    void   SetEnabled(bool enabled);
    void   SetResourceEnabled(size_t resource, bool enabled);
    void   SetResourceMin(size_t resource, size_t amountMin);
    void   SetResourceMax(size_t resource, size_t amountMax);

    bool   IsEnabled() const;
    bool   IsResourceEnabled(size_t resource) const;
    size_t GetResourceMin(size_t resource) const;
    size_t GetResourceMax(size_t resource) const;

    size_t GetFee() const         { return m_fee; }
    size_t GetInterval() const    { return m_interval; }
    size_t GetAmountLimit() const { return m_amountLimit; }

    void   Save();
    void   Load();

private:
    size_t CalcFee(size_t value) const;
    bool   TryBuy(size_t resource, size_t amount);
    bool   TrySell(size_t resource, size_t amount);

    ResParams m_resParams[Game::Resource::Max];
    size_t    m_interval;
    size_t    m_fee; // transaction fee in percent * 100
    size_t    m_amountLimit;

    bool   m_enabled;
    size_t m_nextTime;
    size_t m_curRes;

    HWND   m_ui;
};

}