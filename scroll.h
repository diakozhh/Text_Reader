#ifndef __SCROLL_H__
#define __SCROLL_H__

#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>

#include "textDraw.h"
/**
* Replace scroll position
* @param[in] HWND hwnd is window
* @param[in|out] textDraw_t td is view model
*/
void SC_Scrolls(HWND hwnd, textDraw_t td);

#endif // __SCROLL_H__
