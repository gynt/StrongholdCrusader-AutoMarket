#include "pch.h"
#include "AutoMarket/UI/ResourceButton.h"
#include "AutoMarket/AutoMarket.h"
#include "AutoMarket/UI/Utils.h"
#include "resource.h"
#include "Game/UI.h"

namespace AutoMarket::UI
{

static constexpr float s_disableItemScale = 0.6f;

static constexpr int   s_itemsBitmapGridSize   = 45;
static constexpr DWORD s_itemsBitmapAlphaColor = RGB(255, 0, 255);

static void RedrawParent(HWND hWnd)
{
    HWND hParent = GetParent(hWnd);

    RECT rect;
    GetWindowRect(hWnd, &rect);
    MapWindowPoints(HWND_DESKTOP, hParent, (LPPOINT)&rect, 2);

    // To handle transparency correctly, invalidate the parent, but only in our rect.
    InvalidateRect(hParent, &rect, TRUE);
}

class ResourceButton
{
public:
    ResourceButton(Manager& inMarket, size_t inRes)
        : m_market(inMarket)
        , m_resource(inRes)
        , m_hover(false)
    {
    }

    void OnMouseMove(HWND hWnd)
    {
        if (!m_hover)
        {
            m_hover = true;
            RedrawParent(hWnd);

            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hWnd;
            tme.dwHoverTime = HOVER_DEFAULT;
            TrackMouseEvent(&tme);
        }
    }
    void OnMouseLeave(HWND hWnd)
    {
        if (m_hover)
        {
            m_hover = false;
            RedrawParent(hWnd);
        }
    }

    void OnPaint(HWND hWnd)
    {
        static HBITMAP const s_hbmItems = LoadBitmap(g_module, MAKEINTRESOURCE(IDB_ITEMS));
        static HBITMAP const s_hbmItemsMask = CreateBitmapMask(s_hbmItems, s_itemsBitmapAlphaColor);

        int xSrc = s_itemsBitmapGridSize * m_resource;
        int ySrc = s_itemsBitmapGridSize * m_hover;

        PAINTSTRUCT ps;
        HDC hdc    = BeginPaint(hWnd, &ps);
        HDC hdcMem = CreateCompatibleDC(hdc);

        RECT rect;
        GetClientRect(hWnd, &rect);
        int wDest = rect.right - rect.left;
        int hDest = rect.bottom - rect.top;

        SelectObject(hdcMem, s_hbmItemsMask);
        StretchBlt(hdc, rect.left, rect.top, wDest, hDest, hdcMem, xSrc, ySrc, s_itemsBitmapGridSize, s_itemsBitmapGridSize, SRCAND);
        SelectObject(hdcMem, s_hbmItems);
        StretchBlt(hdc, rect.left, rect.top, wDest, hDest, hdcMem, xSrc, ySrc, s_itemsBitmapGridSize, s_itemsBitmapGridSize, SRCPAINT);

        // Draw the disabled sign over it, but smaller
        if (!m_market.IsResourceEnabled(m_resource))
        {
            int wDestDisable = (int)(s_disableItemScale * wDest);
            int hDestDisable = (int)(s_disableItemScale * hDest);

            int xDestDisable = (wDest - wDestDisable) / 2;
            int yDestDisable = (hDest - hDestDisable) / 2;

            SelectObject(hdcMem, s_hbmItemsMask);
            StretchBlt(hdc, xDestDisable, yDestDisable, wDestDisable, hDestDisable, hdcMem, 0, 0, s_itemsBitmapGridSize, s_itemsBitmapGridSize, SRCAND);
            SelectObject(hdcMem, s_hbmItems);
            StretchBlt(hdc, xDestDisable, yDestDisable, wDestDisable, hDestDisable, hdcMem, 0, 0, s_itemsBitmapGridSize, s_itemsBitmapGridSize, SRCPAINT);
        }

        DeleteDC(hdcMem);
        EndPaint(hWnd, &ps);
    }

    void OnLButtonDown(HWND hWnd)
    {
        bool isEnabled = m_market.IsResourceEnabled(m_resource);
        m_market.SetResourceEnabled(m_resource, !isEnabled);

        RedrawParent(hWnd);
    }

protected:
    Manager& m_market;
    size_t      m_resource;

    bool        m_hover;
};


struct CreateParams
{
    Manager& market;
    size_t      resource;
};

static LRESULT CALLBACK WndProcButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ResourceButton* button;
    if (uMsg == WM_CREATE)
    {
        CreateParams* createParams = (CreateParams*)((LPCREATESTRUCT)lParam)->lpCreateParams;

        button = new ResourceButton(createParams->market, createParams->resource);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)button);
    }
    else
    {
        button = (ResourceButton*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (!button)
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
    case WM_CREATE:
        break;
    case WM_MOUSEMOVE:
        button->OnMouseMove(hWnd);
        break;
    case WM_PAINT:
        button->OnPaint(hWnd);
        break;
    case WM_KEYDOWN:
    case WM_CHAR:
        if (HWND hParent = GetParent(hWnd))
        {
            // reroute to parent
            WNDPROC parentWndProc = (WNDPROC)GetWindowLongPtr(hParent, GWL_WNDPROC);
            return CallWindowProc(parentWndProc, hParent, uMsg, wParam, lParam);
        }
        break;
    case WM_MOUSELEAVE:
        button->OnMouseLeave(hWnd);
        break;
    case WM_LBUTTONDOWN:
        button->OnLButtonDown(hWnd);
        break;
    case WM_DESTROY:
        SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
        delete button;
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

static void RegisterResourceButton()
{
    static bool s_registered = false;
    if (!s_registered)
    {
        WNDCLASS wc = { 0 };
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpszClassName = L"AutoMarketButton";
        wc.hInstance = Game::UI::hInstance;
        wc.lpfnWndProc = WndProcButton;
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        if (!RegisterClass(&wc))
        {
            return;
        }

        s_registered = true;
    }
}

HWND CreateResourceButton(Manager& market, size_t resource, int x, int y, size_t w, size_t h, HWND hParent)
{
    RegisterResourceButton();

    CreateParams createParams{ market, resource };
    return CreateWindow(
        L"AutoMarketButton",
        NULL,
        WS_VISIBLE | WS_CHILD,
        x, y, w, h,
        hParent,
        NULL,
        NULL,
        &createParams);
}

}
