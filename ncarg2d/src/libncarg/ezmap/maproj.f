C
C $Id: maproj.f,v 1.15 2001-11-02 22:36:44 kennison Exp $
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
      SUBROUTINE MAPROJ (ARG1,ARG2,ARG3,ARG4)
        CHARACTER*(*) ARG1
        REAL          ARG2,ARG3,ARG4
        IF (ICFELL('MAPROJ - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
        CALL MDPROJ (ARG1,DBLE(ARG2),DBLE(ARG3),DBLE(ARG4))
        IF (ICFELL('MAPROJ',2).NE.0) RETURN
        RETURN
      END
