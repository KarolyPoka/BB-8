clc
clear all
Kp=0.77818;
Tp=3.1315;
Ti=Tp;
Wp=tf(Kp,[Tp 1])
Wpi=tf([Ti 1],[Ti 0])
Ap=250/188;
%Wpi=Ap*Wpi;
Wpi=minreal(Wpi)
W0=Wpi*Wp;
wcl_pi=feedback(W0,tf(1,1),-1)
ltiview(wcl_pi)
c2d(Wpi, 0.02,'zoh')
bode(wcl_pi)

