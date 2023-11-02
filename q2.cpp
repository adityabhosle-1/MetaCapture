#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <gdiplus.h>
#include <queue>
#include <chrono>
#include <ctime>

using namespace Gdiplus;

bool SaveHBITMAPToJPEG(HBITMAP hBitmap, const wchar_t* filename);
int GetEncoderClsid(const wchar_t* format, CLSID* pClsid);

const int screenshotIntervalMs = 250; // 4 screenshots per second (1000 ms / 4)
const int bufferDurationMs = 10000;    // 10 seconds

struct ScreenshotInfo {
    std::wstring filename;
    std::chrono::system_clock::time_point creationTime;
};

void DeleteOldScreenshots(std::queue<ScreenshotInfo>& screenshotQueue) {
    auto currentTime = std::chrono::system_clock::now();

    while (!screenshotQueue.empty()) {
        const auto& frontScreenshot = screenshotQueue.front();
        std::chrono::duration<double> age = currentTime - frontScreenshot.creationTime;

        if (age.count() >= bufferDurationMs / 1000.0) {
            std::wcout << L"Deleting old screenshot: " << frontScreenshot.filename << std::endl;
            DeleteFileW(frontScreenshot.filename.c_str()); // Use wide-character version of DeleteFile
            screenshotQueue.pop();
        } else {
            // No need to check further in the queue since screenshots are ordered by creation time
            break;
        }
    }
}

int main() {
    int screenshotCounter = 0;
    std::queue<ScreenshotInfo> screenshotQueue;
    auto lastDeleteTime = std::chrono::system_clock::now();

    // Create the "buffer" directory if it doesn't exist
    std::wstring bufferDir = L"buffer2";
    CreateDirectoryW(bufferDir.c_str(), NULL); // Use wide-character version of CreateDirectory

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
        wss << bufferDir << L"\\ss2-" << screenshotCounter << L".jpeg";
        std::wstring filename = wss.str();

        // Save the screenshot as a JPEG file
        if (SaveHBITMAPToJPEG(hBitmap, filename.c_str())) {
            std::wcout << L"Screenshot captured and saved as " << filename << std::endl;
            screenshotQueue.push({filename, std::chrono::system_clock::now()}); // Add to the queue with creation time
            screenshotCounter++;
        } else {
            std::wcerr << L"Failed to save the screenshot." << std::endl;
        }

        // Clean up resources
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);

        // Check if it's time to delete old screenshots
        auto currentTime = std::chrono::system_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastDeleteTime).count() >= bufferDurationMs) {
            DeleteOldScreenshots(screenshotQueue);
            lastDeleteTime = std::chrono::system_clock::now();
        }

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
    if (GetEncoderClsid(L"image/jpeg", &clsid) == -1) {
        return false;
    }

    EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].Guid = EncoderQuality;
    encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParams.Parameter[0].NumberOfValues = 1;
    ULONG quality = 95;
    encoderParams.Parameter[0].Value = &quality;

    return bitmap.Save(filename, &clsid, &encoderParams) == Gdiplus::Ok;
}

int GetEncoderClsid(const wchar_t* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) {
        return -1;
    }

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) {
        return -1;
    }

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}
