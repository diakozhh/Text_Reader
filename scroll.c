#include "scroll.h"

#define MAX(A, B) A > B ? A : B

void SC_Scrolls(HWND hwnd, textDraw_t td) {
  int vertLng, vertPosition;
  int vertRange, horRange, vertPageSize, horPageSize;

  if (td.VMType == VM_FORMAT) {
    vertLng = td.linesCntInWin;
    vertPosition = td.vertScrollCrd;
    ShowScrollBar(hwnd, SB_HORZ, FALSE);
  }
  else {
    vertLng = td.linesCnt;
    vertPosition = td.startLine;
  }

  if (vertLng > td.lineWinCnt) {
    vertPageSize = td.lineWinCnt * td.vertScrollScale;
    if (vertLng - td.lineWinCnt > MAX_SCROLL)
        vertRange = MAX_SCROLL;
    else
        vertRange = vertLng - td.lineWinCnt;

    SetScrollRange(hwnd, SB_VERT, 0, vertRange, TRUE);
    SetScrollPos(hwnd, SB_VERT, (int)(vertPosition * td.vertScrollScale), TRUE);
    ShowScrollBar(hwnd, SB_VERT, TRUE);
  }
  else
    ShowScrollBar(hwnd, SB_VERT, FALSE);


  if (td.VMType == VM_FORMAT)
    ShowScrollBar(hwnd, SB_HORZ, FALSE);
  else {
    if (td.maxLineLng > td.lineLngInWin) {
        horPageSize = td.lineLngInWin * td.horScrollScale;
        if (td.maxLineLng - td.lineLngInWin > MAX_SCROLL)
            horRange = MAX_SCROLL;
        else
            horRange = td.maxLineLng - td.lineLngInWin;

        SetScrollRange(hwnd, SB_HORZ, 0, horRange, TRUE);
        SetScrollPos(hwnd, SB_HORZ, (int)(td.horScrollCrd * td.horScrollScale), TRUE);
        ShowScrollBar(hwnd, SB_HORZ, TRUE);
    }
    else
        ShowScrollBar(hwnd, SB_HORZ, FALSE);
  }
}
