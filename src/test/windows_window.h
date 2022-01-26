#ifndef _META_RENDERER_WINDOWS_WINDOW_H_
#define _META_RENDERER_WINDOWS_WINDOW_H_

#include <Windows.h>

namespace meta_renderer
{

class  WindowsWindow
{
public:
    WindowsWindow(int x, int y, int height, int width);
    void show();
    void hide();
    void exec();

    int get_width();
    int get_height();

    HINSTANCE get_hInstance();
    HWND get_hwnd();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    LRESULT CALLBACK real_wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwnd;
    HINSTANCE m_hInstance;

    int m_width;
    int m_height;
};

}


#endif
