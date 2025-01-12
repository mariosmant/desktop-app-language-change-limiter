#pragma once

#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>

// Custom message for tray icon interactions
#define WM_TRAYICON (WM_USER + 1)

// Function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowContextMenu(HWND hwnd);
void SwitchLanguage();
HKL GetFocusedWindowLanguage();
void BroadcastLanguageChange(HKL nextHKL);
void RegisterTrayIcon(HWND hwnd);
void UnregisterTrayIcon();
void CreateConfigDialog(HINSTANCE hInstance);
void InstallService();
void RemoveService();
void RunAsService();

extern HINSTANCE hInst;
extern HWND hWndMain;
extern HWND hWndConfig;
extern NOTIFYICONDATA nid;
extern bool showTrayIcon;
extern bool runAsService;

extern HKL englishHKL;
extern HKL greekHKL;
extern std::vector<HKL> selectedLanguages;
