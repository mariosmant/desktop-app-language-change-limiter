#include "LanguageChangeLimiter.h"
#include "ConfigDialog.h"
#include "resource.h"
#include <iostream>

HINSTANCE hInst;
HWND hWndMain;
HWND hWndConfig;
NOTIFYICONDATA nid = { 0 };
bool showTrayIcon = true;
bool runAsService = false;

HKL greekHKL = LoadKeyboardLayout(TEXT("00000408"), KLF_ACTIVATE); // Greek (Greece)
HKL englishHKL = LoadKeyboardLayout(TEXT("00000409"), KLF_ACTIVATE); // English (United States)

std::vector<HKL> selectedLanguages = { englishHKL, greekHKL };

HKL GetFocusedWindowLanguage() {
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL || hwnd == GetDesktopWindow()) {
        hwnd = GetDesktopWindow(); // Ensure we target the desktop
    }
    DWORD threadId = GetWindowThreadProcessId(hwnd, NULL);
    HKL hkl = GetKeyboardLayout(threadId);
    return hkl;
}

void BroadcastLanguageChange(HKL nextHKL) {
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL || hwnd == GetDesktopWindow()) {
        hwnd = GetDesktopWindow(); // Ensure we target the desktop
    }

    PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)nextHKL);
    PostMessage(hwnd, WM_INPUTLANGCHANGE, 0, (LPARAM)nextHKL);

    // Send language change message to all top-level windows
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, lParam);
        PostMessage(hwnd, WM_INPUTLANGCHANGE, 0, lParam);
        return TRUE;
        }, (LPARAM)nextHKL);

    // Set the language for the current thread
    AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(GetForegroundWindow(), NULL), TRUE);
    ActivateKeyboardLayout(nextHKL, KLF_REORDER | KLF_SETFORPROCESS | KLF_ACTIVATE);
    AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(GetForegroundWindow(), NULL), FALSE);
}

void SwitchLanguage() {
    HKL currentHKL = GetFocusedWindowLanguage();
    HKL nextHKL = (currentHKL == greekHKL) ? englishHKL : greekHKL;

    if (ActivateKeyboardLayout(nextHKL, KLF_REORDER | KLF_SETFORPROCESS | KLF_ACTIVATE)) {
        std::cout << "Language switched to: " << (nextHKL == greekHKL ? "Greek" : "English") << std::endl;
        BroadcastLanguageChange(nextHKL);
    }
    else {
        std::cerr << "Failed to switch language! (HKL: " << nextHKL << ")" << std::endl;
    }
}

void ShowContextMenu(HWND hwnd) {
    POINT pt;
    GetCursorPos(&pt);
    HMENU hMenu = CreatePopupMenu();
    InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_SETTINGS, L"Settings");
    InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_EXIT, L"Exit");

    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
    DestroyMenu(hMenu);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static bool altPressed = false;

    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            ShowContextMenu(hwnd);
        }
        else if (lParam == WM_LBUTTONDBLCLK) {
            ShowWindow(hWndConfig, SW_SHOW);
            SetForegroundWindow(hWndConfig);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_TRAY_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case ID_TRAY_SETTINGS:
            ShowWindow(hWndConfig, SW_SHOW);
            SetForegroundWindow(hWndConfig);
            break;
        }
        break;
    case WM_HOTKEY:
        if (wParam == 1) { // ID for hotkey
            SwitchLanguage();
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_MENU) { // Alt key
            altPressed = true;
        }
        else if (altPressed && wParam == VK_SHIFT) {
            SwitchLanguage();
            altPressed = false;
        }
        break;
    case WM_KEYUP:
        if (wParam == VK_MENU) { // Alt key
            altPressed = false;
        }
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void RegisterTrayIcon(HWND hwnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"Tray Application");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void UnregisterTrayIcon() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void InstallService() {
    // Code to install the application as a Windows service
}

void RemoveService() {
    // Code to remove the application as a Windows service
}

void RunAsService() {
    // Code to run the application as a Windows service
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"LanguageChangeLimiter";
    RegisterClassEx(&wc);

    hWndMain = CreateWindowEx(0, L"LanguageChangeLimiter", L"Tray Application", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_MESSAGE, NULL, hInstance, NULL);

    if (showTrayIcon) {
        RegisterTrayIcon(hWndMain);
    }

    CreateConfigDialog(hInstance);

    // Register hotkey (Alt+Shift) to switch language
    RegisterHotKey(hWndMain, 1, MOD_ALT | MOD_SHIFT, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (showTrayIcon) {
        UnregisterTrayIcon();
    }

    // Unregister the hotkey
    UnregisterHotKey(hWndMain, 1);

    return 0;
}
