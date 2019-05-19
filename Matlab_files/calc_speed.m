 function [v1,v2,v3]=calc_speed(x,y,w)
  R=0.15;
  v1=-sin(0.52359878)*x+cos(0.52359878)*y+R*w;
  v2=-sin(2.61799388)*x+cos(2.61799388)*y+R*w; 
  v3=-sin(4.71238898)*x+R*w;
 end
  
 