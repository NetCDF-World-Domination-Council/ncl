/*
 *      $Id: Open.c,v 1.8 1994-08-11 21:37:04 boote Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1992			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		Open.c
 *
 *	Author:		Jeff W. Boote
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Mon Aug 31 09:50:11 MDT 1992
 *
 *	Description:	This file contains the functions neccessary to
 *			initialize the hlu library.
 */
#include <ncarg/hlu/hluP.h>
#include <ncarg/hlu/AppI.h>

/*
 * Function:	_NhlOpen
 *
 * Description:	internal init function - called for "C" and  "Fortran"
 *		interface.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	void
 * Side Effect:	
 */
/*ARGSUSED*/
static void _NhlOpen
#if	__STDC__
(
	_NhlC_OR_F	init_type
)
#else
(init_type)
	_NhlC_OR_F	init_type;
#endif
{
	int	tint;

	(void)NhlVACreate(&tint,"hlu",NhlappLayerClass,NhlNOPARENT,
			_NhlNappMode,	init_type,
			_NhlNnoAppDB,	True,
			_NhlNdefApp,	True,
			NULL);

	return;
}

/*
 * Function:	NhlOpen
 *
 * Description:	Init function for "C" interface.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
void NhlOpen
#if	__STDC__
(
	void
)
#else
()
#endif
{
	_NhlOpen(_NhlCLIB);

	return;
}

/*
 * Function:	nhlfopen
 *
 * Description:	init hlu library for use from the "Fortran" bindings.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global
 * Returns:	void
 * Side Effect:	
 */
void
_NHLCALLF(nhl_fopen,NHL_FOPEN)
#if	__STDC__
(
	void
)
#else
()
#endif
{
	_NhlOpen(_NhlFLIB);

	return;
}

/*
 * Function:	_NhlInitialize
 *
 * Description:	internal init function - called for "C" and  "Fortran"
 *		interface.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	static
 * Returns:	void
 * Side Effect:	
 */
/*ARGSUSED*/
static void _NhlInitialize
#if	__STDC__
(
	_NhlC_OR_F	init_type
)
#else
(init_type)
	_NhlC_OR_F	init_type;
#endif
{
	_NhlSetLang(init_type);

	return;
}

/*
 * Function:	NhlInitialize
 *
 * Description:	Init function for "C" interface.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	
 * Returns:	
 * Side Effect:	
 */
void NhlInitialize
#if	__STDC__
(
	void
)
#else
()
#endif
{
	_NhlInitialize(_NhlCLIB);

	return;
}

/*
 * Function:	nhlfinitialize
 *
 * Description:	init hlu library for use from the "Fortran" bindings.
 *
 * In Args:	
 *
 * Out Args:	
 *
 * Scope:	global
 * Returns:	void
 * Side Effect:	
 */
void
_NHLCALLF(nhl_finitialize,NHL_FINITIALIZE)
#if	__STDC__
(
	void
)
#else
()
#endif
{
	_NhlInitialize(_NhlFLIB);

	return;
}
