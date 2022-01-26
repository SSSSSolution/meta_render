#include "windows_window.h"
#include <iostream>
#include <assert.h>

namespace meta_renderer
{

WindowsWindow::WindowsWindow(int x, int y, int width, int height)
{
    m_width = width;
    m_height = height;
    std::cout << "Windows window constructor" << std::endl;
    m_hInstance = GetModuleHandle(0);

    // Register the window class
//        const wchar_t CLASS_NAME[] = L"Meta Scenes Editor Window Class";
    const char *CLASS_NAME  = "Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = m_hInstance;
    wc.lpszClassName = CLASS_NAME;

    std::cout << "register class" << std::endl;
    if (!RegisterClass(&wc))
    {
        std::cout << "register window class failed" << std::endl;
    }

    std::cout << "create window" << std::endl;
    m_hwnd = CreateWindowEx(
                0,
                CLASS_NAME,
                "meta_scenes_editor_window",
                WS_OVERLAPPEDWINDOW,
                // size and position
                x, y, width, height,

                nullptr,
                nullptr,
                m_hInstance,
                nullptr
                );
    assert(m_hwnd != nullptr);
    SetWindowLongPtrA(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    std::cout << "this: " << this;
}

void WindowsWindow::show()
{
    std::cout << "show" << std::endl;
    ShowWindow(m_hwnd, SW_SHOWNORMAL);
}

void WindowsWindow::exec()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int WindowsWindow::get_width()
{
    return m_width;
}

int WindowsWindow::get_height()
{
    return m_height;
}

HINSTANCE WindowsWindow::get_hInstance()
{
    return m_hInstance;
}

HWND WindowsWindow::get_hwnd()
{
    return m_hwnd;
}

LRESULT CALLBACK WindowsWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowsWindow *window = reinterpret_cast<WindowsWindow *>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));
    if (window == nullptr)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return window->real_wndproc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WindowsWindow::real_wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
            std::cout << "paint" << std::endl;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        std::cout << "destroy" << std::endl;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}










}









