#include <Windows.h>
#include <stdio.h>

int main() {
    // Initialize the COM library (required for some functions)
    CoInitialize(NULL);

    // Capture the entire screen
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    HBITMAP hOldBitmap = SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    // Save the screenshot to a file (you can change the file name and format)
    const char* fileName = "screenshot.bmp";
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = screenWidth;
    bih.biHeight = screenHeight;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = BI_RGB;
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;
    DWORD dwBmpSize = ((screenWidth * bih.biBitCount + 31) / 32) * 4 * screenHeight;
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfh.bfType = 0x4D42; // "BM"
    bfh.bfSize = dwSizeofDIB;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    FILE* file = fopen(fileName, "wb");
    if (file != NULL) {
        fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), file);
        fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), file);
        fwrite(hBitmap, 1, dwBmpSize, file);
        fclose(file);
    }

    // Clean up resources
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    CoUninitialize();

    printf("Screenshot saved as %s\n", fileName);

    return 0;
}
