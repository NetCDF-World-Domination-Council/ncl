.\"
.\"	$Id: nhlf77.m,v 1.1 1995-04-26 18:21:25 haley Exp $
.\"
.TH NHLF77 1NCARG "April 1995" NCAR "NCAR GRAPHICS"
.SH NAME
nhlf77 \- Command for compiling Fortran code that uses the NCAR Graphics
high-level utilities.
.SH SYNOPSIS
\fBnhlf77\fP 
[\fB\-libnetcdf\fR]
[\fB\-libhdf\fR] ...
.SH DESCRIPTION
\fInhlf77\fP is a script that invokes the FORTRAN 77 compiler/linker
with the proper NCAR Graphics LLU (low-level utility) and HLU
(high-level utility) libraries.  Arguments presented above are
associated with NCAR Graphics.  All other arguments and options are
identical to the f77 command on your particular machine; arguments
that include quoted strings may have to be enclosed in single quotes.
.sp
In order to run \fInhlf77\fP, you must have your NCARG_ROOT
environment variable set to the directory pathname where the NCAR
Graphics libraries, binaries, and include files were installed.  If
you are not sure what NCARG_ROOT should be set to, please check with 
your system administrator or the site representative for NCAR Graphics.
If the NCAR Graphics libraries, binaries, and include files were not
installed under one root directory, then you will need to set the 
environment variables NCARG_LIB, NCARG_BIN, and NCARG_INCLUDE instead.
Please see "man ncargintro" for more information.
.sp
Note that, on some systems, if you supply your own binary libraries in
addition to the ones automatically referenced by \fInhlf77\fR, all the
libraries must have been created in a similar fashion.
.sp
.I OPTIONS
.IP "\-libnetcdf"
Links in the netCDF library.  This library is not part of NCAR Graphics,
so check with your system administrator if you need it installed.  You
can obtain a copy of it by doing an anonymous ftp to unidata.ucar.edu.
.sp
.IP "\-libhdf"
Links in the HDF library.  This library is not part of NCAR Graphics,
so check with your system administrator if you need it installed.  You
can obtain a copy of it by doing an anonymous ftp to ftp.ncsa.uiuc.edu.
.sp
.SH SEE ALSO
Online:
.BR nhlcc(1NCARG),
.BR ncargf77(1NCARG),
.BR ncargcc(1NCARG),
.BR ncargintro(5NCARG)
.sp
.SH COPYRIGHT
Copyright 1995 University Corporation
for Atmospheric Research
.br
All Rights Reserved
