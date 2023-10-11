#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <gdiplus.h>

using namespace Gdiplus;

bool SaveHBITMAPToJPEG(HBITMAP hBitmap, const wchar_t* filename);
int GetEncoderClsid(const wchar_t* format, CLSID* pClsid);

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
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

        // Generate a unique filename for each screenshot
        std::wstringstream wss;
        wss << L"ss2-" << screenshotCounter << L".jpeg"; // Changed file extension to .jpeg
        std::wstring filename = wss.str();

        // Save the screenshot as a JPEG file
        if (SaveHBITMAPToJPEG(hBitmap, filename.c_str())) {
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

bool SaveHBITMAPToJPEG(HBITMAP hBitmap, const wchar_t* filename) {
    Gdiplus::Bitmap bitmap(hBitmap, NULL);

    CLSID clsid;
    if (GetEncoderClsid(L"image/jpeg", &clsid) == -1) { // Changed format to JPEG
        return false;
    }

    EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].Guid = EncoderQuality;
    encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParams.Parameter[0].NumberOfValues = 1;
    ULONG quality = 95; // Adjust JPEG quality here (0-100)
    encoderParams.Parameter[0].Value = &quality;

    return bitmap.Save(filename, &clsid, &encoderParams) == Gdiplus::Ok;
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
