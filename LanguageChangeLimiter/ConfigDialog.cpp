#include "ConfigDialog.h"
#include <commctrl.h> // Add this header for common controls like WC_BUTTON

#pragma comment(lib, "Comctl32.lib") // Link with Comctl32.lib

// External variables
extern HKL englishHKL;
extern HKL greekHKL;
extern std::vector<HKL> selectedLanguages;
extern bool showTrayIcon;
extern bool runAsService;

LRESULT CALLBACK ConfigWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        // Initialize common controls
        InitCommonControls();

        // Create UI elements for language selection and options
        CreateWindowEx(0, WC_BUTTON, L"English",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            10, 10, 100, 30, hwnd, (HMENU)1001, hInst, NULL);
        CreateWindowEx(0, WC_BUTTON, L"Greek",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            10, 50, 100, 30, hwnd, (HMENU)1002, hInst, NULL);
        CreateWindowEx(0, WC_BUTTON, L"Show Tray Icon",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            10, 90, 150, 30, hwnd, (HMENU)1003, hInst, NULL);
        CreateWindowEx(0, WC_BUTTON, L"Run as Service",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            10, 130, 150, 30, hwnd, (HMENU)1004, hInst, NULL);
        break;
    case WM_COMMAND:
        // Handle checkbox changes and update selected languages and options
        if (HIWORD(wParam) == BN_CLICKED) {
            int id = LOWORD(wParam);
            if (id == 1001) {
                if (SendDlgItemMessage(hwnd, 1001, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    selectedLanguages.push_back(englishHKL);
                }
                else {
                    selectedLanguages.erase(std::remove(selectedLanguages.begin(), selectedLanguages.end(), englishHKL), selectedLanguages.end());
                }
            }
            else if (id == 1002) {
                if (SendDlgItemMessage(hwnd, 1002, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    selectedLanguages.push_back(greekHKL);
                }
                else {
                    selectedLanguages.erase(std::remove(selectedLanguages.begin(), selectedLanguages.end(), greekHKL), selectedLanguages.end());
                }
            }
            else if (id == 1003) {
                showTrayIcon = SendDlgItemMessage(hwnd, 1003, BM_GETCHECK, 0, 0) == BST_CHECKED;
            }
            else if (id == 1004) {
                runAsService = SendDlgItemMessage(hwnd, 1004, BM_GETCHECK, 0, 0) == BST_CHECKED;
            }
        }
        break;
    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void CreateConfigDialog(HINSTANCE hInstance) {
    WNDCLASSEX wcConfig = { 0 };
    wcConfig.cbSize = sizeof(WNDCLASSEX);
    wcConfig.lpfnWndProc = ConfigWndProc;
    wcConfig.hInstance = hInstance;
    wcConfig.lpszClassName = L"ConfigApp";
    RegisterClassEx(&wcConfig);

    hWndConfig = CreateWindowEx(0, L"ConfigApp", L"Language Settings", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, NULL, NULL, hInstance, NULL);

    ShowWindow(hWndConfig, SW_HIDE);
}
