/*
 *      $Id: GridP.h,v 1.3 1999-06-02 03:40:06 dbrown Exp $
 */
/*
(c) Copyright 1994, 1995, 1996 Microline Software, Inc.  ALL RIGHTS RESERVED
  
THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE COPIED AND USED 
ONLY IN ACCORDANCE WITH THE TERMS OF THAT LICENSE AND WITH THE INCLUSION
OF THE ABOVE COPYRIGHT NOTICE.  THIS SOFTWARE AND DOCUMENTATION, AND ITS 
COPYRIGHTS ARE OWNED BY MICROLINE SOFTWARE AND ARE PROTECTED BY UNITED
STATES COPYRIGHT LAWS AND INTERNATIONAL TREATY PROVISIONS.
 
THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT NOTICE
AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY MICROLINE SOFTWARE.

THIS SOFTWARE AND REFERENCE MATERIALS ARE PROVIDED "AS IS" WITHOUT
WARRANTY AS TO THEIR PERFORMANCE, MERCHANTABILITY, FITNESS FOR ANY 
PARTICULAR PURPOSE, OR AGAINST INFRINGEMENT.  MICROLINE SOFTWARE
ASSUMES NO RESPONSIBILITY FOR THE USE OR INABILITY TO USE THIS 
SOFTWARE.

MICROLINE SOFTWARE SHALL NOT BE LIABLE FOR INDIRECT, SPECIAL OR
CONSEQUENTIAL DAMAGES RESULTING FROM THE USE OF THIS PRODUCT. SOME 
STATES DO NOT ALLOW THE EXCLUSION OR LIMITATION OF INCIDENTAL OR
CONSEQUENTIAL DAMAGES, SO THE ABOVE LIMITATIONS MIGHT NOT APPLY TO
YOU.

MICROLINE SOFTWARE SHALL HAVE NO LIABILITY OR RESPONSIBILITY FOR SOFTWARE
ALTERED, MODIFIED, OR CONVERTED BY YOU OR A THIRD PARTY, DAMAGES
RESULTING FROM ACCIDENT, ABUSE OR MISAPPLICATION, OR FOR PROBLEMS DUE
TO THE MALFUNCTION OF YOUR EQUIPMENT OR SOFTWARE NOT SUPPLIED BY
MICROLINE SOFTWARE.
  
U.S. GOVERNMENT RESTRICTED RIGHTS
This Software and documentation are provided with RESTRICTED RIGHTS.
Use, duplication or disclosure by the Government is subject to
restrictions as set forth in subparagraph (c)(1) of the Rights in
Technical Data and Computer Software Clause at DFARS 252.227-7013 or
subparagraphs (c)(1)(ii) and (2) of Commercial Computer Software -
Restricted Rights at 48 CFR 52.227-19, as applicable, supplier is
Microline Software, 41 Sutter St Suite 1374, San Francisco, CA 94104.
*/

#ifndef XmLGridPH
#define XmLGridPH

#include <Xm/XmP.h>
#include <stdlib.h>
#ifndef MOTIF11
#include <Xm/ManagerP.h>
#include <Xm/DrawP.h>
#include <Xm/DragC.h>
#include <Xm/DropTrans.h>
#endif

#include "Grid.h"

#ifdef XmL_ANSIC

void _XmLGridLayout(XmLGridWidget g);

void _XmLGridCellDrawBackground(XmLGridCell cell, Widget w,
	XRectangle *clipRect, XmLGridDrawStruct *ds);
void _XmLGridCellDrawBorders(XmLGridCell cell, Widget w,
	XRectangle *clipRect, XmLGridDrawStruct *ds);
void _XmLGridCellDrawValue(XmLGridCell cell, Widget w,
	XRectangle *clipRect, XmLGridDrawStruct *ds);
typedef int (*XmLGridPreLayoutProc)(XmLGridWidget g, int isVert);

typedef XmLGridRow (*XmLGridRowNewProc)(Widget grid);
typedef void (*XmLGridRowFreeProc)(XmLGridRow);
typedef void (*XmLGridGetRowValueMaskProc)(XmLGridWidget g,
	char *s, long *mask);
typedef void (*XmLGridGetRowValueProc)(XmLGridWidget g, XmLGridRow row,
	XtArgVal value, long mask);
typedef int (*XmLGridSetRowValuesProc)(XmLGridWidget g,
	XmLGridRow row, long mask);

typedef XmLGridColumn (*XmLGridColumnNewProc)(Widget grid);
typedef void (*XmLGridColumnFreeProc)(XmLGridColumn);
typedef void (*XmLGridGetColumnValueMaskProc)(XmLGridWidget g,
	char *s, long *mask);
typedef void (*XmLGridGetColumnValueProc)(XmLGridWidget g, XmLGridColumn col,
	XtArgVal value, long mask);
typedef int (*XmLGridSetColumnValuesProc)(XmLGridWidget g,
	XmLGridColumn col, long mask);

typedef int (*XmLGridSetCellValuesResizeProc)(XmLGridWidget g,
	XmLGridRow row, XmLGridColumn col, XmLGridCell cell, long mask);
typedef int (*XmLGridCellActionProc)(XmLGridCell, Widget,
	XmLGridCallbackStruct *);

#else

void _XmLGridLayout();

void _XmLGridCellDrawBackground();
void _XmLGridCellDrawBorders();
void _XmLGridCellDrawValue();
typedef int (*XmLGridPreLayoutProc)();

typedef XmLGridRow (*XmLGridRowNewProc)();
typedef void (*XmLGridRowFreeProc)() ;
typedef void (*XmLGridGetRowValueMaskProc)();
typedef void (*XmLGridGetRowValueProc)();
typedef int (*XmLGridSetRowValuesProc)();

typedef XmLGridColumn (*XmLGridColumnNewProc)() ;
typedef void (*XmLGridColumnFreeProc)() ;
typedef void (*XmLGridGetColumnValueMaskProc)();
typedef void (*XmLGridGetColumnValueProc)();
typedef int (*XmLGridSetColumnValuesProc)();

typedef int (*XmLGridSetCellValuesResizeProc)();
typedef int (*XmLGridCellActionProc)();

#endif

#define XmLGridClassPartOfWidget(w) \
	((XmLGridWidgetClass)XtClass(w))->grid_class

#define XmInheritGridPreLayout ((XmLGridPreLayoutProc)_XtInherit)

#define XmInheritGridRowNew ((XmLGridRowNewProc)_XtInherit)
#define XmInheritGridRowFree ((XmLGridRowFreeProc)_XtInherit)
#define XmInheritGridGetRowValueMask ((XmLGridGetRowValueMaskProc)_XtInherit)
#define XmInheritGridGetRowValue ((XmLGridGetRowValueProc)_XtInherit)
#define XmInheritGridSetRowValues ((XmLGridSetRowValuesProc)_XtInherit)

#define XmInheritGridColumnNew ((XmLGridColumnNewProc)_XtInherit)
#define XmInheritGridColumnFree ((XmLGridColumnFreeProc)_XtInherit)
#define XmInheritGridGetColumnValueMask \
	((XmLGridGetColumnValueMaskProc)_XtInherit)
#define XmInheritGridGetColumnValue ((XmLGridGetColumnValueProc)_XtInherit)
#define XmInheritGridSetColumnValues ((XmLGridSetColumnValuesProc)_XtInherit)

#define XmInheritGridSetCellValuesResize \
	((XmLGridSetCellValuesResizeProc)_XtInherit)
#define XmInheritGridCellAction ((XmLGridCellActionProc)_XtInherit)

/* row value mask for get/set values */
#define XmLGridRowHeight             (1L<<0)
#define XmLGridRowSizePolicy         (1L<<1)
#define XmLGridRowUserData           (1L<<2)
#define XmLGridRowValueMaskLen            3

/* column value mask for get/set values */
#define XmLGridColumnWidth           (1L<<0)
#define XmLGridColumnSizePolicy      (1L<<1)
#define XmLGridColumnUserData        (1L<<2)
#define XmLGridColumnValueMaskLen         3

/* flags for XmLGridCell flags member */
#define XmLGridCellSelectedFlag     (1 << 0)
#define XmLGridCellValueSetFlag     (1 << 1)
#define XmLGridCellInRowSpanFlag    (1 << 2)
#define XmLGridCellInColumnSpanFlag (1 << 3)

/* cell value mask for get/set values */
#define XmLGridCellAlignment         (1L<<0)
#define XmLGridCellBackground        (1L<<1)
#define XmLGridCellBottomBorderColor (1L<<2)
#define XmLGridCellBottomBorderType  (1L<<3)
#define XmLGridCellColumnSpan        (1L<<4)
#define XmLGridCellEditable          (1L<<5)
#define XmLGridCellFontList          (1L<<6)
#define XmLGridCellForeground        (1L<<7)
#define XmLGridCellLeftBorderColor   (1L<<8)
#define XmLGridCellLeftBorderType    (1L<<9)
#define XmLGridCellMarginBottom      (1L<<10)
#define XmLGridCellMarginLeft        (1L<<11)
#define XmLGridCellMarginRight       (1L<<12)
#define XmLGridCellMarginTop         (1L<<13)
#define XmLGridCellPixmapF           (1L<<14)
#define XmLGridCellPixmapMask        (1L<<15)
#define XmLGridCellRightBorderColor  (1L<<16)
#define XmLGridCellRightBorderType   (1L<<17)
#define XmLGridCellRowSpan           (1L<<18)
#define XmLGridCellString            (1L<<19)
#define XmLGridCellToggleSet         (1L<<20)
#define XmLGridCellTopBorderColor    (1L<<21)
#define XmLGridCellTopBorderType     (1L<<22)
#define XmLGridCellType              (1L<<23)
#define XmLGridCellUserData          (1L<<24)

#define XmLICON_SPACING 4

enum { DrawAll, DrawHScroll, DrawVScroll, DrawRow, DrawCol, DrawCell };
enum { SelectRow, SelectCol, SelectCell };
enum { CursorNormal, CursorHResize, CursorVResize };
enum { InNormal, InSelect, InResize, InMove };
enum { DragLeft = 1, DragRight = 2, DragUp = 4, DragDown = 8 };

typedef struct
	{
	int x, y, width, height;
	int row, col, nrow, ncol;
	} GridReg;

typedef struct
	{
	int row, col;
	} GridDropLoc;

typedef struct
	{
	unsigned char alignment;
	Pixel background;
	Pixel bottomBorderColor;
	char bottomBorderType;
	Dimension bottomMargin;
	int columnSpan;
	Boolean editable;
	short fontHeight;
	XmFontList fontList;
	short fontWidth;
	Pixel foreground;
	Pixel leftBorderColor;
	char leftBorderType;
	Dimension leftMargin;
	int refCount;
	Pixel rightBorderColor;
	char rightBorderType;
	Dimension rightMargin;
	int rowSpan;
	Pixel topBorderColor;
	char topBorderType;
	Dimension topMargin;
	unsigned char type;
	void *userData;
	} XmLGridCellRefValues;

typedef struct
	{
	Pixmap pixmap, pixmask;
	Dimension width, height;
	} XmLGridCellPixmap;

typedef struct
	{
	XmString string;
	XmLGridCellPixmap pix;
	} XmLGridCellIcon;

typedef struct _XmLGridCellPart
	{
	XmLGridCellRefValues *refValues;
	unsigned char flags;
	void *value;
	} XmLGridCellPart;

struct _XmLGridCellRec
	{
	XmLGridCellPart cell;
	};

typedef struct _XmLGridRowPart
	{
	int pos;		/* required first for Array autonumber */
	Dimension height;
	unsigned char sizePolicy;
	Boolean selected;
	XtPointer userData;
	Dimension heightInPixels;
	unsigned int heightInPixelsValid:1;
	Widget grid;
	int visPos;
	XmLArray cellArray;
	} XmLGridRowPart;

struct _XmLGridRowRec
	{
	XmLGridRowPart grid;
	};

typedef struct _XmLGridColumnPart
	{
	int pos;		/* required first for Array autonumber */
	Dimension width;
	unsigned char sizePolicy;
	Boolean selected;
	XtPointer userData;
	XmLGridCellRefValues *defCellValues;
	Widget grid;
	Dimension widthInPixels;
	unsigned int widthInPixelsValid:1;
	int visPos;
	} XmLGridColumnPart;

struct _XmLGridColumnRec
	{
	XmLGridColumnPart grid;
	};

typedef struct _XmLGridPart
	{
	/* resource values */
	int leftFixedCount, rightFixedCount;
	int headingRowCount, footerRowCount;
	int topFixedCount, bottomFixedCount;
	int headingColCount, footerColCount;
	Dimension leftFixedMargin, rightFixedMargin;
	Dimension topFixedMargin, bottomFixedMargin;
	Dimension scrollBarMargin;
	Dimension highlightThickness;
	Dimension toggleSize;
	Dimension globalPixmapWidth, globalPixmapHeight;
	unsigned char selectionPolicy;
	Boolean layoutFrozen, immediateDraw;
	int debugLevel;
	unsigned char vsPolicy, hsPolicy;
	unsigned char hsbDisplayPolicy, vsbDisplayPolicy;
	int rowCount, colCount;
	int hiddenRowCount, hiddenColCount;
	int shadowRegions;
	unsigned char shadowType;
	Widget hsb, vsb, text;
	XmFontList fontList;
	Pixel blankBg, selectBg, selectFg;
	Pixel defaultCellBg, defaultCellFg;
	Pixel toggleTopColor, toggleBotColor;
	int visibleCols, visibleRows;
	char *simpleHeadings, *simpleWidths;
	XtTranslations editTrans, traverseTrans;
	Boolean allowRowHide, allowColHide;
	Boolean allowRowResize, allowColResize;
	Boolean allowDrag, allowDrop;
	Boolean autoSelect;
	Boolean highlightRowMode;
	Boolean useAvgWidth;
	int scrollRow, scrollCol, cScrollRow, cScrollCol;
	XtCallbackList addCallback, deleteCallback;
	XtCallbackList cellDrawCallback, cellFocusCallback;
	XtCallbackList cellDropCallback, cellPasteCallback;
	XtCallbackList cellStartDropCallback;
	XtCallbackList activateCallback, editCallback;
	XtCallbackList selectCallback, deselectCallback;
	XtCallbackList resizeCallback, scrollCallback;

	/* private data */
	GC gc;
	Cursor hResizeCursor, vResizeCursor;
	XFontStruct *fallbackFont;
	char ignoreModifyVerify;
	char focusIn, inEdit, inMode;
	char singleColScrollMode;
	int singleColScrollPos;
	char cursorDefined, textHidden, resizeIsVert;
	char mayHaveRowSpans;
	int layoutStack;
	char needsHorizLayout, needsVertLayout;
	char recalcHorizVisPos, recalcVertVisPos;
	char vertVisChangedHint;
	char dragTimerSet;
	XtIntervalId dragTimerId;
	int resizeRow, resizeCol, resizeLineXY;
	int extendRow, extendCol, extendToRow, extendToCol;
	Boolean extendSelect;
	int lastSelectRow, lastSelectCol;
	Time lastSelectTime;
	int focusRow, focusCol;
	XmLArray rowArray;
	XmLArray colArray;
	GridReg reg[9];
	GridDropLoc dropLoc;

	/* resources use by SetSubValues and GetSubValues */
	Boolean cellDefaults;
	int cellRow, cellCol;
	int cellColRangeStart, cellColRangeEnd;
	int cellRowRangeStart, cellRowRangeEnd;
	int rowStep, colStep;
	unsigned char rowType, colType;

	/* cell resources */
	XmString cellString;
	Boolean cellToggleSet;
	Pixmap cellPixmap, cellPixmapMask;
	Dimension cellPixmapWidth, cellPixmapHeight;
	XmLGridCellRefValues cellValues, *defCellValues;

	/* row resources */
	Dimension rowHeight;
	unsigned char rowSizePolicy;
	Boolean rowSelected;
	XtPointer rowUserData;

	/* column resources */
	Dimension colWidth;
	unsigned char colSizePolicy;
	Boolean colSelected;
	XtPointer colUserData;
	} XmLGridPart;

typedef struct _XmLGridRec
	{
	CorePart core;
	CompositePart composite;
	ConstraintPart constraint;
	XmManagerPart manager;
	XmLGridPart grid;
	} XmLGridRec;

typedef struct _XmLGridClassPart
	{
	int initialRows;
	int initialCols;
	XmLGridPreLayoutProc preLayoutProc;
	int rowRecSize;
	XmLGridRowNewProc rowNewProc;
	XmLGridRowFreeProc  rowFreeProc;
	XmLGridGetRowValueMaskProc getRowValueMaskProc;
	XmLGridGetRowValueProc getRowValueProc;
	XmLGridSetRowValuesProc  setRowValuesProc;
	int columnRecSize;
	XmLGridColumnNewProc columnNewProc;
	XmLGridColumnFreeProc columnFreeProc;
	XmLGridGetColumnValueMaskProc getColumnValueMaskProc;
	XmLGridGetColumnValueProc getColumnValueProc;
	XmLGridSetColumnValuesProc  setColumnValuesProc;
	XmLGridSetCellValuesResizeProc setCellValuesResizeProc;
	XmLGridCellActionProc cellActionProc;
	} XmLGridClassPart;

typedef struct _XmLGridClassRec
	{
	CoreClassPart core_class;
	CompositeClassPart composite_class;
	ConstraintClassPart constraint_class;
	XmManagerClassPart manager_class;
	XmLGridClassPart grid_class;
	} XmLGridClassRec;

extern XmLGridClassRec xmlGridClassRec;

typedef struct _XmLGridConstraintPart
	{
	int unused;
	} XmLGridConstraintPart;

typedef struct _XmLGridConstraintRec
	{
	XmManagerConstraintPart manager;
	XmLGridConstraintPart grid;
	} XmLGridConstraintRec, *XmLGridConstraintPtr;

#endif 