C
C	$Id: dashbd.f,v 1.5 2008-04-04 21:03:01 kennison Exp $
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
      SUBROUTINE DASHBD
C
C Calling this do-nothing subroutine forces "ld" to load the following
C block data routine (but only if they are in the same ".f" file).
C
        RETURN
C
      END
CNOSPLIT
      BLOCKDATA DASHBDX
C
C DASHBDX IS USED TO INITIALIZE VARIABLES IN NAMED COMMON.
C
      COMMON /DASHD1/  ISL,  L,  ISIZE,  IP(100),  NWDSM1,  IPFLAG(100)
     1                 ,MNCSTR, IGP
C
      COMMON /FDFLAG/ IFLAG
C
      COMMON /DDFLAG/ IFCFLG
C
      COMMON /DCFLAG/ IFSTFL
C
      COMMON /DFFLAG/ IFSTF2
C
      COMMON /CFFLAG/ IVCTFG
C
      COMMON /DSAVE3/ IXSTOR,IYSTOR
C
      COMMON /DSAVE5/ XSAVE(70), YSAVE(70), XSVN, YSVN, XSV1, YSV1,
     1                SLP1, SLPN, SSLP1, SSLPN, N, NSEG
C
      COMMON /SMFLAG/ IOFFS
C
      COMMON/INTPR/IPAU,FPART,TENSN,NP,SMALL,L1,ADDLR,ADDTB,MLLINE,
     1    ICLOSE
C
      COMMON /BLGASO/ IBLK,IGAP,ISOL
      CHARACTER*1     IBLK,IGAP,ISOL
C
      SAVE
C
C IFSTFL CONTROLS THAT FRSTD IS CALLED BEFORE VECTD IS CALLED (IN CFVLD)
C WHENEVER DASHDB OR DASHDC HAS BEEN CALLED.
C
      DATA IFSTFL /1/
C
C IVCTFG INDICATES IF VECTD IS BEING CALLED OR LASTD (IN CFVLD)
C
      DATA IVCTFG /1/
C
C ISL IS A FLAG FOR AN ALL SOLID PATTERN (+1) OR AN ALL GAP PATTERN (-1)
C
      DATA ISL /1/
C
C IGP IS AN INTERNAL PARAMETER. IT IS DESCRIBED IN THE DOCUMENTATION
C TO THE DASHED LINE PACKAGE.
C
      DATA IGP /9/
C
C MNCSTR IS THE MAXIMUM NUMBER OF CHARACTERS ALLOWED IN A HOLLERITH
C STRING PASSED TO DASHDC.
C
      DATA MNCSTR /15/
C
C IOFFS IS AN INTERNAL PARAMETER.
C IOFFS IS USED IN FDVDLD AND DRAWPV.
C
      DATA IOFFS /0/
C
C  INTERNAL PARAMETERS
C
      DATA IPAU/3/
      DATA FPART/1./
      DATA TENSN/2.5/
      DATA NP/150/
      DATA SMALL/128./
      DATA L1/70/
      DATA ADDLR/2./
      DATA ADDTB/2./
      DATA MLLINE/384/
      DATA ICLOSE/6/
C
C IFSTF2 IS A FLAG TO CONTROL THAT FRSTD IS CALLED BEFORE VECTD IS
C CALLED (IN SUBROUTINE FDVDLD), WHENEVER DASHDB OR DASHDC
C HAS BEEN CALLED.
C
      DATA IFSTF2 /1/
C
C IFLAG CONTROLS IF LASTD CAN BE CALLED DIRECTLY OR IF IT WAS JUST
C CALLED FROM BY VECTD SO THAT THIS CALL CAN BE IGNORED.
C
      DATA IFLAG /1/
C
C IFCFLG IS THE FIRST CALL FLAG FOR SUBROUTINES DASHDB AND DASHDC.
C  1 = FIRST CALL TO DASHDB OR DASHDC.
C  2 = DASHDB OR DASHDC HAS BEEN CALLED BEFORE.
C
      DATA IFCFLG /1/
C
C IXSTOR AND IYSTOR CONTAIN THE CURRENT PEN POSITION. THEY ARE
C INITIALIZED TO AN IMPOSSIBLE VALUE.
C
      DATA IXSTOR,IYSTOR /-9999,-9999/
C
C SLP1 AND SLPN ARE INITIALIZED TO AVOID THAT THEY ARE PASSED AS ACTUAL
C PARAMETERS FROM FDVDLD TO MSKRV1 WITHOUT BEING DEFINED.
C
      DATA SLP1,SLPN /-9999.,-9999./
C
C Define characters representing a blank, a gap, and a solid.
C
      DATA IBLK,IGAP,ISOL / ' ' , '''' , '$' /
C
      END
