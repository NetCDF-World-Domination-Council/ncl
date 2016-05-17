;-----------------------------------------------------------------
;  NCL User Guide Example:  NUG_grid_resolution_comparison.ncl
; 
;  KMF 30.10.14
;-----------------------------------------------------------------
load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/gsn_code.ncl"
load "$NCARG_ROOT/lib/ncarg/nclscripts/csm/gsn_csm.ncl"

undef("file_ERROR")
procedure file_ERROR(diri,names[*]:string)
begin
   files_exist = fileexists(diri+names)
   if(any(.not.files_exist)) then
     ii = ind(.not.files_exist)
     print("")
     print("You don't have the necessary data file(s) for this script. You can download them from:​")
     print("")
     print("http://www.ncl.ucar.edu/Document/Manuals/NCL_User_Guide/Data/"+names(ii))
     print("")
     print("or use the wget command:")
     print("")
     print("wget http://www.ncl.ucar.edu/Document/Manuals/NCL_User_Guide/Data/"+names(ii))
     print("")
     exit
  end if
end

begin

  lon0    =   0.0
  lon1    =  53.0
  lat0    =   0.0
  lat1    =  38.0
  border1 =  10.0
  border2 =  20.0

  diri = "$NCARG_ROOT/lib/ncarg/data/nug/"
  fil1 = "orog_mod1_rectilinear_grid_2D.nc"
  fil2 = "orog_mod2_rectilinear_grid_2D.nc"
  fil3 = "orog_mod3_rectilinear_grid_2D.nc"

  msk1 = "sftlf_mod1_rectilinear_grid_2D.nc"
  msk2 = "sftlf_mod2_rectilinear_grid_2D.nc"
  msk3 = "sftlf_mod3_rectilinear_grid_2D.nc"  

  file_ERROR(diri,(/fil1,fil2,fil3,msk1,msk2,msk3/))

  file_1    = addfile(diri+fil1, "r")
  variable1 = file_1->orog(:,:)

  mask_1 = addfile(diri+msk1,"r")
  lsm1   = mask_1->sftlf(:,:)
  lsm1   = lsm1/100                 ;-- do this cause data in percent
  lsm1   = where(lsm1.gt.0.5,-9999,lsm1)

;-- mask data over ocean 
  land_only1  = variable1
  land_only1  = mask(variable1,lsm1,-9999)

;-- second file
  file_2     = addfile(diri+fil2, "r")
  variable2  = file_2->orog(0,{lat0:lat1-border1},{lon0+border1:lon1})
  variable2&rlat@units = "degrees_north"
  variable2&rlon@units = "degrees_east"

  mask_2 = addfile(diri+msk2,"r")
  lsm2   = mask_2->sftlf(0,{lat0:lat1-border1},{lon0+border1:lon1})
  lsm2   = where(lsm2.gt.0.5,-9999,lsm2)

;-- mask data over ocean 
  land_only2  = variable2
  land_only2  = mask(variable2,lsm2,-9999)

;-- third file
  file_3     = addfile(diri+fil3, "r")
  variable3  = file_3->orog(0,{lat0:lat1-border2},{lon0+border2:lon1})
  variable3&rlat@units = "degrees_north"
  variable3&rlon@units = "degrees_east"

  mask_3 = addfile(diri+msk3,"r")
  lsm3   = mask_3->sftlf(0,{lat0:lat1-border2},{lon0+border2:lon1})
  lsm3   = where(lsm3.gt.0.5,-9999,lsm3)

;-- mask data over ocean 
  land_only3  = variable3
  land_only3  = mask(variable3,lsm3,-9999)

;-- open workstation
  wks                    = gsn_open_wks("png","plot_grid_resolution_comparison")
  gsn_define_colormap(wks,"OceanLakeLandSnow")

;-- define the global resources for all plots
  cnres                       =  True     ; plot mods desired
  cnres@gsnDraw               =  False
  cnres@gsnFrame              =  False
  cnres@gsnMaximize           =  True     ; Maximize plot in frame
  cnres@gsnAddCyclic          =  False    ; Don't add a cyclic point
  cnres@gsnLeftString         = ""
  cnres@gsnCenterString       = ""
  cnres@gsnRightStringFontHeightF  = -0.02
  cnres@gsnRightStringParallelPosF = 1.1       ; move the RightString slightly right
  cnres@cnInfoLabelOn         =  False
  cnres@cnLinesOn             =  False
  cnres@cnLineLabelsOn        =  False
  cnres@cnLevelSelectionMode  = "ManualLevels"  ; Set contour levels manually
  cnres@cnMinLevelValF        =  0.             ; Minimum contour level
  cnres@cnMaxLevelValF        =  3000.          ; Maximum contour level
  cnres@cnLevelSpacingF       =  20             ; Contour level spacing
  cnres@cnFillOn              =  True           ; Turn on contour fill
  cnres@cnFillMode            = "RasterFill"
  cnres@cnMissingValFillColor = "white"
  cnres@gsnSpreadColors       =  True
  cnres@gsnSpreadColorStart   =   2
  cnres@gsnSpreadColorEnd     = -26
  cnres@lbLabelBarOn          =  True
  cnres@lbLabelStride         =  10
  cnres@lbOrientation         = "Vertical"
  cnres@tiMainOn              =  False

;-- resources first plot (map)
  res = cnres
  res@mpProjection            = "CylindricalEquidistant"
  res@mpLimitMode             = "Corners"
  res@mpLeftCornerLonF        =  lon0
  res@mpRightCornerLonF       =  lon1
  res@mpLeftCornerLatF        =  lat0
  res@mpRightCornerLatF       =  lat1
  res@mpGridAndLimbOn         =  False            ; turn on grid lines
  res@mpDataBaseVersion       = "MediumRes"
  res@pmTickMarkDisplayMode   = "Always"       ; turn on tickmarks
  res@lbLabelBarOn            =  True
  res@lbBoxLinesOn            =  False          ; Turn off labelbar box lines
  res@gsnAddCyclic            =  True

  map = gsn_csm_contour_map(wks, land_only1, res)

;-- second plot
  res2 = cnres
  res2@gsnMaximize            =  False
  res2@gsnRightString         = ""

  plot2 = gsn_csm_contour(wks, land_only2, res2)

;-- third plot
  res3 = cnres
  res3@gsnMaximize            =  False
  res3@gsnRightString         = ""

  plot3 = gsn_csm_contour(wks, land_only3, res3)

;-- overlay the two data sets
  overlay(map,plot2)

;-- draw a shaded box around plot2 ( means draw two boxes with different line width
  shres2                      =  True
  shres2@gsLineThicknessF     =  7.0
  shres2@gsLineColor          = "gray65"

  dx = 0.15
  dy = 0.25

  shypts2 = (/ lat0+dy, lat1-border1+dy, lat1-border1+dy,  lat1-border1+dy,  lat0+dy/)
  shxpts2 = (/ lon0+border1-dx, lon0+border1-dx, lon1-dx, lon0+border1-dx,  lon0+border1-dx/)

  shdum2 = new(4,graphic)  
  do i = 0 , 3
    shdum2(i)=gsn_add_polyline(wks,map,shxpts2(i:i+1),shypts2(i:i+1),shres2)      
  end do

;-- draw a box around plot2
  lnres2                      =  True
  lnres2@gsLineThicknessF     =  1.0
  lnres2@gsLineColor          = "Black"

  ypts2 = (/ lat0,           lat0,  lat1-border1,  lat1-border1,  lat0/)
  xpts2 = (/ lon0+border1,   lon1,  lon1,          lon0+border1,  lon0+border1/)

  dum2 = new(4,graphic)  
  do i = 0 , 3
    dum2(i)=gsn_add_polyline(wks,map,xpts2(i:i+1),ypts2(i:i+1),lnres2)      
  end do

;-- overlay plot3
  overlay(map,plot3)

;-- draw a shaded box around plot3 ( means draw two boxes with different line width
  shres3                      =  True
  shres3@gsLineThicknessF     =  7.0
  shres3@gsLineColor          = "gray65"

  dx = 0.15
  dy = 0.25

  shypts3 = (/ lat0+dy, lat1-border2+dy, lat1-border2+dy,  lat1-border2+dy,  lat0+dy/)
  shxpts3 = (/ lon0+border2-dx, lon0+border2-dx, lon1-dx, lon0+border2-dx,  lon0+border2-dx/)

  shdum3 = new(4,graphic)  
  do i = 0 , 3
    shdum3(i)=gsn_add_polyline(wks,map,shxpts3(i:i+1),shypts3(i:i+1),shres3)      
  end do

;-- draw a box around plot3
  lnres3                      =  True
  lnres3@gsLineThicknessF     =  1.0
  lnres3@gsLineColor          = "Black"

  ypts3 = (/ lat0,           lat0,  lat1-border2,  lat1-border2,  lat0/)
  xpts3 = (/ lon0+border2,   lon1,  lon1,          lon0+border2,  lon0+border2/)

  dum3 = new(4,graphic)  
  do i = 0 , 3
    dum3(i)=gsn_add_polyline(wks,map,xpts3(i:i+1),ypts3(i:i+1),lnres3)      
  end do

;--drawing the map will draw contours and vectors too.
  draw(map)

;-- draw text on plot (map)
  txres                       =  True
  txres@txFontHeightF         =  0.02
  txres@txFontColor           = "Black"
  txres@txBackgroundFillColor = "White"
  txres@txPerimOn             =  True

  gsn_text_ndc(wks,"1.875 ~S~o~N~ GCM", 0.200, 0.730, txres) 
  gsn_text_ndc(wks,"0.440 ~S~o~N~ RCM", 0.330, 0.600, txres) 
  gsn_text_ndc(wks,"0.220 ~S~o~N~ RCM", 0.470, 0.460, txres) 

  frame(wks)
end