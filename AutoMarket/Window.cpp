#include "pch.h"
#include "AutoMarket/Window.h"

#include "AutoMarket/Manager.h"
#include "AutoMarket/Resources.h"
#include "Game/Resources.h"
#include "UI/Controls/Button.h"
#include "UI/Controls/Label.h"
#include "UI/Controls/NumericalEdit.h"
#include "UI/ControlManager.h"

namespace AutoMarket
{
namespace Style
{
    static constexpr int controlDist = 20;
    static constexpr int xFrame = 30;
    static constexpr int yFrame = 20;

    static constexpr int titleSize   = 40;
    static constexpr int exitBtnSize = 32;
    static constexpr int upperRowHeight = std::max(titleSize, exitBtnSize);

    static constexpr int resBtnSize     = 50;
    static constexpr int resEditWidth   = 60;
    static constexpr int resEditHeight  = 30;
    static constexpr int resEditDist    = 6;
    static constexpr int resGroupWidth  = resBtnSize + 2 * (resEditWidth + resEditDist);
    static constexpr int resGroupHeight = std::max(resBtnSize, resEditHeight);
    static constexpr int resGroupDist   = controlDist * 3 / 2;
    static constexpr int resRectWidth   = 4 * resGroupWidth + 3 * controlDist + 2 * xFrame;
    static constexpr int resRectHeight  = 5 * resGroupHeight + 2 * controlDist + 2 * yFrame + 2 * resGroupDist;

    static constexpr int enabledBtnWidth  = 250;
    static constexpr int enabledBtnHeight = 45;
    static constexpr int feeSize          = 45;
    static constexpr int lowerRowHeight   = std::max(enabledBtnHeight, feeSize);

    static constexpr int windowWidth  = resRectWidth + 2 * xFrame;
    static constexpr int windowHeight = upperRowHeight + resRectHeight + lowerRowHeight + 2 * controlDist + 2 * yFrame;
}

static bool IsExitKey(int key)
{
    static constexpr int s_exitKeys[] = { VK_ESCAPE, VK_RETURN, 'M' };
    return std::find(std::begin(s_exitKeys), std::end(s_exitKeys), key) != std::end(s_exitKeys);
}

static RECT GetAutoMarketRect(HWND parent)
{
    RECT winSize;
    GetWindowRect(parent, &winSize);
    int x = (winSize.right - winSize.left - Style::windowWidth) / 2;
    int y = (winSize.bottom - winSize.top - Style::windowHeight) / 2;
    return { x, y, x + Style::windowWidth, y + Style::windowHeight };
}

Window::Window(Manager& market, HWND parent)
    : UI::Image(Resources::GetWindowBackground(), GetAutoMarketRect(parent), parent)
    , m_market(market)
{
    CreateControls();
    RegisterFocusListener();
}

Window::~Window()
{
    DeregisterFocusListener();
}

bool Window::OnKeyDown(int key)
{
    if (IsExitKey(key))
    {
        Close();
        return true;
    }
    return false;
}

void Window::CreateControls()
{
    HWND const self = GetHandle();
    if (!self)
    {
        return;
    }

    int y = Style::yFrame;
    // Title
    {
        int x = Style::xFrame;
        RECT rect{ x, y, x + Style::windowWidth / 2, y + Style::titleSize };
        new UI::Label(L"Auto Market", Style::titleSize, RGB(255, 255, 255), rect, self);
    }
    // Exit Button
    {
        int x = Style::windowWidth - Style::xFrame - Style::exitBtnSize;
        RECT rect{ x, y, x + Style::exitBtnSize, y + Style::exitBtnSize };
        UI::Button* btnExit = new UI::Button(Resources::GetExitButton(), rect, self);
        btnExit->onClick = [this](bool) { Close(); };
    }

    // Resource Rect
    {
        int x = Style::xFrame;
        y += Style::upperRowHeight + Style::controlDist;

        RECT rect{ x, y, x + Style::resRectWidth, y + Style::resRectHeight };
        UI::Image* frame = new UI::Image(Resources::GetWindowBackground(), rect, self);

        x = Style::xFrame;
        y = Style::yFrame;

        CreateResourceControl(Game::Resource::Meat, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Cheese, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Bread, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Apples, x, y, *frame);

        x = Style::xFrame;
        y += Style::resGroupHeight + Style::resGroupDist;

        CreateResourceControl(Game::Resource::Wood, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Stone, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Iron, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Pitch, x, y, *frame);
        x = Style::xFrame;
        y += Style::resGroupHeight + Style::controlDist;
        CreateResourceControl(Game::Resource::Hops, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Ale, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Wheat, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Flour, x, y, *frame);

        x = Style::xFrame;
        y += Style::resGroupHeight + Style::resGroupDist;

        CreateResourceControl(Game::Resource::Spears, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Bows, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Maces, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::LeatherArmor, x, y, *frame);
        x = Style::xFrame;
        y += Style::resGroupHeight + Style::controlDist;
        CreateResourceControl(Game::Resource::Crossbows, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Pikes, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Swords, x, y, *frame);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::MetalArmor, x, y, *frame);
    }
    
    // centered
    int xEnableBtn = (Style::windowWidth - Style::enabledBtnWidth) / 2;
    int yEnableBtn = Style::windowHeight - Style::yFrame - (Style::lowerRowHeight + Style::enabledBtnHeight) / 2;

    // Fee indicator
    {
        int x = Style::xFrame;
        int y = Style::windowHeight - Style::yFrame - (Style::lowerRowHeight + Style::feeSize) / 2; // centered
        new UI::Image(Resources::GetResourceButton(Game::Resource::Gold).normal, { x, y, x + Style::feeSize, y + Style::feeSize }, self);

        x += Style::feeSize + Style::controlDist;
        wchar_t buf[128];
        int size = swprintf_s(buf, L"Fee: %d %%", m_market.GetFee());
        new UI::Label(std::wstring(buf, size), Style::feeSize / 2, RGB(255, 255, 255), { x, y, xEnableBtn, y + Style::feeSize }, self);
    }

    // Enabled button
    {
        UI::Button* btnEnable = new UI::Button(
            L"Enabled",
            Resources::GetButton(),
            { xEnableBtn, yEnableBtn, xEnableBtn + Style::enabledBtnWidth, yEnableBtn + Style::enabledBtnHeight },
            self
        );
        btnEnable->SetPushable(true);
        btnEnable->SetPushed(m_market.IsEnabled());
        btnEnable->onClick = [this](bool checked) { m_market.SetEnabled(checked); };
    }
}

void Window::CreateResourceControl(size_t resource, int x, int y, HWND parent)
{
    UI::Button* btn = new UI::Button(Resources::GetResourceButton(resource), {x, y, x + Style::resBtnSize, y + Style::resBtnSize}, parent);
    btn->SetPushable(true);
    btn->SetPushed(!m_market.IsResourceEnabled(resource));
    btn->onClick = [this, resource](bool checked) { m_market.SetResourceEnabled(resource, !checked); };

    // center edits
    y += Style::resGroupHeight / 2 - Style::resEditHeight / 2;

    x += Style::resBtnSize + Style::resEditDist;
    UI::NumericalEdit* minEdit = new UI::NumericalEdit(
        m_market.GetResourceMin(resource),
        Resources::GetEditBackground(),
        { x, y, x + Style::resEditWidth, y + Style::resEditHeight },
        parent);
    minEdit->onValueChanged = [this, resource](unsigned int value) { m_market.SetResourceMin(resource, value); };
    
    x += Style::resEditWidth + Style::resEditDist;
    UI::NumericalEdit* maxEdit = new UI::NumericalEdit(
        m_market.GetResourceMax(resource),
        Resources::GetEditBackground(),
        { x, y, x + Style::resEditWidth, y + Style::resEditHeight },
        parent);
    maxEdit->onValueChanged = [this, resource](unsigned int value) { m_market.SetResourceMax(resource, value); };
}

void Window::Close() const
{
    if (onExit)
    {
        onExit();
    }
    delete this;
}

void Window::RegisterFocusListener()
{
    // We want to be notified, when the focus is changed, because we want to exit on e.g. oob clicks.
    GetControlManager().onChangeFocus = [this](Control* control)
        {
            if (!control || (control != this && !control->IsDescendant(*this)))
            {
                Close();
            }
        };
}

void Window::DeregisterFocusListener()
{
    GetControlManager().onChangeFocus = {};
}

}
