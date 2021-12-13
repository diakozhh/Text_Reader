#ifndef __WIN_PROCESS_H__
#define __WIN_PROCESS_H__

#include "menu.h"
#include "textDraw.h"
#include "textRead.h"
#include "scroll.h"

#define WIN_W 544
#define WIN_H 375

#define WP_INIT 1
#define WP_NOT_INIT 0

typedef struct winProcess winProcess_t;
struct winProcess {
  char isInit;          // initialization flag (take values 'WP_INIT' or 'WP_NOT_INIT')

  textRead_t tr;      // 'text view' struct
  textDraw_t td;       // 'model view' struct
};
/**
* Function for WM_CREATE
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_Create(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

/**
* Function for WM_HSCROLL
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_HScrollUpdate(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

/**
* Function for WM_VSCROLL
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_VScrollUpdate(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

/**
* Function for WM_COMMAND
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_Command(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

/**
* Function for WM_KEYDOWN
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_KeyDown(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

/**
* Function for WM_MOUSEWHEEL
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_MouseWheel(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

/**
* Function for WM_SIZE
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_Size(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

/**
* Function for WM_PAINT
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_Paint(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

/**
* Function for WM_DESTROY
* @param[in] HWND hwnd is window
* @param[in] WPARAM wParam is message parameter
* @param[in] LPARAM lParam is message parameter
* @param[in|out] winProcess_t *wp is structure with text and view models
*/
void WP_Destroy(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp);

#endif // __WIN_PROCESS_H__
