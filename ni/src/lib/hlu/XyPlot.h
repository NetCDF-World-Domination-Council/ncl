/*
 *      $Id: XyPlot.h,v 1.9 1995-03-15 11:49:00 boote Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		XyPlot.h
 *
 *	Author:		Ethan Alpert
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Wed Dec 30 13:07:27 MST 1992
 *
 *	Description:	public header for xyplotter
 */
#ifndef _NXyPlot_h
#define _NXyPlot_h

#include <ncarg/hlu/DataComm.h>
#include <ncarg/hlu/Overlay.h>
#include <ncarg/hlu/IrregularType2TransObj.h>
#include <ncarg/hlu/LogLinTransObj.h>

/*
 * Resource names
 */

/*
 * DataDep objects resources
 */
#define	NhlNxyDashPattern	"xyDashPattern"
#define	NhlCxyDashPattern	"XyDashPattern"
#define NhlNxyDashPatterns	"xyDashPatterns"
#define NhlCxyDashPatterns	"XyDashPatterns"
#define	NhlNxyMonoDashPattern	"xyMonoDashPattern"
#define	NhlCxyMonoDashPattern	"XyMonoDashPattern"

#define NhlNxyMarkLineMode	"xyMarkLineMode"
#define NhlCxyMarkLineMode	"XyMarkLineMode"
#define NhlNxyMarkLineModes	"xyMarkLineModes"
#define NhlCxyMarkLineModes	"XyMarkLineModes"
#define NhlNxyMonoMarkLineMode	"xyMonoMarkLineMode"
#define NhlCxyMonoMarkLineMode	"XyMonoMarkLineMode"

#define NhlNxyExplicitLegendLabels	"xyExplicitLegendLabels"
#define NhlCxyExplicitLegendLabels	"XyExplicitLegendLabels"

#define	NhlNxyLineColor		"xyLineColor"
#define	NhlCxyLineColor		"XyLineColor"
#define NhlNxyLineColors	"xyLineColors"
#define NhlCxyLineColors	"XyLineColors"
#define	NhlNxyMonoLineColor	"xyMonoLineColor"
#define	NhlCxyMonoLineColor	"XyMonoLineColor"

#define NhlNxyLineLabelColor		"xyLineLabelColor"
#define NhlCxyLineLabelColor		"XyLineLabelColor"
#define NhlNxyLineLabelColors		"xyLineLabelColors"
#define NhlCxyLineLabelColors		"XyLineLabelColors"
#define NhlNxyMonoLineLabelColor	"xyMonoLineLabelColor"
#define NhlCxyMonoLineLabelColor	"XyMonoLineLabelColor"

#define NhlNxyLabelMode		"xyLabelMode"
#define NhlCxyLabelMode		"XyLabelMode"

#define NhlNxyExplicitLabels	"xyExplicitLabels"
#define NhlCxyExplicitLabels	"XyExplicitLabels"

#define NhlNxyLineThicknessF	"xyLineThicknessF"
#define NhlCxyLineThicknessF	"XyLineThicknessF"
#define NhlNxyLineThicknesses	"xyLineThicknesses"
#define NhlCxyLineThicknesses	"XyLineThicknesses"
#define NhlNxyMonoLineThickness	"xyMonoLineThickness"
#define NhlCxyMonoLineThickness	"XyMonoLineThickness"

#define NhlNxyMarker		"xyMarker"
#define NhlCxyMarker		"XyMarker"
#define NhlNxyMarkers		"xyMarkers"
#define NhlCxyMarkers		"XyMarkers"
#define NhlNxyMonoMarker	"xyMonoMarker"
#define NhlCxyMonoMarker	"XyMonoMarker"

#define NhlNxyMarkerColor	"xyMarkerColor"
#define NhlCxyMarkerColor	"XyMarkerColor"
#define NhlNxyMarkerColors	"xyMarkerColors"
#define NhlCxyMarkerColors	"XyMarkerColors"
#define NhlNxyMonoMarkerColor	"xyMonoMarkerColor"
#define NhlCxyMonoMarkerColor	"XyMonoMarkerColor"

#define NhlNxyMarkerSizeF	"xyMarkerSizeF"
#define NhlCxyMarkerSizeF	"XyMarkerSizeF"
#define NhlNxyMarkerSizes	"xyMarkerSizes"
#define NhlCxyMarkerSizes	"XyMarkerSizes"
#define NhlNxyMonoMarkerSize	"xyMonoMarkerSize"
#define NhlCxyMonoMarkerSize	"XyMonoMarkerSize"

#define NhlNxyMarkerThicknessF		"xyMarkerThicknessF"
#define NhlCxyMarkerThicknessF		"XyMarkerThicknessF"
#define NhlNxyMarkerThicknesses		"xyMarkerThicknesses"
#define NhlCxyMarkerThicknesses		"XyMarkerThicknesses"
#define NhlNxyMonoMarkerThickness	"xyMonoMarkerThickness"
#define NhlCxyMonoMarkerThickness	"XyMonoMarkerThickness"

/*
 * XyPlot's resource names
 */

#define NhlNxyCoordData		"xyCoordData"
#define NhlCxyCoordData		"XyCoordData"

#define NhlNxyCoordDataSpec	"xyCoordDataSpec"
#define NhlCxyCoordDataSpec	"XyCoordDataSpec"

#define NhlNxyXStyle		"xyXStyle"
#define NhlCxyXStyle		"XyXStyle"

#define NhlNxyYStyle		"xyYStyle"
#define NhlCxyYStyle		"XyYStyle"

#define NhlNxyXIrregularPoints	"xyXIrregularPoints"
#define NhlCxyXIrregularPoints	"XyXIrregularPoints"

#define NhlNxyYIrregularPoints	"xyYIrregularPoints"
#define NhlCxyYIrregularPoints	"XyYIrregularPoints"

#define	NhlNxyComputeXMin	"xyComputeXMin"
#define	NhlCxyComputeXMin	"XyComputeXMin"

#define	NhlNxyComputeXMax	"xyComputeXMax"
#define	NhlCxyComputeXMax	"XyComputeXMax"

#define	NhlNxyComputeYMin	"xyComputeYMin"
#define	NhlCxyComputeYMin	"XyComputeYMin"

#define	NhlNxyComputeYMax	"xyComputeYMax"
#define	NhlCxyComputeYMax	"XyComputeYMax"

/*
 * These resources have not been implimented yet.
 */
#ifdef	NOT
/*
 * New types
 */
typedef enum _NhlAlternatePlace{
	NhlNONE,
	NhlLEFTAXIS,
	NhlRIGHTAXIS,
	NhlTOPAXIS,
	NhlBOTTOMAXIS
} NhlAlternatePlace;

#define NhlTAlternatePlace	"alternatePlace"

#define NhlNxyXAlternate	"xyXAlternate"
#define NhlCxyXAlternate	"XyXAlternate"

#define NhlNxyYAlternate	"xyYAlternate"
#define NhlCxyYAlternate	"XyYAlternate"

#define NhlNxyXAlternateCoords	"xyXAlternateCoords"
#define NhlCxyXAlternateCoords	"XyXAlternateCoords"

#define NhlNxyXOriginalCoords	"xyXOriginalCoords"
#define NhlCxyXOriginalCoords	"XyXOriginalCoords"

#define NhlNxyYAlternateCoords	"xyYAlternateCoords"
#define NhlCxyYAlternateCoords	"XyYAlternateCoords"

#define NhlNxyYOriginalCoords	"xyYOriginalCoords"
#define NhlCxyYOriginalCoords	"XyYOriginalCoords"
#endif	/* NOT */

#define NhlNxyLineDashSegLenF	"xyLineDashSegLenF"
#define NhlCxyLineDashSegLenF	"XyLineDashSegLenF"

#define NhlNxyLineLabelFontHeightF	"xyLineLabelFontHeightF"
#define NhlCxyLineLabelFontHeightF	"XyLineLabelFontHeightF"


/*
 * Names for new types.
 */
#define NhlTLineLabelMode	"lineLabelMode"

typedef enum _NhlLineLabelMode{
	NhlNOLABELS,
	NhlLETTERED,
	NhlCUSTOM
} NhlLineLabelMode;

extern NhlLayerClass NhlxyPlotLayerClass;

#endif /* _NXyPlot_h */
