C
C       $Id: cssex01.f,v 1.4 1999-10-22 16:46:04 fred Exp $
C

      PROGRAM CSEX01
C
C  Example of Delaunay triangulation and Voronoi diagram
C  on the surface of a sphere.
C
C  Define error file, Fortran unit number, and workstation type,
C  and workstation ID.
C
      PARAMETER (IERRF=6, LUNIT=2, IWTYPE=1, IWKID=1)
C
C  Input dataset on the globe (latitudes and longitudes in degrees).
C  These data points do not cover the globe, but rather are confined
C  to the nothern hemisphere and are enclosed by a boundary.
C
      PARAMETER (NMAX=7)
      DIMENSION RLAT(NMAX),RLON(NMAX)
      DATA RLAT/  70.,  70., 70., 85., 60., 60., 65./
      DATA RLON/-160., -70.,  0., 20., 50., 80.,140./
C
C  Dimension the arrays for holding the points for drawing 
C  circular arcs.
C
      PARAMETER (NARC=50)
      DIMENSION ARCLAT(NARC),ARCLON(NARC)
      DIMENSION ARCUAR(NARC),ARCVAR(NARC)
C
C  Storage for the triangulation, work space, and Voronoi vertices.
C
      PARAMETER (NTMX=2*NMAX, NT6=6*NMAX, LWK=27*NMAX)
      INTEGER LTRI(3,NTMX), IWK(LWK), NV(NMAX)
C
C  Storage for Cartesian coordinates for original data.
C
      REAL    X(NMAX),    Y(NMAX),   Z(NMAX)
C
C  Real workspace.
C
      REAL    RWK(4*NMAX)
C
C  Storage for circumcenters and circum radii.
C
      REAL    XC(NTMX),    YC(NTMX),   ZC(NTMX),    RC(NTMX)
C
C  Constants for converting between radians and degrees.
C
      PARAMETER(D2R=0.017453293, R2D=57.29578)
C
      N = NMAX
C
C  Set X and Y to the values of RLON and RLAT, respectively,
C  in radians.
C
      DO 2 K = 1,N
        X(K) = D2R*RLON(K)
        Y(K) = D2R*RLAT(K)
    2   CONTINUE
C
C  Transform the spherical coordinates X and Y to Cartesian
C  coordinates (X,Y,Z) on the unit sphere (X**2 +  Y**2 + Z**2 = 1).
C
      CALL CSTRANS (N,Y,X, X,Y,Z)
C
C  Create the triangulation, storing the vertices in LTRI.
C
      CALL CSSTRI (N,X,Y,Z, NT,LTRI, IWK,RWK,IER)
C
C  Plot the Delaunay triangulation, the circumcircles, and
C  the Voronoi polygons on a sphere.
C
C  Open GKS.
C
      CALL GOPKS (IERRF, ISZDM)
      CALL GOPWK (IWKID, LUNIT, IWTYPE)
      CALL GACWK (IWKID)
C
C  Color table.
C
      CALL GSCR(IWKID,0,0.,0.,0.)
      CALL GSCR(IWKID,1,1.,1.,1.)
      CALL GSCR(IWKID,2,0.,1.,1.)
      CALL GSCR(IWKID,3,1.,1.,0.)
      CALL GSCR(IWKID,4,1.,0.,0.)
      CALL GSCR(IWKID,5,0.,1.,0.)
      CALL GSCR(IWKID,6,0.,.8,0.)
C
C Draw a map of the North Atlantic, as seen by a satellite.
C
      CALL GSPLCI(6)
      CALL MAPSTR ('SA',4.)
      CALL MAPPOS(0.175, 0.975, 0.025, 0.825)
      CALL SUPMAP (7,72.5,127.5,0.,0.,0.,0.,0.,1,-1000,5,0,IERR)
C
C  Get the circumcenters of the Delaunay triangles.
C
      CALL CSVORO(N,X,Y,Z,1,1,IWK,RWK,NTMX,XC,YC,ZC,RC,
     +            NCA,NUMV,NV,IER)
C
C  Plot the circumcircles whose circumcenters lie in one of
C  the Delaunay triangles composed of original data points
C  (exclude the pseudo points added to complete the
C  triangulation on the whole sphere).
C
      CALL GSLWSC(2.0)
      CALL GSPLCI(4)
      DO 940 I=5,10
C
C  CSSCOORD returns TLAT and TLON in radians.  TLAT is between
C  -PI/2. and PI/2. and TLON is between -PI and PI.
C
        CALL CSSCOORD(XC(I),YC(I),ZC(I),TLAT,TLON,PNM)
C
C  Convert TLAT, TLON and the circumradius to degrees and plot.
C
        TLATD = R2D*TLAT
        TLOND = R2D*TLON
        RCD   = R2D*RC(I)
C
        CALL NGGCOG(TLATD,TLOND,RCD,ARCLAT,ARCLON,NARC)
        CALL MAPIT(ARCLAT(1),ARCLON(1),0)
        DO 930 J=2,NARC-1
          CALL MAPIT(ARCLAT(J),ARCLON(J),1)
  930   CONTINUE
        CALL MAPIT(ARCLAT(NARC),ARCLON(NARC),1)
        CALL MAPIQ
  940 CONTINUE
C
C  Draw the Voronoi polygons.
C
      DO 150 I=1,N
C
C  Get the polygon containing the original data point
C  (X(I),Y(I),Z(I)).  
C
        CALL CSVORO(N,X,Y,Z,I,0,IWK,RWK,NTMX,XC,YC,ZC,RC,
     +              NCA,NUMV,NV,IER)
        DO 855 NN=2,NUMV
C
C  Convert the Cartesian coordinates of the Voronoi polygonal
C  edges to degrees latitiude and longitude.
C
          CALL CSSCOORD(XC(NV(NN-1)),YC(NV(NN-1)),ZC(NV(NN-1)),
     +                  RLAT1,RLON1,PNM)
          CALL CSSCOORD(XC(NV(NN)),YC(NV(NN)),ZC(NV(NN)),
     +                  RLAT2,RLON2,PNM)
C
C  Convert latitudes and longitudes to radians.
C
          RLAT1 = R2D*RLAT1
          RLAT2 = R2D*RLAT2
          RLON1 = R2D*RLON1
          RLON2 = R2D*RLON2
C
C  Plot.
C
          CALL GSPLCI(3)
          CALL MAPGCI(RLAT1,RLON1,RLAT2,RLON2,NARC,ARCLAT,ARCLON)
          CALL MAPIT(RLAT1,RLON1,0)
          DO 920 J=1,NARC
            CALL MAPIT(ARCLAT(J),ARCLON(J),1)
  920     CONTINUE
          CALL MAPIT(RLAT2,RLON2,1)
          CALL MAPIQ
  855   CONTINUE
  150 CONTINUE
C
C  Draw the Delaunay triangles.
C
      CALL GSPLCI(2)
      DO 130 NP=1,NT
        DO 140 NS=1,3
          CALL MAPGCI(RLAT(LTRI(NS,NP)),RLON(LTRI(NS,NP)),
     +                RLAT(LTRI(MOD(NS,3)+1,NP)),
     +                RLON(LTRI(MOD(NS,3)+1,NP)),
     +                NARC,ARCLAT,ARCLON)
          CALL MAPIT(RLAT(LTRI(NS,NP)),RLON(LTRI(NS,NP)),0)
          DO 120 I=1,NARC
            CALL MAPIT(ARCLAT(I),ARCLON(I),1)
  120     CONTINUE
          CALL MAPIT(RLAT(LTRI(MOD(NS,3)+1,NP)),
     +               RLON(LTRI(MOD(NS,3)+1,NP)),1)
  140   CONTINUE
  130 CONTINUE
      CALL MAPIQ
C
C  Mark the original data points.
C
      CALL GSFACI(3)
      DO 400 I=1,N
        CALL NGGCOG(RLAT(I),RLON(I),1.2,ARCLAT,ARCLON,NARC)
        DO 410 L=1,NARC
          CALL MAPTRN(ARCLAT(L),ARCLON(L),ARCUAR(L),ARCVAR(L))
  410   CONTINUE
        CALL GFA(NARC,ARCUAR,ARCVAR)
  400 CONTINUE
C
C  Mark the circumcenters.
C
      CALL GSFACI(5)
      DO 440 I=5,10
        CALL CSSCOORD(XC(I),YC(I),ZC(I),RLAT2,RLON2,PNM)
        RLATD = R2D*RLAT2
        RLOND = R2D*RLON2
        CALL NGGCOG(RLATD,RLOND,0.6,ARCLAT,ARCLON,NARC)
        DO 420 L=1,NARC
          CALL MAPTRN(ARCLAT(L),ARCLON(L),ARCUAR(L),ARCVAR(L))
  420   CONTINUE
        CALL GFA(NARC,ARCUAR,ARCVAR)
  440 CONTINUE
C
C  Put out a legend.
C
      CALL SET(0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1)
C
      CALL CSDRBX(0.02, 0.945, 0.12, 0.955, 2)
      CALL PCSETI('CC',1)
      CALL PLCHHQ(0.14, 0.95, ':F22:Delaunay triangles', 
     +            0.025, 0.0, -1.0)     
C
      CALL CSDRBX(0.02, 0.895, 0.12, 0.905, 3)
      CALL PLCHHQ(0.14, 0.90, ':F22:Voronoi polygons', 0.025, 0.0, -1.0)       
C
      CALL CSDRBX(0.02, 0.845, 0.12, 0.855, 4)
      CALL PLCHHQ(0.14, 0.85, ':F22:Circumcircles', 0.025, 0.0, -1.0)
C
      CALL FRAME
C
C  Close GKS.
C
      CALL GDAWK (IWKID)
      CALL GCLWK (IWKID)
      CALL GCLKS
C
      STOP
      END
      SUBROUTINE CSDRBX(XLL,YLL,XUR,YUR,ICNDX)
C
C  Draw a filled rectangle with the specified corner points 
C  using color index ICNDX.
C
      DIMENSION X(5),Y(5)
C
      CALL GQFACI(IER,ICOLD)
C
      X(1) = XLL
      Y(1) = YLL
      X(2) = XUR
      Y(2) = YLL
      X(3) = XUR
      Y(3) = YUR
      X(4) = XLL
      Y(4) = YUR
      X(5) = XLL
      Y(5) = YLL
C 
      CALL GSFACI(ICNDX)
      CALL GFA(5,X,Y)
      CALL GSFACI(ICOLD)
C
      RETURN
      END
