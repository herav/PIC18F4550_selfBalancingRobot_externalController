clc;
clear all;
close all;
fid=fopen('angle.txt','r');
datos=fscanf(fid,'%f', [11 inf]);
fclose(fid);
t=datos(1,:);
angulo=datos(2,:);
radianes=datos(3,:);
errorPrevio=datos(4,:);
error=datos(5,:);
diferenciaError=datos(6,:);
proporcional=datos(7,:);
derivativa=datos(8,:);
integral=datos(9,:);
esfuerzoPD=datos(10,:);
velocidadDeseada=datos(11,:);
%%
figure
plot(t,proporcional);
title('Ganancia Proporcional')
xlabel('t')
ylabel('ganancia')
grid on;
%%
figure
plot(t,derivativa);
title('Ganancia Derivativa')
xlabel('t')
ylabel('ganancia')
grid on;
%%
figure
plot(t,integral);
title('Ganancia Integral')
xlabel('t')
ylabel('ganancia')
grid on;
%%
figure
plot(t,esfuerzoPD);
title('esfuerzoPD')
xlabel('t')
ylabel('ganancia')
grid on;
%%
figure
plot(t,velocidadDeseada);
title('Velocidad Deseada')
xlabel('t')
ylabel('ganancia')
grid on;
%%
figure
plot(t,angulo);
title('Angulo')
xlabel('t')
ylabel('grados')
grid on;
%%
figure
plot(t,radianes);
title('Radianes')
xlabel('t')
ylabel('rads')
grid on;
%%
figure
plot(t,error);
title('Error')
xlabel('t')
ylabel('Error')
grid on;
%%
figure
plot(t,error,'b',t,errorPrevio,'r');
title('Error vs ErrorPrevio')
xlabel('t')
ylabel('Error')
grid on;
%%
figure
plot(t,diferenciaError);
title('Diferencia Error')
xlabel('t')
ylabel('error')
grid on
%%
datosFiltrados=0;
figure
x=zeros(1,length(esfuerzoPD));
for n =1:length(esfuerzoPD)
    if(esfuerzoPD(n)<-18.85||esfuerzoPD(n)>18.85)
        x(n)=0;
    else
            x(n)=esfuerzoPD(n);
            datosFiltrados=datosFiltrados+1;
    end
end
plot(t,x)
title('Ganancia PD filtrada entre -18.85 y 18.85')
xlabel('t')
ylabel('ganancia')
grid on;
length(esfuerzoPD)
datosFiltrados
Porcentaje=datosFiltrados/length(esfuerzoPD)*100
