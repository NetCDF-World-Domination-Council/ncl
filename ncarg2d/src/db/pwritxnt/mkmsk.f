C
C	$Id: mkmsk.f,v 1.3 2000-08-22 14:36:37 haley Exp $
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

      SUBROUTINE MKMSK
C
C  THIS ROUTINE GENERATES THE MACHINE DEPENDENT DATA VALUES
C  USED BY XTCH.
C
C
      COMMON/PINIT/MASK(4),MASK6(64),MASK12(64),LAST6,LAST12,
     1  IBIT15,MASK14,NUM15U,NBWD
C
C  MASK FOR 15 BIT PARSALS(77777B)
C
      IMSK1 = 32767
C
C
C  FIND THE NUMBER OF BITS PER WORD
C
      NBWD = I1MACH(5)
C
C  COMPUTE THE NUMBER OF 15 BIT PARSALS
C
      NUM15U = NBWD/15
C
C  IPRCT AND IPRCT2 ARE USED TO PREVENT SIGN EXTENSION WHEN DOING LEFT
C  SHIFTS.
C
      ITEMP = ISHIFT(IMSK1,(NBWD-16))
      IPRCT = ITEMP
      IT1 = NUM15U-1
      DO 100 I = 1,IT1
      IPRCT = ISHIFT(IPRCT,-15)
      IPRCT = IOR(ITEMP,IPRCT)
  100 CONTINUE
      IPRCT = IOR(IPRCT,IMSK1)
C
      IPRCT2 = ISHIFT(IPRCT,-14)
C
C  LEFT JUSTIFY
C
      LEFTJS = NBWD-15
      IMSK1 = ISHIFT(IMSK1,LEFTJS)
C
C  GENERATE THE MASKS FOR THE 15 BIT PARSALS
C
C
      DO 10 I = 1 , NUM15U
      MASK(I) = IMSK1
C
C  SHIFT FOR NEXT PARSAL
C
      IMSK1 = ISHIFT(IMSK1,-15)
C
C  PROTECT FROM SIGN EXTENSION(REVELANT ONLY ON FIRST PASS)
C
      IMSK1 = IAND(IMSK1,IPRCT2)
   10 CONTINUE
C
C
C  MASK FOR 6 BIT PARSALS (77B)
C
      IMSK2 = 63
C
C  MASK FOR 12 BIT PARSALS (7777B)
C
      IMSK3 = 4095
C
C  LEFT JUSTIFY
C
      LEFTJS = NBWD-6
      IMSK2 = ISHIFT(IMSK2,LEFTJS)
C
      LEFTJS = NBWD-12
      IMSK3 = ISHIFT(IMSK3,LEFTJS)
C
C  SET UP ALL POSSIBLE 6 AND 12 BIT UNITS
C  NOTE THAT MASKS WILL ALSO BE USED WHEN UNITS CROSS WORD BOUNDRIES
C
C
      DO 20 I = 1 , NBWD
C
C  6 BIT MASKS
C
      MASK6(I) = IMSK2
      IMSK2 = ISHIFT(IMSK2,-1)
C
C PROTECTION FROM SIGN EXTENSION (REVELANT ONLY ON THE FIRST PASS)
C
      IMSK2 = IAND(IMSK2,IPRCT)
C
C  12 BIT MASKS
C
      MASK12(I) = IMSK3
      IMSK3 = ISHIFT(IMSK3,-1)
C
C  PROTECT FROM SIGN EXTENSION (REVELANT ONLY ON THE FIRST PASS)
C
      IMSK3 = IAND(IMSK3,IPRCT)
   20 CONTINUE
C
C
C  MASK FOR 6 BIT RIGHT JUSTIFED UNIT (77B)
C
      LAST6 = 63
C
C  MAST FOR 12 BIT RIGHT JUSTIFED UNIT (7777B)
C
      LAST12 = 4095
C
C  MASK FOR 15-TH BIT FROM THE RIGHT (40000B)
C
      IBIT15 = 16384
C
C  MASK FOR 14 BIT RIGHT JUSTIFED UNIT (37777B)
C
      MASK14 = 16383
      RETURN
      END
