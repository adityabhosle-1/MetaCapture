#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <gdiplus.h>

using namespace Gdiplus;

bool SaveHBITMAPToFile(HBITMAP hBitmap, const wchar_t* filename);
int GetEncoderClsid(const wchar_t* format, CLSID* pClsid); // Declaration added

int main() {
    int screenshotIntervalMs = 250; // 4 screenshots per second (1000 ms / 4)
    int screenshotCounter = 0;

    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    while (true) {
        // Capture the screen
        HDC hdcScreen = GetDC(NULL);
        int screenWidth = 2880;
        int screenHeight = 1800;
        // int screenWidth = 1920;
        // int screenHeight = 1080;
        // use these measures for  windows system 
        // 🔴🔴🔴🔴🔴🔴🔴🔴🔴🔴🔴🔴🔴

        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

        // Generate a unique filename for each screenshot
        std::wstringstream wss;
        wss << L"test5-" << screenshotCounter << L".bmp";
        std::wstring filename = wss.str();

        // Save the screenshot as a BMP file
        if (SaveHBITMAPToFile(hBitmap, filename.c_str())) {
            std::wcout << L"Screenshot captured and saved as " << filename << std::endl;
            screenshotCounter++;
        } else {
            std::wcerr << L"Failed to save the screenshot." << std::endl;
        }

        // Clean up resources
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);

        // Sleep for the specified interval
        Sleep(screenshotIntervalMs);
    }

    // Shutdown GDI+
    GdiplusShutdown(gdiplusToken);

    return 0;
}

bool SaveHBITMAPToFile(HBITMAP hBitmap, const wchar_t* filename) {
    Gdiplus::Bitmap bitmap(hBitmap, NULL);

    CLSID clsid;
    if (GetEncoderClsid(L"image/bmp", &clsid) == -1) {
        return false;
    }

    return bitmap.Save(filename, &clsid, NULL) == Gdiplus::Ok;
}

int GetEncoderClsid(const wchar_t* format, CLSID* pClsid) {
    UINT num = 0;           // Number of image encoders
    UINT size = 0;          // Size of the image encoder array in bytes

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) {
        return -1;  // Failure
    }

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) {
        return -1;  // Failure
    }

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}
