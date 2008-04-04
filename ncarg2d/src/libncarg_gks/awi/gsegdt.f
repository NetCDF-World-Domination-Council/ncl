C
C	$Id: gsegdt.f,v 1.5 2008-04-04 21:02:58 kennison Exp $
C                                                                      
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C This file is free software; you can redistribute it and/or modify
C it under the terms of the GNU General Public License as published
C by the Free Software Foundation; either version 2 of the License, or
C (at your option) any later version.
C
C This software is distributed in the hope that it will be useful, but
C WITHOUT ANY WARRANTY; without even the implied warranty of
C MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
C General Public License for more details.
C
C You should have received a copy of the GNU General Public License
C along with this software; if not, write to the Free Software
C Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
C USA.
C
      SUBROUTINE GSEGDT
C
C Calling this do-nothing subroutine forces "ld" to load the following
C block data routine (but only if they are in the same ".f" file).
C
        RETURN
C
      END
CNOSPLIT
      BLOCKDATA GSEGDTX
C
C  Define all constant data for the segment interpreter.
C
      include 'trstat.h'
      include 'trbufr.h'
      include 'trpars.h'
      include 'trinst.h'
      include 'trcode.h'
      include 'trdefl.h'
C
C  Initialize the record pointer.
C
      DATA METREC /1/
C
C  Default polyline info.
C
      DATA POLIDF, LINTDF, LINWDF, TLNCDF /1, 1, 1.0, 1/
C
C  Default marker info.
C
      DATA MARIDF, MARTDF, MARSDF, TMRCDF, PMRSDF /1, 3, 1.0, 1, 320/
C
C  Default text and character info.
C
      DATA HORIDF, VERTDF, PATHDF, CHIGDF, XUDF, YUDF /0,0,0,320,0,1/
      DATA XBDF, YBDF, TXTIDF, TTXCDF, FINDDF, CEXPDF /1,0,1,1,1,1.0/
      DATA CSPADF, TXTPDF /0.0, 0/
C
C  Default fill info.
C
      DATA FILIDF, INTSDF, HATIDF, PATIDF, TFLCDF /1, 0, 1, 1, 1/
      DATA FILRDF /0,0/
C
C  Define the data field default bit sizes.
C
      DATA MOPDLN, MCIDPR, MFLDPR, MWHDPR /16, 8, 16, 16/
      DATA MIXDPR, MENDPR /16,16/
      DATA BYTSIZ, CONFLG /8, 1/
C
C  Define the default aspect source flags.
C
      DATA ASFSDF /ASFMAX*0/
C
C  Control element codes.
C
        DATA CELMVI, CELMCR, CELMCI /1,5,6/
C
C  Flag controlling whether to ignore clipping in segments.
C
        DATA IGSGCP/0/
C
C  Graphical primitive element codes.
C
        DATA GPELPL, GPELPM, GPELTX, GPELPG, GPELCA, GPELGP
     1          /1, 3, 4, 7, 9, 10/
C
C  Define the attribute element codes.
C       
        DATA ATELLI, ATELLT, ATELLW, ATELLC /1, 2, 3, 4/
        DATA ATELMB, ATELMT, ATELMZ, ATELMC /5, 6, 7, 8/
        DATA ATELTI, ATELTF, ATELTP, ATELCE, ATELCS /9, 10, 11, 12, 13/
        DATA ATELTC, ATELCH, ATELCO, ATELTH, ATELTA /14,15,16,17,18/
        DATA ATELFI, ATELIS, ATELFC, ATELHI, ATELPI /21,22,23,24,25/
        DATA ATELFR, ATELPT, ATELPS, ATELCB, ATELAS /31,32,33,34,35/
C
      END
