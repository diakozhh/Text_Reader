#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "winProcess.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain(HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nCmdShow)
{

    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS | CS_CLASSDC | CS_OWNDC;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(ID_MYMENU);
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(34,39,46));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
        0,                                /* Extended possibilites for variation */
        szClassName,                      /* Classname */
        _T("Text reader"),                /* Title Text */
        WS_OVERLAPPEDWINDOW | WS_VSCROLL, /* default window with vertical scroll */
        CW_USEDEFAULT,                    /* Windows decides the position */
        CW_USEDEFAULT,                    /* where the window ends up on the screen */
        WIN_W,                            /* The programs width */
        WIN_H,                            /* and height in pixels */
        HWND_DESKTOP,                     /* The window is a child-window to desktop */
        NULL,                             /* No menu */
        hThisInstance,                    /* Program Instance handler */
        lpszArgument                      /* The only Window Creation data - filename */
    );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static winProcess_t wp;

    switch (message)
    {
    case WM_DESTROY:
        WP_Destroy(hwnd, wParam, lParam, &wp);
        _CrtDumpMemoryLeaks();
        break;

    case WM_CREATE:
        WP_Create(hwnd, wParam, lParam, &wp);
        break;

    case WM_VSCROLL:
        WP_VScrollUpdate(hwnd, wParam, lParam, &wp);
        break;

    case WM_HSCROLL:
        WP_HScrollUpdate(hwnd, wParam, lParam, &wp);
        break;

    case WM_KEYDOWN:
        WP_KeyDown(hwnd, wParam, lParam, &wp);
        break;

    case WM_MOUSEWHEEL:
        WP_MouseWheel(hwnd, wParam, lParam, &wp);
        break;

    case WM_COMMAND:
        WP_Command(hwnd, wParam, lParam, &wp);
        break;

    case WM_SIZE:
        WP_Size(hwnd, wParam, lParam, &wp);
        break;

    case WM_PAINT:
        WP_Paint(hwnd, wParam, lParam, &wp);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
