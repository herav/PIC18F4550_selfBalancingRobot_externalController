clc;
clear all;
close all;
fid=fopen('selfBalancingRobot_stats.txt','r');
datos=fscanf(fid,'%f', [12 inf]);
fclose(fid);
t=datos(1,:);
phiD_rads=datos(2,:);
phiD_grados=datos(3,:);
phi_rads=datos(4,:);
phi_grados=datos(5,:);
wdd=datos(6,:);%velocidad deseada Motor1
wd=datos(7,:);%velocidad Motor1
ud=datos(8,:);%voltaje Motor1
wid=datos(9,:);%velocidad deseada Motor2
wi=datos(10,:);%velocidad Motor2
ui=datos(11,:);%voltaje Motor2
timer=datos(12,:);
%%
subplot(3,2,[1,2])
plot(t,phiD_grados,'r',t,phi_grados,'b')
title('Angulo de inclinacion')

subplot(3,2,3)
plot(t,wdd,'r',t,wd,'b')
title('Velocidad Motor 1')

subplot(3,2,4)
plot(t,wid,'r',t,wi,'b')
title('Velocidad Motor 2')

subplot(3,2,5)
plot(t,ud)
title('Voltaje Motor 1')

subplot(3,2,6)
plot(t,ui)
title('Voltaje Motor 2')
%%
figure
plot(t,timer);
title('Duraci�n Trama de Datos')
xlabel('t')
ylabel('timer')
grid on;
%%
subplot(3,2,[1,2])
plot(t,phiD_rads,'r',t,phi_rads,'b')

subplot(3,2,3)
plot(t,wdd,'r',t,wd,'b')
title('Velocidad Motor 1')

subplot(3,2,4)
plot(t,wid,'r',t,wi,'b')
title('Velocidad Motor 2')

subplot(3,2,5)
plot(t,ud)
title('Voltaje Motor 1')

subplot(3,2,6)
plot(t,ui)
title('Voltaje Motor 2')
