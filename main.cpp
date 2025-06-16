#include <Windows.h>
#include <cstdio>
#include <string>
#include "resource.h" // Include the resource header

CONST CHAR g_sz_CLASS_NAME[] = "My first Window";
INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam);

HBITMAP g_hBitmap = NULL;  // Global variable to store the bitmap handle

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE pRevInst, LPSTR lpCmdLine, INT nCmdShow)
{
    //1) Регистрация класса окна

    WNDCLASSEX wClass;
    ZeroMemory(&wClass, sizeof(wClass));

    wClass.style = CS_HREDRAW | CS_VREDRAW; // Redraw on horizontal or vertical resize
    wClass.cbSize = sizeof(wClass);
    wClass.cbWndExtra = 0;
    wClass.cbClsExtra = 0;

    wClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wClass.hbrBackground = (HBRUSH)COLOR_WINDOW; // Removed (HBRUSH)COLOR_WINDOW

    wClass.hInstance = hInstance;
    wClass.lpfnWndProc = (WNDPROC)WndProc;
    wClass.lpszMenuName = NULL;
    wClass.lpszClassName = g_sz_CLASS_NAME;

    if (RegisterClassEx(&wClass) == NULL)
    {
        MessageBox(NULL, "Class registration failed", "", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate window size (75% of screen)
    int windowWidth = (int)(screenWidth * 0.75);
    int windowHeight = (int)(screenHeight * 0.75);

    // Calculate window position (center of screen)
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;


    //2) Создание окна
    HWND hwnd = CreateWindowEx
    (
        NULL, //ExStyle
        g_sz_CLASS_NAME, //ClassName
        g_sz_CLASS_NAME, //WindowName (Title) - Will be updated later
        WS_OVERLAPPEDWINDOW,
        windowX, windowY, //Position
        windowWidth, windowHeight, //size
        NULL, //ParentWindow
        NULL,
        hInstance,
        NULL

    );
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window creation failed", "", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    //3) Запуск цикла сообщений:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Release GDI object here
    DeleteObject(g_hBitmap);

    return (int)msg.wParam; // Correct cast for return value
}

INT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        // Load the bitmap from the resource
        HINSTANCE hInstance = GetModuleHandle(NULL);
        g_hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1)); // Use MAKEINTRESOURCE

        if (g_hBitmap == NULL)
        {
            MessageBox(hwnd, "Failed to load bitmap!", "Error", MB_OK | MB_ICONERROR);
        }

        // Get window size and position
        RECT rc;
        GetWindowRect(hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        int x = rc.left;
        int y = rc.top;

        // Format the window title string
        char titleBuffer[256];
        sprintf_s(titleBuffer, sizeof(titleBuffer), "%s (Size: %d x %d, Position: %d, %d)", g_sz_CLASS_NAME, width, height, x, y);

        // Set the window title
        SetWindowTextA(hwnd, titleBuffer);
    }
    break;

    case WM_SIZE:
    {
        // Update window title when the window is resized
        RECT rc;
        GetWindowRect(hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        int x = rc.left;
        int y = rc.top;

        char titleBuffer[256];
        sprintf_s(titleBuffer, sizeof(titleBuffer), "%s (Size: %d x %d, Position: %d, %d)", g_sz_CLASS_NAME, width, height, x, y);
        SetWindowTextA(hwnd, titleBuffer);

        InvalidateRect(hwnd, NULL, TRUE); // Request a repaint
    }
    break;

    case WM_MOVE:
    {
        // Update window title when the window is moved
        RECT rc;
        GetWindowRect(hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        int x = rc.left;
        int y = rc.top;

        char titleBuffer[256];
        sprintf_s(titleBuffer, sizeof(titleBuffer), "%s (Size: %d x %d, Position: %d, %d)", g_sz_CLASS_NAME, width, height, x, y);
        SetWindowTextA(hwnd, titleBuffer);
    }
    break;

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);

        if (g_hBitmap != NULL)
        {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, g_hBitmap);

            BITMAP bm;
            GetObject(g_hBitmap, sizeof(bm), &bm);

            // BitBlt to draw the bitmap.  You can adjust position and size here.
            BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, hBitmapOld);
            DeleteDC(hdcMem);
        }

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        // Release the bitmap handle
        PostQuitMessage(0);
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}