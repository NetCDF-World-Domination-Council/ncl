C
C $Id: mdritd.f,v 1.2 2001-11-02 22:37:17 kennison Exp $
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
      SUBROUTINE MDRITD (IAXS,ANGL,UCRD,VCRD,WCRD)
C
        INTEGER          IAXS
        DOUBLE PRECISION ANGL,UCRD,VCRD,WCRD
C
C Declare common block containing math constants.
C
        COMMON /MAPCM0/  COS1,DTOR,DTRH,OOPI,PI,PIOT,RTDD,RTOD,SIN1,TOPI
        DOUBLE PRECISION COS1,DTOR,DTRH,OOPI,PI,PIOT,RTDD,RTOD,SIN1,TOPI
        SAVE   /MAPCM0/
C
C Declare local variables.
C
        DOUBLE PRECISION SINA,COSA,UTMP,VTMP,WTMP
C
C This routine rotates the point with coordinates (UCRD,VCRD,WCRD) by
C the angle ANGL about the axis specified by IAXS (1 for the U axis,
C 2 for the V axis, 3 for the W axis).  A right-handed coordinate
C system is assumed.
C
        SINA=SIN(DTOR*ANGL)
        COSA=COS(DTOR*ANGL)
C
        UTMP=UCRD
        VTMP=VCRD
        WTMP=WCRD
C
        IF (IAXS.EQ.1) THEN
          VCRD=VTMP*COSA-WTMP*SINA
          WCRD=WTMP*COSA+VTMP*SINA
        ELSE IF (IAXS.EQ.2) THEN
          UCRD=UTMP*COSA+WTMP*SINA
          WCRD=WTMP*COSA-UTMP*SINA
        ELSE
          UCRD=UTMP*COSA-VTMP*SINA
          VCRD=VTMP*COSA+UTMP*SINA
        END IF
C
        RETURN
C
      END
