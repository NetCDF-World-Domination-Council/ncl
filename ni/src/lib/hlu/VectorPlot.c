/*
 *      $Id: VectorPlot.c,v 1.12 1996-04-06 03:49:11 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		VectorPlot.c
 *
 *	Author:		David Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Thu Sep 28 11:02:12 MDT 1995
 *
 *	Description:	Creates and manages a VectorPlot plot object
 */

#include <stdio.h>
#include <math.h>

#include <ncarg/hlu/hluutil.h>
#include <ncarg/hlu/VectorPlotP.h>
#include <ncarg/hlu/Workstation.h>
#include <ncarg/hlu/IrregularTransObjP.h>
#include <ncarg/hlu/MapTransObj.h>
#include <ncarg/hlu/ConvertersP.h>
#include <ncarg/hlu/FortranP.h>

#define N 6
#define M 6
static float U[N*M],V[N*M];
static	float Min_Mag = 1e12, Max_Mag = 0.0;

/*
 * Function:	ResourceUnset
 *
 * Description:	This function can be used to determine if a resource has
 *		been set at initialize time either in the Create call or
 *		from a resource data base. In order to use it a Boolean
 *		variable (by convention '<var_name>_set')
 *		MUST directly proceed the declaration of the subject
 *		resource variable in the LayerPart struct. Also a .nores 
 *		NhlResource struct for the <var_name>_set variable
 *		must directly preceed the Resource of interest in the 
 *		Resource initialization list of this module.
 *
 * In Args:	
 *		NrmName		name,
 *		NrmClass	class,
 *		NhlPointer	base,
 *		unsigned int	offset
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */

/*ARGSUSED*/
static NhlErrorTypes
ResourceUnset
#if	NhlNeedProto
(
	NrmName		name,
	NrmClass	class,
	NhlPointer	base,
	unsigned int	offset
)
#else
(name,class,base,offset)
	NrmName		name;
	NrmClass	class;
	NhlPointer	base;
	unsigned int	offset;
#endif
{
	char *cl = (char *) base;
	NhlBoolean *set = (NhlBoolean *)(cl + offset - sizeof(NhlBoolean));

	*set = False;

	return NhlNOERROR;
}

#define	Oset(field)	NhlOffset(NhlVectorPlotDataDepLayerRec,vcdata.field)
static NhlResource data_resources[] = {

	{NhlNvcFoo,NhlCvcFoo,NhlTInteger,sizeof(int),
		 Oset(foo),NhlTImmediate,_NhlUSET((NhlPointer)NULL),0,NULL}
};
#undef Oset

#define Oset(field)     NhlOffset(NhlVectorPlotLayerRec,vectorplot.field)
static NhlResource resources[] = {

/* Begin-documented-resources */

/* Data resources */

	{NhlNvcVectorFieldData,NhlCvcVectorFieldData,_NhlTDataList,
		 sizeof(NhlGenArray),
		 Oset(vector_field_data),NhlTImmediate,_NhlUSET(NULL),0,
						(NhlFreeFunc)NhlFreeGenArray},
	{NhlNvcScalarFieldData,NhlCvcScalarFieldData,_NhlTDataList,
		 sizeof(NhlGenArray),
		 Oset(scalar_field_data),NhlTImmediate,_NhlUSET(NULL),0,
						(NhlFreeFunc)NhlFreeGenArray},

	{NhlNvcMapDirection,NhlCvcMapDirection,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(map_direction),NhlTImmediate,
		  _NhlUSET((NhlPointer) True),0,NULL},
	{NhlNvcPositionMode,NhlCvcPositionMode,
		  NhlTVectorPositionMode,sizeof(NhlVectorPositionMode),
		  Oset(position_mode),NhlTImmediate,
		  _NhlUSET((NhlPointer) NhlARROWCENTER),0,NULL},
 	{NhlNvcVectorDrawOrder,NhlCvcVectorDrawOrder,NhlTDrawOrder,
		 sizeof(NhlDrawOrder),Oset(vector_order),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlDRAW),0,NULL},


	{ NhlNvcMinDistanceF,NhlCvcMinDistanceF,
		  NhlTFloat,sizeof(float),Oset(min_distance),NhlTString,
		  _NhlUSET("0.0"),0,NULL},
	{ NhlNvcMinMagnitudeF,NhlCvcMinMagnitudeF,
		  NhlTFloat,sizeof(float),Oset(min_magnitude),NhlTString,
		  _NhlUSET("0.0"),0,NULL},
	{ NhlNvcMaxMagnitudeF,NhlCvcMaxMagnitudeF,
		  NhlTFloat,sizeof(float),Oset(max_magnitude),NhlTString,
		  _NhlUSET("0.0"),0,NULL},
	{ NhlNvcRefMagnitudeF,NhlCvcRefMagnitudeF,
		  NhlTFloat,sizeof(float),Oset(ref_magnitude),NhlTString,
		  _NhlUSET("0.0"),0,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(ref_length_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),0,NULL},
	{ NhlNvcRefLengthF,NhlCvcRefLengthF,
		  NhlTFloat,sizeof(float),Oset(ref_length),
		  NhlTProcedure,_NhlUSET((NhlPointer)ResourceUnset),0,NULL},
	{ NhlNvcMinFracLengthF,NhlCvcMinFracLengthF,
		  NhlTFloat,sizeof(float),Oset(min_frac_len),NhlTString,
		  _NhlUSET("0.0"),0,NULL},

/* Level resources */

	{NhlNvcLevels, NhlCvcLevels,  NhlTFloatGenArray,
		 sizeof(NhlPointer),Oset(levels),
		 NhlTImmediate,_NhlUSET((NhlPointer) NULL),0,
		 (NhlFreeFunc)NhlFreeGenArray},
	{ NhlNvcLevelCount,NhlCvcLevelCount,NhlTInteger,sizeof(int),
		  Oset(level_count),NhlTImmediate,
		  _NhlUSET((NhlPointer) 16),_NhlRES_GONLY,NULL},
	{ NhlNvcLevelSelectionMode,NhlCvcLevelSelectionMode,
		  NhlTLevelSelectionMode,sizeof(NhlLevelSelectionMode),
		  Oset(level_selection_mode),
		  NhlTImmediate,
		  _NhlUSET((NhlPointer) NhlAUTOMATICLEVELS),0,NULL},
	{ NhlNvcMaxLevelCount,NhlCvcMaxLevelCount,NhlTInteger,sizeof(int),
		  Oset(max_level_count),NhlTImmediate,
		  _NhlUSET((NhlPointer) 16),0,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(level_spacing_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),0,NULL},
	{ NhlNvcLevelSpacingF,NhlCvcLevelSpacingF,NhlTFloat,sizeof(float),
		  Oset(level_spacing),NhlTProcedure,
		  _NhlUSET((NhlPointer)ResourceUnset),0,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(min_level_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),0,NULL},
	{ NhlNvcMinLevelValF,NhlCvcMinLevelValF,NhlTFloat,sizeof(float),
		  Oset(min_level_val),
		  NhlTProcedure,_NhlUSET((NhlPointer)ResourceUnset),0,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(max_level_set),NhlTImmediate,
		 _NhlUSET((NhlPointer)True),0,NULL},
	{ NhlNvcMaxLevelValF,NhlCvcMaxLevelValF,NhlTFloat,sizeof(float),
		  Oset(max_level_val),
		  NhlTProcedure,_NhlUSET((NhlPointer)ResourceUnset),0,NULL},
	{NhlNvcLevelColors, NhlCvcLevelColors, NhlTColorIndexGenArray,
		 sizeof(NhlGenArray),Oset(level_colors),
		 NhlTImmediate,_NhlUSET((NhlPointer) NULL),0,
		 (NhlFreeFunc)NhlFreeGenArray},
	{NhlNvcUseScalarArray,NhlCvcUseScalarArray,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(use_scalar_array),NhlTImmediate,
		  _NhlUSET((NhlPointer) False),0,NULL},
	{NhlNvcScalarMissingValColor,NhlCvcScalarMissingValColor, 
		 NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(scalar_mval_color),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},

	{NhlNvcMonoLineArrowColor,NhlCvcMonoLineArrowColor,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(mono_l_arrow_color),NhlTImmediate,
		  _NhlUSET((NhlPointer) True),0,NULL},
	{NhlNvcLineArrowColor, NhlCvcLineArrowColor, NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(l_arrow_color),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{NhlNvcLineArrowThicknessF,NhlCvcLineArrowThicknessF,
		  NhlTFloat,sizeof(float),Oset(l_arrow_thickness),NhlTString,
		  _NhlUSET("1.0"),0,NULL},
	{NhlNvcLineArrowHeadMinSizeF,NhlCvcLineArrowHeadMinSizeF,
		  NhlTFloat,sizeof(float),Oset(l_arrowhead_min_size),
		 NhlTString,_NhlUSET("0.005"),0,NULL},
	{NhlNvcLineArrowHeadMaxSizeF,NhlCvcLineArrowHeadMaxSizeF,
		  NhlTFloat,sizeof(float),Oset(l_arrowhead_max_size),
		 NhlTString,_NhlUSET("0.05"),0,NULL},

	{NhlNvcFillArrowsOn,NhlCvcFillArrowsOn,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(fill_arrows_on),NhlTImmediate,
		  _NhlUSET((NhlPointer) False),0,NULL},
	{NhlNvcMonoFillArrowFillColor,NhlCvcMonoFillArrowFillColor,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(mono_f_arrow_fill_color),NhlTImmediate,
		  _NhlUSET((NhlPointer) True),0,NULL},
	{NhlNvcFillArrowFillColor, NhlCvcFillArrowFillColor, NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(f_arrow_fill_color),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{NhlNvcFillOverEdge,NhlCvcFillOverEdge,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(fill_over_edge),NhlTImmediate,
		  _NhlUSET((NhlPointer) True),0,NULL},
	{NhlNvcMonoFillArrowEdgeColor,NhlCvcMonoFillArrowEdgeColor,
		  NhlTBoolean,sizeof(NhlBoolean),
		  Oset(mono_f_arrow_edge_color),NhlTImmediate,
		  _NhlUSET((NhlPointer) True),0,NULL},
	{NhlNvcFillArrowEdgeColor, NhlCvcFillArrowEdgeColor, NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(f_arrow_edge_color),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlBACKGROUND),0,NULL},
	{NhlNvcFillArrowEdgeThicknessF,NhlCvcFillArrowEdgeThicknessF,
		  NhlTFloat,sizeof(float),Oset(f_arrow_edge_thickness),
		 NhlTString,_NhlUSET("2.0"),0,NULL},
	{NhlNvcFillArrowWidthF,NhlCvcFillArrowWidthF,
		  NhlTFloat,sizeof(float),Oset(f_arrow_width),NhlTString,
		  _NhlUSET("0.1"),0,NULL},
	{NhlNvcFillArrowMinFracWidthF,NhlCvcFillArrowMinFracWidthF,
		  NhlTFloat,sizeof(float),Oset(f_arrow_min_width),NhlTString,
		  _NhlUSET("0.25"),0,NULL},
	{NhlNvcFillArrowHeadXF,NhlCvcFillArrowHeadXF,
		  NhlTFloat,sizeof(float),Oset(f_arrowhead_x),NhlTString,
		  _NhlUSET("0.36"),0,NULL},
	{NhlNvcFillArrowHeadMinFracXF,NhlCvcFillArrowHeadMinFracXF,
		  NhlTFloat,sizeof(float),Oset(f_arrowhead_min_x),NhlTString,
		  _NhlUSET("0.25"),0,NULL},
	{NhlNvcFillArrowHeadInteriorXF,NhlCvcFillArrowHeadInteriorXF,
		  NhlTFloat,sizeof(float),Oset(f_arrowhead_interior),
		 NhlTString,_NhlUSET("0.33"),0,NULL},
	{NhlNvcFillArrowHeadYF,NhlCvcFillArrowHeadYF,
		  NhlTFloat,sizeof(float),Oset(f_arrowhead_y),NhlTString,
		  _NhlUSET("0.12"),0,NULL},
	{NhlNvcFillArrowHeadMinFracYF,NhlCvcFillArrowHeadMinFracYF,
		  NhlTFloat,sizeof(float),Oset(f_arrowhead_min_y),NhlTString,
		  _NhlUSET("0.25"),0,NULL},

	{NhlNvcUseRefAnnoRes,NhlCvcUseRefAnnoRes,NhlTBoolean,
		 sizeof(NhlBoolean),Oset(use_refvec_anno_attrs),
		 NhlTImmediate,_NhlUSET((NhlPointer) False),0,NULL},


/* Reference Annotation #1 resources */

	{NhlNvcRefAnnoOn,NhlCvcRefAnnoOn,NhlTBoolean,sizeof(NhlBoolean),
		 Oset(refvec_anno.on),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNvcRefAnnoOrientation,NhlCvcRefAnnoOrientation,NhlTOrientation,
		 sizeof(NhlOrientation),
		 Oset(refvec_anno.orientation),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlVERTICAL),0,NULL},
        {NhlNvcRefAnnoExplicitMagnitudeF,NhlCvcRefAnnoExplicitMagnitudeF,
		 NhlTFloat,sizeof(float),Oset(ref_attrs.vec_mag),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
        {NhlNvcRefAnnoArrowLineColor,NhlCvcRefAnnoArrowLineColor,
		 NhlTColorIndex,sizeof(NhlColorIndex),
		 Oset(ref_attrs.arrow_line_color),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlFOREGROUND),0,NULL},
        {NhlNvcRefAnnoArrowFillColor,NhlCvcRefAnnoArrowFillColor,
		 NhlTColorIndex,sizeof(NhlColorIndex),
		 Oset(ref_attrs.arrow_fill_color),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlFOREGROUND),0,NULL},
        {NhlNvcRefAnnoArrowEdgeColor,NhlCvcRefAnnoArrowEdgeColor,
		 NhlTColorIndex,sizeof(NhlColorIndex),
		 Oset(ref_attrs.arrow_edge_color),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlBACKGROUND),0,NULL},
        {NhlNvcRefAnnoArrowUseVecColor,NhlCvcRefAnnoArrowUseVecColor,
		 NhlTBoolean,sizeof(NhlBoolean),
		 Oset(ref_attrs.use_vec_color),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
        {NhlNvcRefAnnoArrowAngleF,NhlCvcRefAnnoArrowAngleF,
		 NhlTFloat,sizeof(float),Oset(ref_attrs.arrow_angle),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
        {NhlNvcRefAnnoArrowSpaceF,NhlCvcRefAnnoArrowSpaceF,
		 NhlTFloat,sizeof(float),Oset(ref_attrs.arrow_space),
		 NhlTString,_NhlUSET("2.0"),0,NULL},
        {NhlNvcRefAnnoArrowMinOffsetF,NhlCvcRefAnnoArrowMinOffsetF,
		 NhlTFloat,sizeof(float),Oset(ref_attrs.arrow_min_offset),
		 NhlTString,_NhlUSET("0.25"),0,NULL},
	{NhlNvcRefAnnoString1On,NhlCvcRefAnnoString1On,
		 NhlTBoolean,sizeof(NhlBoolean),Oset(refvec_anno.string1_on),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNvcRefAnnoString1,NhlCvcRefAnnoString1,
		 NhlTString,sizeof(NhlString),
		 Oset(refvec_anno.string1),NhlTImmediate,_NhlUSET(NULL),0,
		 (NhlFreeFunc)NhlFree},
	{NhlNvcRefAnnoString2On,NhlCvcRefAnnoString2On,
		 NhlTBoolean,sizeof(NhlBoolean),Oset(refvec_anno.string2_on),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNvcRefAnnoString2,NhlCvcRefAnnoString2,
		 NhlTString,sizeof(NhlString),
		 Oset(refvec_anno.string2),NhlTImmediate,_NhlUSET(NULL),0,
		 (NhlFreeFunc)NhlFree},
#if 0
	{NhlNvcRefAnnoFormat,NhlCvcRefAnnoFormat,
		 NhlTString,sizeof(NhlString),
		 Oset(refvec_anno.format.fstring),NhlTImmediate,
		 _NhlUSET(NhlvcDEF_FORMAT),0,(NhlFreeFunc)NhlFree},
#endif
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(refvec_anno.height_set),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
        {NhlNvcRefAnnoFontHeightF,NhlCvcRefAnnoFontHeightF,
		 NhlTFloat,sizeof(float),Oset(refvec_anno.height),
		 NhlTProcedure,_NhlUSET((NhlPointer)ResourceUnset),0,NULL},
        {NhlNvcRefAnnoTextDirection,NhlCvcRefAnnoTextDirection,
		 NhlTTextDirection,sizeof(NhlTextDirection),
		 Oset(refvec_anno.direction),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlACROSS),0,NULL},
	{NhlNvcRefAnnoFont,NhlCvcRefAnnoFont,NhlTFont, 
		 sizeof(int),Oset(refvec_anno.font),
		 NhlTImmediate,_NhlUSET((NhlPointer) 0),0,NULL},
	{NhlNvcRefAnnoFontColor,NhlCvcRefAnnoFontColor,NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(refvec_anno.color),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{NhlNvcRefAnnoFontAspectF,NhlCvcRefAnnoFontAspectF,NhlTFloat, 
		 sizeof(float),Oset(refvec_anno.aspect),
		 NhlTString, _NhlUSET("1.3125"),0,NULL},
	{NhlNvcRefAnnoFontThicknessF,NhlCvcRefAnnoFontThicknessF,
		 NhlTFloat,sizeof(float),Oset(refvec_anno.thickness),
		 NhlTString, _NhlUSET("1.0"),0,NULL},
	{NhlNvcRefAnnoFontQuality,NhlCvcRefAnnoFontQuality,
		 NhlTFontQuality,sizeof(NhlFontQuality),
		 Oset(refvec_anno.quality),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlHIGH),0,NULL},
	{NhlNvcRefAnnoConstantSpacingF,NhlCvcRefAnnoConstantSpacingF,
		 NhlTFloat,sizeof(float),Oset(refvec_anno.cspacing),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
	{NhlNvcRefAnnoAngleF,NhlCvcRefAnnoAngleF,
		 NhlTFloat,sizeof(float),Oset(refvec_anno.angle),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
	{NhlNvcRefAnnoFuncCode,NhlCvcRefAnnoFuncCode,NhlTCharacter, 
		 sizeof(char),Oset(refvec_anno.fcode[0]),
		 NhlTString, _NhlUSET(":"),0,NULL},
	{NhlNvcRefAnnoBackgroundColor,NhlCvcRefAnnoBackgroundColor,
		  NhlTColorIndex,sizeof(NhlColorIndex),
		  Oset(refvec_anno.back_color),NhlTImmediate,
		  _NhlUSET((NhlPointer) NhlBACKGROUND),0,NULL},
	{ NhlNvcRefAnnoPerimOn,NhlCvcRefAnnoPerimOn,
                  NhlTBoolean,sizeof(NhlBoolean),
		Oset(refvec_anno.perim_on),
		NhlTImmediate,_NhlUSET((NhlPointer) True),0,NULL},
	{ NhlNvcRefAnnoPerimSpaceF,NhlCvcRefAnnoPerimSpaceF,
		  NhlTFloat,sizeof(float),Oset(refvec_anno.perim_space),
		  NhlTString,_NhlUSET("0.33"),0,NULL},
	{NhlNvcRefAnnoPerimColor,NhlCvcRefAnnoPerimColor,NhlTColorIndex,
		  sizeof(NhlColorIndex),Oset(refvec_anno.perim_lcolor),
		  NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{NhlNvcRefAnnoPerimThicknessF,NhlCvcRefAnnoFontThicknessF,
		 NhlTFloat,sizeof(float),Oset(refvec_anno.perim_lthick),
		 NhlTString, _NhlUSET("1.0"),0,NULL},

	{NhlNvcRefAnnoZone,NhlCvcRefAnnoZone,NhlTInteger,
		 sizeof(int),Oset(refvec_anno_rec.zone),NhlTImmediate,
		 _NhlUSET((NhlPointer) 3),0,NULL},
	{NhlNvcRefAnnoSide,NhlCvcRefAnnoSide,NhlTPosition,
		 sizeof(NhlPosition),Oset(refvec_anno_rec.side),NhlTImmediate,
		 _NhlUSET((NhlPointer)NhlBOTTOM),0,NULL},
	{NhlNvcRefAnnoJust,NhlCvcRefAnnoJust,
		 NhlTJustification,sizeof(NhlJustification),
		 Oset(refvec_anno_rec.just),NhlTImmediate,
		 _NhlUSET((NhlPointer)NhlTOPRIGHT),0,NULL},
	{NhlNvcRefAnnoParallelPosF,NhlCvcRefAnnoParallelPosF,NhlTFloat,
		 sizeof(float),Oset(refvec_anno_rec.para_pos),NhlTString,
		 _NhlUSET("1.0"),0,NULL},
	{NhlNvcRefAnnoOrthogonalPosF,NhlCvcRefAnnoOrthogonalPosF,
		 NhlTFloat,
		 sizeof(float),Oset(refvec_anno_rec.ortho_pos),NhlTString,
		 _NhlUSET("0.02"),0,NULL},


/* Reference Annotation #2 resources */

	{NhlNvcMinAnnoOn,NhlCvcMinAnnoOn,NhlTBoolean,sizeof(NhlBoolean),
		 Oset(minvec_anno.on),
		 NhlTImmediate,_NhlUSET((NhlPointer)False),0,NULL},
	{NhlNvcMinAnnoOrientation,NhlCvcMinAnnoOrientation,NhlTOrientation,
		 sizeof(NhlOrientation),
		 Oset(minvec_anno.orientation),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlVERTICAL),0,NULL},
        {NhlNvcMinAnnoExplicitMagnitudeF,NhlCvcMinAnnoExplicitMagnitudeF,
		 NhlTFloat,sizeof(float),Oset(min_attrs.vec_mag),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
        {NhlNvcMinAnnoArrowLineColor,NhlCvcMinAnnoArrowLineColor,
		 NhlTColorIndex,sizeof(NhlColorIndex),
		 Oset(min_attrs.arrow_line_color),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlFOREGROUND),0,NULL},
        {NhlNvcMinAnnoArrowFillColor,NhlCvcMinAnnoArrowFillColor,
		 NhlTColorIndex,sizeof(NhlColorIndex),
		 Oset(min_attrs.arrow_fill_color),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlFOREGROUND),0,NULL},
        {NhlNvcMinAnnoArrowEdgeColor,NhlCvcMinAnnoArrowEdgeColor,
		 NhlTColorIndex,sizeof(NhlColorIndex),
		 Oset(ref_attrs.arrow_edge_color),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlBACKGROUND),0,NULL},
        {NhlNvcMinAnnoArrowUseVecColor,NhlCvcMinAnnoArrowUseVecColor,
		 NhlTBoolean,sizeof(NhlBoolean),
		 Oset(min_attrs.use_vec_color),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
        {NhlNvcMinAnnoArrowAngleF,NhlCvcMinAnnoArrowAngleF,
		 NhlTFloat,sizeof(float),Oset(min_attrs.arrow_angle),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
        {NhlNvcMinAnnoArrowSpaceF,NhlCvcMinAnnoArrowSpaceF,
		 NhlTFloat,sizeof(float),Oset(min_attrs.arrow_space),
		 NhlTString,_NhlUSET("2.0"),0,NULL},
        {NhlNvcMinAnnoArrowMinOffsetF,NhlCvcMinAnnoArrowMinOffsetF,
		 NhlTFloat,sizeof(float),Oset(min_attrs.arrow_min_offset),
		 NhlTString,_NhlUSET("0.25"),0,NULL},
	{NhlNvcMinAnnoString1On,NhlCvcMinAnnoString1On,
		 NhlTBoolean,sizeof(NhlBoolean),Oset(minvec_anno.string1_on),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNvcMinAnnoString1,NhlCvcMinAnnoString1,
		 NhlTString,sizeof(NhlString),
		 Oset(minvec_anno.string1),NhlTImmediate,_NhlUSET(NULL),0,
		 (NhlFreeFunc)NhlFree},
	{NhlNvcMinAnnoString2On,NhlCvcMinAnnoString2On,
		 NhlTBoolean,sizeof(NhlBoolean),Oset(minvec_anno.string2_on),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNvcMinAnnoString2,NhlCvcMinAnnoString2,
		 NhlTString,sizeof(NhlString),
		 Oset(minvec_anno.string2),NhlTImmediate,_NhlUSET(NULL),0,
		 (NhlFreeFunc)NhlFree},
#if 0
	{NhlNvcMinAnnoFormat,NhlCvcMinAnnoFormat,
		 NhlTString,sizeof(NhlString),
		 Oset(minvec_anno.format.fstring),NhlTImmediate,
		 _NhlUSET(NhlvcDEF_FORMAT),0,(NhlFreeFunc)NhlFree},
#endif
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(minvec_anno.height_set),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
        {NhlNvcMinAnnoFontHeightF,NhlCvcMinAnnoFontHeightF,
		 NhlTFloat,sizeof(float),Oset(minvec_anno.height),
		 NhlTProcedure,_NhlUSET((NhlPointer)ResourceUnset),0,NULL},
        {NhlNvcMinAnnoTextDirection,NhlCvcMinAnnoTextDirection,
		 NhlTTextDirection,sizeof(NhlTextDirection),
		 Oset(minvec_anno.direction),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlACROSS),0,NULL},
	{NhlNvcMinAnnoFont,NhlCvcMinAnnoFont,NhlTFont, 
		 sizeof(int),Oset(minvec_anno.font),
		 NhlTImmediate,_NhlUSET((NhlPointer) 0),0,NULL},
	{NhlNvcMinAnnoFontColor,NhlCvcMinAnnoFontColor,NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(minvec_anno.color),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{NhlNvcMinAnnoFontAspectF,NhlCvcMinAnnoFontAspectF,NhlTFloat, 
		 sizeof(float),Oset(minvec_anno.aspect),
		 NhlTString, _NhlUSET("1.3125"),0,NULL},
	{NhlNvcMinAnnoFontThicknessF,NhlCvcMinAnnoFontThicknessF,
		 NhlTFloat,sizeof(float),Oset(minvec_anno.thickness),
		 NhlTString, _NhlUSET("1.0"),0,NULL},
	{NhlNvcMinAnnoFontQuality,NhlCvcMinAnnoFontQuality,
		 NhlTFontQuality,sizeof(NhlFontQuality),
		 Oset(minvec_anno.quality),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlHIGH),0,NULL},
	{NhlNvcMinAnnoConstantSpacingF,NhlCvcMinAnnoConstantSpacingF,
		 NhlTFloat,sizeof(float),Oset(minvec_anno.cspacing),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
	{NhlNvcMinAnnoAngleF,NhlCvcMinAnnoAngleF,
		 NhlTFloat,sizeof(float),Oset(minvec_anno.angle),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
	{NhlNvcMinAnnoFuncCode,NhlCvcMinAnnoFuncCode,NhlTCharacter, 
		 sizeof(char),Oset(minvec_anno.fcode[0]),
		 NhlTString, _NhlUSET(":"),0,NULL},
	{NhlNvcMinAnnoBackgroundColor,NhlCvcMinAnnoBackgroundColor,
		  NhlTColorIndex,sizeof(NhlColorIndex),
		  Oset(minvec_anno.back_color),NhlTImmediate,
		  _NhlUSET((NhlPointer) NhlBACKGROUND),0,NULL},
	{ NhlNvcMinAnnoPerimOn,NhlCvcMinAnnoPerimOn,
                  NhlTBoolean,sizeof(NhlBoolean),
		Oset(minvec_anno.perim_on),
		NhlTImmediate,_NhlUSET((NhlPointer) True),0,NULL},
	{ NhlNvcMinAnnoPerimSpaceF,NhlCvcMinAnnoPerimSpaceF,
		  NhlTFloat,sizeof(float),Oset(minvec_anno.perim_space),
		  NhlTString,_NhlUSET("0.33"),0,NULL},
	{NhlNvcMinAnnoPerimColor,NhlCvcMinAnnoPerimColor,NhlTColorIndex,
		  sizeof(NhlColorIndex),Oset(minvec_anno.perim_lcolor),
		  NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{NhlNvcMinAnnoPerimThicknessF,NhlCvcMinAnnoFontThicknessF,
		 NhlTFloat,sizeof(float),Oset(minvec_anno.perim_lthick),
		 NhlTString, _NhlUSET("1.0"),0,NULL},

	{NhlNvcMinAnnoZone,NhlCvcMinAnnoZone,NhlTInteger,
		 sizeof(int),Oset(minvec_anno_rec.zone),NhlTImmediate,
		 _NhlUSET((NhlPointer) 5),0,NULL},
	{NhlNvcMinAnnoSide,NhlCvcMinAnnoSide,NhlTPosition,
		 sizeof(NhlPosition),Oset(minvec_anno_rec.side),NhlTImmediate,
		 _NhlUSET((NhlPointer)NhlBOTTOM),0,NULL},
	{NhlNvcMinAnnoJust,NhlCvcMinAnnoJust,
		 NhlTJustification,sizeof(NhlJustification),
		 Oset(minvec_anno_rec.just),NhlTImmediate,
		 _NhlUSET((NhlPointer)NhlTOPRIGHT),0,NULL},
	{NhlNvcMinAnnoParallelPosF,NhlCvcMinAnnoParallelPosF,NhlTFloat,
		 sizeof(float),Oset(minvec_anno_rec.para_pos),NhlTString,
		 _NhlUSET("1.0"),0,NULL},
	{NhlNvcMinAnnoOrthogonalPosF,NhlCvcMinAnnoOrthogonalPosF,
		 NhlTFloat,
		 sizeof(float),Oset(minvec_anno_rec.ortho_pos),NhlTString,
		 _NhlUSET("0.02"),0,NULL},

	{NhlNvcNoDataLabelOn,NhlCvcNoDataLabelOn,NhlTBoolean,
		 sizeof(NhlBoolean),Oset(zerof_lbl.string2_on),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNvcNoDataLabelString,NhlCvcNoDataLabelString,
		 NhlTString,sizeof(NhlString),Oset(zerof_lbl.string2),
		 NhlTImmediate,_NhlUSET(NULL),0,(NhlFreeFunc)NhlFree},

/* Constant field label resources */

	{NhlNvcZeroFLabelOn,NhlCvcZeroFLabelOn,NhlTBoolean,
		 sizeof(NhlBoolean),Oset(zerof_lbl.on),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNvcZeroFLabelString,NhlCvcZeroFLabelString,
		 NhlTString,sizeof(NhlString),Oset(zerof_lbl.string1),
		 NhlTImmediate,_NhlUSET(NULL),0,(NhlFreeFunc)NhlFree},
#if 0
	{NhlNvcZeroFLabelFormat,NhlCvcZeroFLabelFormat,
		 NhlTString,sizeof(NhlString),
		 Oset(zerof_lbl.format.fstring),NhlTImmediate,
		 _NhlUSET(NhlvcDEF_FORMAT),0,(NhlFreeFunc)NhlFree},
#endif
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(zerof_lbl.height_set),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
        {NhlNvcZeroFLabelFontHeightF,NhlCvcZeroFLabelFontHeightF,
		 NhlTFloat,sizeof(float),Oset(zerof_lbl.height),
		 NhlTProcedure,_NhlUSET((NhlPointer)ResourceUnset),0,NULL},
        {NhlNvcZeroFLabelTextDirection,NhlCvcZeroFLabelTextDirection,
		 NhlTTextDirection,sizeof(NhlTextDirection),
		 Oset(zerof_lbl.direction),
		 NhlTImmediate,_NhlUSET((NhlPointer)NhlACROSS),0,NULL},
	{NhlNvcZeroFLabelFont,NhlCvcZeroFLabelFont,NhlTFont, 
		 sizeof(int),Oset(zerof_lbl.font),
		 NhlTImmediate,_NhlUSET((NhlPointer) 0),0,NULL},
	{NhlNvcZeroFLabelFontColor,NhlCvcZeroFLabelFontColor,NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(zerof_lbl.color),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{NhlNvcZeroFLabelFontAspectF,NhlCvcZeroFLabelFontAspectF,NhlTFloat, 
		 sizeof(float),Oset(zerof_lbl.aspect),
		 NhlTString, _NhlUSET("1.3125"),0,NULL},
	{NhlNvcZeroFLabelFontThicknessF,NhlCvcZeroFLabelFontThicknessF,
		 NhlTFloat,sizeof(float),Oset(zerof_lbl.thickness),
		 NhlTString, _NhlUSET("1.0"),0,NULL},
	{NhlNvcZeroFLabelFontQuality,NhlCvcZeroFLabelFontQuality,
		 NhlTFontQuality, 
		 sizeof(NhlFontQuality),Oset(zerof_lbl.quality),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlHIGH),0,NULL},
	{NhlNvcZeroFLabelConstantSpacingF,NhlCvcZeroFLabelConstantSpacingF,
		 NhlTFloat,sizeof(float),Oset(zerof_lbl.cspacing),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
	{NhlNvcZeroFLabelAngleF,NhlCvcZeroFLabelAngleF,
		 NhlTFloat,sizeof(float),Oset(zerof_lbl.angle),
		 NhlTString,_NhlUSET("0.0"),0,NULL},
	{NhlNvcZeroFLabelFuncCode,NhlCvcZeroFLabelFuncCode,NhlTCharacter, 
		 sizeof(char),Oset(zerof_lbl.fcode[0]),
		 NhlTString, _NhlUSET(":"),0,NULL},
	{NhlNvcZeroFLabelBackgroundColor,NhlCvcZeroFLabelBackgroundColor,
		 NhlTColorIndex,sizeof(NhlColorIndex),
		 Oset(zerof_lbl.back_color),NhlTImmediate,
		 _NhlUSET((NhlPointer) NhlBACKGROUND),0,NULL},
	{NhlNvcZeroFLabelPerimOn,NhlCvcZeroFLabelPerimOn,
                 NhlTBoolean,sizeof(NhlBoolean),Oset(zerof_lbl.perim_on),
		 NhlTImmediate,_NhlUSET((NhlPointer) True),0,NULL},
	{NhlNvcZeroFLabelPerimSpaceF,NhlCvcZeroFLabelPerimSpaceF,
		 NhlTFloat,sizeof(float),Oset(zerof_lbl.perim_space),
		 NhlTString,_NhlUSET("0.33"),0,NULL},
	{NhlNvcZeroFLabelPerimColor,NhlCvcZeroFLabelPerimColor,
		 NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(zerof_lbl.perim_lcolor),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{NhlNvcZeroFLabelPerimThicknessF,NhlCvcZeroFLabelFontThicknessF,
		 NhlTFloat,sizeof(float),Oset(zerof_lbl.perim_lthick),
		 NhlTString, _NhlUSET("1.0"),0,NULL},

	{NhlNvcZeroFLabelZone,NhlCvcZeroFLabelZone,NhlTInteger,
		 sizeof(int),Oset(zerof_lbl_rec.zone),NhlTImmediate,
		 _NhlUSET((NhlPointer) 0),0,NULL},
	{NhlNvcZeroFLabelSide,NhlCvcZeroFLabelSide,NhlTPosition,
		 sizeof(NhlPosition),Oset(zerof_lbl_rec.side),NhlTImmediate,
		 _NhlUSET((NhlPointer)NhlBOTTOM),0,NULL},
	{NhlNvcZeroFLabelJust,NhlCvcZeroFLabelJust,
		 NhlTJustification,sizeof(NhlJustification),
		 Oset(zerof_lbl_rec.just),NhlTImmediate,
		 _NhlUSET((NhlPointer)NhlCENTERCENTER),0,NULL},
	{NhlNvcZeroFLabelParallelPosF,NhlCvcZeroFLabelParallelPosF,NhlTFloat,
		 sizeof(float),Oset(zerof_lbl_rec.para_pos),NhlTString,
		 _NhlUSET("0.0"),0,NULL},
	{NhlNvcZeroFLabelOrthogonalPosF,NhlCvcZeroFLabelOrthogonalPosF,
		 NhlTFloat,sizeof(float),Oset(zerof_lbl_rec.ortho_pos),
		 NhlTString,_NhlUSET("0.0"),0,NULL},

/* General numerical string format option */

	{NhlNvcMagnitudeScalingMode,NhlCvcMagnitudeScalingMode,
                 NhlTScalingMode,sizeof(NhlScalingMode),
                 Oset(mag_scale.mode),NhlTImmediate,
                 _NhlUSET((NhlPointer) NhlSCALEFACTOR),0,NULL},
        {NhlNvcMagnitudeScaleValueF,NhlCvcMagnitudeScaleValueF,
                 NhlTFloat,sizeof(float),Oset(mag_scale.scale_value),
                 NhlTString,_NhlUSET("1.0"),0,NULL},
        {NhlNvcMagnitudeScaleFactorF,NhlCvcMagnitudeScaleFactorF,
                 NhlTFloat,sizeof(float),Oset(mag_scale.scale_factor),
                 NhlTString,_NhlUSET("1.0"),_NhlRES_GONLY,NULL},
	{NhlNvcMagnitudeFormat,NhlCvcMagnitudeFormat,
		 NhlTString,sizeof(NhlString),
		 Oset(mag_scale.format.fstring),NhlTImmediate,
		 _NhlUSET("*+^sg"),0,(NhlFreeFunc)NhlFree},

	{NhlNvcScalarValueScalingMode,NhlCvcScalarValueScalingMode,
                 NhlTScalingMode,sizeof(NhlScalingMode),
                 Oset(svalue_scale.mode),NhlTImmediate,
                 _NhlUSET((NhlPointer) NhlSCALEFACTOR),0,NULL},
        {NhlNvcScalarValueScaleValueF,NhlCvcScalarValueScaleValueF,
                 NhlTFloat,sizeof(float),Oset(svalue_scale.scale_value),
                 NhlTString,_NhlUSET("1.0"),0,NULL},
        {NhlNvcScalarValueScaleFactorF,NhlCvcScalarValueScaleFactorF,
                 NhlTFloat,sizeof(float),Oset(svalue_scale.scale_factor),
                 NhlTString,_NhlUSET("1.0"),_NhlRES_GONLY,NULL},
	{NhlNvcScalarValueFormat,NhlCvcScalarValueFormat,
		 NhlTString,sizeof(NhlString),
		 Oset(svalue_scale.format.fstring),NhlTImmediate,
		 _NhlUSET("*+^sg"),0,(NhlFreeFunc)NhlFree},

	{NhlNvcExplicitLabelBarLabelsOn,NhlCvcExplicitLabelBarLabelsOn,
		 NhlTBoolean,sizeof(NhlBoolean),
		 Oset(explicit_lbar_labels_on),
		 NhlTImmediate,_NhlUSET((NhlPointer) False),0,NULL},
	{NhlNvcLabelBarEndLabelsOn,NhlCvcLabelBarEndLabelsOn,
		 NhlTBoolean,sizeof(NhlBoolean),
		 Oset(lbar_end_labels_on),
		 NhlTImmediate,_NhlUSET((NhlPointer) False),0,NULL},

/* General label resources */

	{NhlNvcLabelsOn,NhlCvcLabelsOn,NhlTBoolean,sizeof(NhlBoolean),
		 Oset(lbls.on),
		 NhlTImmediate,_NhlUSET((NhlPointer)False),0,NULL},
	{NhlNvcLabelsUseVectorColor,NhlCvcLabelsUseVectorColor,NhlTBoolean,
		 sizeof(NhlBoolean),Oset(labels_use_vec_color),
		 NhlTImmediate,_NhlUSET((NhlPointer) False),0,NULL},
	{NhlNvcLabelFontColor,NhlCvcLabelFontColor,NhlTColorIndex,
		 sizeof(NhlColorIndex),Oset(lbls.color),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlFOREGROUND),0,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(lbls.height_set),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
        {NhlNvcLabelFontHeightF,NhlCvcLabelFontHeightF,
		 NhlTFloat,sizeof(float),Oset(lbls.height),
		 NhlTProcedure,_NhlUSET((NhlPointer)ResourceUnset),0,NULL},

/* End-documented-resources */

	{NhlNvcDataChanged,NhlCvcDataChanged,NhlTBoolean,sizeof(NhlBoolean),
		 Oset(data_changed),
		 NhlTImmediate,_NhlUSET((NhlPointer) True),0,NULL},

/* Intercepted resources */

	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(x_min_set),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNtrXMinF,NhlCtrXMinF,NhlTFloat,sizeof(float),
		 Oset(x_min),NhlTProcedure,
		 _NhlUSET((NhlPointer)ResourceUnset),0,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(x_max_set),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{NhlNtrXMaxF,NhlCtrXMaxF,NhlTFloat,sizeof(float),
		 Oset(x_max),NhlTProcedure,
		 _NhlUSET((NhlPointer)ResourceUnset),0,NULL},
	{ NhlNtrXLog,NhlCtrXLog,NhlTBoolean,sizeof(NhlBoolean),
		Oset(x_log),NhlTImmediate,_NhlUSET((NhlPointer)False),0,NULL},
	{ NhlNtrXReverse,NhlCtrXReverse,NhlTBoolean,sizeof(NhlBoolean),
		Oset(x_reverse),
		  NhlTImmediate,_NhlUSET((NhlPointer)False),0,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(y_min_set),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{ NhlNtrYMinF,NhlCtrYMinF,NhlTFloat,sizeof(float),
		Oset(y_min),NhlTProcedure,
		 _NhlUSET((NhlPointer)ResourceUnset),0,NULL},
	{"no.res","No.res",NhlTBoolean,sizeof(NhlBoolean),
		 Oset(y_max_set),
		 NhlTImmediate,_NhlUSET((NhlPointer)True),0,NULL},
	{ NhlNtrYMaxF,NhlCtrYMaxF,NhlTFloat,sizeof(float),
		Oset(y_max),NhlTProcedure,
		 _NhlUSET((NhlPointer)ResourceUnset),0,NULL},
	{ NhlNtrYLog,NhlCtrYLog,NhlTBoolean,sizeof(NhlBoolean),
		Oset(y_log),NhlTImmediate,_NhlUSET((NhlPointer)False),0,NULL},
	{ NhlNtrYReverse,NhlCtrYReverse,NhlTBoolean,sizeof(NhlBoolean),
		Oset(y_reverse),
		NhlTImmediate,_NhlUSET((NhlPointer)False),0,NULL},

	{ NhlNpmLabelBarDisplayMode,NhlCpmLabelBarDisplayMode,
		 NhlTAnnotationDisplayMode,sizeof(NhlAnnotationDisplayMode),
		 Oset(display_labelbar),
		 NhlTImmediate,_NhlUSET((NhlPointer) NhlNEVER),0,NULL},
	{NhlNlbLabelStrings, NhlClbLabelStrings, NhlTStringGenArray,
		 sizeof(NhlPointer),Oset(lbar_labels_res),
		 NhlTImmediate,_NhlUSET((NhlPointer) NULL),0,
		 (NhlFreeFunc)NhlFreeGenArray},
	{NhlNlbLabelFuncCode, NhlClbLabelFuncCode, NhlTCharacter,
		 sizeof(char),Oset(lbar_func_code),
		 NhlTString,_NhlUSET(":"),0,NULL },
	{NhlNlbLabelAlignment,NhlClbLabelAlignment,NhlTlbLabelAlignmentMode, 
		 sizeof(NhllbLabelAlignmentMode), 
		 Oset(lbar_alignment),NhlTImmediate,
		 _NhlUSET((NhlPointer)NhlINTERIOREDGES),0,NULL},
		
	{NhlNpmLegendDisplayMode,NhlCpmLegendDisplayMode,
		  NhlTAnnotationDisplayMode,sizeof(NhlAnnotationDisplayMode),
		  Oset(display_legend),
		  NhlTImmediate,_NhlUSET((NhlPointer) NhlNOCREATE),0,NULL},
	{NhlNpmTickMarkDisplayMode,NhlCpmTickMarkDisplayMode,
		  NhlTAnnotationDisplayMode,sizeof(NhlAnnotationDisplayMode),
		  Oset(display_tickmarks),
		  NhlTImmediate,_NhlUSET((NhlPointer) NhlCONDITIONAL),0,NULL},
	{NhlNpmTitleDisplayMode,NhlCpmTitleDisplayMode,
		  NhlTAnnotationDisplayMode,sizeof(NhlAnnotationDisplayMode),
		  Oset(display_titles),
		  NhlTImmediate,_NhlUSET((NhlPointer) NhlCONDITIONAL),0,NULL},
	{ NhlNpmUpdateReq,NhlCpmUpdateReq,NhlTInteger,sizeof(int),
		  Oset(update_req),
		  NhlTImmediate,_NhlUSET((NhlPointer) False),0,NULL}
};
#undef Oset


typedef enum _vcCoord { vcXCOORD, vcYCOORD} vcCoord;

typedef enum _vcValueType { 
	vcMAGVAL,
	vcMAXMAG,
	vcREFMAG,
	vcMINMAG,
	vcMINUVAL,
	vcMAXUVAL,
	vcMINVVAL,
	vcMAXVVAL,
	vcMAGSCALEFACTOR,
	vcSVAL,
	vcMAXSVAL,
	vcMINSVAL,
	vcSVALSCALEFACTOR
} vcValueType;

typedef enum _vcLabelType { 
	vcREFVECANNO,
	vcMINVECANNO,
	vcZEROFLBL
} vcLabelType;


/* base methods */

static NhlErrorTypes VectorPlotClassInitialize(
#if	NhlNeedProto
	void
#endif
);

static NhlErrorTypes VectorPlotClassPartInitialize(
#if	NhlNeedProto
	NhlClass	lc
#endif
);

static NhlErrorTypes VectorPlotInitialize(
#if	NhlNeedProto
        NhlClass,  /* class */
        NhlLayer,       /* req */
        NhlLayer,       /* new */
        _NhlArgList,    /* args */
        int             /* num_args */
#endif
);

static NhlErrorTypes VectorPlotSetValues(
#if	NhlNeedProto
        NhlLayer,       /* old */
        NhlLayer,       /* reference */
        NhlLayer,       /* new */
        _NhlArgList,    /* args */
        int             /* num_args*/
#endif
);


static NhlErrorTypes    VectorPlotGetValues(
#if	NhlNeedProto
	NhlLayer,       /* l */
	_NhlArgList,    /* args */
	int             /* num_args */
#endif
);

static NhlErrorTypes VectorPlotDestroy(
#if	NhlNeedProto
        NhlLayer        /* inst */
#endif
);

static NhlErrorTypes VectorPlotGetBB(
#if	NhlNeedProto
        NhlLayer        instance,
        NhlBoundingBox	*thebox
#endif
);

static NhlErrorTypes VectorPlotPreDraw(
#if	NhlNeedProto
        NhlLayer	/* layer */
#endif
);

static NhlErrorTypes VectorPlotDraw(
#if	NhlNeedProtof
        NhlLayer	/* layer */
#endif
);

static NhlErrorTypes VectorPlotPostDraw(
#if	NhlNeedProto
        NhlLayer	/* layer */
#endif
);


static NhlErrorTypes vcDraw(
#if	NhlNeedProto
        NhlVectorPlotLayer	vcl,
	NhlDrawOrder	order
#endif
);

static NhlErrorTypes vcInitSegment(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcl,
	NhlTransDat	**seg_dat,
	NhlString	entry_name
#endif
);

static NhlErrorTypes vcSegDraw(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcl,
	NhlDrawOrder	order
#endif
);

static NhlErrorTypes vcInitDraw(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcl,
	NhlString	entry_name
#endif
);

static NhlErrorTypes VectorPlotUpdateData(
#if	NhlNeedProto
	NhlDataCommLayer	new,
	NhlDataCommLayer	old
#endif
);

static NhlErrorTypes VectorPlotDataInitialize(
#if	NhlNeedProto
        NhlClass	class,
        NhlLayer	req,
        NhlLayer	new,
        _NhlArgList	args,
        int             num_args
#endif
);

/* internal static functions */

static NhlErrorTypes InitCoordBounds(
#if	NhlNeedProto
        NhlVectorPlotLayerPart	*vcp,
	char			*entry_name
#endif
);

static NhlErrorTypes SetUpLLTransObj(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init
#endif
);

static NhlErrorTypes SetCoordBounds(
#if	NhlNeedProto
	NhlVectorPlotLayerPart	*vcp,
	vcCoord			ctype,
	int			count,
	NhlString		entry_name
#endif
);

static NhlErrorTypes SetUpIrrTransObj(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init
#endif
);

static NhlErrorTypes SetFormat(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init
#endif
);

static NhlErrorTypes ManageLabels(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
#endif
);

static NhlErrorTypes SetScale(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlvcScaleInfo		*sip,
	NhlvcScaleInfo		*osip,
	NhlBoolean		do_levels,
	NhlBoolean		init
#endif
);

static NhlErrorTypes ManageOverlay(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
#endif
);

static NhlErrorTypes ManageTickMarks(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
#endif
);


static NhlErrorTypes ManageTitles(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
#endif
);


static NhlErrorTypes ManageLabelBar(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
#endif
);

static NhlErrorTypes ManageVecAnno(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean		init,
	NhlvcLabelAttrs		*ilp,
	NhlvcLabelAttrs		*oilp,
	NhlAnnotationRec	*anrp,
	NhlAnnotationRec	*oanrp,
	int			*idp,
	NhlString		def_string1,
	NhlString		def_string2,
	NhlSArg			*sargs,
	int			*nargs
#endif
);

static NhlErrorTypes ManageZeroFLabel(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
#endif
);

static NhlErrorTypes ManageAnnotation(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew,
	NhlBoolean		init,
	NhlAnnotationRec	*rec,
	NhlAnnotationRec	*orec,
	int			*idp,
	NhlBoolean		on
#endif
);

static NhlErrorTypes SetTextPosition(
#if	NhlNeedProto
	NhlVectorPlotLayer		vcnew,
	NhlVectorPlotLayerPart	*ovcp,
	_vcAnnoType		atype,
	NhlBoolean		*pos_changed,
	NhlString		entry_name
#endif
);

static NhlErrorTypes ReplaceSubstitutionChars(
#if	NhlNeedProto
	NhlVectorPlotLayerPart	*vcp,
	NhlVectorPlotLayerPart	*ovcp,
	NhlBoolean		init,
	_vcAnnoType		atype,
	NhlBoolean		*text_changed,
	NhlString		entry_name
#endif
);

static void Substitute(
#if	NhlNeedProto
	char		*buf,
	int		replace_count,
	char		*subst
#endif
);


static NhlErrorTypes SetFormatRec(
#if	NhlNeedProto
	NhlFormatRec	*format,
	NhlString	resource,
	NhlString	entry_name
#endif
);

static NhlErrorTypes    SetupLevels(
#if	NhlNeedProto
	NhlLayer	new,
	NhlLayer	old,
        NhlBoolean	init,					    
	float		**levels,
	NhlBoolean	*modified				    
#endif
);

static NhlErrorTypes    SetupLevelsManual(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew, 
	NhlVectorPlotLayer	vcold,
	float			**levels,
        float			min,
	float			max,
	char			*entry_name
#endif
);

static NhlErrorTypes    SetupLevelsEqual(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew, 
	NhlVectorPlotLayer	vcold,
	float			**levels,
        float			min,
	float			max,
	char			*entry_name
#endif
);

static NhlErrorTypes    SetupLevelsAutomatic(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew, 
	NhlVectorPlotLayer	vcold,
	float			**levels,
        float			min,
	float			max,
	char			*entry_name
#endif
);

static NhlErrorTypes    SetupLevelsExplicit(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew, 
	NhlVectorPlotLayer	vcold,
	NhlBoolean		init,
	float			**levels,
        float			min,
	float			max,
	char			*entry_name
#endif
);

static NhlErrorTypes ChooseSpacingLin(
#if	NhlNeedProto
	float		*tstart,
	float		*tend,
	float		*spacing,
	int		convert_precision,
	int		max_ticks,
	NhlString	entry_name
#endif
);

static NhlErrorTypes    ManageVectorData(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew, 
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	_NhlArgList	args,
	int		num_args
#endif
);

static NhlErrorTypes    ManageScalarData(
#if	NhlNeedProto
	NhlVectorPlotLayer	vcnew, 
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	_NhlArgList	args,
	int		num_args
#endif
);

static NhlErrorTypes    ManageViewDepResources(
#if	NhlNeedProto
	NhlLayer	new,
	NhlLayer	old,
        NhlBoolean	init,					    
	_NhlArgList	args,
	int		num_args
#endif
);

static NhlErrorTypes    CopyTextAttrs(
#if	NhlNeedProto
	NhlvcLabelAttrs *dest,
	NhlvcLabelAttrs *source,
	NhlString	entry_name
#endif
);

static NhlErrorTypes    AdjustText(
#if	NhlNeedProto
	NhlvcLabelAttrs *lbl_attrp,
	NhlVectorPlotLayer	new, 
	NhlVectorPlotLayer	old,
	NhlBoolean	init
#endif
);

static NhlErrorTypes    ManageDynamicArrays(
#if	NhlNeedProto
	NhlLayer	new,
	NhlLayer	old,
        NhlBoolean	init,					    
	_NhlArgList	args,
	int		num_args
#endif
);

static NhlErrorTypes    ManageGenArray(
#if	NhlNeedProto
	NhlGenArray	*ga,
	int		count,
	NhlGenArray	copy_ga,
	NrmQuark	type,
	NhlPointer	init_val,
	int		*old_count,
	int		*init_count,
	NhlBoolean	*need_check,
	NhlBoolean	*changed,				       
	NhlString	resource_name,
	NhlString	entry_name
#endif
);

static NhlErrorTypes	CheckColorArray(
#if	NhlNeedProto
	NhlVectorPlotLayer	cl,
	NhlGenArray	ga,
	int		count,
	int		init_count,
	int		last_count,
	int		**gks_colors,
	NhlString	resource_name,
	NhlString	entry_name
#endif
);

static NhlGenArray GenArraySubsetCopy(
#if	NhlNeedProto
        NhlGenArray     ga,
        int             length
#endif
);

extern void   (_NHLCALLF(hluvvmpxy,HLUVVMPXY))(
#if	NhlNeedProto
 float *x,
 float *y, 
 float *u, 
 float *v, 
 float *uvm, 
 float *xb, 
 float *yb, 
 float *xe, 
 float *ye, 
 int *ist
#endif
);

static void   load_hlucp_routines(
#if	NhlNeedProto
	NhlBoolean	flag
#endif
);

extern void (_NHLCALLF(vvmpxy,VVMPXY))(
#if	NhlNeedProto
 float *x,
 float *y, 
 float *u, 
 float *v, 
 float *uvm, 
 float *xb, 
 float *yb, 
 float *xe, 
 float *ye, 
 int *ist
#endif
);

extern void (_NHLCALLF(vvgetmapinfo,VVGETMAPINFO))(
#if	NhlNeedProto
 int *imp,
 int *itr,
 float *dmx,
 float *sxd,
 float *syd,
 float *xmn,
 float *xmx,
 float *ymn,
 float *ymx
#endif
);

NhlVectorPlotDataDepClassRec NhlvectorPlotDataDepClassRec = {
	/* base_class */
        {
/* class_name			*/	"vectorPlotDataDepClass",
/* nrm_class			*/	NrmNULLQUARK,
/* layer_size			*/	sizeof(NhlVectorPlotDataDepLayerRec),
/* class_inited			*/	False,
/* superclass			*/	(NhlClass)
						&NhldataSpecClassRec,
/* cvt_table			*/	NULL,

/* layer_resources		*/	data_resources,
/* num_resources		*/	NhlNumber(data_resources),
/* all_resources		*/	NULL,

/* class_part_initialize	*/	NULL,
/* class_initialize		*/	NULL,
/* layer_initialize		*/	VectorPlotDataInitialize,
/* layer_set_values		*/	NULL,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	NULL,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	NULL
	},
	/* dataspec_class */
	{
/* foo				*/	0
	},
	/* vector datadep_class */
	{
/* foo				*/	0
	}
};

NhlVectorPlotClassRec NhlvectorPlotClassRec = {
	/* base_class */
        {
/* class_name			*/      "vectorPlotClass",
/* nrm_class			*/      NrmNULLQUARK,
/* layer_size			*/      sizeof(NhlVectorPlotLayerRec),
/* class_inited			*/      False,
/* superclass			*/  (NhlClass)&NhldataCommClassRec,
/* cvt_table			*/	NULL,

/* layer_resources		*/	resources,
/* num_resources		*/	NhlNumber(resources),
/* all_resources		*/	NULL,

/* class_part_initialize	*/	VectorPlotClassPartInitialize,
/* class_initialize		*/	VectorPlotClassInitialize,
/* layer_initialize		*/	VectorPlotInitialize,
/* layer_set_values		*/	VectorPlotSetValues,
/* layer_set_values_hook	*/	NULL,
/* layer_get_values		*/	VectorPlotGetValues,
/* layer_reparent		*/	NULL,
/* layer_destroy		*/	VectorPlotDestroy,

/* child_resources		*/	NULL,

/* layer_draw			*/      VectorPlotDraw,

/* layer_pre_draw		*/      VectorPlotPreDraw,
/* layer_draw_segonly		*/	NULL,
/* layer_post_draw		*/      VectorPlotPostDraw,
/* layer_clear			*/      NULL

        },
	/* view_class */
	{
/* segment_wkid			*/	0,
/* get_bb			*/	VectorPlotGetBB
	},
	/* trans_class */
	{
/* overlay_capability 		*/	_tfOverlayBaseOrMember,
/* data_to_ndc			*/	NhlInheritTransFunc,
/* ndc_to_data			*/	NhlInheritTransFunc,
/* data_polyline		*/	NhlInheritPolyTransFunc,
/* ndc_polyline			*/	NhlInheritPolyTransFunc,
/* data_polygon			*/	NhlInheritPolyTransFunc,
/* ndc_polygon			*/	NhlInheritPolyTransFunc,
/* data_polymarker		*/	NhlInheritPolyTransFunc,
/* ndc_polymarker		*/	NhlInheritPolyTransFunc
	},
	/* datacomm_class */
	{
/* data_offsets			*/	NULL,
/* update_data			*/	VectorPlotUpdateData
	},
	{
/* foo				*/	NULL
	}
};
	

NhlClass NhlvectorPlotDataDepClass =
		(NhlClass) &NhlvectorPlotDataDepClassRec;
NhlClass NhlvectorPlotClass = 
		(NhlClass) &NhlvectorPlotClassRec;

static NrmQuark	Qfloat = NrmNULLQUARK;
static NrmQuark Qint = NrmNULLQUARK;
static NrmQuark Qcolorindex = NrmNULLQUARK;
static NrmQuark Qstring = NrmNULLQUARK;
static NrmQuark	Qlevels = NrmNULLQUARK; 
static NrmQuark	Qlevel_colors = NrmNULLQUARK; 
static NrmQuark	Qmax_magnitude_format = NrmNULLQUARK; 
static NrmQuark	Qmax_svalue_format = NrmNULLQUARK; 
static NrmQuark	Qrefvec_anno_string1 = NrmNULLQUARK; 
static NrmQuark	Qrefvec_anno_string2 = NrmNULLQUARK; 
static NrmQuark	Qrefvec_anno_format = NrmNULLQUARK; 
static NrmQuark	Qminvec_anno_string1 = NrmNULLQUARK; 
static NrmQuark	Qminvec_anno_string2 = NrmNULLQUARK; 
static NrmQuark	Qminvec_anno_format = NrmNULLQUARK; 
static NrmQuark	Qno_data_label_string = NrmNULLQUARK; 
static NrmQuark	Qzerof_label_string = NrmNULLQUARK; 
static NrmQuark	Qlb_label_strings = NrmNULLQUARK;
static NrmQuark	Qref_magnitude = NrmNULLQUARK;
static NrmQuark	Qmin_magnitude = NrmNULLQUARK;
static NrmQuark	Qmax_magnitude = NrmNULLQUARK;
static NrmQuark	Qmin_frac_len = NrmNULLQUARK;
static NrmQuark	Qref_length = NrmNULLQUARK;

static char *InitName = "VectorPlotInitialize";
static char *SetValuesName = "VectorPlotSetValues";

static NhlVectorPlotLayer	Vcl = NULL;
static NhlVectorPlotLayerPart	*Vcp = NULL;
static NhlBoolean		Need_Info;

/*
 * Function:	nhlfvectorplotlayerclass
 *
 * Description:	fortran ref to vector class
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global Fortran
 * Returns:	NhlClass
 * Side Effect:	
 */
NhlClass
_NHLCALLF(nhlfvectorplotclass,NHLFVECTORPLOTCLASS)
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	return NhlvectorPlotClass;
}

/*
 * Function:	nhlfvectorplotdatadeplayerclass
 *
 * Description:	fortran ref to vectorplot datadep class
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global Fortran
 * Returns:	NhlClass
 * Side Effect:	
 */
NhlClass
_NHLCALLF(nhlfvectorplotdatadepclass,NHLFVECTORPLOTDATADEPCLASS)
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{
	return NhlvectorPlotDataDepClass;
}

/*
 * Function:	VectorPlotDataInitialize
 *
 * Description:	Initializes the VectorPlotData Dependent class instance.
 *
 * In Args:	
 *		NhlClass	class,
 *		NhlLayer		req,
 *		NhlLayer		new,
 *		_NhlArgList	args,
 *		int		num_args
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
/*ARGSUSED*/
static NhlErrorTypes
VectorPlotDataInitialize
#if	NhlNeedProto
(
	NhlClass	class,
	NhlLayer		req,
	NhlLayer		new,
	_NhlArgList	args,
	int		num_args
)
#else
(class,req,new,args,num_args)
        NhlClass      class;
        NhlLayer           req;
        NhlLayer           new;
        _NhlArgList     args;
        int             num_args;
#endif
{
	NhlErrorTypes	ret = NhlNOERROR;

	return ret;
}


/*
 * Function:	VectorPlotUpdateData
 *
 * Description:	This function is called whenever the data pointed to by the
 *		data resources change.  This function needs to check if
 *		this specific data resource changed - it may have been another
 *		data resource in a sub/super class.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */
/*ARGSUSED*/
static NhlErrorTypes
VectorPlotUpdateData
#if	NhlNeedProto
(
	NhlDataCommLayer	new,
	NhlDataCommLayer	old
)
#else
(new,old)
	NhlDataCommLayer	new;
	NhlDataCommLayer	old;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR;

/*
 * For now simply call SetValues setting the data changed resource true
 */
	NhlVASetValues(new->base.id,NhlNvcDataChanged,True,
		       NULL);

	return ret;
}

/*
 * Function:	VectorPlotClassInitialize
 *
 * Description:
 *
 * In Args:	NONE
 *
 * Out Args:	NONE
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:	NONE
 */
static NhlErrorTypes
VectorPlotClassInitialize
#if	NhlNeedProto
(
	void
)
#else
()
#endif
{

        _NhlEnumVals   positionmodelist[] = {
        {NhlARROWHEAD,		"arrowhead"},
        {NhlARROWTAIL, 		"arrowtail"},
        {NhlARROWCENTER, 	"arrowcenter"}
        };

	load_hlucp_routines(False);

	_NhlRegisterEnumType(NhlvectorPlotClass,NhlTVectorPositionMode,
		positionmodelist,NhlNumber(positionmodelist));

	Qint = NrmStringToQuark(NhlTInteger);
	Qstring = NrmStringToQuark(NhlTString);
	Qfloat = NrmStringToQuark(NhlTFloat);
	Qcolorindex = NrmStringToQuark(NhlTColorIndex);
	Qlevels = NrmStringToQuark(NhlNvcLevels);
	Qlevel_colors = NrmStringToQuark(NhlNvcLevelColors);
	Qmax_magnitude_format = NrmStringToQuark(NhlNvcMagnitudeFormat);
	Qmax_svalue_format = NrmStringToQuark(NhlNvcScalarValueFormat);
	Qrefvec_anno_string1 = NrmStringToQuark(NhlNvcRefAnnoString1);
	Qrefvec_anno_string2 = NrmStringToQuark(NhlNvcRefAnnoString2);
	Qrefvec_anno_format = NrmStringToQuark(NhlNvcRefAnnoFormat);
	Qminvec_anno_string1 = NrmStringToQuark(NhlNvcMinAnnoString1);
	Qminvec_anno_string2 = NrmStringToQuark(NhlNvcMinAnnoString2);
	Qminvec_anno_format = NrmStringToQuark(NhlNvcMinAnnoFormat);
	Qzerof_label_string = NrmStringToQuark(NhlNvcZeroFLabelString);
	Qno_data_label_string = NrmStringToQuark(NhlNvcNoDataLabelString);
	Qlb_label_strings = NrmStringToQuark(NhlNlbLabelStrings);
	Qref_magnitude = NrmStringToQuark(NhlNvcRefMagnitudeF);
	Qmin_magnitude = NrmStringToQuark(NhlNvcMinMagnitudeF);
	Qmax_magnitude = NrmStringToQuark(NhlNvcMaxMagnitudeF);
	Qmin_frac_len = NrmStringToQuark(NhlNvcMinFracLengthF);
	Qref_length = NrmStringToQuark(NhlNvcRefLengthF);

	{
		int i,j;
		float igrid, jgrid;
		float pi = 3.14159;
		float tmag, u, v;
		igrid = 2.0 * pi / (float) M;
		jgrid = 2.0 * pi / (float) N;
		for (j = 0; j < N; j++) {
			for (i = 0; i < M; i++) {
				u = *(U + j * M + i) = cos(igrid * (float) i);
				v = *(V + j * M + i) = cos(jgrid * (float) j);
				u = *(U + j * M + i) = (float)i + 0.5;
				v = *(V + j * M + i) = 0;
#if 0
				u = *(U + j * M + i) = 0.0;
				v = *(V + j * M + i) = 0.0;
#endif
				tmag = u * u + v * v;
				if (tmag == 0.0)
					continue;
				if (tmag > Max_Mag) Max_Mag = tmag;
				if (tmag < Min_Mag) Min_Mag = tmag;
			}
		}
		Max_Mag = sqrt(Max_Mag);
		Min_Mag = sqrt(Min_Mag);
	}

	return NhlNOERROR;
}

/*
 * Function:	VectorPlotClassPartInitialize
 *
 * Description:	This function initializes fields in the 
 *		NhlVectorPlotClassPart that cannot be initialized statically.
 *		Calls _NhlRegisterChildClass for the overlay manager object.
 *
 * In Args:	
 *		NhlClass	lc	NhlLayer Class to init
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	NhlErrorTypes
 * Side Effect:	
 */

static NhlErrorTypes
VectorPlotClassPartInitialize
#if	NhlNeedProto
(
	NhlClass	lc	/* Layer Class to init	*/
)
#else
(lc)
	NhlClass	lc;	/* Layer Class to init	*/
#endif
{
	NhlErrorTypes	ret = NhlNOERROR, subret = NhlNOERROR;
	char		*e_text;
	char		*entry_name = "VectorPlotClassPartInitialize";

/*
 * Register children objects
 */
	subret = _NhlRegisterChildClass(lc,NhlplotManagerClass,
					False,False,
					NhlNpmLegendDisplayMode,
					NhlNpmLabelBarDisplayMode,
					NhlNpmTickMarkDisplayMode,
					NhlNpmTitleDisplayMode,
					NhlNlbBoxCount,
					NhlNlbLabelAlignment,
					NhlNlbLabelStrings,
					NhlNlbLabelFuncCode,
					NhlNlbMonoFillColor,
					NhlNlbFillColor,
					NhlNlbFillColors,
					NhlNlbMonoFillPattern,
					NhlNlbFillPattern,
					NhlNlbFillPatterns,
					NhlNlbMonoFillScale,
					NhlNlbFillScaleF,
					NhlNlbFillScales,
					NULL);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error registering %s";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name,
			  "NhlplotManagerClass");
		return(NhlFATAL);
	}

	subret = _NhlRegisterDataRes((NhlDataCommClass)lc,
				     NhlNvcVectorFieldData,
				     NULL,
				     NhlvectorPlotDataDepClass,
				     NhlvectorFieldFloatClass,
				     NULL);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error registering data resource %s";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name,
			  "NhlcoordArrTableFloatClass");
		return(NhlFATAL);
	}


	subret = _NhlRegisterDataRes((NhlDataCommClass)lc,
				     NhlNvcScalarFieldData,
				     NULL,
				     NhlvectorPlotDataDepClass,
				     NhlscalarFieldFloatClass,
				     NULL);

	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error registering data resource %s";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name,
			  "NhlcoordArrTableFloatClass");
		return(NhlFATAL);
	}

	return ret;
}

/*
 * Function:	VectorPlotInitialize
 *
 * Description: 
 *
 * In Args: 	class	objects layer_class
 *		req	instance record of requested values
 *		new	instance record of new object
 *		args	list of resources and values for reference
 *		num_args 	number of elements in args.
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	state change in GKS due to mapping transformations.
 */
/*ARGSUSED*/
static NhlErrorTypes
VectorPlotInitialize
#if	NhlNeedProto
(
	NhlClass	class,
	NhlLayer		req,
	NhlLayer		new,
	_NhlArgList	args,
	int		num_args
)
#else
(class,req,new,args,num_args)
        NhlClass      class;
        NhlLayer           req;
        NhlLayer           new;
        _NhlArgList     args;
        int             num_args;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name = InitName;
	char			*e_text;
	NhlVectorPlotLayer	vcnew = (NhlVectorPlotLayer) new;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlSArg			sargs[64];
	int			nargs = 0;

	vcp->refvec_anno_id = NhlNULLOBJID;
	vcp->minvec_anno_id = NhlNULLOBJID;
	vcp->zerof_anno_id = NhlNULLOBJID;
	vcp->refvec_anno_rec.id = NhlNULLOBJID;
	vcp->minvec_anno_rec.id = NhlNULLOBJID;
	vcp->zerof_lbl_rec.id = NhlNULLOBJID;

/* Initialize unset resources */

	if (! vcp->level_spacing_set) vcp->level_spacing = 5.0;
	if (! vcp->min_level_set) vcp->min_level_val = -FLT_MAX;
	if (! vcp->max_level_set) vcp->max_level_val = FLT_MAX;

	if (! vcp->lbls.height_set) 
		vcp->lbls.height = 0.010;
	if (! vcp->refvec_anno.height_set) 
		vcp->refvec_anno.height = 0.01;
	if (! vcp->minvec_anno.height_set) 
		vcp->minvec_anno.height = 0.01;
	if (! vcp->zerof_lbl.height_set) 
		vcp->zerof_lbl.height = 0.01;
	if (! vcp->ref_length_set) 
		vcp->ref_length = 0.0;
	if (! vcp->x_min_set)
		vcp->x_min = 0.0;
	if (! vcp->x_max_set)
		vcp->x_max = 1.0;
	if (! vcp->y_min_set)
		vcp->y_min = 0.0;
	if (! vcp->y_max_set)
		vcp->y_max = 1.0;

/* Initialize private members */

	vcp->lbls.fcode[1] = '\0';
	vcp->refvec_anno.fcode[1] = '\0';
	vcp->minvec_anno.fcode[1] = '\0';
	vcp->zerof_lbl.fcode[1] = '\0';
	vcp->new_draw_req = True;
	vcp->predraw_dat = NULL;
	vcp->draw_dat = NULL;
	vcp->postdraw_dat = NULL;
	vcp->update_req = False;
	vcp->overlay_object = NULL;
	vcp->data_changed = True;
	vcp->data_init = False;
	vcp->scalar_data_init = False;
	vcp->use_irr_trans = False;
	vcp->zero_field = False;
	vcp->lbar_labels_set = False;
	vcp->lbar_labels = NULL;
	vcp->lbar_labels_res_set = vcp->lbar_labels_res ? True : False;
	vcp->vfp = NULL;
	vcp->ovfp = NULL;
	vcp->sfp = NULL;
	vcp->osfp = NULL;
	vcp->fws_id = NhlNULLOBJID;
/*
 * Constrain resources
 */
	if (vcp->min_frac_len < 0.0 || vcp->min_frac_len > 1.0) {
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: %s out of bounds: constraining";
		NhlPError(ret,NhlEUNKNOWN,
			  e_text,entry_name, NhlNvcMinFracLengthF);
		vcp->min_frac_len = MIN(1.0,MAX(0.0,vcp->min_frac_len));
	}
	if (vcp->ref_length < 0.0) {
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: %s out of bounds: constraining";
		NhlPError(ret,NhlEUNKNOWN,
			  e_text,entry_name, NhlNvcRefLengthF);
		vcp->ref_length = 0.0;
	}
	if (vcp->min_magnitude < 0.0) {
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: %s out of bounds: constraining";
		NhlPError(ret,NhlEUNKNOWN,
			  e_text,entry_name, NhlNvcMinMagnitudeF);
		vcp->min_magnitude = 0.0;
	}
	if (vcp->max_magnitude < 0.0) {
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: %s out of bounds: constraining";
		NhlPError(ret,NhlEUNKNOWN,
			  e_text,entry_name, NhlNvcMaxMagnitudeF);
		vcp->max_magnitude = 0.0;
	}
/*
 * Set up the data
 */
	subret = ManageVectorData(vcnew,(NhlVectorPlotLayer) req,
				  True,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting view dependent resources";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(NhlFATAL);
	}
	subret = ManageScalarData(vcnew,(NhlVectorPlotLayer) req,
				  True,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting view dependent resources";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
 
	subret = InitCoordBounds(vcp,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING) return(ret);

/* Set view dependent resources */

	subret = ManageViewDepResources(new,req,True,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting view dependent resources";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

/* Set the label formats - must precede dynamic array handling */

	subret = SetFormat(vcnew,(NhlVectorPlotLayer)req,True);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting label formats";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

/* Set up the dynamic arrays (line label array is handled here) */

	subret = ManageDynamicArrays(new,req,True,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error initializing dynamic arrays";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

/* 
 * Set up the labels (except for the line label array) 
 * Note: may add arguments to the PlotManager argument list.
 */

	subret = ManageLabels(vcnew,(NhlVectorPlotLayer)req,True,sargs,&nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error initializing labels";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}


/* Set up the vector object transformation  */

	vcp->do_low_level_log = False;
	if (vcp->use_irr_trans) {
		subret = SetUpIrrTransObj(vcnew,(NhlVectorPlotLayer) req,True);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error setting up transformation";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return(ret);
		}
	}
	else {
		subret = SetUpLLTransObj(vcnew,(NhlVectorPlotLayer) req,True);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error setting up transformation";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return(ret);
		}
	}

/* 
 * Manage the PlotManager (including setting up the annotations managed by it)
 */
	subret = ManageOverlay(vcnew,
			       (NhlVectorPlotLayer)req,True,sargs,&nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		return(ret);
	}

	if (vcnew->trans.overlay_status != _tfNotInOverlay) {
		if (vcp->zerof_lbl.on || vcp->zerof_lbl.string2_on) {
			subret = ManageAnnotation(vcnew,True,
						  &vcp->zerof_lbl_rec,
						  NULL,
						  &vcp->zerof_anno_id,
						  vcp->display_zerof_no_data);
			if ((ret = MIN(ret,subret)) < NhlWARNING)
				return ret;
		}
		if (vcp->refvec_anno.on) {
			subret = ManageAnnotation(vcnew,True,
						  &vcp->refvec_anno_rec,NULL,
						  &vcp->refvec_anno_id,
						  !vcp->display_zerof_no_data);
			if ((ret = MIN(ret,subret)) < NhlWARNING)
				return ret;
		}
		if (vcp->minvec_anno.on) {
			subret = ManageAnnotation(vcnew,True,
						  &vcp->minvec_anno_rec,NULL,
						  &vcp->minvec_anno_id,
						  !vcp->display_zerof_no_data);
			if ((ret = MIN(ret,subret)) < NhlWARNING)
				return ret;
		}
	}

	vcp->data_changed = False;
	vcp->level_spacing_set = False;
	vcp->lbls.height_set = False;
	vcp->refvec_anno.height_set = False;
	vcp->minvec_anno.height_set = False;
	vcp->zerof_lbl.height_set = False;
	vcp->lbar_labels_res_set = False;
	vcp->ref_length_set = False;

	return ret;
}

/*
 * Function:	VectorPlotSetValues
 *
 * Description: 
 *
 * In Args:	old	copy of old instance record
 *		reference	requested instance record
 *		new	new instance record	
 *		args 	list of resources and values for reference
 *		num_args	number of elements in args.
 *
 * Out Args:	NONE
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:
 */
/*ARGSUSED*/
static NhlErrorTypes VectorPlotSetValues
#if	NhlNeedProto
(
	NhlLayer	old,
	NhlLayer	reference,
	NhlLayer	new,
	_NhlArgList	args,
	int		num_args
)
#else
(old,reference,new,args,num_args)
	NhlLayer	old;
	NhlLayer	reference;
	NhlLayer	new;
	_NhlArgList	args;
	int		num_args;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name = SetValuesName;
	char			*e_text;
	NhlVectorPlotLayer		vcnew = (NhlVectorPlotLayer) new;
 	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayer		vcold = (NhlVectorPlotLayer) old;
 	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	/* Note that ManageLabelBar add to sargs */
	NhlSArg			sargs[128];
	int			nargs = 0;

	if (vcnew->view.use_segments != vcold->view.use_segments) {
		vcnew->view.use_segments = vcold->view.use_segments;
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: attempt to set create-only resource overridden";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
	}

	if (_NhlArgIsSet(args,num_args,NhlNvcLevelSpacingF))
		vcp->level_spacing_set = True;

	if (_NhlArgIsSet(args,num_args,NhlNvcLabelFontHeightF))
		vcp->lbls.height_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNvcRefAnnoFontHeightF))
		vcp->refvec_anno.height_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNvcMinAnnoFontHeightF))
		vcp->minvec_anno.height_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNvcZeroFLabelFontHeightF))
		vcp->zerof_lbl.height_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNvcRefLengthF))
		vcp->ref_length_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrXMinF))
		vcp->x_min_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrXMaxF))
		vcp->x_max_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrYMinF))
		vcp->y_min_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNtrYMaxF))
		vcp->y_max_set = True;
	if (_NhlArgIsSet(args,num_args,NhlNlbLabelStrings))
		vcp->lbar_labels_res_set = True;

	if (_NhlArgIsSet(args,num_args,NhlNvcMinLevelValF))
		vcp->min_level_set = True;
	else if (vcp->use_scalar_array != ovcp->use_scalar_array) {
		vcp->min_level_val = -FLT_MAX;
		vcp->min_level_set = False;
	}

	if (_NhlArgIsSet(args,num_args,NhlNvcMaxLevelValF))
		vcp->max_level_set = True;
	else if (vcp->use_scalar_array != ovcp->use_scalar_array) {
		vcp->max_level_val = FLT_MAX;
		vcp->max_level_set = False;
	}

/*
 * Constrain resources
 */
	if (vcp->min_frac_len < 0.0 || vcp->min_frac_len > 1.0) {
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: %s out of bounds: constraining";
		NhlPError(ret,NhlEUNKNOWN,
			  e_text,entry_name, NhlNvcMinFracLengthF);
		vcp->min_frac_len = MIN(1.0,MAX(0.0,vcp->min_frac_len));
	}
	if (vcp->ref_length < 0.0) {
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: %s out of bounds: constraining";
		NhlPError(ret,NhlEUNKNOWN,
			  e_text,entry_name, NhlNvcRefLengthF);
		vcp->ref_length = 0.0;
	}
	if (vcp->min_magnitude < 0.0) {
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: %s out of bounds: constraining";
		NhlPError(ret,NhlEUNKNOWN,
			  e_text,entry_name, NhlNvcMinMagnitudeF);
		vcp->min_magnitude = 0.0;
	}
	if (vcp->max_magnitude < 0.0) {
		ret = MIN(ret,NhlWARNING);
		e_text = "%s: %s out of bounds: constraining";
		NhlPError(ret,NhlEUNKNOWN,
			  e_text,entry_name, NhlNvcMaxMagnitudeF);
		vcp->max_magnitude = 0.0;
	}

/* Manage the data */

	subret = ManageVectorData(vcnew,vcold,False,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting view dependent resources";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
	subret = ManageScalarData(vcnew,vcold,False,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting view dependent resources";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

	subret = InitCoordBounds(vcp,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING) return(ret);

/* Set view dependent resources */

	subret = ManageViewDepResources(new,old,False,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting view dependent resources";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}


/* Set the label formats - must precede dynamic array handling */

	subret = SetFormat(vcnew,vcold,False);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting label formats";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}


/* Manage the dynamic arrays  (line labels handled here) */

	subret = ManageDynamicArrays(new,old,False,args,num_args);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error initializing dynamic arrays";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

/* 
 * Set up the labels (except for the line label array) 
 * Note: may add arguments to the PlotManager argument list.
 */

	subret = ManageLabels(vcnew,vcold,False,sargs,&nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing labels";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}


/* Set up the vector object transformation  */

	vcp->do_low_level_log = False;
	if (vcp->use_irr_trans) {
		subret = SetUpIrrTransObj(vcnew,
					  (NhlVectorPlotLayer) old,False);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error setting up transformation";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return(ret);
		}
	}
	else {
		subret = SetUpLLTransObj(vcnew,(NhlVectorPlotLayer) old,False);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error setting up transformation";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return(ret);
		}
	}
/* 
 * Manage the PlotManager (including the PlotManager annotations)
 */
	subret = ManageOverlay(vcnew,vcold,False,sargs,&nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		return(ret);
	}

	vcp->update_req = False;
	vcp->data_changed = False;
	vcp->level_spacing_set = False;
	vcp->lbls.height_set = False;
	vcp->refvec_anno.height_set = False;
	vcp->minvec_anno.height_set = False;
	vcp->zerof_lbl.height_set = False;
	vcp->lbar_labels_res_set = False;
	vcp->ref_length_set = False;

	return ret;
}

/*
 * Function:    VectorPlotGetValues
 *
 * Description: Retrieves the current setting of VectorPlot resources.
 *      This routine only retrieves resources that require special methods
 *      that the generic GetValues method cannot handle. For now this means
 *      all the GenArray resources. Note that space is allocated; the user
 *      is responsible for freeing this space.
 *
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 *      Memory is allocated when any of the following resources are retrieved:
 *              NhlNvcLevels
 *              NhlNvcLineColors
 *              NhlNvcLineThicknesses
 *		NhlNvcLineLabelStrings
 *		NhlNvcLineLabelFontColors
 *      The caller is responsible for freeing this memory.
 */

static NhlErrorTypes    VectorPlotGetValues
#if	NhlNeedProto
(NhlLayer l, _NhlArgList args, int num_args)
#else
(l,args,num_args)
        NhlLayer        l;
        _NhlArgList     args;
        int     	num_args;
#endif
{
        NhlVectorPlotLayer cl = (NhlVectorPlotLayer)l;
        NhlVectorPlotLayerPart *vcp = &(cl->vectorplot);
        NhlGenArray ga;
	NhlString ts;
        char *e_text;
        int i, count = 0;
        char *type = "";

        for( i = 0; i< num_args; i++ ) {

                ga = NULL;
                if(args[i].quark == Qlevels) {
                        ga = vcp->levels;
                        count = vcp->level_count;
                        type = NhlNvcLevels;
                }
                else if (args[i].quark == Qlevel_colors) {
                        ga = vcp->level_colors;
                        count = vcp->level_count + 1;
                        type = NhlNvcLevelColors;
                }
                if (ga != NULL) {
                        if ((ga = GenArraySubsetCopy(ga, count)) == NULL) {
                                e_text = "%s: error copying %s GenArray";
                                NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
                                          "VectorPlotGetValues",type);
                                return NhlFATAL;
                        }
                        *((NhlGenArray *)(args[i].value.ptrval)) = ga;
			continue;
                }

		ts = NULL;
		if (args[i].quark == Qmax_magnitude_format){
			ts = vcp->mag_scale.format.fstring;
		}
		else if (args[i].quark == Qmax_svalue_format){
			ts = vcp->svalue_scale.format.fstring;
		}
		else if(args[i].quark == Qrefvec_anno_string1){
			ts = vcp->refvec_anno.string1;
		}
		else if(args[i].quark == Qrefvec_anno_string2){
			ts = vcp->refvec_anno.string2;
		}
		else if(args[i].quark == Qminvec_anno_string1){
			ts = vcp->minvec_anno.string1;
		}
		else if(args[i].quark == Qminvec_anno_string2){
			ts = vcp->minvec_anno.string2;
		}
		else if(args[i].quark == Qno_data_label_string){
			ts = vcp->zerof_lbl.string2;
		}
		else if(args[i].quark == Qzerof_label_string){
			ts = vcp->zerof_lbl.string1;
		}
#if 0
		else if(args[i].quark == Qminvec_anno_format){
			ts = vcp->minvec_anno.format.fstring;
		}
		else if(args[i].quark == Qrefvec_anno_format){
			ts = vcp->refvec_anno.format.fstring;
		}
#endif
                if (ts != NULL) {
			*((NhlString*)(args[i].value.ptrval)) =
				NhlMalloc(strlen(ts)+1);
			if(!*((NhlString*)(args[i].value.ptrval))){
                                e_text = "%s: error copying String";
                                NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
					  "VectorPlotGetValues");
                                return NhlFATAL;
                        }
			strcpy(*((NhlString*)(args[i].value.ptrval)),ts);
			continue;
                }
		ga = NULL;
		if (args[i].quark == Qlb_label_strings){
			if (vcp->overlay_object != NULL)
				NhlVAGetValues(vcp->overlay_object->base.id,
					       NhlNlbLabelStrings,&ga,NULL);
                        *((NhlGenArray *)(args[i].value.ptrval)) = ga;
		}
		else if (args[i].quark == Qref_magnitude) {
			if (vcp->ref_magnitude == 0.0)
				*((float *)args[i].value.ptrval) = vcp->zmax;
		}
		else if (args[i].quark == Qmin_magnitude) {
			if (vcp->min_magnitude == 0.0)
				*((float *)args[i].value.ptrval) = vcp->zmin;
		}
		else if (args[i].quark == Qmax_magnitude) {
			if (vcp->max_magnitude == 0.0)
				*((float *)args[i].value.ptrval) = vcp->zmax;
		}
		else if (args[i].quark == Qref_length) {
			*((float *)args[i].value.ptrval) 
				= vcp->real_ref_length;
		}
        }

        return(NhlNOERROR);
}

/*
 * Function:	InitCoordBounds
 *
 * Description: Sets coordinate boundary variables that must be 
 *              initialized regardless of whether any data has been supplied
 *
 * In Args:
 *
 * Out Args:	NONE
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:
 */
/*ARGSUSED*/
static NhlErrorTypes InitCoordBounds
#if	NhlNeedProto
(
        NhlVectorPlotLayerPart	*vcp,
	char			*entry_name
)
#else
(vcp,entry_name)
        NhlVectorPlotLayerPart	*vcp;
	char			*entry_name;
#endif
{
	NhlErrorTypes	ret = NhlNOERROR;
	char		*e_text;
	float		ftmp;

	if (vcp->data_init) {
		if (! vcp->x_min_set)
			vcp->x_min = MIN(vcp->vfp->x_start,vcp->vfp->x_end);
		if (! vcp->x_max_set)
			vcp->x_max = MAX(vcp->vfp->x_start,vcp->vfp->x_end);
		if (! vcp->y_min_set)
			vcp->y_min = MIN(vcp->vfp->y_start,vcp->vfp->y_end);
		if (! vcp->y_max_set)
			vcp->y_max = MAX(vcp->vfp->y_start,vcp->vfp->y_end);
	}

	if (vcp->x_min == vcp->x_max) {
		e_text = "%s: Zero X coordinate span: defaulting";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		vcp->x_min = 0.0; 
		vcp->x_max = 1.0;
	}
	else if (vcp->x_min > vcp->x_max) {
		e_text = "%s: Min X coordinate exceeds max: exchanging";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ftmp = vcp->x_min;
		vcp->x_min = vcp->x_max;
		vcp->x_max = ftmp;
	}
	if (vcp->x_log && vcp->x_min <= 0.0) {
		e_text = 
		   "%s: Log style invalid for X coordinates: setting %s off";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,NhlNtrXLog);
		vcp->x_log = False;
	}

	if (vcp->y_min == vcp->y_max) {
		e_text = "%s: Zero Y coordinate span: defaulting";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		vcp->y_min = 0.0; 
		vcp->y_max = 1.0;
	}
	else if (vcp->y_min > vcp->y_max) {
		e_text = "%s: Min Y coordinate exceeds max: exchanging";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ftmp = vcp->y_min;
		vcp->y_min = vcp->y_max;
		vcp->y_max = ftmp;
	}
	if (vcp->y_log && vcp->y_min <= 0.0) {
		e_text = 
		    "%s: Log style invalid for Y coordinates: setting %s off";
		ret = MIN(ret,NhlWARNING);
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,NhlNtrYLog);
		vcp->y_log = False;
	}

	if (! vcp->data_init) {
		if (! vcp->x_reverse) {
			vcp->xc1 = vcp->x_min;
			vcp->xcm = vcp->x_max;
		}
		else {
			vcp->xc1 = vcp->x_max;
			vcp->xcm = vcp->x_min;
		}
		if (! vcp->y_reverse) {
			vcp->yc1 = vcp->y_min;
			vcp->ycn = vcp->y_max;
		}
		else {
			vcp->yc1 = vcp->y_max;
			vcp->ycn = vcp->y_min;
		}
	}

	return ret;
}

/*
 * Function:  GenArraySubsetCopy
 *
 * Description: Since the internal GenArrays maintained by VectorPlot 
 *      may be bigger than the size currently in use, this function allows
 *      a copy of only a portion of the array to be created. This is for
 *      use by the GetValues routine when returning GenArray resources to
 *      the user level. The array is assumed to be valid. The only pointer
 *      type arrays that the routine can handle are NhlString arrays.
 *      Note: this might be another candidate for inclusion as a global
 *      routine.
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects:
 */

static NhlGenArray GenArraySubsetCopy
#if	NhlNeedProto
        (NhlGenArray    ga,
        int             length)
#else
(ga,length)
        NhlGenArray     ga;
        int             length;
#endif
{
        NhlGenArray gto;

        if (length > ga->num_elements)
                return NULL;

        if ((gto = _NhlCopyGenArray(ga,False)) == NULL) {
                return NULL;
        }
        if ((gto->data = (NhlPointer) NhlMalloc(length * ga->size)) == NULL) {
                return NULL;
        }
        if (ga->typeQ != Qstring) {
                memcpy((void *)gto->data, (Const void *) ga->data,
                       length * ga->size);
        }
        else {
                NhlString *cfrom = (NhlString *) ga->data;
                NhlString *cto = (NhlString *) gto->data;
                int i;
                for (i=0; i<length; i++) {
                        if ((*cto = (char *)
                             NhlMalloc(strlen(*cfrom)+1)) == NULL) {
                                return NULL;
                        }
                        strcpy(*cto++,*cfrom++);
                }
        }
        gto->num_elements = length;
        return gto;
}


/*
 * Function:	VectorPlotDestroy
 *
 * Description:
 *
 * In Args:	inst		instance record pointer
 *
 * Out Args:	NONE
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:	NONE
 */
static NhlErrorTypes VectorPlotDestroy
#if	NhlNeedProto
(NhlLayer inst)
#else
(inst)
NhlLayer inst;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlVectorPlotLayerPart	*vcp = 
		&(((NhlVectorPlotLayer) inst)->vectorplot);
	NhlTransformLayerPart	*vctp = &(((NhlTransformLayer) inst)->trans);
	int			ovbase_id;

/*
 * Note that the transform layer and the vector layer overlay objects
 * may be the same or different. The code must handle either case.
 */

	if (vctp->overlay_status == _tfCurrentOverlayMember ||
	    vctp->overlay_status == _tfCurrentOverlayBase) {
		if (vcp->refvec_anno_id != NhlNULLOBJID) {
			subret = _NhlUnregisterAnnotation(vctp->overlay_object,
					 _NhlGetLayer(vcp->refvec_anno_id),
							  NULL);
			if ((ret = MIN(subret,ret)) < NhlWARNING)
				return NhlFATAL;
		}
		if (vcp->minvec_anno_id != NhlNULLOBJID) {
			subret = _NhlUnregisterAnnotation(vctp->overlay_object,
					 _NhlGetLayer(vcp->minvec_anno_id),
							  NULL);
			if ((ret = MIN(subret,ret)) < NhlWARNING)
				return NhlFATAL;
		}
		if (vcp->zerof_anno_id != NhlNULLOBJID) {
			subret = _NhlUnregisterAnnotation(vctp->overlay_object,
					 _NhlGetLayer(vcp->zerof_anno_id),
							  NULL);
			if ((ret = MIN(subret,ret)) < NhlWARNING)
				return NhlFATAL;
		}
	}
	if (vctp->overlay_status == _tfCurrentOverlayMember) {
		ovbase_id = vctp->overlay_object->base.parent->base.id;
		subret = NhlRemoveOverlay(ovbase_id,inst->base.id,False);
		if ((ret = MIN(subret,ret)) < NhlWARNING)
			return NhlFATAL;
	}

	if (vcp->overlay_object != NULL) {
		(void) _NhlDestroyChild(vcp->overlay_object->base.id,inst);
		vcp->overlay_object = NULL;
	}
	if (vctp->trans_obj != NULL) {
		(void) NhlDestroy(vctp->trans_obj->base.id);
		vctp->trans_obj = NULL;
	}
	if (vcp->refvec_anno_id != NhlNULLOBJID) {
		(void) NhlDestroy(vcp->refvec_anno_id);
	}
	if (vcp->minvec_anno_id != NhlNULLOBJID) {
		(void) NhlDestroy(vcp->minvec_anno_id);
	}
	if (vcp->zerof_anno_id != NhlNULLOBJID) {
		(void) NhlDestroy(vcp->zerof_anno_id);
	}
	if (vcp->refvec_anno_rec.id != NhlNULLOBJID) {
 		(void) NhlDestroy(vcp->refvec_anno_rec.id);
	}
	if (vcp->minvec_anno_rec.id != NhlNULLOBJID) {
 		(void) NhlDestroy(vcp->minvec_anno_rec.id);
	}
	if (vcp->zerof_lbl_rec.id != NhlNULLOBJID) {
 		(void) NhlDestroy(vcp->zerof_lbl_rec.id);
	}

	NhlFreeGenArray(vcp->levels);
	NhlFreeGenArray(vcp->level_colors);
	NhlFree(vcp->gks_level_colors);
	if (vcp->ovfp != NULL)
		NhlFree(vcp->ovfp);
	if (vcp->osfp != NULL)
		NhlFree(vcp->osfp);

	if (vcp->lbar_labels != NULL) {
		NhlFreeGenArray(vcp->lbar_labels);
	}
	if (vcp->level_strings != NULL) {
		int i;
		for (i = 0; i < vcp->level_count; i++) {
			if (vcp->level_strings[i] != NULL)
				NhlFree(vcp->level_strings[i]);
		}
		NhlFree(vcp->level_strings);
	}
	
        if (vcp->mag_scale.format.fstring != NULL)
                NhlFree(vcp->mag_scale.format.fstring);
        if (vcp->svalue_scale.format.fstring != NULL)
                NhlFree(vcp->svalue_scale.format.fstring);

        if (vcp->refvec_anno.string1 != NULL)
                NhlFree(vcp->refvec_anno.string1);
        if (vcp->refvec_anno.string2 != NULL)
                NhlFree(vcp->refvec_anno.string2);
	if (vcp->refvec_anno.text1 != NULL)
                NhlFree(vcp->refvec_anno.text1);
	if (vcp->refvec_anno.text2 != NULL)
                NhlFree(vcp->refvec_anno.text2);

        if (vcp->minvec_anno.string1 != NULL)
                NhlFree(vcp->minvec_anno.string1);
        if (vcp->minvec_anno.string2 != NULL)
                NhlFree(vcp->minvec_anno.string2);
	if (vcp->minvec_anno.text1 != NULL)
                NhlFree(vcp->minvec_anno.text1);
	if (vcp->minvec_anno.text2 != NULL)
                NhlFree(vcp->minvec_anno.text2);

        if (vcp->zerof_lbl.string2 != NULL)
                NhlFree(vcp->zerof_lbl.string2);
        if (vcp->zerof_lbl.string1 != NULL)
                NhlFree(vcp->zerof_lbl.string1);
	if (vcp->zerof_lbl.text1 != NULL)
                NhlFree(vcp->zerof_lbl.text1);
	if (vcp->zerof_lbl.text2 != NULL)
                NhlFree(vcp->zerof_lbl.text2);
#if 0
	if (vcp->refvec_anno.format.fstring != NULL)
                NhlFree(vcp->refvec_anno.format.fstring);
	if (vcp->minvec_anno.format.fstring != NULL)
                NhlFree(vcp->minvec_anno.format.fstring);
	if (vcp->zerof_lbl.format.fstring != NULL)
                NhlFree(vcp->zerof_lbl.format.fstring);
#endif


	return(ret);
}


/*
 * Function:    VectorPlotGetBB
 *
 * Description: 
 *
 * In Args:     instance        the object instance record
 *              thebox          a data structure used to hold bounding box 
 *                              information.
 *
 * Out Args:    NONE
 *
 * Return Values:       Error Conditions
 *
 * Side Effects:        NONE
 */
static NhlErrorTypes VectorPlotGetBB
#if	NhlNeedProto
(NhlLayer instance, NhlBoundingBox *thebox)
#else
(instance,thebox)
	NhlLayer instance;
	NhlBoundingBox *thebox;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR,subret = NhlNOERROR;
	char			*entry_name  = "VectorPlotGetBB";
	char			*e_text;
	NhlVectorPlotLayer		vcl = (NhlVectorPlotLayer) instance;
	NhlVectorPlotLayerPart	*vcp = &(vcl->vectorplot);
	NhlTransformLayerPart	*vctp = &(((NhlTransformLayer)vcl)->trans);
	NhlViewLayerPart	*vcvp = &(((NhlViewLayer) vcl)->view);
	float			x,y,width,height;

	if (! _NhlIsTransform(instance)) {
		e_text = "%s: invalid object id";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(NhlFATAL);
	}
/*
 * If the VectorPlot object is a overlay base, return the bounding box
 * of the complete overlay. If it is a member of an overlay, return
 * only the VectorPlot's viewport, since it does not 'own' any of its
 * annotations. If it is not in an overlay at all, return its viewport
 * plus the info label and constant field annotation viewports 
 * (instantiated directly by the VectorPlot) as appropriate.
 */
	if (vctp->overlay_status == _tfCurrentOverlayBase) {
		return _NhlGetBB(vctp->overlay_object,thebox);
	}

	_NhlAddBBInfo(vcvp->y,vcvp->y - vcvp->height,
		      vcvp->x + vcvp->width,vcvp->x,thebox);

	if (vctp->overlay_status == _tfCurrentOverlayMember)
		return ret;

	if (vcp->refvec_anno_id != NhlNULLOBJID && vcp->refvec_anno.on &&
	    ! vcp->display_zerof_no_data) {
		subret = NhlVAGetValues(vcp->refvec_anno_id,
					NhlNvpXF,&x,
					NhlNvpYF,&y,
					NhlNvpWidthF,&width,
					NhlNvpHeightF, &height, NULL);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;

		_NhlAddBBInfo(y,y-height,x+width,x,thebox);
	}
	if (vcp->minvec_anno_id != NhlNULLOBJID && vcp->minvec_anno.on &&
	    ! vcp->display_zerof_no_data) {
		subret = NhlVAGetValues(vcp->minvec_anno_id,
					NhlNvpXF,&x,
					NhlNvpYF,&y,
					NhlNvpWidthF,&width,
					NhlNvpHeightF, &height, NULL);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;

		_NhlAddBBInfo(y,y-height,x+width,x,thebox);
	}
	if (vcp->zerof_anno_id != NhlNULLOBJID && 
	    vcp->display_zerof_no_data) {
		subret = NhlVAGetValues(vcp->zerof_anno_id,
					NhlNvpXF,&x,
					NhlNvpYF,&y,
					NhlNvpWidthF,&width,
					NhlNvpHeightF, &height, NULL);

		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
		_NhlAddBBInfo(y,y-height,x+width,x,thebox);
	}

	return ret;
}

/*
 * Function:	vcSegDraw
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes vcSegDraw
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcl,
	NhlDrawOrder	order
)
#else
(vcl,order)
        NhlVectorPlotLayer vcl;
	NhlDrawOrder	order;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name  = NULL;
	char			*e_text;
	NhlVectorPlotLayerPart	*vcp = &(vcl->vectorplot);
	NhlTransDat		*seg_dat;

	switch (order) {
	case NhlPREDRAW:
		entry_name = "VectorPlotPreDraw";
		seg_dat = vcp->predraw_dat;
		break;
	case NhlDRAW:
		entry_name = "VectorPlotDraw";
		seg_dat = vcp->draw_dat;
		break;
	case NhlPOSTDRAW:
		entry_name = "VectorPlotPostDraw";
		seg_dat = vcp->postdraw_dat;
		break;
	default:
		e_text = "%s: internal enumeration error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(NhlFATAL);
	}

	if (seg_dat == NULL)
		return NhlNOERROR;

	subret = _NhlActivateWorkstation(vcl->base.wkptr);
	if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	subret = _NhlDrawSegment(seg_dat,_NhlWorkstationId(vcl->base.wkptr));
	if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	subret = _NhlDeactivateWorkstation(vcl->base.wkptr);
	return MIN(subret,ret);
}


/*
 * Function:	vcInitDraw
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes vcInitDraw
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcl,
	NhlString	entry_name
)
#else
(vcl,entry_name)
        NhlVectorPlotLayer vcl;
	NhlString	entry_name;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR;
	NhlVectorPlotLayerPart	*vcp = &(vcl->vectorplot);
	NhlString		e_text;

	vcp->fws = NULL;
	vcp->wk_active = False;
	vcp->seg_open = False;

	NhlVASetValues(vcl->base.wkptr->base.id,
		       _NhlNwkReset,	True,
		       NULL);

	if (vcp->min_distance > 0.0) {
		if (vcp->fws_id == NhlNULLOBJID) {
			if ((vcp->fws_id = 
			     _NhlNewWorkspace(NhlwsOTHER,NhlwsNONE,
					    100*sizeof(float))) < 0) {
				e_text = 
					"%s: float workspace allocation error";
				NhlPError(NhlFATAL,
					  NhlEUNKNOWN,e_text,entry_name);
				return NhlFATAL;
			}
		}
		if ((vcp->fws = _NhlUseWorkspace(vcp->fws_id)) == NULL) {
			e_text = "%s: error reserving float workspace";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return(ret);
		}
	}

	return ret;
}

/*
 * Function:	vcInitSegment
 *
 * Description:	
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes vcInitSegment
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcl,
	NhlTransDat	**seg_dat,
	NhlString	entry_name
)
#else
(vcl,seg_dat,entry_name)
        NhlVectorPlotLayer vcl;
	NhlTransDat	**seg_dat;
	NhlString	entry_name;
#endif
{
	char			*e_text;

	if (*seg_dat != NULL)
		_NhlDeleteViewSegment((NhlLayer) vcl,*seg_dat);
	if ((*seg_dat = _NhlNewViewSegment((NhlLayer) vcl)) == NULL) {
		e_text = "%s: error opening segment";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}
	_NhlStartSegment(*seg_dat);
	vcl->vectorplot.seg_open = True;

	return NhlNOERROR;
}
/*
 * Function:	VectorPlotPreDraw
 *
 * Description:	
 *
 * In Args:	layer	VectorPlot instance
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes VectorPlotPreDraw
#if	NhlNeedProto
(NhlLayer layer)
#else
(layer)
        NhlLayer layer;
#endif
{
	NhlErrorTypes ret = NhlNOERROR, subret = NhlNOERROR;
	NhlString	entry_name = "VectorPlotPreDraw";
	NhlVectorPlotLayer		vcl = (NhlVectorPlotLayer) layer;
	NhlVectorPlotLayerPart	*vcp = &vcl->vectorplot;

	Vcp = vcp;
	Vcl = vcl;

	if (! vcp->data_init || vcp->display_zerof_no_data) {
		Vcp = NULL;
		return NhlNOERROR;
	}

	if (vcl->view.use_segments && ! vcp->new_draw_req) {
		ret = vcSegDraw(vcl,NhlPREDRAW);
		Vcp = NULL;
		return ret;
	}

	subret = vcInitDraw(vcl,entry_name);
	if ((ret = MIN(subret,ret)) < NhlWARNING) {
		Vcp = NULL;
		return ret;
	}

	subret = vcDraw(vcl,NhlPREDRAW);

	Vcp = NULL;
	return MIN(subret,ret);
}

/*
 * Function:	VectorAbortDraw
 *
 * Description:	cleans up if a fatal error occurs while drawing
 *
 * In Args:	layer	VectorPlot instance
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static void VectorAbortDraw
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcl
)
#else
(vcl)
	NhlVectorPlotLayer	vcl;
#endif
{
	NhlVectorPlotLayerPart	*vcp = &vcl->vectorplot;
	NhlTransformLayerPart	*tfp = &(vcl->trans);
	char *e_text;

	Vcp = NULL;
	Vcl = NULL;

	if (vcl->view.use_segments && vcp->seg_open)
		_NhlEndSegment();

	if (vcp->wk_active)
		_NhlDeactivateWorkstation(vcl->base.wkptr);

	if (vcp->do_low_level_log) 
		NhlVASetValues(tfp->trans_obj->base.id,
			       NhlNtrLowLevelLogOn,False,NULL);

	if (vcp->fws != NULL) {
		_NhlIdleWorkspace(vcp->fws);
		vcp->fws = NULL;
	}

	e_text = "%s: draw error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,"VectorPlotDraw");
}

/*
 * Function:	VectorPlotDraw
 *
 * Description:	
 *
 * In Args:	layer	VectorPlot instance
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes VectorPlotDraw
#if	NhlNeedProto
(NhlLayer layer)
#else
(layer)
        NhlLayer layer;
#endif
{
	NhlErrorTypes ret;
	NhlVectorPlotLayer	vcl = (NhlVectorPlotLayer) layer;
	NhlVectorPlotLayerPart	*vcp = &vcl->vectorplot;

	Vcp = vcp;
	Vcl = vcl;

	if (! vcp->data_init || vcp->display_zerof_no_data) {
		Vcp = NULL;
		return NhlNOERROR;
	}

	if (vcl->view.use_segments && ! vcp->new_draw_req) {
		ret = vcSegDraw(vcl,NhlDRAW);
		Vcp = NULL;
		return ret;
	}

	ret = vcDraw((NhlVectorPlotLayer) layer,NhlDRAW);
	Vcp = NULL;
	return ret;
}

/*
 * Function:	VectorPlotPostDraw
 *
 * Description:	
 *
 * In Args:	layer	VectorPlot instance
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes VectorPlotPostDraw
#if	NhlNeedProto
(NhlLayer layer)
#else
(layer)
        NhlLayer layer;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlVectorPlotLayer		vcl = (NhlVectorPlotLayer) layer;
	NhlVectorPlotLayerPart	*vcp = &vcl->vectorplot;
	NhlTransformLayerPart	*tfp = &vcl->trans;

	Vcp = vcp;
	Vcl = vcl;

	if (vcp->display_zerof_no_data) {
		if (tfp->overlay_status == _tfNotInOverlay) {
			subret = NhlDraw(vcp->zerof_lbl_rec.id);
			if ((ret = MIN(subret,ret)) < NhlWARNING) {
				Vcp = NULL;
				return ret;
			}
		}
		Vcp = NULL;
		return ret;
	}

	if (vcl->view.use_segments && ! vcp->new_draw_req) {
		ret = vcSegDraw(vcl,NhlPOSTDRAW);
		Vcp = NULL;
		return ret;
	}

	ret = vcDraw((NhlVectorPlotLayer) layer,NhlPOSTDRAW);
	vcp->new_draw_req = False;
	Vcp = NULL;

	if (tfp->overlay_status == _tfNotInOverlay) {
		if (vcp->refvec_anno.on) {
			subret = NhlDraw(vcp->refvec_anno_rec.id);
			ret = MIN(subret,ret);
		}
		if (vcp->minvec_anno.on) {
			subret = NhlDraw(vcp->minvec_anno_rec.id);
			ret = MIN(subret,ret);
		}
	}
	if (vcp->fws != NULL) {
		subret = _NhlIdleWorkspace(vcp->fws);
		ret = MIN(subret,ret);
		vcp->fws = NULL;
	}

	return ret;
}


/*
 * Function:	vcDraw
 *
 * Description:	
 *
 * In Args:	layer	VectorPlot instance
 *
 * Out Args:	NONE
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */	

static NhlErrorTypes vcDraw
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcl,
	NhlDrawOrder	order
)
#else
(vcl,order)
        NhlVectorPlotLayer vcl;
	NhlDrawOrder	order;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name = NULL;
	char			*e_text;
	NhlVectorPlotLayerPart	*vcp = &(vcl->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcl->trans);
	NhlBoolean		low_level_log = False;
	float			vfr,vlc,vrl,vhc,vrm;
	float			*u_data,*v_data,*p_data;
        NhlBoolean		all_mono = False;

	if (vcp->vector_order != order)
		return NhlNOERROR;

	switch (order) {
	case NhlPREDRAW:
		entry_name = "VectorPlotPreDraw";
		break;
	case NhlDRAW:
		entry_name = "VectorPlotDraw";
		break;
	case NhlPOSTDRAW:
		entry_name = "VectorPlotPostDraw";
		break;
	default:
		e_text = "%s: internal enumeration error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(NhlFATAL);
	}

	c_vvrset();
	subret = _NhlActivateWorkstation(vcl->base.wkptr);
	if ((ret = MIN(subret,ret)) < NhlWARNING) {
		e_text = "%s: Error activating workstation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		VectorAbortDraw(vcl);
		return NhlFATAL;
	}
	vcp->wk_active = True;

	if (vcl->view.use_segments) {
		switch (order) {
		case NhlPREDRAW:
			subret = vcInitSegment(vcl,&vcp->draw_dat,
					       entry_name);
			if ((ret = MIN(subret,ret)) < NhlWARNING) {
				VectorAbortDraw(vcl);
				return ret;
			}
			break;
		case NhlDRAW:
			subret = vcInitSegment(vcl,&vcp->draw_dat,
					       entry_name);
			if ((ret = MIN(subret,ret)) < NhlWARNING) {
				VectorAbortDraw(vcl);
				return ret;
			}
			break;
		case NhlPOSTDRAW:
			subret = vcInitSegment(vcl,&vcp->postdraw_dat,
					       entry_name);
			if ((ret = MIN(subret,ret)) < NhlWARNING) {
				VectorAbortDraw(vcl);
				return ret;
			}
			break;
		}
	}
	
/*
 * If the plot is an overlay member, use the overlay manager's trans object.
 */
	if (tfp->overlay_status == _tfCurrentOverlayMember && 
	    tfp->overlay_trans_obj != NULL) {
		vcp->trans_obj = tfp->overlay_trans_obj;
		if (vcp->do_low_level_log) {
			if (vcp->x_log) {
				subret = NhlVASetValues(
						   tfp->trans_obj->base.id,
						NhlNtrXAxisType,NhlLINEARAXIS,
						NULL);
			}
			else {
				subret = NhlVASetValues(
						   tfp->trans_obj->base.id,
						NhlNtrYAxisType,NhlLINEARAXIS,
						NULL);
			}
			if ((ret = MIN(ret,subret)) < NhlWARNING) {
				VectorAbortDraw(vcl);
				return(ret);
			}
		}
	}
	else {
		vcp->trans_obj = tfp->trans_obj;

		if (vcp->do_low_level_log) {
			subret = NhlVASetValues(vcp->trans_obj->base.id,
						NhlNtrLowLevelLogOn,True,
						NULL);
			if ((ret = MIN(ret,subret)) < NhlWARNING) {
				VectorAbortDraw(vcl);
				return(ret);
			}
			low_level_log = True;
		}
		subret = _NhlSetTrans(tfp->trans_obj, (NhlLayer)vcl);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error setting transformation";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
			VectorAbortDraw(vcl);
			return(ret);
 		}
	}
	
	switch (vcp->vfp->miss_mode) {
	case vfBOTH:
		c_vvsetr("USV",vcp->vfp->u_missing_value);
		c_vvsetr("VSV",vcp->vfp->v_missing_value);
		c_vvseti("SVF",3);
		break;
	case vfUONLY:
		c_vvsetr("USV",vcp->vfp->u_missing_value);
		c_vvseti("SVF",1);
		break;
	case vfVONLY:
		c_vvsetr("VSV",vcp->vfp->v_missing_value);
		c_vvseti("SVF",2);
		break;
	case vfNONE:
		c_vvseti("SVF",0);
		break;
	}

	if (low_level_log && vcp->x_log) {
		c_vvsetr("XC1",(float)vcp->vfp->x_start);
		c_vvsetr("XCM",(float)vcp->vfp->x_end);
	}
	else {
		c_vvsetr("XC1",(float)vcp->xc1);
		c_vvsetr("XCM",(float)vcp->xcm);
	}
	if (low_level_log && vcp->y_log) {
		c_vvsetr("YC1",(float)vcp->vfp->y_start);
		c_vvsetr("YCN",(float)vcp->vfp->y_end);
	}
	else {
		c_vvsetr("YC1",(float)vcp->yc1);
		c_vvsetr("YCN",(float)vcp->ycn);
	}
	c_vvsetc("MXT"," ");
	c_vvsetc("MNT"," ");
        c_vvseti("SET",0);
        c_vvseti("MAP",NhlvcMAPVAL);
#if 0
        c_vvseti("XIN",vcp->vfp->x_stride);
        c_vvseti("YIN",vcp->vfp->y_stride);
#endif

	if (vcp->map_direction)
		c_vvseti("TRT",1);
	else
		c_vvseti("TRT",0);

	switch (vcp->position_mode) {
	case NhlARROWHEAD:
		c_vvseti("VPO",-1);
		break;
	case NhlARROWCENTER:
	default:
		c_vvseti("VPO",0);
		break;
	case NhlARROWTAIL:
		c_vvseti("VPO",1);
		break;
	}
	vfr = MIN(1.0,MAX(0.0,vcp->min_frac_len));
	c_vvsetr("VFR",vfr);
	vlc = MAX(0.0,vcp->min_magnitude);
	c_vvsetr("VLC",-vlc);
	vhc = MAX(0.0,vcp->max_magnitude);
	c_vvsetr("VHC",-vhc);
	vrm = MAX(0.0,vcp->ref_magnitude);
	c_vvsetr("VRM",vrm);
	vrl = MAX(0.0,vcp->real_ref_length / vcl->view.width);
	c_vvsetr("VRL",vrl);
	if (vcp->scalar_mval_color <= NhlTRANSPARENT) {
		c_vvseti("SPC",0);
	}
	else {
		c_vvseti("SPC",vcp->scalar_mval_color);
	}

	if (vcp->lbls.on) {
		c_vvseti("LBL",1);
		if (vcp->labels_use_vec_color) {
			c_vvseti("LBC",-1);
		}
		else {
			c_vvseti("LBC",vcp->lbls.color);
		}
		c_vvsetr("LBS", vcp->lbls.real_height / vcl->view.width);
	}
	else {
		c_vvseti("LBL",0);
	}

	if (! vcp->fill_arrows_on) {
		c_vvseti("AST",0);
		c_vvsetr("LWD",MAX(0.0,vcp->l_arrow_thickness));
		c_vvsetr("AMN",MAX(0.0,vcp->l_arrowhead_min_size));
		c_vvsetr("AMX",MAX(0.0,vcp->l_arrowhead_max_size));
                if (vcp->mono_l_arrow_color) {
                        int lcolor = vcp->l_arrow_color;

                        all_mono = True;
                        if (lcolor <= NhlTRANSPARENT) lcolor = NhlFOREGROUND;
                        gset_line_colr_ind(
                                    _NhlGetGksCi(vcl->base.wkptr,lcolor));
                }
	}
	else {
                int fcolor = vcp->f_arrow_fill_color;
                int lcolor = vcp->f_arrow_edge_color;

		c_vvseti("AST",1);
		c_vvsetr("AWR",vcp->f_arrow_width);
		c_vvsetr("AWF",vcp->f_arrow_min_width);
		c_vvsetr("AXR",vcp->f_arrowhead_x);
		c_vvsetr("AXF",vcp->f_arrowhead_min_x);
		c_vvsetr("AYR",vcp->f_arrowhead_y);
		c_vvsetr("AYF",vcp->f_arrowhead_min_y);
		c_vvsetr("AIR",vcp->f_arrowhead_interior);
		c_vvseti("AFO",vcp->fill_over_edge);
		c_vvsetr("LWD",MAX(0.0,vcp->f_arrow_edge_thickness));

                if (vcp->mono_f_arrow_edge_color &&
                    vcp->mono_f_arrow_fill_color) {
                        all_mono = True;
                        if (fcolor <= NhlTRANSPARENT) {
                                c_vvseti("ACM",-1);
                                if (lcolor <= NhlTRANSPARENT)
                                        lcolor = NhlFOREGROUND;
                                gset_line_colr_ind( 
                                       _NhlGetGksCi(vcl->base.wkptr,lcolor));
                        }
                        else if (lcolor <= NhlTRANSPARENT) {
                                c_vvseti("ACM",-2); 
                                gset_fill_colr_ind(
                                       _NhlGetGksCi(vcl->base.wkptr,fcolor));
                        }
                        else {
                                c_vvseti("ACM",0);
                                gset_fill_colr_ind(
                                     _NhlGetGksCi(vcl->base.wkptr,fcolor));
                                gset_line_colr_ind(
                                     _NhlGetGksCi(vcl->base.wkptr,lcolor));
                        }
                }
                else if (vcp->mono_f_arrow_edge_color) {
                        if (lcolor <= NhlTRANSPARENT) {
                                c_vvseti("ACM",-2);
                        }
                        else {
                                c_vvseti("ACM",0);
                                gset_line_colr_ind(
                                     _NhlGetGksCi(vcl->base.wkptr,lcolor));
                        }
                }
                else if (vcp->mono_f_arrow_fill_color) {
                        if (fcolor <= NhlTRANSPARENT) {
                                c_vvseti("ACM",-1);
                        }
                        else {
                                c_vvseti("ACM",1);
                                gset_fill_colr_ind(
                                       _NhlGetGksCi(vcl->base.wkptr,fcolor));
                        }
                }
                else {
                        c_vvseti("ACM",3);
                }
                        
        }
	if (all_mono) {
		c_vvseti("CTV",0);
	}
	else {
		float *tvl = (float *) vcp->levels->data;
		int   *clr = (int *) vcp->level_colors->data;
		int i;

		c_vvseti("NLV",vcp->level_count + 1);
		if (! (vcp->use_scalar_array && vcp->scalar_data_init))
			c_vvseti("CTV",-1);
		else {
			c_vvseti("CTV",-2);
			if (! vcp->sfp->missing_value_set)
				c_vvseti("SPC", -1);
			else {
				if (vcp->scalar_mval_color < 0)
					c_vvseti("SPC", 0);
				else
					c_vvseti("SPC",
					     _NhlGetGksCi(vcl->base.wkptr,
						   vcp->scalar_mval_color));
				c_vvsetr("PSV",vcp->sfp->missing_value);
			}
		}

		for (i=0; i < vcp->level_count; i++) {
			c_vvseti("PAI",i+1);
			c_vvsetr("TVL",tvl[i]);
			c_vvseti("CLR",clr[i]);
		}
		c_vvseti("PAI",vcp->level_count+1);
		c_vvsetr("TVL",1E12);
		c_vvseti("CLR",clr[vcp->level_count]);
	}

	c_vvsetr("VMD",vcp->min_distance);
				 
	/* Draw the vectors */

	gset_clip_ind(GIND_CLIP);
	Need_Info = True;

	u_data = &((float *) vcp->vfp->u_arr->data)[vcp->vfp->begin]; 
	v_data = &((float *) vcp->vfp->v_arr->data)[vcp->vfp->begin];
		
	if (vcp->scalar_data_init) {
		p_data = &((float *) vcp->sfp->d_arr->data)[vcp->sfp->begin];
		subret = _NhlVvinit(u_data,vcp->vfp->fast_dim,
				    v_data,vcp->vfp->fast_dim,
				    p_data,vcp->sfp->fast_dim,
				    vcp->vfp->fast_len,vcp->vfp->slow_len,
				    vcp->fws,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error drawing vectors";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
			VectorAbortDraw(vcl);
			return(ret);
 		}
		subret = _NhlVvectr(u_data,v_data,p_data,
				    NULL,NULL,vcp->fws,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error drawing vectors";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
			VectorAbortDraw(vcl);
			return(ret);
 		}
	}
	else {
		subret = _NhlVvinit(u_data,vcp->vfp->fast_dim,
				    v_data,vcp->vfp->fast_dim,
				    NULL,0,
				    vcp->vfp->fast_len,vcp->vfp->slow_len,
				    vcp->fws,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error drawing vectors";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
			VectorAbortDraw(vcl);
			return(ret);
 		}
		subret = _NhlVvectr(u_data,v_data,NULL,
				    NULL,NULL,vcp->fws,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error drawing vectors";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
			VectorAbortDraw(vcl);
			return(ret);
 		}
	}
		
	{ /* set draw params for the vector annotion */
		float wlx,wrx,wby,wty; 
		int lnlg,invx,invy;
		_NhlvaDrawParams tdparams;
		_NhlvaDrawParams *dp = &vcp->d_params;

		memcpy(&tdparams,dp,sizeof(_NhlvaDrawParams));

		c_getset(&dp->xvpl,&dp->xvpr,&dp->xvpb,&dp->xvpt,
			 &wlx,&wrx,&wby,&wty,&lnlg);

		invx = wlx < wrx ? 0 : 1;
		invy = wby < wty ? 0 : 1;
		dp->invx = (float) invx;
		dp->invy = (float) invy;
		dp->lnlg = (float) lnlg;

		if (invx) {
			dp->wxmn = wrx;
			dp->wxmx = wlx;
		}
		else {
			dp->wxmn = wlx;
			dp->wxmx = wrx;
		}
		if (invy) {
			dp->wymn = wty;
			dp->wymx = wby;
		}
		else {
			dp->wymn = wby;
			dp->wymx = wty;
		}
		if (memcmp(&tdparams,dp,sizeof(_NhlvaDrawParams))) {
			if (vcp->refvec_anno_rec.id != NhlNULLOBJID) {
				subret = NhlVASetValues(
						vcp->refvec_anno_rec.id,
						NhlNvaDrawParams,dp,NULL);
				if ((ret = MIN(ret,subret)) < NhlWARNING) {
					VectorAbortDraw(vcl);
					return(ret);
				}
			}
			if (vcp->minvec_anno_rec.id != NhlNULLOBJID) {
				subret = NhlVASetValues(
						vcp->minvec_anno_rec.id,
						NhlNvaDrawParams,dp,NULL);
				if ((ret = MIN(ret,subret)) < NhlWARNING) {
					VectorAbortDraw(vcl);
					return(ret);
				}
			}
		}
  	}



	gset_clip_ind(GIND_NO_CLIP);
	if (vcl->view.use_segments) {
		_NhlEndSegment();
	}

	if (low_level_log) {
		subret = NhlVASetValues(vcp->trans_obj->base.id,
					NhlNtrLowLevelLogOn,False,NULL);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			VectorAbortDraw(vcl);
			return(ret);
		}
	}
        subret = _NhlDeactivateWorkstation(vcl->base.wkptr);
	ret = MIN(subret,ret);

	return MIN(subret,ret);
}


/*
 * Function:	SetCoordBounds
 *
 * Description: Sets the max and min coord bounds
 *
 * In Args:	vcp	pointer to VectorPlot layer part
 *		ctype	which coordinate
 *		count	length of irregular coord array if > 0
 *                      if 0 indicates a non-irregular transformation
 *		entry_name the high level entry point
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:   A number of fields in the VectorPlotLayerPart are set
 */

static NhlErrorTypes SetCoordBounds
#if	NhlNeedProto
(
	NhlVectorPlotLayerPart	*vcp,
	vcCoord			ctype,
	int			count,
	NhlString		entry_name
)
#else 
(vcp,ctype,count,entry_name)
	NhlVectorPlotLayerPart	*vcp;
	vcCoord			ctype;
	int			count;
	NhlString		entry_name;
#endif
{
	NhlErrorTypes	ret = NhlNOERROR;
	char		*e_text;
	float		tmin,tmax;
	NhlBoolean	rev;

	if (ctype == vcXCOORD) {

		rev = vcp->vfp->x_start > vcp->vfp->x_end;
		if (! rev) {
			tmin = MAX(vcp->vfp->x_start,vcp->x_min); 
			tmax = MIN(vcp->vfp->x_end,vcp->x_max);
		}
		else {
			tmin = MAX(vcp->vfp->x_end,vcp->x_min); 
			tmax = MIN(vcp->vfp->x_start,vcp->x_max);
		}
		vcp->xlb = tmin;
		vcp->xub = tmax;

		if (count == 0) {
			vcp->xc1 = vcp->vfp->x_start;
			vcp->xcm = vcp->vfp->x_end;
		}
		else {
			vcp->xc1 = 0;
			vcp->xcm = count - 1;

			if (tmin > vcp->x_min) {
				e_text = 
"%s: irregular transformation requires %s to be within data coordinate range: resetting";
				NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
					  entry_name,NhlNtrXMinF);
				ret = MIN(ret,NhlWARNING);
				vcp->x_min = tmin;
			}
			if (tmax < vcp->x_max) {
				e_text = 
"%s: irregular transformation requires %s to be within data coordinate range: resetting";
				NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
					  entry_name,NhlNtrXMaxF);
				ret = MIN(ret,NhlWARNING);
				vcp->x_max = tmax;
			}
		}
	}
	else if (ctype == vcYCOORD) {

		rev = vcp->vfp->y_start > vcp->vfp->y_end;
		if (! rev) {
			tmin = MAX(vcp->vfp->y_start,vcp->y_min); 
			tmax = MIN(vcp->vfp->y_end,vcp->y_max);
		}
		else {
			tmin = MAX(vcp->vfp->y_end,vcp->y_min); 
			tmax = MIN(vcp->vfp->y_start,vcp->y_max);
		}
		vcp->ylb = tmin;
		vcp->yub = tmax;

		if (count == 0) {
			vcp->yc1 = vcp->vfp->y_start;
			vcp->ycn = vcp->vfp->y_end;
		}
		else {
			vcp->yc1 = 0;
			vcp->ycn = count - 1;
			if (tmin > vcp->y_min) {
				e_text = 
"%s: irregular transformation requires %s to be within data coordinate range: resetting";
				NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
					  entry_name,NhlNtrYMinF);
				ret = MIN(ret,NhlWARNING);
				vcp->y_min = tmin;
			}
			if (tmax < vcp->y_max) {
				e_text = 
"%s: irregular transformation requires %s to be within data coordinate range: resetting";
				NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
					  entry_name,NhlNtrYMaxF);
				ret = MIN(ret,NhlWARNING);
				vcp->y_max = tmax;
			}
		}
	}
	return ret;
}

/*
 * Function:	SetUpLLTransObj
 *
 * Description: Sets up a LogLinear transformation object.
 *
 * In Args:	xnew	new instance record
 *		xold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes SetUpLLTransObj
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init
)
#else 
(vcnew,vcold,init)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
#endif
{
 	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	char			*entry_name;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcnew->trans);
	char			buffer[_NhlMAXRESNAMLEN];
	int			tmpid;
        NhlSArg			sargs[16];
        int			nargs = 0;
	NhlBoolean		yrev = False,xrev = False,oyrev,oxrev;
#if 0
	float			x,y,xr,yb;
#endif

	entry_name = (init) ? InitName : SetValuesName;

#if 0
	x = vcnew->view.x;
	y = vcnew->view.y;
	xr = x + vcnew->view.width;
	yb = y - vcnew->view.height;
	if (x < 0.0 || y > 1.0 || xr > 1.0 || yb < 0.0) {
		e_text = "%s: View extent is outside NDC range: constraining";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ret = MIN(ret,NhlWARNING);
		x = MAX(x,0.0);
		xr = MIN(xr,1.0);
		y = MIN(y,1.0);
		yb = MAX(yb,0.0);
		_NhlInternalSetView((NhlViewLayer) vcnew,x,y,
				    xr - x, y - yb, vcnew->view.keep_aspect);
	}
#endif
	if (init)
		tfp->trans_obj = NULL;
	else if (ovcp->use_irr_trans && tfp->trans_obj != NULL) {
		subret = NhlDestroy(tfp->trans_obj->base.id);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: Error destroying irregular trans object";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		tfp->trans_obj = NULL;
	}

	if (vcp->data_init) {
		subret = SetCoordBounds(vcp,vcXCOORD,0,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING)
			return ret;
		subret = SetCoordBounds(vcp,vcYCOORD,0,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING)
			return ret;
	}

	if (tfp->trans_obj == NULL) {

		vcp->new_draw_req = True;
		NhlSetSArg(&sargs[nargs++],NhlNtrXLog,vcp->x_log);
		NhlSetSArg(&sargs[nargs++],NhlNtrYLog,vcp->y_log);

		NhlSetSArg(&sargs[nargs++],NhlNtrXMinF,vcp->x_min);
		NhlSetSArg(&sargs[nargs++],NhlNtrXMaxF,vcp->x_max);
		NhlSetSArg(&sargs[nargs++],NhlNtrYMinF,vcp->y_min);
		NhlSetSArg(&sargs[nargs++],NhlNtrYMaxF,vcp->y_max);

		if (vcp->x_min_set && vcp->x_max_set)
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrXReverse,vcp->x_reverse);
		else {
			xrev = (xrev && vcp->x_reverse) || 
				(! xrev && ! vcp->x_reverse) ? False : True;
			NhlSetSArg(&sargs[nargs++],NhlNtrXReverse,xrev);
		}
		if (vcp->y_min_set && vcp->y_max_set)
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrYReverse,vcp->y_reverse);
		else {
			yrev = (yrev && vcp->y_reverse) || 
				(! yrev && ! vcp->y_reverse) ? False : True;
			NhlSetSArg(&sargs[nargs++],NhlNtrYReverse,yrev);
		}
		sprintf(buffer,"%s",vcnew->base.name);
		strcat(buffer,".Trans");

		subret = NhlALCreate(&tmpid,buffer,
				     NhllogLinTransObjClass,
				     vcnew->base.id,sargs,nargs);

		ret = MIN(subret,ret);

		tfp->trans_obj = _NhlGetLayer(tmpid);

		if(tfp->trans_obj == NULL){
			e_text = "%s: Error creating transformation object";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}

		return ret;
	}

	if (vcp->x_min != ovcp->x_min)
		NhlSetSArg(&sargs[nargs++],NhlNtrXMinF,vcp->x_min);
	if (vcp->x_max != ovcp->x_max)
		NhlSetSArg(&sargs[nargs++],NhlNtrXMaxF,vcp->x_max);
	if (vcp->y_min != ovcp->y_min)
		NhlSetSArg(&sargs[nargs++],NhlNtrYMinF,vcp->y_min);
	if (vcp->y_max != ovcp->y_max)
		NhlSetSArg(&sargs[nargs++],NhlNtrYMaxF,vcp->y_max);

	if (vcp->ovfp == NULL) {
		oxrev = False;
		oyrev = False;
	}
	else {
		oxrev = vcp->ovfp->x_start > vcp->ovfp->x_end;
		oyrev = vcp->ovfp->y_start > vcp->ovfp->y_end;
	}

	if (vcp->x_reverse != ovcp->x_reverse || oxrev != xrev) {
		if (vcp->x_min_set && vcp->x_max_set)
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrXReverse,vcp->x_reverse);
		else {
			xrev = (xrev && vcp->x_reverse) || 
				(! xrev && ! vcp->x_reverse) ? False : True;
			NhlSetSArg(&sargs[nargs++],NhlNtrXReverse,xrev);
		}
	}
	if (vcp->y_reverse != ovcp->y_reverse || oyrev != yrev) {
		if (vcp->y_min_set && vcp->y_max_set)
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrYReverse,vcp->y_reverse);
		else {
			yrev = (yrev && vcp->y_reverse) || 
				(! yrev && ! vcp->y_reverse) ? False : True;
			NhlSetSArg(&sargs[nargs++],NhlNtrYReverse,yrev);
		}
	}
	if (vcp->x_log != ovcp->x_log)
		NhlSetSArg(&sargs[nargs++],NhlNtrXLog,vcp->x_log);
	if (vcp->y_log != ovcp->y_log)
		NhlSetSArg(&sargs[nargs++],NhlNtrYLog,vcp->y_log);

	subret = NhlALSetValues(tfp->trans_obj->base.id,sargs,nargs);
	if (nargs > 0) {
		vcp->new_draw_req = True;
		vcp->update_req = True;
	}
	return MIN(ret,subret);

}

/*
 * Function:	SetUpIrrTransObj
 *
 * Description: Sets up an Irregular transformation object.
 *
 * In Args:	xnew	new instance record
 *		xold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes SetUpIrrTransObj
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init
)
#else 
(vcnew,vcold,init)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
#endif
{
 	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	char			*entry_name;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcnew->trans);
	char			buffer[_NhlMAXRESNAMLEN];
	int			tmpid;
        NhlSArg			sargs[16];
        int			nargs = 0;
	NhlBoolean		x_irr,y_irr;
	NhlBoolean		xrev,yrev,oxrev,oyrev;
	int			count;

	entry_name = (init) ? InitName : SetValuesName;

	if (! init &&
	    ! ovcp->use_irr_trans && 
	    tfp->trans_obj != NULL) {
		subret = NhlDestroy(tfp->trans_obj->base.id);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: Error destroying irregular trans object";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		tfp->trans_obj = NULL;
	}

	if (! vcp->data_init) return ret;

	x_irr = vcp->vfp->x_arr == NULL ? False : True;
	y_irr = vcp->vfp->y_arr == NULL ? False : True;
	xrev = vcp->vfp->x_start > vcp->vfp->x_end;
	yrev = vcp->vfp->y_start > vcp->vfp->y_end;
	if (! x_irr && ! y_irr) {
		e_text = "%s: Internal inconsistency setting irregular trans";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}

	if (x_irr && vcp->x_log) {
		e_text = 
"%s: X Axis cannot be logarithmic and irregular simultaneously: turning %s off";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,NhlNtrXLog);
		vcp->x_log = False;
		ret = MIN(NhlWARNING,ret);
	}
	if (y_irr && vcp->y_log) {
		e_text = 
"%s: Y Axis cannot be logarithmic and irregular simultaneously: turning %s off";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,NhlNtrYLog);
		vcp->y_log = False;
		ret = MIN(NhlWARNING,ret);
	}

	if (vcp->x_log || vcp->y_log)
		vcp->do_low_level_log = True;

	if (init || tfp->trans_obj == NULL ||
	    vcp->ovfp == NULL ||
	    vcp->vfp->x_arr != vcp->ovfp->x_arr ||
	    vcp->vfp->x_start != vcp->ovfp->x_start ||
	    vcp->vfp->x_end != vcp->ovfp->x_end ||
	    vcp->x_min != ovcp->x_min ||
	    vcp->x_max != ovcp->x_max ||
	    vcp->x_log != ovcp->x_log) {

		if (x_irr) {
			NhlSetSArg(&sargs[nargs++],NhlNtrXCoordPoints,
				   vcp->vfp->x_arr);
			count = vcp->vfp->x_arr->len_dimensions[0];
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrXAxisType,NhlIRREGULARAXIS);
		}
		else {
			if (vcp->x_log)
				NhlSetSArg(&sargs[nargs++],
					   NhlNtrXAxisType,NhlLOGAXIS);
			else
				NhlSetSArg(&sargs[nargs++],
					   NhlNtrXAxisType,NhlLINEARAXIS);
			count = 3;
		}
		subret = SetCoordBounds(vcp,vcXCOORD,count,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING)
			return ret;
		NhlSetSArg(&sargs[nargs++],NhlNtrXMinF,vcp->x_min);
		NhlSetSArg(&sargs[nargs++],NhlNtrXMaxF,vcp->x_max);
	}

	if (init || tfp->trans_obj == NULL ||
	    vcp->ovfp == NULL ||
	    vcp->vfp->y_arr != vcp->ovfp->y_arr ||
	    vcp->vfp->y_start != vcp->ovfp->y_start ||
	    vcp->vfp->y_end != vcp->ovfp->y_end ||
	    vcp->y_min != ovcp->y_min ||
	    vcp->y_max != ovcp->y_max ||
	    vcp->y_log != ovcp->y_log) {

		if (y_irr) {
			NhlSetSArg(&sargs[nargs++],NhlNtrYCoordPoints,
				   vcp->vfp->y_arr);
			count = vcp->vfp->y_arr->len_dimensions[0];
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrYAxisType,NhlIRREGULARAXIS);
		}
		else {
			if (vcp->y_log)
				NhlSetSArg(&sargs[nargs++],
					   NhlNtrYAxisType,NhlLOGAXIS);
			else
				NhlSetSArg(&sargs[nargs++],
					   NhlNtrYAxisType,NhlLINEARAXIS);
			count = 3;
		}
		subret = SetCoordBounds(vcp,vcYCOORD,count,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING)
			return ret;

		NhlSetSArg(&sargs[nargs++],NhlNtrYMinF,vcp->y_min);
		NhlSetSArg(&sargs[nargs++],NhlNtrYMaxF,vcp->y_max);
	}

	if (init || tfp->trans_obj == NULL) {

		vcp->new_draw_req = True;

		if (vcp->x_min_set && vcp->x_max_set)
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrXReverse,vcp->x_reverse);
		else {
			xrev = (xrev && vcp->x_reverse) || 
				(! xrev && ! vcp->x_reverse) ? False : True;
			NhlSetSArg(&sargs[nargs++],NhlNtrXReverse,xrev);
		}
		if (vcp->y_min_set && vcp->y_max_set)
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrYReverse,vcp->y_reverse);
		else {
			yrev = (yrev && vcp->y_reverse) || 
				(! yrev && ! vcp->y_reverse) ? False : True;
			NhlSetSArg(&sargs[nargs++],NhlNtrYReverse,yrev);
		}
		sprintf(buffer,"%s",vcnew->base.name);
		strcat(buffer,".Trans");

		subret = NhlALCreate(&tmpid,buffer,
				     NhlirregularTransObjClass,
				     vcnew->base.id,sargs,nargs);

		ret = MIN(subret,ret);

		tfp->trans_obj = _NhlGetLayer(tmpid);

		if(tfp->trans_obj == NULL){
			e_text = "%s: Error creating transformation object";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}

		return ret;
	}

	if (vcp->ovfp == NULL) {
		oxrev = False;
		oyrev = False;
	}
	else {
		oxrev = vcp->ovfp->x_start > vcp->ovfp->x_end;
		oyrev = vcp->ovfp->y_start > vcp->ovfp->y_end;
	}
	if (vcp->x_reverse != ovcp->x_reverse || xrev != oxrev) {
		if (vcp->x_min_set && vcp->x_max_set)
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrXReverse,vcp->x_reverse);
		else {
			xrev = (xrev && vcp->x_reverse) || 
				(! xrev && ! vcp->x_reverse) ? False : True;
			NhlSetSArg(&sargs[nargs++],NhlNtrXReverse,xrev);
		}
	}
	if (vcp->y_reverse != ovcp->y_reverse || yrev != oyrev) {
		if (vcp->y_min_set && vcp->y_max_set)
			NhlSetSArg(&sargs[nargs++],
				   NhlNtrYReverse,vcp->y_reverse);
		else {
			yrev = (yrev && vcp->y_reverse) || 
				(! yrev && ! vcp->y_reverse) ? False : True;
			NhlSetSArg(&sargs[nargs++],NhlNtrYReverse,yrev);
		}
	}
	subret = NhlALSetValues(tfp->trans_obj->base.id,sargs,nargs);

	if (nargs > 0) {
		vcp->new_draw_req = True;
		vcp->update_req = True;
	}
	return MIN(ret,subret);

}


/*
 * Function:	SetFormat
 *
 * Description: Sets up the format records for all the Conpack label.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes SetFormat
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init
)
#else 
(vcnew,vcold,init)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
#endif
{
	NhlErrorTypes ret = NhlNOERROR, subret = NhlNOERROR;
	NhlString e_text;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlString entry_name;

	entry_name =  init ? InitName : SetValuesName;
/*
 * check the constant spacing value - by the name of the routine this
 * does not belong here -- but for now, it will do
 */
	e_text = 
		"%s: Constant spacing cannot be less than zero, defaulting %s";
	if (vcp->refvec_anno.cspacing < 0.0) {
		vcp->refvec_anno.cspacing = 0.0;
		NhlPError(NhlWARNING,NhlEUNKNOWN,
			  e_text,entry_name,NhlNvcRefAnnoConstantSpacingF);
		ret = MIN(NhlWARNING,ret);
	}
	if (vcp->minvec_anno.cspacing < 0.0) {
		vcp->minvec_anno.cspacing = 0.0;
		NhlPError(NhlWARNING,NhlEUNKNOWN,
			  e_text,entry_name,NhlNvcMinAnnoConstantSpacingF);
		ret = MIN(NhlWARNING,ret);
	}
	if (vcp->zerof_lbl.cspacing < 0.0) {
		vcp->zerof_lbl.cspacing = 0.0;
		NhlPError(NhlWARNING,NhlEUNKNOWN,
			  e_text,entry_name,NhlNvcZeroFLabelConstantSpacingF);
		ret = MIN(NhlWARNING,ret);
	}

	if (init) {
		subret = SetFormatRec(&vcp->mag_scale.format,
				      NhlNvcMagnitudeFormat,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
		subret = SetFormatRec(&vcp->svalue_scale.format,
				      NhlNvcScalarValueFormat,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
		
		return ret;
	}
	if (vcp->mag_scale.format.fstring != 
	    ovcp->mag_scale.format.fstring) {
		subret = SetFormatRec(&vcp->mag_scale.format,
				      NhlNvcMagnitudeFormat,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
		if (ovcp->mag_scale.format.fstring != NULL)
			NhlFree(ovcp->mag_scale.format.fstring);
		ovcp->mag_scale.format.fstring = NULL;
	}
	if (vcp->svalue_scale.format.fstring != 
	    ovcp->svalue_scale.format.fstring) {
		subret = SetFormatRec(&vcp->svalue_scale.format,
				      NhlNvcScalarValueFormat,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
		if (ovcp->svalue_scale.format.fstring != NULL)
			NhlFree(ovcp->svalue_scale.format.fstring);
		ovcp->svalue_scale.format.fstring = NULL;
	}
	return ret;
}

/*
 * Function:	ManageLabels
 *
 * Description: Manages all the non-array label types (not line labels).
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes ManageLabels
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
)
#else 
(vcnew,vcold, init, sargs, nargs)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	NhlSArg		*sargs;
	int		*nargs;
#endif
{
	NhlErrorTypes ret = NhlNOERROR, subret = NhlNOERROR;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlString entry_name, e_text;

	entry_name =  init ? InitName : SetValuesName;

	if (init) {
		vcp->refvec_anno.name = ".RefVecAnno";
		vcp->minvec_anno.name = ".MinVecAnno";
		vcp->zerof_lbl.name = ".ZeroField";
		vcp->refvec_anno.text1 = NULL;
		vcp->refvec_anno.text2 = NULL;
		vcp->refvec_anno.aap = &vcp->ref_attrs;
		ovcp->refvec_anno.aap = &vcp->ref_attrs;
		vcp->minvec_anno.text1 = NULL;
		vcp->minvec_anno.text2 = NULL;
		vcp->minvec_anno.aap = &vcp->min_attrs;
		ovcp->minvec_anno.aap = &vcp->min_attrs;
		vcp->zerof_lbl.text1 = NULL;
		vcp->zerof_lbl.text2 = NULL;
		vcp->zerof_lbl.aap = NULL;
		vcp->zerof_lbl.string1_on = False;
		vcp->lbls.text1 = NULL;
		vcp->lbls.text2 = NULL;
	}
	else {
		vcp->refvec_anno.aap = &vcp->ref_attrs;
		ovcp->refvec_anno.aap = &ovcp->ref_attrs;
		vcp->minvec_anno.aap = &vcp->min_attrs;
		ovcp->minvec_anno.aap = &ovcp->min_attrs;
	}
		
/*
 * Set up the label strings and the format records
 */
	if (vcp->use_refvec_anno_attrs && vcp->refvec_anno.on) {
		subret = CopyTextAttrs(&vcp->zerof_lbl,
				       &vcp->refvec_anno,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;
	}
	if (vcp->use_refvec_anno_attrs && vcp->refvec_anno.on) {
		subret = CopyTextAttrs(&vcp->minvec_anno,
				       &vcp->refvec_anno,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;
		vcp->minvec_anno.aap->arrow_space = 
			vcp->refvec_anno.aap->arrow_space;
		vcp->minvec_anno.aap->arrow_min_offset = 
			vcp->refvec_anno.aap->arrow_min_offset;
		vcp->minvec_anno.aap->use_vec_color = 
			vcp->refvec_anno.aap->use_vec_color;
		vcp->minvec_anno.aap->arrow_line_color = 
			vcp->refvec_anno.aap->arrow_line_color;
		vcp->minvec_anno.aap->arrow_fill_color = 
			vcp->refvec_anno.aap->arrow_fill_color;
	}

/* Manage constant field label */

	subret = ManageZeroFLabel(vcnew,vcold,init,sargs,nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing constant field label";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
	

/* Manage the vector annotations */


	vcp->refvec_anno.aap->vec_len = vcp->real_ref_length;
	if (vcp->refvec_anno.aap->vec_mag <= 0.0) {
		if (vcp->ref_magnitude > 0.0)
			vcp->refvec_anno.aap->real_vec_mag 
				= vcp->ref_magnitude;
		else
			vcp->refvec_anno.aap->real_vec_mag = vcp->zmax;
	}
	else {
		vcp->refvec_anno.aap->real_vec_mag = 
			vcp->refvec_anno.aap->vec_mag;
		if (vcp->min_frac_len > 0.0) {
			float minlen, minmag, refmag;

                        minlen = vcp->min_frac_len * vcp->real_ref_length;
			refmag = (vcp->ref_magnitude > 0.0) ?
				vcp->ref_magnitude : 
                                        MIN(vcp->max_magnitude,vcp->zmax);
                        minmag = (vcp->min_magnitude > 0.0) ?
                                vcp->min_magnitude : vcp->zmin;

			vcp->refvec_anno.aap->vec_len = minlen +
			      (vcp->real_ref_length - minlen) *
			      (vcp->refvec_anno.aap->real_vec_mag - minmag) / 
					(refmag - minmag);
		}
		else {
			float refmag = (vcp->ref_magnitude > 0.0) ?
				vcp->ref_magnitude : vcp->zmax;
			vcp->refvec_anno.aap->vec_len = vcp->real_ref_length *
				vcp->refvec_anno.aap->vec_mag / refmag;
		}
			
	}
	subret = ManageVecAnno(vcnew,vcold,init,
			       &vcp->refvec_anno,&ovcp->refvec_anno,
			       &vcp->refvec_anno_rec,
			       &ovcp->refvec_anno_rec,
			       &vcp->refvec_anno_id,
			       NhlvcDEF_REFVEC_STRING1,
			       NhlvcDEF_REFVEC_STRING2,
			       sargs,nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing vector annotation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

	if (vcp->min_frac_len > 0.0) {
		vcp->minvec_anno.aap->vec_len = 
			vcp->min_frac_len * vcp->real_ref_length;
	}
	else if (vcp->ref_magnitude > 0.0) {
		vcp->minvec_anno.aap->vec_len = vcp->real_ref_length *
			MAX(vcp->zmin,vcp->min_magnitude) / vcp->ref_magnitude;
	}
	else {
		vcp->minvec_anno.aap->vec_len = vcp->real_ref_length *
			MAX(vcp->zmin,vcp->min_magnitude) / vcp->zmax;
	}

	if (vcp->minvec_anno.aap->vec_mag <= 0.0) {
		vcp->minvec_anno.aap->real_vec_mag = 
			MAX(vcp->a_params.uvmn,vcp->zmin);
	}
	else {
		vcp->minvec_anno.aap->real_vec_mag = 
			vcp->minvec_anno.aap->vec_mag;
		if (vcp->min_frac_len > 0.0) {
			float minlen, minmag, refmag;

                        minlen = vcp->min_frac_len * vcp->real_ref_length;
			refmag = (vcp->ref_magnitude > 0.0) ?
				vcp->ref_magnitude : 
                                        MIN(vcp->max_magnitude,vcp->zmax);
                        minmag = (vcp->min_magnitude > 0.0) ?
                                vcp->min_magnitude : vcp->zmin;

			vcp->minvec_anno.aap->vec_len = minlen +
			       (vcp->real_ref_length - minlen) *
			       (vcp->minvec_anno.aap->real_vec_mag - minmag) / 
					(refmag - minmag);
		}
		else {
			float refmag = (vcp->ref_magnitude > 0.0) ?
				vcp->ref_magnitude : vcp->zmax;
			vcp->minvec_anno.aap->vec_len = vcp->real_ref_length *
				vcp->minvec_anno.aap->vec_mag / refmag;
		}
			
	}
	subret = ManageVecAnno(vcnew,vcold,init,
			       &vcp->minvec_anno,&ovcp->minvec_anno,
			       &vcp->minvec_anno_rec,
			       &ovcp->minvec_anno_rec,
			       &vcp->minvec_anno_id,
			       NhlvcDEF_MINVEC_STRING1,
			       NhlvcDEF_MINVEC_STRING2,
			       sargs,nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing vector annotation";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
	return ret;
}

/*
 * Function:	SetScale
 *
 * Description: Determines the label scale factor based on the label
 *		scale mode and the label scale value resources. Note that
 *		the scale factor is the amount by which the label values
 *		are multiplied to arrive at the true values in the vector
 *		field data. Therefore the data values are divided by the
 *		scale factor to get the label values.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	
 */
static NhlErrorTypes SetScale
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlvcScaleInfo		*sip,
	NhlvcScaleInfo		*osip,
	NhlBoolean		do_levels,
	NhlBoolean		init
)
#else 
(vcnew,vcold,sip,osip,init)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlvcScaleInfo		*sip;
	NhlvcScaleInfo		*osip;
	NhlBoolean		do_levels;
	NhlBoolean		init;
#endif
{
	NhlErrorTypes ret = NhlNOERROR, subret = NhlNOERROR;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlString entry_name, e_text;
	float sigval,t;
	int power,i,count;
	int divpwr,sig_digits;
	float *fp;
	int max_digit = 0;
	float test_high,test_low,max_fac = 1.0;

	if (! init &&
	    (vcp->levels == ovcp->levels) &&
	    (sip->mode == osip->mode) &&
	    (sip->scale_value == osip->scale_value) &&
	    (sip->min_val == osip->min_val) &&
	    (sip->max_val == osip->max_val))
		return ret;

	entry_name =  init ? InitName : SetValuesName;

	sigval = MAX(fabs(sip->max_val),fabs(sip->min_val));
	subret = _NhlGetScaleInfo(sigval,&divpwr,&sig_digits,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		return NhlFATAL;
	}
	sip->left_sig_digit = divpwr - 1;
	sip->sig_digits = 4;
	sig_digits = (sip->format.sig_digits_flag == NhlffUNSPECED) ?
		sip->sig_digits : sip->format.sig_digits;

	switch (sip->mode) {
	case NhlSCALEFACTOR:
		if (sip->scale_value <= 0.0) {
			e_text = 
			     "%s: invalid value for scale value: defaulting";
			NhlPError(NhlWARNING,NhlEUNKNOWN,e_text, entry_name);
			ret = MIN(ret,NhlWARNING);
			sip->scale_value = 1.0;
		}
		sip->scale_factor = sip->scale_value;
		break;
	case NhlCONFINETORANGE:
		if (sip->scale_value <= 0.0) {
			e_text = 
			     "%s: invalid value for scale value: defaulting";
			NhlPError(NhlWARNING,NhlEUNKNOWN,e_text, entry_name);
			ret = MIN(ret,NhlWARNING);
			sip->scale_value = 1.0;
		}
		power = 1;
		if (sigval >= sip->scale_value) {
			for (t = sigval/10.0;
			     t >=sip->scale_value; t /= 10.0) {
				power++;
			}
			sip->scale_factor = pow(10.0,(double)power);
		}
		else {
			for (t = sigval * 10;
			     t < sip->scale_value; t *= 10.0) {
				power++;
			}
			power--;
			sip->scale_factor = pow(10.0,-(double)power);
		}
		break;
	case NhlTRIMZEROS:
		if (divpwr < 0) 
			power = divpwr;
		else
			power = MAX(0,divpwr - sig_digits);
		sip->scale_factor = pow(10.0,(double)power);
		break;
	case NhlMAXSIGDIGITSLEFT:
		power = divpwr - sig_digits;
		sip->scale_factor = pow(10.0,(double)power);
		break;
	case NhlALLINTEGERS:
		if (! do_levels) {
			fp = &sigval;
			count = 1;
		}
		else {
			fp = (float *) vcp->levels->data;
			count = vcp->level_count;
		}
		test_high = pow(10.0,sig_digits);
		test_low  = pow(10.0,sig_digits - 1);

		for (i = 0; i < count; i++) {
			int	j;
			float	test_fac = 1.0;
			char	buf[32];

			t = fabs(fp[i]);
			if (t == 0.0) 
				continue;
			if (fabs(fp[i]) < test_low) {
				while (t < test_low) {
					t *= 10.0;
					test_fac *= 10.0;
				}
			}
			else if (fabs(fp[i]) >= test_high) {
				while (t >= test_high) {
					t /= 10.0;
					test_fac /= 10.0;
				}
			}
			t = (float) (int) (t + 0.5);

			sprintf(buf,"%f",t);
			j = strcspn(buf,"0.");
			if (j > max_digit) {
				max_digit = j;
				max_fac = test_fac;
			}
		}
		while ((t = sig_digits) > max_digit) {
			max_fac /= 10.0;
			t--;
		}
	
		sip->scale_factor = 1.0 / max_fac;
		break;
	default:
		e_text = "%s: internal enumeration error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}

	return ret;
}
/*
 * Function:	ManageOverlay
 *
 * Description: Sets up arguments for annotations handled internally by
 *		the overlay object (TickMark,Title,LabelBar), then
 *		calls the overlay interface function _NhlManageOverlay.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	
 */
static NhlErrorTypes ManageOverlay
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
)
#else 
(vcnew,vcold, init, sargs, nargs)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	NhlSArg		*sargs;
	int		*nargs;
#endif
{
	NhlErrorTypes ret = NhlNOERROR, subret = NhlNOERROR;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlString entry_name, e_text;

	entry_name =  init ? InitName : SetValuesName;

/* Manage TickMarks object */

	/* 18 arguments possible */
	subret = ManageTickMarks(vcnew,vcold,init,sargs,nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing TickMarks";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

/* Manage Legend object */

	/* 18 arguments possible */
	subret = ManageTitles(vcnew,vcold,init,sargs,nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing Titles";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}


/* Manage LabelBar object */

	subret = ManageLabelBar(vcnew,vcold,init,sargs,nargs);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error managing LabelBar";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

/* Manage the overlay */

	/* 1 arg */
	if (vcp->update_req) {
		vcp->new_draw_req = True;
		NhlSetSArg(&sargs[(*nargs)++],NhlNpmUpdateReq,True);
	}
		
	subret = _NhlManageOverlay(&vcp->overlay_object,
				   (NhlLayer)vcnew,(NhlLayer)vcold,init,
				   sargs,*nargs,entry_name);
	ret = MIN(ret,subret);
	return ret;

}
/*
 * Function:	ManageTickMarks
 *
 * Description: If the VectorPlot object has an overlay object attached, and
 *		the TickMarks are activated, manages the TickMark resources 
 *		relevant to the VectorPlot object.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes ManageTickMarks
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
)
#else 
(vcnew, vcold, init, sargs, nargs)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	NhlSArg		*sargs;
	int		*nargs;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*entry_name;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcnew->trans);

	entry_name = (init) ? InitName : SetValuesName;

 	if (! tfp->plot_manager_on ||
	    vcp->display_tickmarks == NhlNOCREATE) 
		return NhlNOERROR;

	if (init || 
	    vcp->display_tickmarks != ovcp->display_tickmarks) {
			NhlSetSArg(&sargs[(*nargs)++],
				   NhlNpmTickMarkDisplayMode,
				   vcp->display_tickmarks);
	}

	return ret;
}

/*
 * Function:	ManageTitles
 *
 * Description: If the VectorPlot object has an overlay object attached, and
 *		the Titles are activated, manages the Title resources 
 *		relevant to the VectorPlot object.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes ManageTitles
#if	NhlNeedProto
(
 	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
)
#else 
(vcnew, vcold, init, sargs, nargs)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	NhlSArg		*sargs;
	int		*nargs;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*entry_name;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcnew->trans);

	entry_name = (init) ? InitName : SetValuesName;

 	if (! tfp->plot_manager_on ||
	    vcp->display_titles == NhlNOCREATE) 
		return NhlNOERROR;

	if (init || 
	    vcp->display_titles != ovcp->display_titles) {
			NhlSetSArg(&sargs[(*nargs)++],
				   NhlNpmTitleDisplayMode,
				   vcp->display_titles);
	}

	return ret;
}


/*
 * Function:	ManageLabelBar
 *
 * Description: If the VectorPlot object has an overlay object attached, and
 *		the LabelBar is activated, manages the LabelBar resources 
 *		relevant to the VectorPlot object.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes ManageLabelBar
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
)
#else 
(vcnew,vcold, init, sargs, nargs)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	NhlSArg		*sargs;
	int		*nargs;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*e_text;
	char			*entry_name;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcnew->trans);
	NhlBoolean		redo_level_strings = False;
	NhlBoolean		set_all = False;

	entry_name = (init) ? InitName : SetValuesName;

 	if (! tfp->plot_manager_on ||
	    vcp->display_labelbar == NhlNOCREATE) 
		return NhlNOERROR;

	if (init || 
	    vcp->display_labelbar != ovcp->display_labelbar ||
	    vcp->zero_field != ovcp->zero_field ||
	    vcp->data_init != ovcp->data_init) {

		if ( vcp->zero_field) {
			e_text = "%s: zero field: turning Labelbar off";
			NhlPError(NhlINFO,NhlEUNKNOWN,e_text,entry_name);
			ret = MIN(ret,NhlINFO);
			NhlSetSArg(&sargs[(*nargs)++],
				   NhlNpmLabelBarDisplayMode,NhlNEVER);
			return ret;
		}
		else {
			NhlSetSArg(&sargs[(*nargs)++],
				   NhlNpmLabelBarDisplayMode,
				   vcp->display_labelbar);
			if (init || vcp->zero_field != ovcp->zero_field) 
				set_all = True;
		}
	}

	if (vcp->zero_field) return ret;

	if (! vcp->explicit_lbar_labels_on) {
		vcp->lbar_labels_set = False;
		if (init || set_all ||
		    vcp->lbar_end_labels_on != ovcp->lbar_end_labels_on ||
		    vcp->explicit_lbar_labels_on 
		    			!= ovcp->explicit_lbar_labels_on ||
		    vcp->level_strings != ovcp->level_strings) {
			redo_level_strings = True;
		}
		if (vcp->lbar_end_labels_on)
			vcp->lbar_alignment = NhlEXTERNALEDGES;
		else
			vcp->lbar_alignment = NhlINTERIOREDGES;
	}
	else {
		if (vcp->lbar_labels_res_set) {
			NhlGenArray ga;
			if (vcp->lbar_labels != NULL) 
				NhlFreeGenArray(vcp->lbar_labels);

			if ((ga = _NhlCopyGenArray(vcp->lbar_labels_res,
						   True)) == NULL) {
				e_text = "%s: error copying GenArray";
				NhlPError(NhlFATAL,
					  NhlEUNKNOWN,e_text,entry_name);
				return NhlFATAL;
			}
			vcp->lbar_labels = ga;
			ovcp->lbar_labels = NULL;
		}
		else if (! vcp->lbar_labels_set) {
			redo_level_strings = True;
			if (vcp->lbar_end_labels_on)
				vcp->lbar_alignment = NhlEXTERNALEDGES;
			else
				vcp->lbar_alignment = NhlINTERIOREDGES;
		}
		vcp->lbar_labels_set = True;
	}

	if (redo_level_strings) {
		NhlGenArray ga;
		NhlString *to_sp, *from_sp;
		NhlString s;
		int i, count;
		NhlBoolean copy = False;

		from_sp = (NhlString *) vcp->level_strings;
		if (vcp->lbar_labels != NULL) 
			NhlFreeGenArray(vcp->lbar_labels);

		if (! vcp->lbar_end_labels_on) {
			/* level_strings is used for lbar_labels data */
			count = vcp->level_count;
			to_sp = from_sp;
		}
		else {
			float fval;
			NhlFormatRec *frec;
			NhlvcScaleInfo	*sip;

			sip=(vcp->use_scalar_array && vcp->scalar_data_init) ?
				   &vcp->svalue_scale : &vcp->mag_scale;
			frec = &sip->format;
			copy = True;
			count = vcp->level_count + 2;
			to_sp = NhlMalloc(sizeof(NhlString) * count);
			if (to_sp == NULL) {
				e_text = "%s: dynamic memory allocation error";
				NhlPError(NhlFATAL,
					  NhlEUNKNOWN,e_text,entry_name);
				return NhlFATAL;
			}

			fval = sip->min_val / sip->scale_factor;
			s = _NhlFormatFloat(frec,fval,NULL,
					    &sip->sig_digits,
					    &sip->left_sig_digit,
					    NULL,NULL,NULL,
					    vcp->lbar_func_code,
					    entry_name);
			if (s == NULL) return NhlFATAL;
			to_sp[0] = NhlMalloc(strlen(s) + 1);
			if (to_sp[0] == NULL) {
				e_text = "%s: dynamic memory allocation error";
				NhlPError(NhlFATAL,
					  NhlEUNKNOWN,e_text,entry_name);
				return NhlFATAL;
			}
			strcpy(to_sp[0],s);
			for (i = 1; i < count - 1; i++) {
				to_sp[i] = NhlMalloc(strlen(from_sp[i-1]) + 1);
				if (to_sp[i] == NULL) {
					e_text = 
					"%s: dynamic memory allocation error";
					NhlPError(NhlFATAL,NhlEUNKNOWN,
						  e_text,entry_name);
					return NhlFATAL;
				}
				strcpy(to_sp[i],from_sp[i-1]);
			}
			fval = sip->max_val / sip->scale_factor;
			s = _NhlFormatFloat(frec,fval,NULL,
					    &sip->sig_digits,
					    &sip->left_sig_digit,
					    NULL,NULL,NULL,
					    vcp->lbar_func_code,
					    entry_name);
			if (s == NULL) return NhlFATAL;
			to_sp[count - 1] = NhlMalloc(strlen(s) + 1);
			if (to_sp[count - 1] == NULL) {
				e_text = "%s: dynamic memory allocation error";
				NhlPError(NhlFATAL,
					  NhlEUNKNOWN,e_text,entry_name);
				return NhlFATAL;
			}
			strcpy(to_sp[count - 1],s);
		}
		ga = NhlCreateGenArray((NhlPointer)to_sp,NhlTString,
				       sizeof(NhlString),1,&count);
		if (ga == NULL) {
			e_text = "%s: error creating GenArray";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		ga->my_data = copy ? True : False;
		vcp->lbar_labels = ga;
		ovcp->lbar_labels = NULL;
	}
	if (init || set_all) {

		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbBoxCount,vcp->level_count + 1);
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbLabelAlignment,vcp->lbar_alignment);
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbLabelStrings,vcp->lbar_labels);
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbMonoFillColor,False);
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbFillColors,vcp->level_colors);
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbMonoFillPattern,True);
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbMonoFillScale,True);
		return ret;
	}

	if (vcp->level_count != ovcp->level_count)
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbBoxCount,vcp->level_count + 1);
	if (vcp->lbar_alignment != ovcp->lbar_alignment)
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbLabelAlignment,vcp->lbar_alignment);
	if (vcp->lbar_labels != ovcp->lbar_labels)
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbLabelStrings,vcp->lbar_labels);
	if (vcp->level_colors != ovcp->level_colors)
		NhlSetSArg(&sargs[(*nargs)++],
			   NhlNlbFillColors,vcp->level_colors);
	return ret;
}


/*
 * Function:	PrepareAnnoString
 *
 * Description: Prepare a string for use as an annotation
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes PrepareAnnoString
#if	NhlNeedProto
(
	NhlVectorPlotLayerPart	*vcp,
	NhlVectorPlotLayerPart	*ovcp,
	NhlBoolean		init,
	float			value,
	float			old_value,
	NhlString		*new_string,
	NhlString		old_string,
	NhlString		def_string,
	NhlString		*formatted_string,
	char			func_code,
	NhlBoolean		*changed,
	NhlString		entry_name
)
#else 
(vcp,ovcp,init,value,old_value,new_string,old_string,def_string,
 formatted_string,func_code,changed,entry_name)
	NhlVectorPlotLayerPart	*vcp;
	NhlVectorPlotLayerPart	*ovcp;
	NhlBoolean		init;
	float			value;
	float			old_value;
	NhlString		*new_string;
	NhlString		old_string;
	NhlString		def_string;
	NhlString		*formatted_string;
	char			func_code;
	NhlBoolean		*changed;
	NhlString		entry_name;
#endif
{
	char			*e_text;
	NhlErrorTypes		ret = NhlNOERROR;
	NhlString		lstring;
	NhlBoolean		done = False;
	char			buffer[256];
	char			*matchp,*subst;
	float			val;
	NhlvcScaleInfo		*sip;
	NhlBoolean		modify;
	int			left_sig_digit;
	NhlffStat		left_sig_digit_flag;

	*changed = False;

	if (! vcp->data_changed && ! init && 
	    (vcp->levels == ovcp->levels) &&
	    (value == old_value) &&
	    (*new_string == old_string)) {
		return NhlNOERROR;
	}

	if (init || *new_string != old_string) {
		int strsize = *new_string == NULL ? 
			strlen(def_string) + 1 : strlen(*new_string) + 1;
		if ((lstring = NhlMalloc(strsize)) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		if (*new_string == NULL)
			strcpy(lstring,def_string);
		else
			strcpy(lstring,*new_string);
		*new_string = lstring;
		if (!init && old_string != NULL) {
			NhlFree(old_string);
			old_string = NULL;
		}
	}
	strcpy(buffer,*new_string);
	while (! done) {
		modify = False;
		if ((matchp = strstr(buffer,"$VMG$")) != NULL) {
			val = value / vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$MNM$")) != NULL) {
			val = vcp->zmin / vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$MXM$")) != NULL) {
			val = vcp->zmax / vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$RFM$")) != NULL) {
			val = vcp->ref_magnitude / vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$MSF$")) != NULL) {
			modify = True;
			val = vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$MNU$")) != NULL) {
			val = vcp->umin / vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$MXU$")) != NULL) {
			val = vcp->umax / vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$MNV$")) != NULL) {
			val = vcp->vmin / vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$MXV$")) != NULL) {
			val = vcp->vmax / vcp->mag_scale.scale_factor;
			sip = &vcp->mag_scale;
		}
		else if ((matchp = strstr(buffer,"$MNS$")) != NULL) {
			val = vcp->scalar_min / vcp->svalue_scale.scale_factor;
			sip = &vcp->svalue_scale;
		}
		else if ((matchp = strstr(buffer,"$MXS$")) != NULL) {
			val = vcp->scalar_max / vcp->svalue_scale.scale_factor;
			sip = &vcp->svalue_scale;
		}
		else if ((matchp = strstr(buffer,"$SSF$")) != NULL) {
			modify = True;
			val = vcp->svalue_scale.scale_factor;
			sip = &vcp->svalue_scale;
		}
		else {
			done = True;
			break;
		}
		if (modify) {
			left_sig_digit = sip->format.left_sig_digit;
			left_sig_digit_flag = sip->format.left_sig_digit_flag;
			sip->format.left_sig_digit_flag = NhlffUNSPECED;
			sip->format.left_sig_digit = -10000;
		}
			
		subst = _NhlFormatFloat(&sip->format,val,NULL,
					&sip->sig_digits,&sip->left_sig_digit,
					NULL,NULL,NULL,func_code,entry_name);
		if (subst == NULL) return NhlFATAL;
		Substitute(matchp,5,subst);

		if (modify) {
			sip->format.left_sig_digit_flag = left_sig_digit_flag;
			sip->format.left_sig_digit = left_sig_digit;
		}
	}
	if (*formatted_string != NULL)
		NhlFree(*formatted_string);
	if ((*formatted_string = 
	     NhlMalloc(strlen(buffer)+1)) == NULL) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}
	strcpy(*formatted_string,buffer);

	*changed = True;

	return ret;
}

/*
 * Function:	ManageVecAnno
 *
 * Description: If the vector annotation is
 *		activated text items to store the strings are created.
 *		If there is a PlotManager an AnnoManager is 
 *		created so that the plotManager object can manage the
 *		annotations.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes ManageVecAnno
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean		init,
	NhlvcLabelAttrs		*ilp,
	NhlvcLabelAttrs		*oilp,
	NhlAnnotationRec	*anrp,
	NhlAnnotationRec	*oanrp,
	int			*idp,
	NhlString		def_string1,
	NhlString		def_string2,
	NhlSArg			*sargs,
	int			*nargs
)
#else 
(vcnew,vcold,init,ilp,oilp,anrp,oanrp,idp,def_string1,def_string2,sargs,nargs)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean		init;
	NhlvcLabelAttrs		*ilp;
	NhlvcLabelAttrs		*oilp;
	NhlAnnotationRec	*anrp;
	NhlAnnotationRec	*oanrp;
	int			*idp,
	NhlString		def_string1;
	NhlString		def_string2;
	NhlSArg			*sargs;
	int			*nargs;
#endif
{
	char			*e_text;
	char			*entry_name;
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcnew->trans);
	NhlBoolean		text_changed;
	NhlSArg			targs[32];
	int			targc = 0;
	char			buffer[_NhlMAXRESNAMLEN];
	int			tmpid;
	NhlBoolean		on;

	entry_name = (init) ? InitName : SetValuesName;

	subret = PrepareAnnoString(vcp,ovcp,init,
				   ilp->aap->real_vec_mag,
				   oilp->aap->real_vec_mag,
				   &ilp->string1,oilp->string1,
				   def_string1,&ilp->text1,
				   ilp->fcode[0],
				   &text_changed,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
	if (text_changed) oilp->text1 = NULL;

	subret = PrepareAnnoString(vcp,ovcp,init,
				   ilp->aap->real_vec_mag,
				   oilp->aap->real_vec_mag,
				   &ilp->string2,oilp->string2,
				   def_string2,&ilp->text2,
				   ilp->fcode[0],
				   &text_changed,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
	if (text_changed) oilp->text2 = NULL;

	ilp->aap->real_arrow_line_thickness = vcp->fill_arrows_on ?
		vcp->f_arrow_edge_thickness : vcp->l_arrow_thickness;

	if (! ilp->aap->use_vec_color) {
		ilp->aap->real_arrow_line_color = vcp->fill_arrows_on ?
			ilp->aap->arrow_edge_color : 
				ilp->aap->arrow_line_color;
		ilp->aap->real_arrow_fill_color = ilp->aap->arrow_fill_color;
	}
	else {
		int i;
		float *fp = (float *) vcp->levels->data;
		int *ip = (int *) vcp->level_colors->data;
		float mag = ilp->aap->real_vec_mag;
		NhlBoolean set = False;
		int color = 1;
		for (i = 0; i < vcp->level_count; i++) {
			if (mag <= fp[i]) {
				color = ip[i];
				set = True;
				break;
			}
		}
		if (! set) {
			color = ip[vcp->level_count];
		}
		if (vcp->mono_f_arrow_fill_color ||
		    (vcp->use_scalar_array && vcp->scalar_data_init)) {
			ilp->aap->real_arrow_fill_color = 
				vcp->f_arrow_fill_color;
		}
		else {
			ilp->aap->real_arrow_fill_color = color;
		}
		if (! vcp->fill_arrows_on) {
			if (vcp->mono_l_arrow_color ||
			    (vcp->use_scalar_array && vcp->scalar_data_init)) {
				ilp->aap->real_arrow_line_color = 
					vcp->l_arrow_color;
			}
			else {
				ilp->aap->real_arrow_line_color = color;
			}
		}
		else {
			if (vcp->mono_f_arrow_edge_color ||
			    (vcp->use_scalar_array && vcp->scalar_data_init)) {
				ilp->aap->real_arrow_line_color = 
					vcp->f_arrow_edge_color;
			}
			else {
				ilp->aap->real_arrow_line_color = color;
			}
		}
	}

	vcp->a_params.ast_iast = (float) vcp->fill_arrows_on;
	vcp->a_params.fw2w = vcnew->view.width;
	vcp->a_params.uvmg = ilp->aap->real_vec_mag;
	vcp->a_params.vlc_vlom = MAX(0.0,vcp->min_magnitude);
	vcp->a_params.vhc_vhim = MAX(0.0,vcp->max_magnitude);
	vcp->a_params.vfr_vfrc = MIN(1.0,MAX(0.0,vcp->min_frac_len));
	vcp->a_params.vrl_vrln = 
		MAX(0.0,vcp->real_ref_length / vcnew->view.width);
	vcp->a_params.vrm_vrmg = MAX(0.0,vcp->ref_magnitude);
	vcp->a_params.amn_famn = MAX(0.0,vcp->l_arrowhead_min_size);
	vcp->a_params.amx_famx = MAX(0.0,vcp->l_arrowhead_max_size);
	vcp->a_params.air_fair = vcp->f_arrowhead_interior;
	vcp->a_params.awr_fawr = vcp->f_arrow_width;
	vcp->a_params.awf_fawf = vcp->f_arrow_min_width;
	vcp->a_params.axr_faxr = vcp->f_arrowhead_x;
	vcp->a_params.axr_faxf = vcp->f_arrowhead_min_x;
	vcp->a_params.ayr_fayr = vcp->f_arrowhead_y;
	vcp->a_params.ayf_fayf = vcp->f_arrowhead_min_y;
	vcp->a_params.afo_iafo = (float)vcp->fill_over_edge;

	if (init || anrp->id == NhlNULLOBJID) {
		NhlSetSArg(&targs[(targc)++],NhlNvaString1On,
			   ilp->string1_on);
		NhlSetSArg(&targs[(targc)++],NhlNvaString1,
			   ilp->text1);
		NhlSetSArg(&targs[(targc)++],NhlNvaString2On,
			   ilp->string2_on);
		NhlSetSArg(&targs[(targc)++],NhlNvaString2,
			   ilp->text2);
		NhlSetSArg(&targs[(targc)++],NhlNvaVectorLenF,
			   ilp->aap->vec_len);
		NhlSetSArg(&targs[(targc)++],NhlNvaVectorLineColor,
			   ilp->aap->real_arrow_line_color);
		NhlSetSArg(&targs[(targc)++],NhlNvaVectorFillColor,
			   ilp->aap->real_arrow_fill_color);
		NhlSetSArg(&targs[(targc)++],NhlNvaArrowLineThicknessF,
			   ilp->aap->real_arrow_line_thickness);
		NhlSetSArg(&targs[(targc)++],NhlNvaArrowAngleF,
			   ilp->aap->arrow_angle);
		NhlSetSArg(&targs[(targc)++],NhlNvaArrowSpaceF,
			   ilp->aap->arrow_space);
		NhlSetSArg(&targs[(targc)++],NhlNvaArrowMinOffsetF,
			   ilp->aap->arrow_min_offset);
		NhlSetSArg(&targs[(targc)++],NhlNvaArrowParams,
			   &vcp->a_params);
		NhlSetSArg(&targs[(targc)++],NhlNtxFontHeightF,ilp->height);
		NhlSetSArg(&targs[(targc)++],NhlNtxDirection,ilp->direction);
		NhlSetSArg(&targs[(targc)++],NhlNtxAngleF,ilp->angle);
		NhlSetSArg(&targs[(targc)++],NhlNtxFont,ilp->font);
		NhlSetSArg(&targs[(targc)++],NhlNtxFontColor,ilp->color);
		NhlSetSArg(&targs[(targc)++],NhlNtxFontAspectF,ilp->aspect);
		NhlSetSArg(&targs[(targc)++],
			   NhlNtxFontThicknessF,ilp->thickness);
		NhlSetSArg(&targs[(targc)++],
			   NhlNtxConstantSpacingF,ilp->cspacing);
		NhlSetSArg(&targs[(targc)++],NhlNtxFontQuality,ilp->quality);
		NhlSetSArg(&targs[(targc)++],NhlNtxFuncCode,ilp->fcode[0]);

		NhlSetSArg(&targs[(targc)++],NhlNvaPerimOn,ilp->perim_on);
		NhlSetSArg(&targs[(targc)++],
			   NhlNvaPerimColor,ilp->perim_lcolor);
		NhlSetSArg(&targs[(targc)++],
			   NhlNvaPerimThicknessF,ilp->perim_lthick);
		NhlSetSArg(&targs[(targc)++],
			   NhlNvaPerimSpaceF,ilp->perim_space);
		NhlSetSArg(&targs[(targc)++],
			   NhlNvaBackgroundFillColor,ilp->back_color);

		sprintf(buffer,"%s",vcnew->base.name);
		strcat(buffer,ilp->name);
		subret = NhlALCreate(&tmpid,buffer,NhlvecAnnoClass,
				     vcnew->base.id,targs,targc);
		
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			e_text = 
			     "%s: error creating vector reference annotation";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		anrp->id = tmpid;
		if (tfp->overlay_status == _tfNotInOverlay) {
			targc = 0;
			/* go on so that text position can be set */
		}
		else {
			on = ilp->on && ! vcp->display_zerof_no_data;
			subret = ManageAnnotation(vcnew,True,
						  anrp,oanrp,idp,on);
			return MIN(ret,subret);
		}
	}
	if (! init) {
		if (ilp->string1_on != oilp->string1_on)
			NhlSetSArg(&targs[(targc)++],NhlNvaString1On,
				   ilp->string1_on);
		if (ilp->text1 != oilp->text1)
			NhlSetSArg(&targs[(targc)++],NhlNvaString1,
				   ilp->text1);
		if (ilp->string2_on != oilp->string2_on)
			NhlSetSArg(&targs[(targc)++],NhlNvaString2On,
				   ilp->string2_on);
		if (ilp->text2 != oilp->text2)
			NhlSetSArg(&targs[(targc)++],NhlNvaString2,
				   ilp->text2);
		if (ilp->aap->vec_len != oilp->aap->vec_len)
			NhlSetSArg(&targs[(targc)++],NhlNvaVectorLenF,
				   ilp->aap->vec_len);
		if (ilp->aap->real_arrow_line_color != 
		    oilp->aap->real_arrow_line_color)
			NhlSetSArg(&targs[(targc)++],NhlNvaVectorLineColor,
				   ilp->aap->real_arrow_line_color);
		if (ilp->aap->real_arrow_fill_color != 
		    oilp->aap->real_arrow_fill_color)
			NhlSetSArg(&targs[(targc)++],NhlNvaVectorFillColor,
				   ilp->aap->real_arrow_fill_color);
		if (ilp->aap->real_arrow_line_thickness !=
		    oilp->aap->real_arrow_line_thickness)
			NhlSetSArg(&targs[(targc)++],NhlNvaArrowLineThicknessF,
				   ilp->aap->real_arrow_line_thickness);
		if (ilp->aap->arrow_angle != oilp->aap->arrow_angle)
			NhlSetSArg(&targs[(targc)++],NhlNvaArrowAngleF,
				   ilp->aap->arrow_angle);
		if (ilp->aap->arrow_space != oilp->aap->arrow_space)
			NhlSetSArg(&targs[(targc)++],NhlNvaArrowSpaceF,
				   ilp->aap->arrow_space);
		if (ilp->aap->arrow_min_offset != oilp->aap->arrow_min_offset)
			NhlSetSArg(&targs[(targc)++],NhlNvaArrowMinOffsetF,
				   ilp->aap->arrow_min_offset);
		if (memcmp(&vcp->a_params,
			   &ovcp->a_params,sizeof(_NhlvaArrowParams)))
			NhlSetSArg(&targs[(targc)++],NhlNvaArrowParams,
				   &vcp->a_params);
		if (ilp->height != oilp->height)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontHeightF,ilp->height);
		if (ilp->direction != oilp->direction)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxDirection,ilp->direction);
		if (ilp->angle != oilp->angle)
			NhlSetSArg(&targs[(targc)++],NhlNtxAngleF,ilp->angle);
		if (ilp->font != oilp->font)
			NhlSetSArg(&targs[(targc)++],NhlNtxFont,ilp->font);
		if (ilp->color != oilp->color)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontColor,ilp->color);
		if (ilp->aspect != oilp->aspect)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontAspectF,ilp->aspect);
		if (ilp->thickness != oilp->thickness)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontThicknessF,ilp->thickness);
		if (ilp->cspacing != oilp->cspacing)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxConstantSpacingF,ilp->cspacing);
		if (ilp->quality != oilp->quality)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontQuality,ilp->quality);
		if (init || ilp->fcode[0] != oilp->fcode[0])
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFuncCode,ilp->fcode[0]);
		
		if (init || ilp->perim_on != oilp->perim_on)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxPerimOn,ilp->perim_on);
		if (init || ilp->perim_lcolor != oilp->perim_lcolor)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxPerimColor,ilp->perim_lcolor);
		if (init || ilp->perim_lthick != oilp->perim_lthick)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxPerimThicknessF,ilp->perim_lthick);
		if (init || ilp->perim_space != oilp->perim_space)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxPerimSpaceF,ilp->perim_space);
		if (init || ilp->back_color != oilp->back_color)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxBackgroundFillColor,ilp->back_color);
	}
	subret = NhlALSetValues(anrp->id,targs,targc);

	if ((ret = MIN(subret,ret)) < NhlWARNING) {
		e_text = "%s: error setting values for information label";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}

	if (tfp->overlay_status != _tfNotInOverlay) {
		on = ilp->on && ! vcp->display_zerof_no_data;
		subret = ManageAnnotation(vcnew,True,anrp,oanrp,idp,on);
		ret = MIN(ret,subret);
	}
	return ret;
}


/*
 * Function:	ManageZeroFLabel
 *
 * Description: If a constant field is detected a constant field label
 *		is created, or turned on.
 *		If there is an PlotManager an AnnoManager object is 
 *		created so that the overlay object can manage the
 *		annotations.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes ManageZeroFLabel
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	NhlSArg		*sargs,
	int		*nargs
)
#else 
(vcnew,vcold, init, sargs, nargs)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	NhlSArg		*sargs;
	int		*nargs;
#endif
{
	char			*e_text;
	char			*entry_name;
	NhlErrorTypes		ret = NhlNOERROR,subret = NhlNOERROR;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcnew->trans);
	NhlvcLabelAttrs		*cflp = &vcp->zerof_lbl;
	NhlvcLabelAttrs		*ocflp = &ovcp->zerof_lbl;
	NhlString		lstring, tstring;
	NhlBoolean		text_changed = False, pos_changed = False;
	NhlSArg			targs[24];
	int			targc = 0;
	char			buffer[_NhlMAXRESNAMLEN];
	int			tmpid;

	entry_name = (init) ? InitName : SetValuesName;

/*
 * The constant field label resource must be turned on AND a constant
 * field condition must exist for the constant field annotation  
 * to be displayed.
 */


	if (init || vcp->zerof_lbl.string1 != ovcp->zerof_lbl.string1) {
		text_changed = True;
		tstring = vcp->zerof_lbl.string1 == NULL ?
			NhlvcDEF_ZEROF_LABEL : vcp->zerof_lbl.string1; 
		if ((lstring = NhlMalloc(strlen(tstring) + 1)) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		strcpy(lstring,tstring);
		vcp->zerof_lbl.string1 = lstring;
	}
	if (init || vcp->zerof_lbl.string2 != ovcp->zerof_lbl.string2) {
		text_changed = True;
		tstring = vcp->zerof_lbl.string2 == NULL ?
			NhlvcDEF_NODATA_LABEL : vcp->zerof_lbl.string2; 
		if ((lstring = NhlMalloc(strlen(tstring) + 1)) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		strcpy(lstring,tstring);
		vcp->zerof_lbl.string2 = lstring;
	}
	if (! vcp->data_init)
		vcp->zerof_no_data_string = vcp->zerof_lbl.string2;
	else
		vcp->zerof_no_data_string = vcp->zerof_lbl.string1;
	if (text_changed)
		ovcp->zerof_no_data_string = NULL;

	vcp->display_zerof_no_data = 
		(cflp->on && vcp->zero_field) || 
			(vcp->zerof_lbl.string2_on && ! vcp->data_init);

	subret = ReplaceSubstitutionChars(vcp,ovcp,init,_vcZEROF,
				  &text_changed,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;

	if (init || vcp->zerof_lbl_rec.id == NhlNULLOBJID) {
		if (pos_changed) {
			NhlSetSArg(&targs[(targc)++],NhlNtxPosXF,cflp->x_pos);
			NhlSetSArg(&targs[(targc)++],NhlNtxPosYF,cflp->y_pos);
			NhlSetSArg(&targs[(targc)++],NhlNtxJust,cflp->just);
		}
		NhlSetSArg(&targs[(targc)++],NhlNtxString,
			   (NhlString)cflp->text1);
		NhlSetSArg(&targs[(targc)++],NhlNtxFontHeightF,cflp->height);
		NhlSetSArg(&targs[(targc)++],NhlNtxDirection,cflp->direction);
		NhlSetSArg(&targs[(targc)++],NhlNtxAngleF,cflp->angle);
		NhlSetSArg(&targs[(targc)++],NhlNtxFont,cflp->font);
		NhlSetSArg(&targs[(targc)++],NhlNtxFontColor,cflp->color);
		NhlSetSArg(&targs[(targc)++],NhlNtxFontAspectF,cflp->aspect);
		NhlSetSArg(&targs[(targc)++],
			   NhlNtxFontThicknessF,cflp->thickness);
		NhlSetSArg(&targs[(targc)++],
			   NhlNtxConstantSpacingF,cflp->cspacing);
		NhlSetSArg(&targs[(targc)++],NhlNtxFontQuality,cflp->quality);
		NhlSetSArg(&targs[(targc)++],NhlNtxFuncCode,cflp->fcode[0]);

		NhlSetSArg(&targs[(targc)++],NhlNtxPerimOn,cflp->perim_on);
		NhlSetSArg(&targs[(targc)++],
			   NhlNtxPerimColor,cflp->perim_lcolor);
		NhlSetSArg(&targs[(targc)++],
			   NhlNtxPerimThicknessF,cflp->perim_lthick);
		NhlSetSArg(&targs[(targc)++],
			   NhlNtxPerimSpaceF,cflp->perim_space);
		NhlSetSArg(&targs[(targc)++],
			   NhlNtxBackgroundFillColor,cflp->back_color);

		sprintf(buffer,"%s",vcnew->base.name);
		strcat(buffer,cflp->name);
		subret = NhlALCreate(&tmpid,buffer,NhltextItemClass,
				     vcnew->base.id,targs,targc);
		
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			e_text = "%s: error creating information label";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		vcp->zerof_lbl_rec.id = tmpid;

		if (tfp->overlay_status == _tfNotInOverlay) {
			targc = 0; 
			/* go on so text position can be set */
		}
		else {
			subret = ManageAnnotation(vcnew,init,
						  &vcp->zerof_lbl_rec,
						  &ovcp->zerof_lbl_rec,
						  &vcp->zerof_anno_id,
						  vcp->display_zerof_no_data);
			return MIN(ret,subret);
		}
	}

	if (tfp->overlay_status == _tfNotInOverlay) {
		NhlVectorPlotLayerPart *op;
		op = init ? NULL : ovcp;
		subret = SetTextPosition(vcnew,op,_vcZEROF,
					 &pos_changed,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING) return ret;
	}
	if (pos_changed) {
		NhlSetSArg(&targs[(targc)++],NhlNtxPosXF,cflp->x_pos);
		NhlSetSArg(&targs[(targc)++],NhlNtxPosYF,cflp->y_pos);
		NhlSetSArg(&targs[(targc)++],NhlNtxJust,cflp->just);
	}
	if (! init) {
		if (text_changed)
			NhlSetSArg(&targs[(targc)++],NhlNtxString,
				   (NhlString)cflp->text1);
		if (cflp->height != ocflp->height)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontHeightF,cflp->height);
		if (cflp->direction != ocflp->direction)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxDirection,cflp->direction);
		if (cflp->angle != ocflp->angle)
			NhlSetSArg(&targs[(targc)++],NhlNtxAngleF,cflp->angle);
		if (cflp->font != ocflp->font)
			NhlSetSArg(&targs[(targc)++],NhlNtxFont,cflp->font);
		if (cflp->color != ocflp->color)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontColor,cflp->color);
		if (cflp->aspect != ocflp->aspect)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontAspectF,cflp->aspect);
		if (cflp->thickness != ocflp->thickness)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontThicknessF,cflp->thickness);
		if (cflp->cspacing != ocflp->cspacing)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxConstantSpacingF,cflp->cspacing);
		if (cflp->quality != ocflp->quality)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFontQuality,cflp->quality);
		if (cflp->fcode[0] != ocflp->fcode[0])
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxFuncCode,cflp->fcode[0]);
		
		if (cflp->perim_on != ocflp->perim_on)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxPerimOn,cflp->perim_on);
		if (cflp->perim_lcolor != ocflp->perim_lcolor)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxPerimColor,cflp->perim_lcolor);
		if (cflp->perim_lthick != ocflp->perim_lthick)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxPerimThicknessF,cflp->perim_lthick);
		if (cflp->perim_space != ocflp->perim_space)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxPerimSpaceF,cflp->perim_space);
		if (cflp->back_color != ocflp->back_color)
			NhlSetSArg(&targs[(targc)++],
				   NhlNtxBackgroundFillColor,cflp->back_color);
	}
	
	subret = NhlALSetValues(vcp->zerof_lbl_rec.id,targs,targc);

	if ((ret = MIN(subret,ret)) < NhlWARNING) {
		e_text = "%s: error setting values forinformation label";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}

	if (tfp->overlay_status != _tfNotInOverlay) {
		subret = ManageAnnotation(vcnew,init,
					  &vcp->zerof_lbl_rec,
					  &ovcp->zerof_lbl_rec,
					  &vcp->zerof_anno_id,
					  vcp->display_zerof_no_data);
		ret = MIN(ret,subret);
	}
	return ret;
}

/*
 * Function:	ManageAnnotation
 *
 * Description: 
 *		
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes ManageAnnotation
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew,
	NhlBoolean		init,
	NhlAnnotationRec	*rec,
	NhlAnnotationRec	*orec,
	int			*idp,
	NhlBoolean		on
)
#else 
(vcnew,init,rec,orec,idp,on)
	NhlVectorPlotLayer	vcnew;
	NhlBoolean		init;
	NhlAnnotationRec	*rec;
	NhlAnnotationRec	*orec;
	int			*idp;
	NhlBoolean		on;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	char			*entry_name;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlTransformLayerPart	*tfp = &(vcnew->trans);
	NhlSArg			sargs[16];
	int			nargs = 0;
	char			buffer[_NhlMAXRESNAMLEN];
	int			tmpid;

	entry_name = (init) ? InitName : SetValuesName;

	rec->on = on;

	if (*idp <= NhlNULLOBJID) {
		NhlSetSArg(&sargs[(nargs)++],NhlNamOn,rec->on);
		NhlSetSArg(&sargs[(nargs)++],NhlNamViewId,rec->id);
		NhlSetSArg(&sargs[(nargs)++],NhlNamZone,rec->zone);
		NhlSetSArg(&sargs[(nargs)++],NhlNamSide,rec->side);
		NhlSetSArg(&sargs[(nargs)++],NhlNamJust,rec->just);
		NhlSetSArg(&sargs[(nargs)++],
			   NhlNamParallelPosF,rec->para_pos);
		NhlSetSArg(&sargs[(nargs)++],
			   NhlNamOrthogonalPosF,rec->ortho_pos);
		sprintf(buffer,"%s",vcnew->base.name);
		strcat(buffer,".AnnoManager");
		subret = NhlALCreate(&tmpid,buffer,NhlannoManagerClass,
				     vcnew->base.id,sargs,nargs);
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			e_text = "%s: error creating AnnoManager layer";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		*idp = tmpid;
/*
 * If the VectorPlot plot is an overlay plot base register the AnnoManager
 * with its own base, ensuring that it will always follow the overlay.
 */
		if (tfp->plot_manager_on)
			subret = _NhlRegisterAnnotation(vcp->overlay_object,
							_NhlGetLayer(*idp),
							NULL);
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			e_text = "%s: error registering annotation";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		return ret;
	}
	if (rec->on != orec->on) 
		NhlSetSArg(&sargs[(nargs)++],NhlNamOn,rec->on);
	if (rec->zone != orec->zone) 
		NhlSetSArg(&sargs[(nargs)++],NhlNamZone,rec->zone);
	if (rec->side != orec->side) 
		NhlSetSArg(&sargs[(nargs)++],NhlNamSide,rec->side);
	if (rec->just != orec->just) 
		NhlSetSArg(&sargs[(nargs)++],NhlNamJust,rec->just);
	if (rec->para_pos != orec->para_pos) 
		NhlSetSArg(&sargs[(nargs)++],
			   NhlNamParallelPosF,rec->para_pos);
	if (rec->ortho_pos != orec->ortho_pos) 
		NhlSetSArg(&sargs[(nargs)++],
			   NhlNamOrthogonalPosF,rec->ortho_pos);
	
	subret = NhlALSetValues(*idp,sargs,nargs);

	if ((ret = MIN(subret,ret)) < NhlWARNING) {
		e_text = "%s: error setting AnnoManager object values";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}
	return ret;
}

/*
 * Function:	ReplaceSubstitutionChars
 *
 * Description: Replaces the substitution characters in the info and
 *		zero field labels with the correct numerical values.
 *
 * In Args:	vcnew	new instance record
 *		vcold	old instance record if not initializing
 *		init	true if initialization
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static NhlErrorTypes ReplaceSubstitutionChars
#if	NhlNeedProto
(
	NhlVectorPlotLayerPart	*vcp,
	NhlVectorPlotLayerPart	*ovcp,
	NhlBoolean		init,
	_vcAnnoType		atype,
	NhlBoolean		*text_changed,
	NhlString		entry_name
)
#else 
(vcp,ovcp,init,atype,text_changed,entry_name)
	NhlVectorPlotLayerPart	*vcp;
	NhlVectorPlotLayerPart	*ovcp;
	NhlBoolean		init;
	_vcAnnoType		atype;
	NhlBoolean		*text_changed;
	NhlString		entry_name;
#endif
{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*e_text;
	char			buffer[256];

	*text_changed = False;

	if (! init && (vcp->zmax == ovcp->zmax) &&
	    (vcp->zerof_no_data_string == 
	     ovcp->zerof_no_data_string))
		return NhlNOERROR;
	strcpy(buffer,vcp->zerof_no_data_string);
	if (vcp->zerof_lbl.text1 != NULL)
		NhlFree(vcp->zerof_lbl.text1);
	if ((vcp->zerof_lbl.text1 = 
	     NhlMalloc(strlen(buffer)+1)) == NULL) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}
	strcpy((NhlString)vcp->zerof_lbl.text1,buffer);

	*text_changed = True;

	return ret;
}


/*
 * Function:	Substitute
 *
 * Description: substitutes a string for a Conpack substitution sequence.
 *
 * In Args:	
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:	Objects created and destroyed.
 */
static void Substitute
#if	NhlNeedProto
(
	char		*buf,
	int		replace_count,
	char		*subst
)
#else 
(buf,replace_count,subst)
	char		*buf;
	int		replace_count;
	char		*subst;
#endif
{
	int subst_count,add,buflen;
	char *from, *to;

	buflen = strlen(buf);
	subst_count = strlen(subst);
	if (subst_count - replace_count < 0) {
		for (from = buf+replace_count,to = buf+subst_count; ;
		     to++,from++) { 
			*to = *from;
			if (*from == '\0')
				break;
		}
	}
	else if ((add = subst_count - replace_count) > 0) {
		for (from = buf + buflen,to = buf + buflen + add; 
		     from >= buf + replace_count;)
			*to-- = *from--;
	}
	strncpy(buf,subst,subst_count);
}

/*
 * Function:	SetFormatRec
 *
 * Description: sets up the format record for a label type
 *
 * In Args:	NhlFormatRec *format -> to the format record
 *		NhlString    resource    the format resource - for error.
 *		NhlString    entry_name  the caller
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects: 
 */
static NhlErrorTypes SetFormatRec
#if	NhlNeedProto
(
	NhlFormatRec	*format,
	NhlString	resource,
	NhlString	entry_name
)
#else 
(format,resource,entry_name)
	NhlFormatRec	*format;
	NhlString	resource;
	NhlString	entry_name;
#endif
{
	NhlErrorTypes	ret = NhlNOERROR;
	char		*e_text;
	NhlFormatRec	*frec;


	if (format->fstring == NULL || format->fstring[0] == '\0') {
		e_text = "%s: empty format string supplied for %s; defaulting";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,resource);
		ret = NhlWARNING;
		format->fstring = NhlvcDEF_FORMAT;
	}
	if ((frec = _NhlScanFString(format->fstring,entry_name)) == NULL) {
		e_text = "%s: error in format string for %s: defaulting";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,resource);
		ret = NhlWARNING;
		format->fstring = NhlvcDEF_FORMAT;
		if ((frec = _NhlScanFString(format->fstring,
					    entry_name)) == NULL) {
			e_text = "%s: internal error getting format";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return(NhlFATAL);
		}
	}
	memcpy((void *)format,(Const void *)frec,sizeof(NhlFormatRec));

/* 
 * Since at this point the format string itself is not owned by the 
 * VectorPlot object, make a copy.
 */
	if (format->fstring != NULL) {
		char *cp;
		if ((cp = NhlMalloc(strlen(format->fstring)+1)) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		strcpy(cp,format->fstring);
		format->fstring = cp;
	}

	return ret;
}

/*
 * Function:	ConstrainJustification
 *
 * Description: Constrain justification depending on the annotation side;
 *
 * In Args:	NhlAnnoRec	anno_rec - the annotation record
 *
 * Out Args:
 *
 * Return Values:	ErrorConditions
 *
 * Side Effects:	NONE
 */
static NhlJustification
ConstrainJustification
#if	NhlNeedProto
(
	NhlAnnotationRec	*anno_rec
)
#else
(anno_rec)
	NhlAnnotationRec	*anno_rec;
#endif
{
	switch (anno_rec->side) {
	case NhlBOTTOM:
		switch (anno_rec->just) {
		case NhlTOPLEFT:
		case NhlCENTERLEFT:
		case NhlBOTTOMLEFT:
			return NhlTOPLEFT;
		case NhlTOPCENTER:
		case NhlCENTERCENTER:
		case NhlBOTTOMCENTER:
			return NhlTOPCENTER;
		case NhlTOPRIGHT:
		case NhlCENTERRIGHT:
		case NhlBOTTOMRIGHT:
			return NhlTOPRIGHT;
		}
	case NhlTOP:
		switch (anno_rec->just) {
		case NhlTOPLEFT:
		case NhlCENTERLEFT:
		case NhlBOTTOMLEFT:
			return NhlBOTTOMLEFT;
		case NhlTOPCENTER:
		case NhlCENTERCENTER:
		case NhlBOTTOMCENTER:
			return NhlBOTTOMCENTER;
		case NhlTOPRIGHT:
		case NhlCENTERRIGHT:
		case NhlBOTTOMRIGHT:
			return NhlBOTTOMRIGHT;
		}
	case NhlLEFT:
		switch (anno_rec->just) {
		case NhlTOPLEFT:
		case NhlTOPCENTER:
		case NhlTOPRIGHT:
			return NhlTOPRIGHT;
		case NhlCENTERLEFT:
		case NhlCENTERCENTER:
		case NhlCENTERRIGHT:
			return NhlCENTERRIGHT;
		case NhlBOTTOMLEFT:
		case NhlBOTTOMCENTER:
		case NhlBOTTOMRIGHT:
			return NhlBOTTOMRIGHT;
		}
	case NhlRIGHT:
		switch (anno_rec->just) {
		case NhlTOPLEFT:
		case NhlTOPCENTER:
		case NhlTOPRIGHT:
			return NhlTOPLEFT;
		case NhlCENTERLEFT:
		case NhlCENTERCENTER:
		case NhlCENTERRIGHT:
			return NhlCENTERLEFT;
		case NhlBOTTOMLEFT:
		case NhlBOTTOMCENTER:
		case NhlBOTTOMRIGHT:
			return NhlBOTTOMLEFT;
		}
	case NhlCENTER:
	default:
		break;
	}
	return (NhlJustification) NhlFATAL;

}

/*
 * Function:	SetTextPosition
 *
 * Description: Sets the text positional attribute fields in label 
 *		annotation strings when they are not members of an overlay, 
 *		and therefore have no AnnoManager.
 *
 * In Args:	
 *		
 *		
 *
 * Out Args:	NONE
 *
 * Return Values:	Error Conditions
 *
 * Side Effects:
 */
static NhlErrorTypes SetTextPosition
#if	NhlNeedProto
(
	NhlVectorPlotLayer		vcnew,
	NhlVectorPlotLayerPart	*ovcp,
	_vcAnnoType		atype,
	NhlBoolean		*pos_changed,
	NhlString		entry_name
)
#else 
(vcnew,ovcp,atype,pos_changed,entry_name)
	NhlVectorPlotLayer		vcnew;
	NhlVectorPlotLayerPart	*ovcp;
	_vcAnnoType		atype;
	NhlBoolean		*pos_changed;
	NhlString		entry_name;
#endif
{
	char			*e_text;
	NhlErrorTypes		ret = NhlNOERROR,subret = NhlNOERROR;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlAnnotationRec	*anno_rec;
	NhlvcLabelAttrs		*lap;
	NhlvcLabelAttrs		*olap;
	float			width_vp, height_vp;
	float			x_start, y_start;
	int			sign;

	if (atype == _vcREFVECANNO) {
		anno_rec = &vcp->refvec_anno_rec;
		lap = &vcp->refvec_anno;
		olap = ovcp == NULL ? NULL : &ovcp->refvec_anno;
	}
	else if (atype == _vcMINVECANNO) {
		anno_rec = &vcp->minvec_anno_rec;
		lap = &vcp->minvec_anno;
		olap = ovcp == NULL ? NULL : &ovcp->minvec_anno;
	}
	else {
		anno_rec = &vcp->zerof_lbl_rec;
		lap = &vcp->zerof_lbl;
		olap = ovcp == NULL ? NULL : &ovcp->zerof_lbl;
	}
	subret = NhlVAGetValues(anno_rec->id,
				NhlNvpWidthF,&width_vp,
				NhlNvpHeightF,&height_vp,
				NULL);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error getting embedded annotation values";
		NhlPError(ret,NhlEUNKNOWN,e_text,entry_name);
		return ret;
	}

	x_start = anno_rec->zone != 0 ? vcnew->view.x :
		vcnew->view.x + 0.5 * vcnew->view.width; 
	y_start = anno_rec->zone != 0 ? vcnew->view.y - vcnew->view.height :
		vcnew->view.y - 0.5 * vcnew->view.height;
	sign = anno_rec->zone == 1 ? 1.0 : - 1.0;


	switch (anno_rec->side) {
	case NhlBOTTOM:
		lap->x_pos = x_start + anno_rec->para_pos * vcnew->view.width;
		lap->y_pos = y_start - 
			sign * anno_rec->ortho_pos * vcnew->view.height;
		break;
	case NhlTOP:
		lap->x_pos = x_start + anno_rec->para_pos * vcnew->view.width;
		lap->y_pos = y_start + 
			sign * anno_rec->ortho_pos * vcnew->view.height;
		break;
	case NhlLEFT:
		lap->x_pos = x_start - 
			sign * anno_rec->ortho_pos * vcnew->view.width;
		lap->y_pos = y_start +
			anno_rec->para_pos * vcnew->view.height;
		break;
	case NhlRIGHT:
		lap->x_pos = x_start + vcnew->view.width + 
			sign * anno_rec->ortho_pos * vcnew->view.width;
		lap->y_pos = y_start +
			anno_rec->para_pos * vcnew->view.height;
		break;
	default:
		e_text = "%s: internal enumeration error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(ret);
	}
	if (anno_rec->just < NhlTOPLEFT || anno_rec->just > NhlBOTTOMRIGHT) {
		e_text = "%s: internal enumeration error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text, entry_name);
		return(NhlFATAL);
	}

	if (anno_rec->zone > 0)
		lap->just = ConstrainJustification(anno_rec);
	else 
		lap->just = anno_rec->just;

	if (olap == NULL ||
	    lap->x_pos != olap->x_pos ||
	    lap->y_pos != olap->y_pos ||
	    lap->just != olap->just) {
		*pos_changed = True;
	}
	return ret;
}

/*
 * Function:  ManageVectorData
 *
 * Description: Handles updating of the vector data
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    ManageVectorData
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew, 
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	_NhlArgList	args,
	int		num_args
)
#else
(vcnew,vcold,init,args,num_args)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	_NhlArgList	args;
	int		num_args;
#endif

{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*entry_name;
	char			*e_text;
	NhlVectorPlotLayerPart	*vcp = &vcnew->vectorplot;
	NhlVectorPlotLayerPart	*ovcp = &vcold->vectorplot;
	NhlVectorFieldFloatLayer	vfl;
	_NhlDataNodePtr			*dlist = NULL;
	NhlBoolean			new;
	int				ndata = 0;


	entry_name = (init) ? InitName : SetValuesName;

	if (! vcp->data_changed && 
	    ! _NhlArgIsSet(args,num_args,NhlNvcVectorFieldData))
		return NhlNOERROR;

	if (vcp->vector_field_data != NULL)
		ndata = _NhlGetDataInfo(vcp->vector_field_data,&dlist);
	if (ndata <= 0) {
		vcp->a_params.uvmn = vcp->mag_scale.min_val = vcp->zmin = 0.01;
		vcp->a_params.uvmx = vcp->mag_scale.max_val = vcp->zmax = 1.0;
		vcp->data_init = False;
		vcp->vfp = NULL;
		return NhlNOERROR;
	}
	else if (ndata != 1) {
		vcp->data_init = False;
		e_text = "%s: internal error retrieving data info";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}

	if (vcp->vfp != NULL && vcp->ovfp == NULL) {
		vcp->ovfp = NhlMalloc(sizeof(NhlVectorFieldFloatLayerPart));
		if (vcp->ovfp == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
	}
	if (vcp->vfp != NULL) {
		memcpy(vcp->ovfp,
		       vcp->vfp,sizeof(NhlVectorFieldFloatLayerPart));	
	}

 	vfl = (NhlVectorFieldFloatLayer) _NhlGetDataSet(dlist[0],&new);
	if (vfl == NULL) {
		vcp->data_init = False;
		e_text = "%s: internal error retrieving data set";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}
	
	vcp->vfp = (NhlVectorFieldFloatLayerPart *) &vfl->vfieldfloat;

	if (vcp->min_magnitude > 0.0)
		vcp->zmin = vcp->min_magnitude;
	else
		vcp->zmin = vcp->vfp->mag_min;

	if (vcp->max_magnitude > 0.0)
		vcp->zmax = vcp->max_magnitude;
	else
		vcp->zmax = vcp->vfp->mag_max;
	vcp->mag_scale.max_val = vcp->zmax;
	vcp->mag_scale.min_val = vcp->zmin;
	vcp->a_params.uvmn = vcp->vfp->mag_min;
	vcp->a_params.uvmx = vcp->vfp->mag_max;

	vcp->zero_field = _NhlCmpFAny(vcp->zmax,vcp->zmin,8) <= 0.0 ?
		True : False;
	if (vcp->zero_field) {
		e_text = 
		 "%s: zero vector field; no VectorPlot possible";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ret = MIN(NhlWARNING,ret);
	}

	vcp->data_init = True;
	vcp->data_changed = True;
	vcp->use_irr_trans = (vcp->vfp->x_arr == NULL &&
			      vcp->vfp->y_arr == NULL) ? False : True;

	return ret;
}


/*
 * Function:  ManageScalarData
 *
 * Description: Handles updating of the scalar data
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    ManageScalarData
#if	NhlNeedProto
(
	NhlVectorPlotLayer	vcnew, 
	NhlVectorPlotLayer	vcold,
	NhlBoolean	init,
	_NhlArgList	args,
	int		num_args
)
#else
(vcnew,vcold,init,args,num_args)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	_NhlArgList	args;
	int		num_args;
#endif

{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*entry_name;
	char			*e_text;
	NhlVectorPlotLayerPart	*vcp = &vcnew->vectorplot;
	NhlVectorPlotLayerPart	*ovcp = &vcold->vectorplot;
	NhlScalarFieldFloatLayer	sfl;
	_NhlDataNodePtr			*dlist = NULL;
	NhlBoolean			new;
	int				ndata = 0;


	entry_name = (init) ? InitName : SetValuesName;

	if (! vcp->data_changed && 
	    ! _NhlArgIsSet(args,num_args,NhlNvcScalarFieldData))
		return NhlNOERROR;

	if (vcp->scalar_field_data != NULL)
		ndata = _NhlGetDataInfo(vcp->scalar_field_data,&dlist);
	if (ndata != 1) {
		vcp->svalue_scale.min_val = vcp->scalar_min = 0.0;
		vcp->svalue_scale.max_val = vcp->scalar_max = 1.0;
		vcp->scalar_data_init = False;
		vcp->sfp = NULL;
		if (ndata > 1) {
			e_text = "%s: internal error retrieving data info";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		else  {
			return NhlNOERROR;
		}
	}

	if (vcp->sfp != NULL && vcp->osfp == NULL) {
		vcp->osfp = NhlMalloc(sizeof(NhlScalarFieldFloatLayerPart));
		if (vcp->osfp == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
	}
	if (vcp->sfp != NULL) {
		memcpy(vcp->osfp,
		       vcp->sfp,sizeof(NhlScalarFieldFloatLayerPart));	
	}

 	sfl = (NhlScalarFieldFloatLayer) _NhlGetDataSet(dlist[0],&new);
	if (sfl == NULL) {
		vcp->scalar_data_init = False;
		e_text = "%s: internal error retrieving data set";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}
	
	vcp->sfp = (NhlScalarFieldFloatLayerPart *) &sfl->sfieldfloat;

	if (vcp->data_init && 
	    (vcp->sfp->fast_len != vcp->vfp->fast_len ||
	     vcp->sfp->slow_len != vcp->vfp->slow_len)) {
		e_text = "%s: ignoring %s: size does not match %s";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name,
			  NhlNvcScalarFieldData,NhlNvcVectorFieldData);
		ret = NhlWARNING;
		vcp->scalar_data_init = False;
		vcp->sfp = NULL;
		vcp->svalue_scale.min_val = vcp->scalar_min = 0.0;
		vcp->svalue_scale.max_val = vcp->scalar_max = 1.0;
	}
	else {
		vcp->svalue_scale.min_val = 
			vcp->scalar_min = vcp->sfp->data_min;
		vcp->svalue_scale.max_val = 
			vcp->scalar_max = vcp->sfp->data_max;
		vcp->scalar_data_init = True;
		vcp->data_changed = True;
	}
	return ret;
}

/*
 * Function:  ManageViewDepResources
 *
 * Description: Modifies resources that may need to change when the view
 *	is modified.
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    ManageViewDepResources
#if	NhlNeedProto
	(NhlLayer	new, 
	NhlLayer	old,
	NhlBoolean	init,
	_NhlArgList	args,
	int		num_args)
#else
(new,old,init,args,num_args)
	NhlLayer	new;
	NhlLayer	old;
	NhlBoolean	init;
	_NhlArgList	args;
	int		num_args;
#endif

{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*entry_name;
	NhlVectorPlotLayer		vcnew = (NhlVectorPlotLayer) new;
	NhlVectorPlotLayerPart		*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayer		vcold = (NhlVectorPlotLayer) old;

	entry_name = (init) ? InitName : SetValuesName;

/* adjust the reference length if it is not set */

	if (! vcp->data_init) {
		vcp->a_params.dvmx = vcnew->view.width * 0.05;
	}
	else if (init || vcp->data_changed) {
		int nx,ny;
		float sx,sy;
		nx = vcp->vfp->fast_len;
		ny = vcp->vfp->slow_len;
		sx = vcnew->view.width / nx;
		sy = vcnew->view.height / ny;
		vcp->a_params.dvmx = sqrt((sx*sx + sy*sy) / 2.0);
	}

	if (vcp->ref_length > 0.0) {
		vcp->real_ref_length = vcp->ref_length;
	}
	else if (init || vcp->data_changed || ! vcp->data_init) {
		vcp->real_ref_length = vcp->a_params.dvmx;
		vcp->ref_length_set = True;
	}

	if (! vcp->ref_length_set) {
		if (vcnew->view.width != vcold->view.width ||
		    vcnew->view.height != vcold->view.height) {
			float ratio;
			ratio = sqrt((vcnew->view.width * vcnew->view.width
				   + vcnew->view.height * vcnew->view.height) /
				  (vcold->view.width * vcold->view.width
				   + vcold->view.height * vcold->view.height));
			vcp->real_ref_length *= ratio;
			if (vcp->ref_length > 0.0)
				vcp->ref_length = vcp->real_ref_length;
		}
	}

	vcp->lbls.aspect = 1.325;
	subret = AdjustText(&vcp->lbls,vcnew,vcold,init);
	if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	subret = AdjustText(&vcp->refvec_anno,vcnew,vcold,init);
	if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	subret = AdjustText(&vcp->minvec_anno,vcnew,vcold,init);
	if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	subret = AdjustText(&vcp->zerof_lbl,vcnew,vcold,init);
	if ((ret = MIN(subret,ret)) < NhlWARNING) return ret;

	return ret;
}


/*
 * Function:  CopyTextAttrs
 *
 * Description: Copies the text attributes from one label type to another.
 *              The format string is copied to into a separate memory
 *		area; the text string and the angle are not copied.
 *	       
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    CopyTextAttrs
#if	NhlNeedProto
(
	NhlvcLabelAttrs *dest,
	NhlvcLabelAttrs *source,
	NhlString	entry_name
)
#else
(dest,source,entry_name)
	NhlvcLabelAttrs *dest;
	NhlvcLabelAttrs *source;
	NhlString	entry_name;
#endif
{
	NhlString	save_name;
	NhlString	save_text1;
	NhlString	save_text2;
	NhlString	save_string1;
	NhlString	save_string2;
	NhlvcArrowAttrs	*save_attrs;
	float		save_angle;
	NhlBoolean	string1_on;
	NhlBoolean	string2_on;
	NhlBoolean	on;
	
	save_name = dest->name;
	save_text1 = dest->text1;
	save_text2 = dest->text2;
	save_string1 = dest->string1;
	save_string2 = dest->string2;
	save_attrs = dest->aap;
	save_angle = dest->angle;
	string1_on = dest->string1_on;
	string2_on = dest->string2_on;
	on = dest->on;

	memcpy((void *)dest,(Const void *)source,sizeof(NhlvcLabelAttrs));

	dest->name = save_name;
	dest->text1 = save_text1;
	dest->text2 = save_text2;
	dest->string1 = save_string1;
	dest->string2 = save_string2;
	dest->angle = save_angle;
	dest->aap = save_attrs;
	dest->string1_on = string1_on;
	dest->string2_on = string2_on;
	dest->on = on;

	return NhlNOERROR;
}

/*
 * Function:  AdjustText
 *
 * Description: Adjusts the text height and aspect ratio
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    AdjustText
#if	NhlNeedProto
(
	NhlvcLabelAttrs *lbl_attrp,
	NhlVectorPlotLayer	new, 
	NhlVectorPlotLayer	old,
	NhlBoolean	init
)
#else
(lbl_attrp,new,old,init)
	NhlvcLabelAttrs *lbl_attrp;
	NhlLayer	new;
	NhlLayer	old;
	NhlBoolean	init;
#endif

{
	NhlErrorTypes		ret = NhlNOERROR;
	char 			*e_text;
	char			*entry_name;
	NhlVectorPlotLayer		vcnew = (NhlVectorPlotLayer) new;
	NhlVectorPlotLayer		vcold = (NhlVectorPlotLayer) old;

	entry_name = (init) ? InitName : SetValuesName;

/* 
 * Adjust text height. Then determine principal width and height
 * and the "real " text height based on aspect ratio. 21.0 is the default 
 * principle height. This code handles aspect ratio like the TextItem.
 */

	if (! lbl_attrp->height_set) {
		if (init) {
			lbl_attrp->height *= 
				vcnew->view.width / Nhl_vcSTD_VIEW_WIDTH;
		}
		else if (vcnew->view.width != vcold->view.width) {
			lbl_attrp->height *= 
				vcnew->view.width / vcold->view.width;
		}
	}

        if (lbl_attrp->aspect <= 0.0 ) {
		e_text = "%s: Invalid value for text aspect ratio %d";
                NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,
			  entry_name,lbl_attrp->aspect);
                ret = NhlWARNING;
                lbl_attrp->aspect = 1.3125;
        }
        if (lbl_attrp->aspect <= 1.0) {
                lbl_attrp->pheight = 21.0 * lbl_attrp->aspect;
                lbl_attrp->pwidth = 21.0;
        } else {
                lbl_attrp->pwidth = 21.0 * 1.0/lbl_attrp->aspect;
                lbl_attrp->pheight = 21.0;
        }
	/*
	 * The 1.125 factor compensates for the PLOTCHAR 'SA' parameter
	 */
        lbl_attrp->real_height = 
		1.0 / lbl_attrp->aspect * lbl_attrp->height * 1.125;

	return ret;
}
/*
 * Function:  ManageDynamicArrays
 *
 * Description: Creates and manages internal copies of each of the 
 *	VectorPlot GenArrays. Populates the copies with the values specified 
 *	via VectorPlotCreate or VectorPlotSetValues calls. Assigns default 
 *	values and sizes to any array resource not set by the caller.
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    ManageDynamicArrays
#if	NhlNeedProto
	(NhlLayer		new, 
	NhlLayer		old,
	NhlBoolean	init,
	_NhlArgList	args,
	int		num_args)
#else
(new,old,init,args,num_args)
	NhlLayer		new;
	NhlLayer		old;
	NhlBoolean	init;
	_NhlArgList	args;
	int		num_args;
#endif

{
	NhlVectorPlotLayer	vcnew = (NhlVectorPlotLayer) new;
	NhlVectorPlotLayerPart *vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayer	vcold = (NhlVectorPlotLayer) old;
	NhlVectorPlotLayerPart *ovcp = &(vcold->vectorplot);
	NhlErrorTypes ret = NhlNOERROR, subret = NhlNOERROR;
	int i,count;
	NhlGenArray ga;
	char *entry_name;
	char *e_text;
	int	init_count;
	NhlBoolean need_check,changed;
	int old_count;
	float *levels = NULL;
	NhlBoolean levels_modified = False;
	NhlvcScaleInfo 		*sip,*osip;
	NhlBoolean		scalar_labels, mag_labels;

	entry_name =  init ? InitName : SetValuesName;

/* 
 * If constant field don't bother setting up the arrays: they will not
 * be used -- but the label scaling still needs to be set up for the
 * benefit of the constant field label
 */

	if (vcp->zero_field) {
		subret = SetScale(vcnew,vcold,
				  &vcp->mag_scale,&ovcp->mag_scale,
				  False,init);
		if ((ret = MIN(ret,subret)) < NhlWARNING) {
			e_text = "%s: error setting up label scaling";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return(ret);
		}
		return NhlNOERROR;
	}

/* Determine the vector level state */

	subret = SetupLevels(new,old,init,&levels,&levels_modified);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error getting vector level information";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
	count = vcp->level_count;

/*=======================================================================*/

/* 
 * The levels array 
 */
	ga = init ? NULL : ovcp->levels;
	subret = ManageGenArray(&ga,count,vcp->levels,Qfloat,NULL,
				&old_count,&init_count,&need_check,&changed,
				NhlNvcLevels,entry_name);

	if ((ret = MIN(ret,subret)) < NhlWARNING)
		return ret;

	ovcp->levels = changed || levels_modified ? NULL : vcp->levels;
	vcp->levels = ga;
	if (levels_modified) {
		if (levels == NULL) {
			e_text = "%s: internal error getting levels";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		NhlFree(vcp->levels->data);
		vcp->levels->data = (NhlPointer) levels;
#if 0
		printf("no of levels: %d\n", vcp->level_count);
		for (i= 0; i < vcp->level_count; i++)
			printf("level %d: %f\n", i, levels[i]);
#endif
	}


/* Set up label scaling - the levels must have been set */

	if (vcp->use_scalar_array && vcp->scalar_data_init) {
		sip = &vcp->svalue_scale;
		osip = &ovcp->svalue_scale;
		scalar_labels = True;
		mag_labels = False;
	}
	else {
		sip = &vcp->mag_scale;
		osip = &ovcp->mag_scale;
		scalar_labels = False;
		mag_labels = True;
	}

	subret = SetScale(vcnew,vcold,&vcp->mag_scale,
			  &ovcp->mag_scale,mag_labels,init);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting up label scaling";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
	subret = SetScale(vcnew,vcold,&vcp->svalue_scale,
			  &ovcp->svalue_scale,scalar_labels,init);
	if ((ret = MIN(ret,subret)) < NhlWARNING) {
		e_text = "%s: error setting up label scaling";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

/*=======================================================================*/
	
/*
 * Level colors
 */
	ga = init ? NULL : ovcp->level_colors;
	count = vcp->level_count + 1;
	subret = ManageGenArray(&ga,count,vcp->level_colors,Qcolorindex,NULL,
				&old_count,&init_count,&need_check,&changed,
				NhlNvcLevelColors,entry_name);
	if ((ret = MIN(ret,subret)) < NhlWARNING)
		return ret;
	ovcp->level_colors = changed ? NULL : vcp->level_colors;
	vcp->level_colors = ga;

		
	if (need_check) {
		subret = CheckColorArray(vcnew,ga,count,init_count,old_count,
					 &vcp->gks_level_colors,
					 NhlNvcLevelColors,entry_name);
		if ((ret = MIN(ret,subret)) < NhlWARNING)
			return ret;
	}
/*=======================================================================*/
	
/*
 * Level String Values
 */
	count = vcp->level_count;
	if (init) vcp->level_strings = NULL;

	if (init || levels_modified || 
	    sip->format.fstring != osip->format.fstring ||
	    sip->mode != osip->mode ||
	    sip->scale_value != osip->scale_value ||
	    sip->scale_factor != osip->scale_factor) {
		NhlBoolean modified = False;
		NhlString cp;
		float *fp = (float *) vcp->levels->data;
		NhlString *sp = vcp->level_strings;

		if (sp != NULL) {
			int i;
			for (i = 0; i < ovcp->level_count; i++) {
				if (sp[i] != NULL)
					NhlFree(sp[i]);
			}
			NhlFree(sp);
		}
		if ((sp = (NhlString *) 
		     NhlMalloc(count * sizeof(NhlString))) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
		for (i=0; i<count; i++) {
			float fval = fp[i] / sip->scale_factor;
			NhlFormatRec *frec = &sip->format;

			cp = _NhlFormatFloat(frec,fval,NULL,
					     &sip->sig_digits,
					     &sip->left_sig_digit,
                                             NULL,NULL,NULL,
					     vcp->lbar_func_code,
					     entry_name);
			if (cp == NULL) return NhlFATAL;
			if ((sp[i] = (char *) 
			     NhlMalloc(strlen(cp)+1)) == NULL) {
				e_text = "%s: dynamic memory allocation error";
				NhlPError(NhlFATAL,NhlEUNKNOWN,
					  e_text,entry_name);
				return NhlFATAL;
			}
			strcpy(sp[i],cp);
			modified = True;
		}
		vcp->level_strings = sp;
		ovcp->level_strings = NULL;
	}
				

/*=======================================================================*/

/*
 * Test for changes that require a new draw (when in segment mode)
 */
	if (
	    vcp->level_colors != ovcp->level_colors ||
	    vcp->mono_f_arrow_fill_color != ovcp->mono_f_arrow_fill_color ||
	    vcp->f_arrow_fill_color != ovcp->f_arrow_fill_color ||
	    vcp->mono_f_arrow_edge_color != ovcp->mono_f_arrow_edge_color ||
	    vcp->f_arrow_edge_color != ovcp->f_arrow_edge_color ||
	    vcp->mono_l_arrow_color != ovcp->mono_l_arrow_color ||
	    vcp->l_arrow_color != ovcp->l_arrow_color ||
	    vcp->l_arrow_thickness != ovcp->l_arrow_thickness ||
	    vcp->f_arrow_edge_thickness != ovcp->f_arrow_edge_thickness ||
	    vcp->fill_arrows_on != ovcp->fill_arrows_on ||
	    vcp->fill_over_edge != ovcp->fill_over_edge ||
	    vcp->levels != ovcp->levels ||
	    vcp->level_strings != ovcp->level_strings) {
		vcp->new_draw_req = True;
	}

	return ret;
}

/*
 * Function:    ManageGenArray
 *
 * Description:	Handles details of managing a GenArray
 *
 * In Args:	count		number of elements to create in the GenArray
 *		copy_ga 	GenArray to copy values from - if
 *				NULL it is ignored.
 *		type		type of GenArray to create - int,float,string
 *		*init_val	if non-null an initialization value to use -
 *				strings have the array index appended
 *		resource_name	name of the GenArray resource 		
 *		entry_name	name of the high level caller of the routine 
 *
 * Out Args:	*ga		If non-NULL on input, contains a previously
 *				allocated GenArray, whose data will be 
 *				replaced if necessary.
 *				Out: An allocated GenArray with allocated data
 *		*old_count	the previous count in the old gen array
 *		*init_count	number of values initialized - if init_val is
 *				non-NULL, will contain count; if init_val is
 *				NULL will contain MIN(count,number of 
 *				elements in copy_ga); if copy_ga is also NULL
 *				will contain 0.
 *		*need_check     True if a GenArray copy occurs or the number
 *				of elements increases and no initialization
 *				value is supplied. False otherwise.
 *		*changed	True if the data has been modified in any way.
 *
 *
 * Return Values:
 *
 * Side Effects: The internal copy of each GenArray is modified to reflect
 *	changes requested via VectorPlotSetValues
 */

/*ARGSUSED*/
static NhlErrorTypes    ManageGenArray
#if	NhlNeedProto
	(NhlGenArray	*ga,
	 int		count,
	 NhlGenArray	copy_ga,
	 NrmQuark	type,
	 NhlPointer	init_val,
	 int		*old_count,
	 int		*init_count,
	 NhlBoolean	*need_check,
	 NhlBoolean	*changed,
	 NhlString	resource_name,
	 NhlString	entry_name)
#else
(ga,count,copy_ga,type,init_val,old_count,init_count,
 need_check,changed,resource_name,entry_name)
	NhlGenArray	*ga;
	int		count;
	NhlGenArray	copy_ga;
	NrmQuark	type;
	NhlPointer	init_val;
	int		*old_count;
	int		*init_count;
	NhlBoolean	*need_check;
	NhlBoolean	*changed;
	NhlString	resource_name;
	NhlString	entry_name;
#endif
{
	char		*str_type;
	NhlErrorTypes	ret = NhlNOERROR;
	int		i, size;
	NhlPointer	datap;
	char		*e_text;

	*init_count = 0;
	*need_check = False;
	*changed = False;
	*old_count = 0;

	if (type == Qint) {
		str_type = NhlTInteger;
		size = sizeof(int);
	}
	else if (type == Qcolorindex) {
		str_type = NhlTColorIndex;
		size = sizeof(NhlColorIndex);
	}
	else if (type == Qfloat) {
		str_type = NhlTFloat;
		size = sizeof(float);
	}
	else if (type == Qstring) {
		str_type = NhlTString;
		size = sizeof(NhlString);
	}
	else {
		e_text = "%s: internal error; unsupported type for %s";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name,
			  resource_name);
		return NhlFATAL;
	}

	if (*ga != NULL) {
		datap = (*ga)->data;
		*old_count = (*ga)->num_elements;
		*init_count = *old_count;

		if (count > (*ga)->num_elements) {
			if ((datap = (NhlPointer)
			     NhlRealloc(datap, count * size)) == NULL) {
				e_text = "%s: error reallocating %s data";
				NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
					  entry_name,resource_name);
				return NhlFATAL;
			}
			(*ga)->data = datap;
			(*ga)->num_elements = count;
			*changed = True;
		}
		else if (*ga == copy_ga) {
			*init_count = (*ga)->num_elements;
			return ret;
		}
	}
	else {
		if ((datap = (NhlPointer) NhlMalloc(count * size)) == NULL) {
			e_text = "%s: error creating %s array";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name,
				  resource_name);
			return NhlFATAL;
		}

		if ((*ga = NhlCreateGenArray((NhlPointer)datap,str_type,
					     size,1,&count)) == NULL) {
			e_text = "%s: error creating %s GenArray";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name,
				  resource_name);
			return NhlFATAL;
		}
		(*ga)->my_data = True;
		*changed = True;
	}

/* 
 * If there is a GenArray to copy, copy it; then initialize all remaining
 * uninitialized elements if an initialization value has been passed in.
 */

	if (copy_ga != NULL && copy_ga != *ga) {

		*need_check = True;
		ret = _NhlValidatedGenArrayCopy(ga,copy_ga,Nhl_vcMAX_LEVELS+1,
						True,False,resource_name, 
						entry_name);
		if (ret < NhlWARNING) {
			e_text = "%s: error copying %s GenArray";
			NhlPError(ret,NhlEUNKNOWN,e_text,entry_name,
				  resource_name);
			return ret;
		}
		*init_count = copy_ga->num_elements;
		*changed = True;
	}

	if (*init_count < count) {

		if (init_val == NULL) {
			if (type == Qstring) {
				NhlString *sp = (NhlString *) datap;
				for (i = *init_count; i< count; i++) {
					if (i < *old_count) NhlFree(sp[i]);
					sp[i] = NULL;
				}
			}
			*need_check = True;
			return ret;
		}
		else if (type == Qint)
			for (i = *init_count; i< count; i++)
				((int *)datap)[i] = *((int *)init_val);
		else if (type == Qcolorindex)
			for (i = *init_count; i< count; i++)
				((NhlColorIndex *)datap)[i] =
						*((NhlColorIndex *)init_val);
		else if (type == Qfloat)
			for (i = *init_count; i< count; i++)
				((float *)datap)[i] = *((float *)init_val);
		else if (type == Qstring) {
			char *sp;
			char *init_str = (char *) init_val;
			char numstr[10];
			for (i = *init_count; i< count; i++) {
				sprintf(numstr,"%d",i);
				if ((sp = (char *) 
				     NhlMalloc(sizeof(init_str)+
					       sizeof(numstr)+1)) == NULL) {
					e_text = "%s: error creating %s array";
					NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,
						  entry_name,resource_name);
					return NhlFATAL;
				}
				((char **)datap)[i] = sp;
				strcpy(sp,init_str);
				strcat(sp,numstr);
			}
		}
		*init_count = count;
		*changed = True;
	}

	return ret;
}


/*
 * Function:    CheckColorArray
 *
 * Description:	Checks color array values for validity, initializing any
 *		currently uninitialized values and keeps the GKS index
 *		arrays up-to-date.
 *
 * In Args:	count		number of elements to create in the GenArray
 *		resource_name	name of the GenArray resource 		
 *		entry_name	name of the high level caller of the routine 
 *
 * Out Args:	*ga		If non-NULL on input, contains a previously
 *				allocated GenArray, whose data will be 
 *				replaced if necessary.
 *				Out: An allocated GenArray with allocated data
 *		*init_count	number of values initialized - if init_val is
 *				non-NULL, will contain count; if init_val is
 *				NULL will contain MIN(count,number of 
 *				elements in copy_ga); if copy_ga is also NULL
 *				will contain 0.
 *
 * Return Values:
 *
 * Side Effects: The internal copy of each GenArray is modified to reflect
 *	changes requested via VectorPlotSetValues
 */

/*ARGSUSED*/

static NhlErrorTypes	CheckColorArray
#if	NhlNeedProto
	(NhlVectorPlotLayer	cl,
	NhlGenArray	ga,
	int		count,
	int		init_count,
	int		last_count,
	int		**gks_colors,
	NhlString	resource_name,
	NhlString	entry_name)
#else
(cl,ga,count,init_count,last_count,gks_colors,resource_name,entry_name)
	NhlVectorPlotLayer	cl;
	NhlGenArray	ga;
	int		count;
	int		init_count;
	int		last_count;
	int		**gks_colors;
	NhlString	resource_name;
	NhlString	entry_name;
#endif
{
	NhlErrorTypes ret = NhlNOERROR;
	char *e_text;
	int *ip;
	int i;
	

	ip = (int *) ga->data;

	for (i=init_count; i < count; i++) {
		ip[i] = 1 + i;
	}

	if (last_count == 0) {
		if ((*gks_colors = 
		     (int *) NhlMalloc(count * sizeof(int))) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
	}
	else if (count > last_count) {
		if ((*gks_colors = 
		     (int *) NhlRealloc(*gks_colors,
					count * sizeof(int))) == NULL) {
			e_text = "%s: dynamic memory allocation error";
			NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
			return NhlFATAL;
		}
	}

	for (i=0; i<count; i++) {
                if (ip[i] == NhlTRANSPARENT)
                        (*gks_colors)[i] = NhlTRANSPARENT;
                else
                        (*gks_colors)[i] =
                                _NhlGetGksCi(cl->base.wkptr,ip[i]);
	}
	return ret;
}


/*
 * Function:  SetupLevels
 *
 * Description:
 *
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    SetupLevels
#if	NhlNeedProto
	(NhlLayer	new, 
	 NhlLayer	old,
	 NhlBoolean	init,
	 float		**levels,
	 NhlBoolean	*modified)
#else
(new,old,init,levels,modified)
	NhlLayer		new;
	NhlLayer		old;
	NhlBoolean	init;
	float		**levels;
	NhlBoolean	*modified;

#endif

{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	char			*entry_name;
	NhlVectorPlotLayer		vcnew = (NhlVectorPlotLayer) new;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayer		vcold = (NhlVectorPlotLayer) old;
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	float			min,max;

	entry_name = init ? InitName : SetValuesName;
	*modified = False;

	if ((! init) && 
	    (! vcp->data_changed) &&
	    (! vcp->level_spacing_set) && 
	    (vcp->levels == ovcp->levels) &&
	    (vcp->level_selection_mode == ovcp->level_selection_mode) &&
	    (vcp->max_level_count == ovcp->max_level_count) &&
	    (vcp->min_level_val == ovcp->min_level_val) &&
	    (vcp->max_level_val == ovcp->max_level_val) &&
	    (vcp->zero_field == ovcp->zero_field) &&
	    (vcp->use_scalar_array == ovcp->use_scalar_array))
		return ret;

	vcp->new_draw_req = True;
	if (vcp->level_spacing_set && vcp->level_spacing <= 0.0) {
		e_text = 
			"%s: Invalid level spacing value set: defaulting";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ret = MIN(ret,NhlWARNING);
		vcp->level_spacing = 5.0;
	}
	

	if (! vcp->use_scalar_array) {
		min = vcp->zmin;
		max = vcp->zmax;
	}
	else if (! vcp->scalar_data_init) {
		e_text = 
		      "%s: No scalar data: using vector magnitude for levels";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ret = MIN(ret,NhlWARNING);
		min = vcp->zmin;
		max = vcp->zmax;
	}
	else {
		min = vcp->scalar_min;
		max = vcp->scalar_max;
	}
		
	if (vcp->min_level_val >= vcp->max_level_val) {
		e_text =
		"%s: Invalid level values set: defaulting to AUTOMATIC mode ";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ret = MIN(ret,NhlWARNING);
		vcp->min_level_val = min;
		vcp->max_level_val = max;
		vcp->level_selection_mode = NhlAUTOMATICLEVELS;
	}
	if (max <= vcp->min_level_val || min >=  vcp->max_level_val) {
		e_text =
	   "%s: Data values and min/max levels are disjoint sets: defaulting";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ret = MIN(ret,NhlWARNING);
		vcp->min_level_val = min;
		vcp->max_level_val = max;
	}
	
	switch (vcp->level_selection_mode) {

	case NhlMANUALLEVELS:

		if (! vcp->min_level_set) {
			subret = SetupLevelsAutomatic(vcnew,vcold,levels,
						      min,max,entry_name);
		}
		else {
			subret = SetupLevelsManual(vcnew,vcold,levels,
						   min,max,entry_name);
		}
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			return ret;
		}
		*modified = True;
		break;

	case NhlEQUALSPACEDLEVELS:

		subret = SetupLevelsEqual(vcnew,vcold,levels,
					  min,max,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			return ret;
		}
		*modified = True;
		break;

	case NhlAUTOMATICLEVELS:

		subret = SetupLevelsAutomatic(vcnew,vcold,levels,
					      min,max,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			return ret;
		}
		*modified = True;
		break;
			
	case NhlEXPLICITLEVELS:

		if (init && vcp->levels == NULL) {
			subret = SetupLevelsAutomatic(vcnew,vcold,levels,
						      min,max,entry_name);
		}
		else {
			subret = SetupLevelsExplicit(vcnew,vcold,init,levels,
						     max,min,entry_name);
		}
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			return ret;
		}
		*modified = True;
		break;

	default:
		ret = NhlFATAL;
		e_text = "%s: Invalid level selection mode";
		NhlPError(ret,NhlEUNKNOWN,e_text,entry_name);
		return NhlFATAL;
	}

	vcp->min_level_set = True;
	vcp->max_level_set = True;

		
	return ret;

}

/*
 * Function:  SetupLevelsManual
 *
 * Description: Sets up Manual mode levels
 *
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    SetupLevelsManual
#if	NhlNeedProto
	(NhlVectorPlotLayer	vcnew, 
	 NhlVectorPlotLayer	vcold,
	 float		**levels,
        float		min,
	float		max,
	 char		*entry_name)
#else
(vcnew,vcold,levels,entry_name)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	float		**levels;
        float		min,
	float		max,
	char		*entry_name;

#endif

{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*e_text;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	int			i, count;
	float			lmin,lmax,spacing;
	float			*fp;

	spacing = vcp->level_spacing;
	if (vcp->min_level_set) {
		lmin = vcp->min_level_val;
	}
	else {
		lmin = min;
	}
	
	if (vcp->max_level_set) {
		lmax = vcp->max_level_val;
	}
	else {
		lmax = floor(((max - lmin) / spacing) * spacing + lmin);
		if (_NhlCmpFAny(lmax,vcp->zmax,6) == 0.0) {
			lmax -= spacing;
		}
		vcp->max_level_val = lmax;
	}

	if (fmod((lmax - lmin), vcp->level_spacing) > 0.0)
		count =	(lmax - lmin) / vcp->level_spacing + 2.0;
	else
		count =	(lmax - lmin) / vcp->level_spacing + 1.5;

	if (count <= 0) {
		e_text = 
	  "%s: vcLevelSpacingF value exceeds or equals data range: defaulting";
		NhlPError(NhlWARNING,NhlEUNKNOWN,e_text,entry_name);
		ret = SetupLevelsAutomatic(vcnew,vcold,levels,
					   min,max,entry_name);
		return MIN(NhlWARNING,ret);
	}
	if (count >  Nhl_vcMAX_LEVELS) {
		ret = MIN(NhlWARNING,ret);
		e_text = 
 "%s: vcLevelSpacingF value causes level count to exceed maximum: defaulting";
		NhlPError(ret,NhlEUNKNOWN,e_text,entry_name);
		vcp->level_spacing = (lmax - lmin) / 
			(vcp->max_level_count - 1);
		count = vcp->max_level_count;
	}
	else {
		vcp->max_level_count = MAX(vcp->max_level_count, count);
	}

	if ((*levels = (float *) 
	     NhlMalloc(count * sizeof(float))) == NULL) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(NhlFATAL);
	}
	for (i=0, fp = *levels; i < count - 1; i++) {
		*(fp++) = lmin + i * vcp->level_spacing;
	}
	*fp = lmax;

	vcp->level_count = count;

	return ret;
}

/*
 * Function:  SetupLevelsEqual
 *
 * Description: Sets up Equally spaced levels
 *
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    SetupLevelsEqual
#if	NhlNeedProto
	(NhlVectorPlotLayer	vcnew,
	 NhlVectorPlotLayer	vcold,
	 float		**levels,
        float		min,
	float		max,
	 char		*entry_name)
#else
(vcnew,vcold,levels,entry_name)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	float		**levels;
        float		min,
	float		max,
	char		*entry_name;

#endif

{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*e_text;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	int			i;
	float			lmin,lmax,size;

	lmin = min;
	lmax = max;
	size = (lmax - lmin) / (vcp->max_level_count + 1);

	vcp->level_count = vcp->max_level_count;
	if ((*levels = (float *) 
	     NhlMalloc(vcp->level_count * sizeof(float))) == NULL) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
	for (i=0; i < vcp->level_count; i++) {
		(*levels)[i] = vcp->zmin + (i+1) * size;
	}
	
	vcp->min_level_val = (*levels)[0];
	vcp->max_level_val = (*levels)[vcp->level_count - 1];
	vcp->level_spacing = size;

	return ret;
}

/*
 * Function:  SetupLevelsAutomatic
 *
 * Description: Sets up Automatic mode levels
 *
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    SetupLevelsAutomatic
#if	NhlNeedProto
	(NhlVectorPlotLayer	vcnew, 
	 NhlVectorPlotLayer	vcold,
	 float		**levels,
        float		min,
	float		max,
	 char		*entry_name)
#else
(vcnew,vcold,levels,entry_name)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	float		**levels;
        float		min,
	float		max,
	char		*entry_name;

#endif

{
	NhlErrorTypes		ret = NhlNOERROR, subret = NhlNOERROR;
	char			*e_text;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	int			i,count = 0;

	float			lmin,lmax,ftmp,ftest;
	float			spacing;
	NhlBoolean		choose_spacing = True;

	lmin = min;
	lmax = max;

	if (vcp->level_spacing_set) {
		spacing = vcp->level_spacing;
		lmin = ceil(lmin / spacing) * spacing;
		lmax = MIN(lmax,floor(lmax / spacing) * spacing);
		count =	(int)((lmax - lmin) / vcp->level_spacing + 1.5);
		if (_NhlCmpFAny(lmin,min,6) == 0.0) {
			lmin += spacing;
			count--;
		}
		if (_NhlCmpFAny(lmax,max,6) == 0.0) {
			lmax -= spacing;
			count--;
		}
		if (count <= 0) {
			ret = MIN(NhlWARNING,ret);
			lmin = min;
			lmax = max;
			e_text = 
	  "%s: vcLevelSpacingF value exceeds or equals data range: defaulting";
			NhlPError(ret,NhlEUNKNOWN,e_text,entry_name);
		}
		else if (count >  Nhl_vcMAX_LEVELS) {
			ret = MIN(NhlWARNING,ret);
			e_text = 
 "%s: vcLevelSpacingF value causes level count to exceed maximum: defaulting";
			NhlPError(ret,NhlEUNKNOWN,e_text,entry_name);
		}
		else {
			vcp->max_level_count = 
				MAX(vcp->max_level_count, count);
			choose_spacing = False;
		}
	}
	if (choose_spacing) {
		subret = ChooseSpacingLin(&lmin,&lmax,&spacing,7,
					  vcp->max_level_count,entry_name);
		if ((ret = MIN(subret,ret)) < NhlWARNING) {
			e_text = "%s: error choosing spacing";
			NhlPError(ret,NhlEUNKNOWN,e_text,entry_name);
			return ret;
		}
		if (_NhlCmpFAny(lmin,min,6) == 0.0) {
			lmin += spacing;
		}
		ftmp = lmin;
		ftest = max;
		count = 0;
		while (_NhlCmpFAny(ftmp,ftest,6) < 0.0) {
			count++;
			ftmp = lmin + count * spacing;
		}
	}
	if (count == 0) {
		e_text = "%s: error setting automatic levels";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}

	if ((*levels = (float *) NhlMalloc(count * sizeof(float))) == NULL) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
	for (i =  0; i <  count; i++)
		(*levels)[i] = lmin + i * spacing;

	vcp->level_spacing = spacing;
	vcp->level_count = count;
	vcp->min_level_val = lmin;
	vcp->max_level_val = (*levels)[count - 1];

	return ret;
}

/*
 * Function:  SetupLevelsExplicit
 *
 * Description: Sets up Explicit mode levels
 *
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static NhlErrorTypes    SetupLevelsExplicit
#if	NhlNeedProto
	(NhlVectorPlotLayer	vcnew, 
	 NhlVectorPlotLayer	vcold,
	 NhlBoolean		init,
	 float			**levels,
	 float			min,
	 float			max,
	 char			*entry_name)
#else
(vcnew,vcold,levels,entry_name)
	NhlVectorPlotLayer	vcnew;
	NhlVectorPlotLayer	vcold;
	NhlBoolean	init;
	float		**levels;
        float		min,
	float		max,
	char		*entry_name;

#endif

{
	NhlErrorTypes		ret = NhlNOERROR;
	char			*e_text;
	NhlVectorPlotLayerPart	*vcp = &(vcnew->vectorplot);
	NhlVectorPlotLayerPart	*ovcp = &(vcold->vectorplot);
	int			i,j,count;
	float			*fp;
	float			ftmp;

	if (vcp->levels == NULL || vcp->levels->num_elements < 1) {
		ret = MIN(NhlWARNING,ret);
		e_text = 
	      "%s: %s is NULL: defaulting to Automatic level selection mode";
		NhlPError(ret,NhlEUNKNOWN,e_text,entry_name,NhlNvcLevels);
		return SetupLevelsAutomatic(vcnew,vcold,levels,
					    min,max,entry_name);
	}
	if (init || vcp->levels != ovcp->levels)
		count = vcp->levels->num_elements;
	else 
		count = vcp->level_count;

	if (count > Nhl_vcMAX_LEVELS) {
		ret = MIN(NhlWARNING,ret);
		e_text = 
  "%s: Explicit level array count exceeds max level count: defaulting to Automatic level selection mode";
		NhlPError(ret,NhlEUNKNOWN,e_text,entry_name);
		return SetupLevelsAutomatic(vcnew,vcold,levels,
					    min,max,entry_name);
	}
/*
 * Allocate space for the levels
 */
	fp = (float *) vcp->levels->data;
	if ((*levels = (float *) NhlMalloc(count * sizeof(float))) == NULL) {
		e_text = "%s: dynamic memory allocation error";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(ret);
	}
	for (i = 0; i < count; i++)
		(*levels)[i] = fp[i];

	fp = *levels;
		
/*
 * Sort the array into ascending order
 */
	for (i = 0; i < count; i++) {
		int min = i;
		for (j = i + 1; j < count; j++)
			if (fp[j] < fp[min])
				min = j;
		if (min != i) {
			ftmp = fp[min];
			fp[min] = fp[i];
			fp[i] = ftmp;
		}
	}
/*
 * Find the average spacing
 */
	ftmp = 0;
	for (i = 1; i < count; i++) {
		ftmp += fp[i] - fp[i-1];
	}

	vcp->level_spacing = ftmp / (count - 1);
	vcp->min_level_val = fp[0];
	vcp->max_level_val = fp[count - 1];

	vcp->level_count = count;

	return ret;
}

/*
 * Function:	ChooseSpacingLin
 *
 * Description: Ethan's tick mark spacing code adapted to choosing 'nice'
 *		VectorPlot values; adapted by exchanging the ciel and floor
 *		functions - since the max and min vector values must be
 *		within the data space rather than just outside it as is 
 *		appropriate for tick marks. (Eventually should probably
 *		generalize the code with another parameter to handle either
 *		situation.)
 *
 * In Args:	tstart	requested tick starting location
 *		tend	requested tick ending location
 *		convert_precision  precision used to compare with
 *		max_ticks the maximum number of ticks to be choosen
 * Out Args:
 *		tstart  new start
 *		tend	new end
 *		spacing new spacing
 *
 * Return Values: Error Conditions
 *
 * Side Effects: NONE
 */
static NhlErrorTypes ChooseSpacingLin
#if	NhlNeedProto
(
	float *tstart,
	float *tend,
	float *spacing,
	int convert_precision,
	int max_ticks,
	NhlString entry_name)
#else
(tstart,tend,spacing,convert_precision,max_ticks,entry_name)
	float *tstart;
	float *tend;
	float *spacing;
	int	convert_precision;
	int	max_ticks;
#endif
{
	double	table[] = 
	{ 1.0,2.0,2.5,4.0,5.0,
		  10.0,20.0,25.0,40.0,50.0,
		  100.0,200.0,250.0,400.0,500.0 };
	double	d,u,t,am1,ax1;
	double	am2=0.0,ax2=0.0;
	int	npts = 15;
	int	i;
	char	*e_text;

	if(_NhlCmpFAny(*tend,*tstart,8)<=0.0) {
		e_text = "%s: Vector field is constant";
		NhlPError(NhlFATAL,NhlEUNKNOWN,e_text,entry_name);
		return(NhlFATAL);
	}
		
	d = pow(10.0,floor(log10(*tend-*tstart)) - 2.0);
	u = *spacing = 1e30;
	for(i=0;i<npts; i++) {
		t = table[i] * d;
		am1 = ceil(*tstart/t) *t;
		ax1 = floor(*tend/t) * t;
		if(((i>=npts-1)&&(*spacing == u))||
		   ((t <= *spacing)&&
		    (_NhlCmpFAny((ax1-am1)/t,(double)max_ticks,
				 convert_precision) <= 0.0))){
			*spacing = t;
			ax2 = ax1;
			am2 = am1;
		}
	}
	*tstart = am2;
	*tend = ax2;
	return(NhlNOERROR);
	
}

/*
 * Function:  hluvvmpxy
 *
 * Description: 
 *
 * In Args:
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

void (_NHLCALLF(hluvvmpxy,HLUVVMPXY))
#if	NhlNeedProto
(
 float *x,
 float *y, 
 float *u, 
 float *v, 
 float *uvm, 
 float *xb, 
 float *yb, 
 float *xe, 
 float *ye, 
 int *ist
)
#else
(x,y,u,v,uvm,xb,yb,xe,ye,ist)
	float *x;
	float *y; 
	float *u; 
	float *v; 
	float *uvm; 
	float *xb; 
	float *yb; 
	float *xe; 
	float *ye; 
	int *ist;
#endif
{
	int status = 1;
	float tmpx,tmpy;
	float xout, yout;
	float xdata,ydata;
	float dv1, duv, dv2, xt, xtf, yt, ytf, sgn = 1.0;
	int ict = 0;
	NhlLayer trans_p;
        static int imap,itrt;
        static float dvmx,sxdc,sydc,wxmn,wxmx,wymn,wymx;
	static NhlLayer trans_obj, overlay_trans_obj;
	static NhlBoolean over_map;
	float xsc = 1.0, ysc = 1.0;

        if (Vcp == NULL) {
		_NHLCALLF(vvmpxy,VVMPXY)(x,y,u,v,uvm,xb,yb,xe,ye,ist);
		return;
	}

	*ist = 0;

	if (Need_Info) {
		over_map = False;
		_NHLCALLF(vvgetmapinfo,VVGETMAPINFO) 
                        (&imap,&itrt,
                         &dvmx,&sxdc,&sydc,&wxmn,&wxmx,&wymn,&wymx);
		if (imap != NhlvcMAPVAL)
			return;
		if (Vcl->trans.overlay_status == _tfCurrentOverlayMember) {
			overlay_trans_obj = Vcl->trans.overlay_trans_obj;
			trans_obj = Vcl->trans.trans_obj;
			if (!_NhlIsTransObj(overlay_trans_obj)) {
				overlay_trans_obj = NULL;
				printf("error- overlay_trans_obj is NULL\n");
			}
			else if ((overlay_trans_obj->base.layer_class)->base_class.class_name ==
				NhlmapTransObjClass->base_class.class_name) {
				over_map = True;
			}
		}
		else {
			overlay_trans_obj = NULL;
			trans_obj = Vcp->trans_obj;
		}
		if (!_NhlIsTransObj(trans_obj)) {
			trans_obj = NULL;
			printf("error - trans_obj is NULL\n");
		}
		Need_Info = False;
	}
	if (trans_obj == NULL) {
		*ist = -5;
		return;
	}
	else if (overlay_trans_obj == NULL) {
		trans_p = trans_obj;

		_NhlCompcToWin(trans_p,x,y,1,&xout,&yout,&status,NULL,NULL);
		if(status) {
			*ist = -5;
			return;
		}
		*xb = c_cufx(xout);
		*yb = c_cufy(yout);
		*xe = *xb + *u * sxdc * xsc;
		*ye = *yb + *v * sydc * ysc;
#if 0
		printf("compc %f %f : win %f %f", *x, *y, xout, yout);
		printf("ndc %f %f\n", *xb, *yb);
#endif

		if (itrt < 1)
			return;
			
		_NhlCompcToData(trans_p,x,y,1,&xdata,&ydata,
			&status,NULL,NULL);
		if(status) {
			*ist = -5;
			return;
		}
#if 0
		printf("data %f %f\n", xdata,ydata);
#endif
		

	} else { /* do overlay */

		_NhlCompcToData(trans_obj,x,y,1,&xdata,&ydata,
			&status,NULL,NULL);
		if(status) {
			*ist = -5;
			return;
		}

		_NhlDataToWin(overlay_trans_obj,
			      &xdata, &ydata,1,&xout,&yout,
			      &status,NULL,NULL);
		if(status) {
			*ist = -5;
			return;
		}
#if 0
		printf("compc %f %f : win %f %f\n", *x, *y, xout, yout);
		printf("data %f %f\n", xdata,ydata);
#endif

		*xb = c_cufx(xout);
		*yb = c_cufy(yout);
		*xe = *xb + *u * sxdc * xsc;
		*ye = *yb + *v * sydc * ysc;

		if (itrt < 1)
			return;

		trans_p = overlay_trans_obj;
			
	}

	
	dv1=sqrt((*xe-*xb)*(*xe-*xb)+(*ye-*yb)*(*ye-*yb));

	duv = 0.1 / *uvm;

	do {

/*
 * calculate the incremental end points, then check to see if 
 * they take us out of the user coordinate boundaries. if they
 * do, try incrementing in the other direction
 */
#define DEG2RAD 0.017453292519943 

		if (over_map) {
			if ((int)(fabs(ydata)*1e5+0.5) == (int) (1e5*90)) {
				*ist = -1;
				return;
			}
			else 
				xt = xdata + (sgn * *u * duv * xsc) /
					cos(DEG2RAD*ydata);
		}
		else {
			xt = xdata + sgn * *u * duv * xsc;
		}
		yt = ydata + sgn * *v * duv * ysc;
		
		_NhlDataToWin(trans_p,&xt,&yt,1,&tmpx,&tmpy,
			      &status,NULL,NULL);
		if (status)
			tmpx = xt, tmpy = yt;
		
		if (tmpx < wxmn || tmpx > wxmx || 
		    tmpy < wymn || tmpy > wymx) {
			if (sgn == 1.0) {
				sgn = -1.0;
				continue;
			}
			else {
				*ist=-4;
			 	return;
			}
		}
		
/*
 * convert to fractional coordinates and find the incremental
 * distance in the fractional system. to ensure that this distance
 * is meaningful, we require that it be between 1e3 and 1e4
 * times smaller than the maximum vector length.
 */ 
		xtf=c_cufx(tmpx);
		ytf=c_cufy(tmpy);
		dv2=sqrt((xtf-*xb)*(xtf-*xb)+
			 (ytf-*yb)*(ytf-*yb));
		if (dv2*1e3 > dvmx) {
			ict=ict+1;
			duv=duv/2.0;
			continue;
		}
		else if (dv2*1e4 < dvmx) {
			ict=ict+1;
			duv=duv*2.0;
			continue;
		}
/*
 * the actual endpoints are found using the ratio of the incremental
 * distance to the actual distance times the fractional component
 * length
 */
		*xe=*xb+sgn*(xtf-*xb)*dv1/dv2;
		*ye=*yb+sgn*(ytf-*yb)*dv1/dv2;
		return;

	}
	while (ict < 40);
	*ist = -3;
	return;
}


/*
 * Function:  load_hlucp_routines
 *
 * Description: Forces the hlucp... routines to load from the HLU library
 *
 * In Args:   NhlBoolean flag - should always be False - dont actually
 *			        want to call the routines.
 *
 * Out Args:
 *
 * Return Values:
 *
 * Side Effects: 
 */

/*ARGSUSED*/
static void   load_hlucp_routines
#if	NhlNeedProto
(
	NhlBoolean	flag
)
#else
(flag)
	NhlBoolean	flag;
#endif
{

	if (flag) {
		float fdm;
		int   idm;
		(_NHLCALLF(hluvvmpxy,HLUVVMPXY))
			(&fdm,&fdm,&fdm,&fdm,&fdm,&fdm,&fdm,&fdm,&fdm,&idm);
	}
	return;
}

