#include "textDraw.h"

#define MIN(A, B) A > B ? B : A
#define MAX(A, B) A > B ? A : B

#define SHIFT_TYPE_UP -1
#define SHIFT_TYPE_DOWN 1

int TD_Init(textDraw_t* td, textRead_t tr, int winW, int winH, HWND hwnd) {
  int i, lastBeginIndex = 0, lineIndex = 0, linesInReserv = 0, isChar = 0;

  if (winW < 0 || winH < 0)
    return 0;

  td->oldWidth = td->newWidth = winW;
  td->oldHeight = td->newHeight = winH;
  //create and set font
  HDC hdc = GetDC(hwnd);
  HFONT textFont = CreateFont(FONT_SIZE, (int)(FONT_SIZE / 2), 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, OUT_OUTLINE_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, (LPCWSTR)"Consolas");
  SelectObject(hdc, textFont);
  SetTextColor(hdc, RGB(255, 255, 255));
  SetBkMode(hdc, TRANSPARENT);

  //parse lines
  td->maxLineLng = 0;
  if (tr.bufLng == 0) {
    td->linesCnt = 0;
    td->lines = NULL;
  }
  else {
    td->linesCnt = 0;
    for (i = 0; tr.buf[i] != 0; i++) {
      if (isChar == 0 && tr.buf[i] != '\n') {
        td->linesCnt += linesInReserv;
        linesInReserv = 0;
        isChar = 1;
      }
      else if (isChar == 0 && tr.buf[i] == '\n') {
        linesInReserv++;
      }
      else if (isChar == 1 && tr.buf[i] == '\n') {
        isChar = 0;
        td->linesCnt++;
      }
    }
    if (isChar == 1)
      td->linesCnt++;

    if ((td->lines = (line_t*)malloc(sizeof(line_t) * td->linesCnt)) == NULL)
      return 0;

    // parse lines
    for (i = 0; lineIndex < td->linesCnt; i++) {
      if (tr.buf[i] == '\n' || tr.buf[i] == 0) {
        td->lines[lineIndex++] = &(tr.buf[lastBeginIndex]);

        if (td->maxLineLng < i - lastBeginIndex)
          td->maxLineLng = i - lastBeginIndex;


        if (lineIndex == td->linesCnt)
          td->lastLineLng = i - lastBeginIndex > 0 ? i - lastBeginIndex : 0;

        lastBeginIndex = i + 1;
      }

      if (tr.buf[i] == 0)
        break;
    }
  }

  td->startSubline = td->startLine =
  td->horScrollCrd = td->vertScrollCrd = 0;
  td->VMType = VM_FORMAT;

  return 1;
}

static unsigned int TD_LineLng(textDraw_t td, unsigned int ind) {
    if (ind < td.linesCnt)
      return ind + 1 < td.linesCnt ? td.lines[ind + 1] - td.lines[ind] - 1 : td.lastLineLng;
    return 0;
}

static unsigned int TD_SublineCnt(textDraw_t td, unsigned int ind) {
    if (ind < td.linesCnt) {
      unsigned int currentLineLength = TD_LineLng(td, ind);
      if (currentLineLength == 0)
          return 1;

      unsigned int sublinesCnt = currentLineLength / td.lineLngInWin + (currentLineLength % td.lineLngInWin > 0 ? 1 : 0);;
      return sublinesCnt;
    }
    return 0;
}

void TD_SwitchType(textDraw_t* td, HWND hwnd) {
  if (td->VMType == VM_FORMAT) {
    td->VMType = VM_ORIG;
    td->horScrollCrd = 0;

    ShowScrollBar(hwnd, SB_HORZ, TRUE);
  }
  else {
    td->VMType = VM_FORMAT;
    td->startSubline = 0;

    ShowScrollBar(hwnd, SB_HORZ, FALSE);
  }
}

void TD_Update(textDraw_t* td, int newWidth, int newHeight) {
  if (newWidth <= 0 || newHeight <= 0)
    return;

  td->oldWidth = td->newWidth;
  td->oldHeight = td->newHeight;
  td->newWidth = newWidth;
  td->newHeight = newHeight;
}

int TD_CheckReparse(textDraw_t td) {
  int oSymbPerH = td.oldHeight / (FONT_SIZE), oSymbPerW = td.oldWidth / (int)(FONT_SIZE / 2),
    nSymbPerH = td.newHeight / (FONT_SIZE), nSymbPerW = td.newWidth / (int)(FONT_SIZE / 2);

  if (oSymbPerH != nSymbPerH || oSymbPerW != nSymbPerW)
    return 1;
  return 0;
}

static void TD_OffsetPosition_Orig(textDraw_t* td, unsigned int linesToShift, int shiftType) {
  int i = 0;

  if (shiftType == SHIFT_TYPE_DOWN) {
  for (i = 0; i < linesToShift; i++)
    if (td->startLine == td->linesCnt - 1)
      return;
    else
      td->startLine++;
  }

  else if (shiftType == SHIFT_TYPE_UP) {
    for (i = 0; i < linesToShift; i++)
      if (td->startLine == 0)
        return;
      else
        td->startLine--;
  }
}

static void TD_OffsetPosition_Format(textDraw_t* td, unsigned int linesToShift, int shiftType) {
  unsigned int i = 0, linesInWindowSize = TD_SublineCnt(*td, td->startLine);

  if (shiftType == SHIFT_TYPE_DOWN) {
    for (i = 0; i < linesToShift; i++) {
      if (td->startLine == td->linesCnt - 1 && linesInWindowSize - 1 == td->startSubline)
        return;
      else if (linesInWindowSize - 1 == td->startSubline) {
        td->startLine++;
        linesInWindowSize = TD_SublineCnt(*td, td->startLine);
        td->startSubline = 0;
      }
      else
        td->startSubline++;
      td->vertScrollCrd++;
    }
  }

  else if (shiftType == SHIFT_TYPE_UP) {
    for (i = 0; i < linesToShift; i++) {
      if (td->startLine == 0 && td->startSubline == 0)
        return;
      else if (td->startSubline == 0) {
        td->startLine--;
        linesInWindowSize = TD_SublineCnt(*td, td->startLine);
        td->startSubline = linesInWindowSize - 1;
      }
      else
        td->startSubline--;
      td->vertScrollCrd--;
    }
  }

}

void TD_OffsetPosition(textDraw_t* td, unsigned int linesToShift, int shiftType) {
  if (td->VMType == VM_FORMAT)
    TD_OffsetPosition_Format(td, linesToShift, shiftType);
  else
    TD_OffsetPosition_Orig(td, linesToShift, shiftType);
}

void TD_OffsetStartLine(textDraw_t* td, unsigned int charsToShift, int shiftType) {
  if (shiftType == SHIFT_TYPE_UP)
    td->horScrollCrd = MAX(0, td->horScrollCrd - charsToShift);
  else if (shiftType == SHIFT_TYPE_DOWN)
    td->horScrollCrd = MIN(td->maxLineLng - 1, td->horScrollCrd + charsToShift);
}

static void TD_Reparse_Orig(textDraw_t* td) {
  td->lineLngInWin = td->newWidth / (int)(FONT_SIZE / 2);
  td->lineWinCnt = td->newHeight / (FONT_SIZE);

  // update scrolls values
  if (td->linesCnt - td->lineWinCnt > MAX_SCROLL)
    td->vertScrollScale = ((float)MAX_SCROLL) / ((float)(td->linesCnt - td->lineWinCnt));
  else
    td->vertScrollScale = 1;

  if (td->maxLineLng - td->lineLngInWin > MAX_SCROLL)
    td->horScrollScale = ((float)MAX_SCROLL) / ((float)(td->maxLineLng - td->lineLngInWin));
  else
    td->horScrollScale = 1;

  // offset strings down to remove empty space, which created at the end of file
  if (td->linesCnt > td->lineWinCnt && td->linesCnt - td->startLine < td->lineWinCnt) {
    TD_OffsetPosition_Orig(td, td->lineWinCnt - (td->linesCnt - td->startLine), -1);
    td->startLine = MAX(0, td->startLine - (td->lineWinCnt - (td->linesCnt - td->startLine)));
  }
  else if (td->linesCnt <= td->lineWinCnt) {
    TD_OffsetPosition_Orig(td, td->linesCnt, -1);
    td->startLine = 0;
  }
  if (td->maxLineLng - td->horScrollCrd < td->lineLngInWin && td->maxLineLng > td->lineLngInWin) {
    TD_OffsetStartLine(td, td->lineLngInWin - (td->maxLineLng - td->horScrollCrd), -1);
    td->horScrollCrd = MAX(0, td->horScrollCrd - (td->lineLngInWin - (td->maxLineLng - td->horScrollCrd)));
  }
  else if (td->maxLineLng <= td->lineLngInWin && td->horScrollCrd > 0) {
    TD_OffsetStartLine(td, td->horScrollCrd, -1);
    td->horScrollCrd = 0;
  }
}

static void TD_Reparse_Format(textDraw_t* td) {
  unsigned int i;

  td->linesCntInWin = 0;
  td->lineLngInWin = td->newWidth / (int)(FONT_SIZE / 2);
  td->lineWinCnt = td->newHeight / (FONT_SIZE);
  for (i = 0; i < td->linesCnt; i++)
    td->linesCntInWin += TD_SublineCnt(*td, i);

  if (TD_SublineCnt(*td, td->startLine) - 1 < td->startSubline)
    td->startSubline = TD_SublineCnt(*td, td->startLine) - 1;

  // new scroll position
  td->vertScrollCrd = 0;
  for (i = 0; i < td->startLine; i++)
    td->vertScrollCrd += TD_SublineCnt(*td, i);
  td->vertScrollCrd += td->startSubline;

  // shift strings down to remove empty space, which created at the end of file
  if (td->linesCntInWin > td->lineWinCnt && td->linesCntInWin - td->vertScrollCrd < td->lineWinCnt) {
    TD_OffsetPosition_Format(td, td->lineWinCnt - (td->linesCntInWin - td->vertScrollCrd), -1);
    td->vertScrollCrd = MAX(td->vertScrollCrd - (td->lineWinCnt - (td->linesCntInWin - td->vertScrollCrd)), 0);
  }
  else if (td->linesCntInWin <= td->lineWinCnt) {
    TD_OffsetPosition_Format(td, td->linesCntInWin, -1);
    td->startLine = 0;
  }

  // update scrolls values
  if (td->linesCntInWin - td->lineWinCnt > MAX_SCROLL)
      td->vertScrollScale = (float)(MAX_SCROLL) / (float)(td->linesCntInWin - td->lineWinCnt);
  else
      td->vertScrollScale = 1;
}

void TD_Reparse(textDraw_t* td) {
  if (td->VMType == VM_FORMAT)
    TD_Reparse_Format(td);
  else
    TD_Reparse_Orig(td);
}

static void TD_DrawText_Orig(HWND hwnd, textDraw_t td) {
  if (td.lineWinCnt <= 0)
    return;

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);

  unsigned int vert, linesDrawed = 0, currentLineLength;
  for (vert = td.startLine; vert < td.linesCnt; vert++) {
    currentLineLength = TD_LineLng(td, vert);

    if (currentLineLength > td.horScrollCrd)
      TextOut(hdc, 0, linesDrawed++ * FONT_SIZE, td.lines[vert] + td.horScrollCrd,
        MIN(currentLineLength - td.horScrollCrd, td.lineLngInWin));
    else
      linesDrawed++;


    if (linesDrawed == td.lineWinCnt)
      break;
  }

  EndPaint(hwnd, &ps);
}


static void TD_DrawText_Format(HWND hwnd, textDraw_t td) {
  if (td.lineWinCnt <= 0)
    return;

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);

  unsigned int hor, vert, linesDrawed = 0, currentLineLength,
      linesInStartLine = TD_SublineCnt(td, td.startLine),
      linesInCurLine;
  for (hor = td.startSubline; hor < linesInStartLine; hor++) {
    currentLineLength = TD_LineLng(td, td.startLine);

    TextOut(hdc, 0, linesDrawed++ * FONT_SIZE, td.lines[td.startLine] + td.lineLngInWin * hor,
      MIN(currentLineLength - td.lineLngInWin * hor, td.lineLngInWin));

    if (linesDrawed == td.lineWinCnt)
      break;
  }

  if (linesDrawed == td.lineWinCnt) {
    EndPaint(hwnd, &ps);
    return;
  }
  for (vert = td.startLine + 1; vert < td.linesCnt; vert++) {
    currentLineLength = TD_LineLng(td, vert);
    linesInCurLine = TD_SublineCnt(td, vert);

    for (hor = 0; hor < linesInCurLine; hor++) {
      TextOut(hdc, 0, linesDrawed++ * FONT_SIZE, td.lines[vert] + td.lineLngInWin * hor,
        MIN(currentLineLength - td.lineLngInWin * hor, td.lineLngInWin));

      if (linesDrawed == td.lineWinCnt)
        break;
    }
    if (linesDrawed == td.lineWinCnt)
      break;
  }

  EndPaint(hwnd, &ps);
}

void TD_DrawText(HWND hwnd, textDraw_t td) {
  if (td.VMType == VM_FORMAT)
    TD_DrawText_Format(hwnd, td);
  else
    TD_DrawText_Orig(hwnd, td);
}

void TD_Clear(textDraw_t* td) {
  if (td->lines != NULL)
    free(td->lines);

  td->linesCnt = td->linesCntInWin = 0;
}
