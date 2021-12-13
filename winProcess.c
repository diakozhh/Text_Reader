#include "winProcess.h"

#define ABS(X) X < 0 ? X * (-1) : X
#define SIGN(X) X < 0 ? -1 : 1

#define MAX_TXT_PATH 300

void WP_Create(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  CREATESTRUCT* tmpStrct = (CREATESTRUCT*)lParam;

  if (TR_Init(&(wp->tr), (char*)tmpStrct->lpCreateParams) != 0) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    if (TD_Init(&(wp->td), wp->tr, rect.right - rect.left, rect.bottom - rect.top, hwnd) == 0) {
      TR_Clear(&(wp->tr));
      MessageBox(hwnd, _T("Can't parse file into lines!"), _T("Error"), MB_ICONERROR);
      PostQuitMessage(0);
    }

    wp->isInit = WP_INIT;
    TD_Reparse(&(wp->td));
    SC_Scrolls(hwnd, wp->td);
  }
  else {
    wp->isInit = WP_NOT_INIT;
    ShowScrollBar(hwnd, SB_VERT, FALSE);
  }
}

static void WP_VertScroll(HWND hwnd, int shift, winProcess_t *wp) {
  if (wp->isInit == WP_NOT_INIT)
    return;

  unsigned int vertScrollSize = wp->td.VMType == VM_FORMAT ? wp->td.linesCntInWin : wp->td.linesCnt,
               vertScrollPos = wp->td.VMType == VM_FORMAT ? wp->td.vertScrollCrd : wp->td.startLine;

  // no need to scroll - all lines can fit in window
  if (vertScrollSize < wp->td.lineWinCnt)
    return;
//sing of shift type (like in textDraw)
  int sign = SIGN(shift);

  if (sign < 0 && shift * sign > vertScrollPos)
      shift = sign * vertScrollPos;
  else if (sign > 0 && shift * sign > ((signed int)vertScrollSize - (signed int)wp->td.lineWinCnt - (signed int)vertScrollPos))
      shift = (signed int)vertScrollSize - (signed int)wp->td.lineWinCnt - (signed int)vertScrollPos;

  if (shift != 0) {
    TD_OffsetPosition(&(wp->td), (shift) * sign, sign);

    SC_Scrolls(hwnd, wp->td);

    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
  }
}

static void WP_HorzScroll(HWND hwnd, int shift, winProcess_t *wp) {
  if (wp->isInit == WP_NOT_INIT)
    return;

  int sign = SIGN(shift);

  // no need to scroll - all lines can fit in window
  if (wp->td.maxLineLng < wp->td.lineLngInWin)
    return;

  if (sign < 0 && shift * sign > wp->td.horScrollCrd)
      shift = (-1) * (signed int)wp->td.horScrollCrd;
  else if (sign > 0 && shift * sign > (signed int)wp->td.maxLineLng - (signed int)wp->td.lineLngInWin - (signed int)wp->td.horScrollCrd)
      shift = (signed int)wp->td.maxLineLng - (signed int)wp->td.lineLngInWin - (signed int)wp->td.horScrollCrd;

  if (shift != 0) {
    TD_OffsetStartLine(&(wp->td), shift * sign, sign);

    SC_Scrolls(hwnd, wp->td);

    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
  }
}

void WP_HScrollUpdate(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  if (wp->isInit == WP_NOT_INIT)
    return;

  int shift = 0, newPos;

  switch (LOWORD(wParam)) {
  //Scrolls left by one unit
  case SB_LINELEFT:
    shift = -1;
    break;
  //Scrolls right by one unit
  case SB_LINERIGHT:
    shift = 1;
    break;
  //Scrolls left by the width of the window
  case SB_PAGELEFT:
    shift = -1 * (signed int)(wp->td.lineLngInWin);
    break;
  //Scrolls right by the width of the window
  case SB_PAGERIGHT:
    shift = wp->td.lineLngInWin;
    break;
  //The user is dragging the scroll box.
  //This message is sent repeatedly until the user releases the mouse button
  case SB_THUMBTRACK:
    //The HIWORD indicates the position that the scroll box has been dragged to
    newPos = HIWORD(wParam) / wp->td.horScrollScale;
    shift = newPos - wp->td.horScrollCrd;
    break;
  default:
    break;
  }

  WP_HorzScroll(hwnd, shift, wp);
}

void WP_VScrollUpdate(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  if (wp->isInit == WP_NOT_INIT)
    return;

  int shift = 0, newPos;
  unsigned int vertScrollPos = wp->td.VMType == VM_FORMAT ? wp->td.vertScrollCrd : wp->td.startLine;

  switch (LOWORD(wParam)) {
  //Scrolls one line up
  case SB_LINEUP:
    shift = -1;
    break;
  //Scrolls one line down.
  case SB_LINEDOWN:
    shift = 1;
    break;
  //Scrolls one page up
  case SB_PAGEUP:
    shift = -1 * (signed int)(wp->td.lineWinCnt);
    break;
  //Scrolls one page down
  case SB_PAGEDOWN:
    shift = wp->td.lineWinCnt;
    break;
  //The user is dragging the scroll box.
  //This message is sent repeatedly until the user releases the mouse button
  case SB_THUMBTRACK:
    //The HIWORD indicates the position that the scroll box has been dragged to
    newPos = HIWORD(wParam) / wp->td.vertScrollScale;
    shift = newPos - vertScrollPos;
    break;
  default:
    break;
  }

  WP_VertScroll(hwnd, shift, wp);
}


static void CloseOldTxt(winProcess_t *wp) {
  if (wp->isInit == WP_INIT) {
    wp->isInit = WP_NOT_INIT;

    TR_Clear(&(wp->tr));
    TD_Clear(&(wp->td));
  }
}


static void OpenNewTxt(HWND hwnd, winProcess_t *wp) {

  OPENFILENAME ofn;
  char fileName[MAX_TXT_PATH];
  fileName[0] = 0;

  // init OPENFILENAME
  memset(&ofn, 0, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = sizeof(fileName);
  ofn.lpstrFilter = "*.txt\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  if (GetOpenFileName(&ofn) == TRUE) {
    CloseOldTxt(wp);

    if (TR_Init(&(wp->tr), ofn.lpstrFile) == 0) {
      MessageBox(hwnd, _T("Can't load file to read!"), _T("Error"), MB_ICONERROR);
      PostQuitMessage(0);
    }

    RECT rect;
    GetClientRect(hwnd, &rect);

    if (TD_Init(&(wp->td), wp->tr, rect.right - rect.left, rect.bottom - rect.top, hwnd) == 0) {
      TR_Clear(&(wp->tr));
      MessageBox(hwnd, _T("Can't parse file into lines!"), _T("Error"), MB_ICONERROR);
      PostQuitMessage(0);
    }

    wp->isInit = WP_INIT;

    TD_Reparse(&(wp->td));
    SC_Scrolls(hwnd, wp->td);


    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
  }

  return;
}

void WP_Command(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  switch (LOWORD(wParam)) {
    case IDM_FILE_OPEN:
      OpenNewTxt(hwnd, wp);
      return;

    case IDM_FILE_CLOSE:
      CloseOldTxt(wp);

      ShowScrollBar(hwnd, SB_VERT, FALSE);
      ShowScrollBar(hwnd, SB_HORZ, FALSE);

      InvalidateRect(hwnd, NULL, TRUE);
      UpdateWindow(hwnd);
      return;

    case IDM_FILE_EXIT:
      WP_Destroy(hwnd, wParam, lParam, wp);
      return;

    case IDM_VIEW_SWITCH:
      if (wp->isInit == WP_NOT_INIT)
        return;

      TD_SwitchType(&(wp->td), hwnd);
      TD_Reparse(&(wp->td));

      SC_Scrolls(hwnd, wp->td);
      // draw text
      InvalidateRect(hwnd, NULL, TRUE);
      UpdateWindow(hwnd);
      return;
  }
}

void WP_KeyDown(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  if (wp->isInit == WP_NOT_INIT)
    return;

  int shift = 0;

  switch (wParam) {
    // horizontal scroll navigation
    case VK_LEFT:
    case VK_RIGHT:
      if(wp->td.VMType == VM_FORMAT)
        return;

      shift = (wParam == VK_RIGHT ? 1 : -1);

      WP_HorzScroll(hwnd, shift, wp);
      return;

    // vertical scroll navigation
    case VK_UP:
    case VK_DOWN:
    case VK_PRIOR:
    case VK_NEXT:
      if (wParam == VK_DOWN)
        shift = 1;
      else if (wParam == VK_UP)
        shift = -1;
      else if (wParam == VK_PRIOR)
        shift = -1 * (signed int)wp->td.lineWinCnt;
      else
        shift = (signed int)wp->td.lineWinCnt;

      WP_VertScroll(hwnd, shift, wp);
      return;
  }
}

void WP_MouseWheel(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  if (wp->isInit == WP_NOT_INIT)
    return;

  int shift = -1 * GET_WHEEL_DELTA_WPARAM(wParam) / 120;
  WP_VertScroll(hwnd, shift, wp);
}

void WP_Size(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  if (wp->isInit == WP_NOT_INIT)
    return;

  TD_Update(&(wp->td), LOWORD(lParam), HIWORD(lParam));
  if (TD_CheckReparse(wp->td) == 1) {
    TD_Reparse(&(wp->td));

    SC_Scrolls(hwnd, wp->td);

    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
  }
}

void WP_Paint(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  if (wp->isInit == WP_NOT_INIT)
    return;

  TD_DrawText(hwnd, wp->td);
}

void WP_Destroy(HWND hwnd, WPARAM wParam, LPARAM lParam, winProcess_t *wp) {
  CloseOldTxt(wp);
  PostQuitMessage(0);
}
