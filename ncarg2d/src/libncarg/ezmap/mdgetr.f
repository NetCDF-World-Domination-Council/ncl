C
C $Id: mdgetr.f,v 1.4 2008-04-04 21:02:46 kennison Exp $
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
      SUBROUTINE MDGETR (WHCH,RVAL)
C
        CHARACTER*(*) WHCH
        REAL          RVAL
C
C Declare required common blocks.  See MAPBDX for descriptions of these
C common blocks and the variables in them.
C
        COMMON /MAPCM4/  GRDR,GRID,GRLA,GRLO,GRPO,OTOL,PHIA,PHIO,PLA1,
     +                   PLA2,PLA3,PLA4,PLB1,PLB2,PLB3,PLB4,PLTR,ROTA,
     +                   SRCH,XLOW,XROW,YBOW,YTOW,IDOT,IDSH,IDTL,ILCW,
     +                   ILTS,JPRJ,ELPF,INTF,LBLF,PRMF
        DOUBLE PRECISION GRDR,GRID,GRLA,GRLO,GRPO,OTOL,PHIA,PHIO,PLA1,
     +                   PLA2,PLA3,PLA4,PLB1,PLB2,PLB3,PLB4,PLTR,ROTA,
     +                   SRCH,XLOW,XROW,YBOW,YTOW
        INTEGER          IDOT,IDSH,IDTL,ILCW,ILTS,JPRJ
        LOGICAL          ELPF,INTF,LBLF,PRMF
        SAVE   /MAPCM4/
C
        COMMON /MAPCMA/  DATL,DBTD,DDTS,DPLT,DPSQ,DSCA,DSSQ
        DOUBLE PRECISION DATL,DBTD,DDTS,DPLT,DPSQ,DSCA,DSSQ
        SAVE   /MAPCMA/
C
        COMMON /MAPSAT/  ALFA,BETA,DCSA,DCSB,DSNA,DSNB,SALT,SSMO,SRSS
        DOUBLE PRECISION ALFA,BETA,DCSA,DCSB,DSNA,DSNB,SALT,SSMO,SRSS
        SAVE   /MAPSAT/
C
        IF (ICFELL('MDGETR - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
C
        IF      (WHCH(1:2).EQ.'DD'.OR.WHCH(1:2).EQ.'dd') THEN
          RVAL=REAL(DDTS)
        ELSE IF (WHCH(1:2).EQ.'GD'.OR.WHCH(1:2).EQ.'gd') THEN
          RVAL=REAL(GRDR)
        ELSE IF (WHCH(1:2).EQ.'GN'.OR.WHCH(1:2).EQ.'gn') THEN
          RVAL=REAL(GRLO)
        ELSE IF (WHCH(1:2).EQ.'GP'.OR.WHCH(1:2).EQ.'gp') THEN
          RVAL=REAL(GRPO)
        ELSE IF (WHCH(1:2).EQ.'GR'.OR.WHCH(1:2).EQ.'gr') THEN
          RVAL=REAL(GRID)
        ELSE IF (WHCH(1:2).EQ.'GT'.OR.WHCH(1:2).EQ.'gt') THEN
          RVAL=REAL(GRLA)
        ELSE IF (WHCH(1:2).EQ.'MV'.OR.WHCH(1:2).EQ.'mv') THEN
          RVAL=REAL(DPLT)
        ELSE IF (WHCH(1:2).EQ.'OT'.OR.WHCH(1:2).EQ.'ot') THEN
          RVAL=REAL(OTOL)
        ELSE IF (WHCH(1:2).EQ.'PN'.OR.WHCH(1:2).EQ.'pn') THEN
          RVAL=REAL(PHIO)
        ELSE IF (WHCH(1:2).EQ.'PT'.OR.WHCH(1:2).EQ.'pt') THEN
          RVAL=REAL(PHIA)
        ELSE IF (WHCH(1:2).EQ.'P1'.OR.WHCH(1:2).EQ.'p1') THEN
          RVAL=REAL(PLA1)
        ELSE IF (WHCH(1:2).EQ.'P2'.OR.WHCH(1:2).EQ.'p2') THEN
          RVAL=REAL(PLA2)
        ELSE IF (WHCH(1:2).EQ.'P3'.OR.WHCH(1:2).EQ.'p3') THEN
          RVAL=REAL(PLA3)
        ELSE IF (WHCH(1:2).EQ.'P4'.OR.WHCH(1:2).EQ.'p4') THEN
          RVAL=REAL(PLA4)
        ELSE IF (WHCH(1:2).EQ.'P5'.OR.WHCH(1:2).EQ.'p5') THEN
          RVAL=REAL(PLB1)
        ELSE IF (WHCH(1:2).EQ.'P6'.OR.WHCH(1:2).EQ.'p6') THEN
          RVAL=REAL(PLB2)
        ELSE IF (WHCH(1:2).EQ.'P7'.OR.WHCH(1:2).EQ.'p7') THEN
          RVAL=REAL(PLB3)
        ELSE IF (WHCH(1:2).EQ.'P8'.OR.WHCH(1:2).EQ.'p8') THEN
          RVAL=REAL(PLB4)
        ELSE IF (WHCH(1:2).EQ.'RE'.OR.WHCH(1:2).EQ.'re') THEN
          RVAL=REAL(PLTR)
        ELSE IF (WHCH(1:2).EQ.'RO'.OR.WHCH(1:2).EQ.'ro') THEN
          RVAL=REAL(ROTA)
        ELSE IF (WHCH(1:2).EQ.'SA'.OR.WHCH(1:2).EQ.'sa') THEN
          RVAL=REAL(SALT)
        ELSE IF (WHCH(1:2).EQ.'S1'.OR.WHCH(1:2).EQ.'s1') THEN
          RVAL=REAL(ALFA)
        ELSE IF (WHCH(1:2).EQ.'S2'.OR.WHCH(1:2).EQ.'s2') THEN
          RVAL=REAL(BETA)
        ELSE IF (WHCH(1:2).EQ.'SR'.OR.WHCH(1:2).EQ.'sr') THEN
          RVAL=REAL(SRCH)
        ELSE IF (WHCH(1:2).EQ.'XL'.OR.WHCH(1:2).EQ.'xl') THEN
          RVAL=REAL(XLOW)
        ELSE IF (WHCH(1:2).EQ.'XR'.OR.WHCH(1:2).EQ.'xr') THEN
          RVAL=REAL(XROW)
        ELSE IF (WHCH(1:2).EQ.'YB'.OR.WHCH(1:2).EQ.'yb') THEN
          RVAL=REAL(YBOW)
        ELSE IF (WHCH(1:2).EQ.'YT'.OR.WHCH(1:2).EQ.'yt') THEN
          RVAL=REAL(YTOW)
        ELSE
          GO TO 901
        END IF
C
C Done.
C
        RETURN
C
C Error exits.
C
  901   CALL MDPCEM ('MDGETR - UNKNOWN PARAMETER NAME ',WHCH,2,1)
        RVAL=0.
        RETURN
C
      END
