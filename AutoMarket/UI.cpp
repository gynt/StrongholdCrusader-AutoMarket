#include "pch.h"
#include "AutoMarket/UI.h"

#include "AutoMarket/AutoMarket.h"
#include "AutoMarket/UI/Button.h"
#include "AutoMarket/UI/EnableButton.h"
#include "AutoMarket/UI/ResourceButton.h"
#include "AutoMarket/UI/ResourceEdit.h"
#include "AutoMarket/UI/Utils.h"
#include "Game/Resources.h"
#include "Game/UI.h"
#include "resource.h"

namespace AutoMarket::UI
{
namespace Style
{
    static constexpr int btnWidth  = 100;
    static constexpr int btnHeight = 30;

    static constexpr int resBtnSize     = 50;
    static constexpr int resEditWidth   = 50;
    static constexpr int resEditHeight  = 25;
    static constexpr int resEditDist    = 5;
    static constexpr int resGroupWidth  = resBtnSize + 2 * (resEditWidth + resEditDist);
    static constexpr int resGroupHeight = std::max(resBtnSize, resEditHeight);

    static constexpr int controlDist = 20;

    static constexpr int windowWidth  = 5 * controlDist + 4 * resGroupWidth;
    static constexpr int windowHeight = 8 * controlDist + 5 * resGroupHeight + (controlDist + btnHeight);
}

inline bool IsExitKey(int key)
{
    static constexpr int s_exitKeys[] = { VK_ESCAPE, VK_RETURN, 'M' };
    return std::find(std::begin(s_exitKeys), std::end(s_exitKeys), key) != std::end(s_exitKeys);
}

class AutoMarketUI
{
    static constexpr DWORD s_idBtnExit   = 0x00;
    static constexpr DWORD s_idBtnEnable = 0x01;
    static constexpr DWORD s_idEditsMin  = 0x100;
    static constexpr DWORD s_idEditsMax  = 0x200;

public:
    AutoMarketUI(Manager& market, std::function<void()> onExit)
        : m_market(market)
        , m_onExit(std::move(onExit))
    {
    }

    void Init(HWND hWnd)
    {
        int x = Style::controlDist;
        int y = Style::controlDist;

        CreateResourceControl(Game::Resource::Meat, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Cheese, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Bread, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Apples, hWnd, x, y);

        x = Style::controlDist;
        y += Style::resGroupHeight + 2 * Style::controlDist;

        CreateResourceControl(Game::Resource::Wood, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Stone, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Iron, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Pitch, hWnd, x, y);
        x = Style::controlDist;
        y += Style::resGroupHeight + Style::controlDist;
        CreateResourceControl(Game::Resource::Hops, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Ale, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Wheat, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Flour, hWnd, x, y);

        x = Style::controlDist;
        y += Style::resGroupHeight + 2 * Style::controlDist;

        CreateResourceControl(Game::Resource::Spears, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Bows, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Maces, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::LeatherArmor, hWnd, x, y);
        x = Style::controlDist;
        y += Style::resGroupHeight + Style::controlDist;
        CreateResourceControl(Game::Resource::Crossbows, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Pikes, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::Swords, hWnd, x, y);
        x += Style::resGroupWidth + Style::controlDist;
        CreateResourceControl(Game::Resource::MetalArmor, hWnd, x, y);

        y = Style::windowHeight - Style::controlDist - Style::btnHeight;

        x = Style::controlDist;
        CreateEnableButton(m_market, x, y, Style::btnWidth, Style::btnHeight, hWnd, s_idBtnEnable);
        x = Style::windowWidth - Style::controlDist - Style::btnWidth;
        CreateButton(L"Exit", x, y, Style::btnWidth, Style::btnHeight, hWnd, s_idBtnExit);
    }

    void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
            OnButtonClick(LOWORD(wParam), (HWND)lParam);
            break;
        case EN_CHANGE:
        case EN_UPDATE:
            OnEditUpdate(hWnd, LOWORD(wParam), (HWND)lParam);
            break;
        default:
            break;
        }
    }

    void OnPaint(HWND hWnd)
    {
        static HBITMAP s_background = LoadBitmap(g_module, MAKEINTRESOURCE(IDB_BACKGROUND));

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HDC hdcMem = CreateCompatibleDC(hdc);

        RECT rect;
        GetClientRect(hWnd, &rect);

        BITMAP bm{};
        GetObject(s_background, sizeof(BITMAP), &bm);

        SelectObject(hdcMem, s_background);
        StretchBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

        DeleteDC(hdcMem);
        EndPaint(hWnd, &ps);
    }

    void OnKeyDown(HWND hWnd, int key)
    {
        if (IsExitKey(key))
        {
            TryExit();
        }
    }

protected:
    void TryExit()
    {
        if (m_onExit)
        {
            m_onExit();
        }
    }

    void CreateResourceControl(size_t resource, HWND hParent, int x, int y)
    {
        CreateResourceButton(m_market, resource, x, y, Style::resBtnSize, Style::resBtnSize, hParent);
        x += Style::resBtnSize + Style::resEditDist;

        // center edits
        y += Style::resGroupHeight / 2 - Style::resEditHeight / 2;

        CreateResourceEdit(m_market.GetResourceMin(resource), m_market, x, y, Style::resEditWidth, Style::resEditHeight, hParent, s_idEditsMin + resource);
        x += Style::resEditWidth + Style::resEditDist;

        CreateResourceEdit(m_market.GetResourceMax(resource), m_market, x, y, Style::resEditWidth, Style::resEditHeight, hParent, s_idEditsMax + resource);
    }

    void SetResourceMax(HWND hWnd, size_t resource, size_t value)
    {
        m_market.SetResourceMax(resource, value);
        if (value < m_market.GetResourceMin(resource))
        {
            // Also set the min value in this case.
            HWND hMinEdit = FindChildWindowById(hWnd, (HMENU)(s_idEditsMin + resource));
            SetResourceEditValue(hMinEdit, value); // this will send a command, which will update the variable.
        }
    }

    void SetResourceMin(HWND hWnd, size_t resource, size_t value)
    {
        m_market.SetResourceMin(resource, value);
        if (value > m_market.GetResourceMax(resource))
        {
            // Also set the max value in this case.
            HWND hMaxEdit = FindChildWindowById(hWnd, (HMENU)(s_idEditsMax + resource));
            SetResourceEditValue(hMaxEdit, value); // this will send a command, which will update the variable.
        }
    }

    void OnEditUpdate(HWND hWnd, DWORD id, HWND hEdit)
    {
        size_t value = GetResourceEditValue(hEdit);
        if (id >= s_idEditsMax)
        {
            size_t resource = id - s_idEditsMax;
            if (resource >= 0 && resource < Game::Resource::Max)
            {
                SetResourceMax(hWnd, resource, value);
            }
        }
        else if (id >= s_idEditsMin)
        {
            size_t resource = id - s_idEditsMin;
            if (resource >= 0 && resource < Game::Resource::Max)
            {
                SetResourceMin(hWnd, resource, value);
            }
        }
    }

    void OnButtonClick(size_t id, HWND hButton)
    {
        if (id == s_idBtnExit)
        {
            TryExit();
        }
        else if (id == s_idBtnEnable)
        {
            LRESULT result = SendMessage(hButton, BM_GETCHECK, NULL, NULL);
            m_market.SetEnabled((result & BST_CHECKED) != 0);
        }
    }

protected:
    Manager&              m_market;
    std::function<void()> m_onExit;
};

struct CreateParams
{
    Manager&              market;
    std::function<void()> onExit;
};

static LRESULT CALLBACK WndProcUI(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    AutoMarketUI* ui;
    if (uMsg == WM_CREATE)
    {
        CreateParams* params = (CreateParams*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        ui = new AutoMarketUI(params->market, std::move(params->onExit));
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)ui);

        ui->Init(hWnd); // Init now that the UserData ptr has been set.
    }
    else
    {
        ui = (AutoMarketUI*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (!ui)
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        ui->OnCommand(hWnd, wParam, lParam);
        break;
    case WM_PAINT:
        ui->OnPaint(hWnd);
        break;
    case WM_KEYDOWN:
    case WM_CHAR:
        ui->OnKeyDown(hWnd, wParam);
        break;
    case WM_DESTROY:
        SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
        delete ui;
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

static void RegisterAutoMarketUI()
{
    static bool s_registered = false;
    if (!s_registered)
    {
        WNDCLASS wc = { 0 };
        wc.lpszClassName = L"AutoMarketUI";
        wc.hInstance     = (HINSTANCE)GetWindowLongPtr(*Game::UI::hWindow, GWLP_HINSTANCE);
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc   = WndProcUI;
        wc.hCursor       = LoadCursor(0, IDC_ARROW);

        if (!RegisterClass(&wc))
        {
            return;
        }

        s_registered = true;
    }
}

static HWND CreateAutoMarketUI(Manager& market, std::function<void()> onExit, int x, int y, int w, int h, HWND hParent)
{
    RegisterAutoMarketUI();

    CreateParams createParams{ market, std::move(onExit) };
    return CreateWindow(
        L"AutoMarketUI",
        NULL,
        WS_CHILD | WS_VISIBLE,
        x, y, w, h,
        hParent,
        NULL,
        NULL,
        (LPVOID)&createParams);
}

HWND Open(Manager& market, std::function<void()> onExit)
{
    RECT winSize;
    GetWindowRect(*Game::UI::hWindow, &winSize);
    int x = (winSize.right - winSize.left - Style::windowWidth) / 2;
    int y = (winSize.bottom - winSize.top - Style::windowHeight) / 2;
    return CreateAutoMarketUI(market, std::move(onExit), x, y, Style::windowWidth, Style::windowHeight, *Game::UI::hWindow);
}

void Close(HWND hWnd)
{
    DestroyWindow(hWnd);
}

}
