#!/bin/csh -f
#
#	$Id: nhlf77.csh,v 1.9 1997-04-11 18:11:14 haley Exp $
#

#*********************************************#
#                                             #
# Make sure NCARG_ROOT is set for this script #
#                                             #
#*********************************************#
setenv NCARG_ROOT  `ncargpath root`

if ($status != 0) then
	exit 1
endif

set xlibs = "SED_XLIB"
set system   = "SED_SYSTEM_INCLUDE"
set f77       = "SED_FC"
set loadflags  = "SED_LDFFLAGS"
set libdir   = `ncargpath SED_LIBDIR`
set incdir   = `ncargpath SED_INCDIR`
set syslibdir = "SED_LIBSEARCH"
set sysincdir = "SED_INCSEARCH"
set ro       = "$libdir/SED_NCARGDIR/SED_ROBJDIR"
set f77libs  = "SED_CTOFLIBS"
set newargv = "$f77 $loadflags"
set libpath = "-L$libdir $syslibdir"
set incpath = "-I$incdir $sysincdir"

#
# set up default libraries
#
set libncarg    = "-lncarg"
set libgks      = "-lncarg_gks"
set libmath     = ""
set libncarg_c  = "-lncarg_c"
set libhlu      = "-lhlu"
set extra_libs

foreach arg ($argv)
  switch ($arg)

  case "-ngmath":
    set libmath     = "-lngmath"
    breaksw

  case "-netcdf":
  case "-cdf":
    set extra_libs = "$extra_libs SED_NCDFLIBS"
    breaksw

  case "-hdf":
    set extra_libs = "$extra_libs SED_HDFLIB"
    breaksw

  default:
    set newargv = "$newargv $arg"
  endsw
end
set ncarg_libs = "$libhlu $libncarg $libgks $libncarg_c $libmath"

set newargv = "$newargv $libpath $incpath $extra_libs $ncarg_libs $xlibs $f77libs"

echo $newargv
eval $newargv
