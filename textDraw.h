#ifndef __TEXT_DRAW_H__
#define __TEXT_DRAW_H__

#include <windows.h>
#include <tchar.h>

#include <stdlib.h>

#include "textRead.h"

#define MAX_SCROLL 65535
#define FONT_SIZE 18

/**
* Typedef char array
*/

typedef char* line_t;
/**
* View model type
*/
enum ViewMode {
  VM_FORMAT,                      ///< has only vertical scroll
  VM_ORIG                       ///< vertical and horizontal scrolls
};

/**
* View model structure
*/
typedef struct textDraw textDraw_t;
struct textDraw {
  unsigned int oldWidth, oldHeight, newWidth, newHeight;  ///< old and new window sizes
  unsigned int lineWinCnt;           ///< count of lines, which Window can fit
  unsigned int lineLngInWin;    ///< max line in Window length

  line_t* lines;                        ///< lines pointer (after init: massive)
  unsigned int linesCnt,                ///< lines count
               linesCntInWin,         ///< total different lines count
               lastLineLng;          ///< last line length
/**
* Scroll parametres
*/
  char VMType;                   ///< model view type (watch 'ViewMode')
  unsigned int startLine;               ///< number of line, where placed started subline
/**
* Original view model
*/
  unsigned int maxLineLng,           ///< maximum of lines length
    horScrollCrd;            ///< horizontal scroll coord (after reparse)
  float horScrollScale;                   ///< scale for horizontal scroll
/**
* Formated view model
*/
  unsigned int startSubline;            ///< number of started subline in line
  unsigned int vertScrollCrd;            ///< vertical scroll coord (after reparse)
  float vertScrollScale;                   ///< scale for vertical scroll

};
/**
* View model initialization
* @param[in|out] textDraw_t* td is view model
* @param[in] textRead_t tr is text structure
* @param[in] HWND hwnd is window
* @param[in] int winW is window width
* @param[in] int winH is window height
* @return 1 if inited, 0 if fail
*/
int TD_Init(textDraw_t* td, textRead_t tr, int winW, int winH, HWND hwnd);

/**
* Switch view model
* @param[in|out] textDraw_t* td is view model
* @param[in] HWND hwnd is window
*/
void TD_SwitchType(textDraw_t* td, HWND hwnd);


/**
* Update window size
* @param[in|out] textDraw_t* td is view model
* @param[in] HWND hwnd is window
* @param[in] int newWidth is new window width
* @param[in] int newHeight is new window height
*/
void TD_Update(textDraw_t* td, int newWidth, int newHeight);

/**
* Check text reparse
* @param[in|out] textDraw_t td is view model
* @return 1 if need to reparse, 0 if don't need to reparse
*/
int TD_CheckReparse(textDraw_t td);
/**
* Text reparse in window
* @param[in] textDraw_t td is view model
*/
void TD_Reparse(textDraw_t* td);

/** Offset vertical text position
* @param[in|out] textDraw_t *td is model view with text to offset
* @param[in] unsigned int linesToShift is line to offset
* @param[in] int shiftType - '-1' - offset up, '1' - offset down
*/
void TD_OffsetPosition(textDraw_t* td, unsigned int linesToShift, int shiftType);

/** Offset horizontal text position
* @param[in|out] textDraw_t *td is model view with text to offset
* @param[in] unsigned int charsToShift is chars to offset
* @param[in] int shiftType - '-1' - offset up, '1' - offset down
*/
void TD_OffsetStartLine(textDraw_t* td, unsigned int charsToShift, int shiftType);

/**
* Draw text in window
* @param[in|out] textDraw_t* td is view model
* @param[in] HWND hwnd is window
*/
void TD_DrawText(HWND hwnd, textDraw_t td);

/**
* Destroy view model
* @param[out] textDraw_t* td is view model
*/
void TD_Clear(textDraw_t* td);

#endif // __TEXT_DRAW_H__
