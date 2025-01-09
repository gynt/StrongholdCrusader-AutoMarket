#include "pch.h"
#include "AutoMarket/UI/Utils.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace AutoMarket::UI
{

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    // Create monochrome (1 bit) mask bitmap.
    BITMAP bm{};
    GetObject(hbmColour, sizeof(BITMAP), &bm);
    HBITMAP hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Get some HDCs that are compatible with the display driver
    HDC hdcMem = CreateCompatibleDC(0);
    HDC hdcMem2 = CreateCompatibleDC(0);

    SelectObject(hdcMem, hbmColour); // SelectBitmap
    SelectObject(hdcMem2, hbmMask); // SelectBitmap

    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(hdcMem, crTransparent);

    // Copy the bits from the colour image to the B+W mask... everything
    // with the background colour ends up white while everythig else ends up
    // black...Just what we wanted.
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Take our new mask and use it to turn the transparent colour in our
    // original colour image to black so the transparency effect will
    // work right.
    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

    // Clean up.
    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);
    return hbmMask;
}

}
