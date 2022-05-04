clc;
clear all;
close all;
fid=fopen('motor2.txt','r');
datos=fscanf(fid,'%f', [12 inf]);
fclose(fid);
t=datos(1,:);
cuentas=datos(2,:);
diferenciaPosicion=datos(3,:);
velocidadDeseada=datos(4,:);
velocidad=datos(5,:);
errorPrevio=datos(6,:);
error=datos(7,:);
proporcional=datos(8,:);
integral=datos(9,:);
esfuerzoPI=datos(10,:);
voltaje=datos(11,:);
pwm=datos(12,:);
%%
figure
plot(t,velocidad,'b',t,velocidadDeseada,'r');
title('velocidad vs velocidad Deseada')
xlabel('t')
ylabel('posicion')
grid on;
%%
figure
plot(t,esfuerzoPI);
title('Esfuerzo PI')
xlabel('t')
ylabel('voltaje')
grid on;
%%
figure
plot(t,voltaje);
title('Voltaje')
xlabel('t')
ylabel('voltaje')
grid on;
%%
figure
plot(t,cuentas);
title('Cuentas')
xlabel('t')
ylabel('cuentas')
grid on;
%%
figure
plot(t,diferenciaPosicion);
title('Diferencia de Posicion')
xlabel('t')
ylabel('posicion')
grid on;
%%
figure
plot(t,velocidad,'b');
title('velocidad')
xlabel('t')
ylabel('posicion')
grid on;
%%
figure
plot(t,error);
title('Error')
xlabel('t')
ylabel('error')
grid on;
%%
figure
plot(t,error,'b',t,errorPrevio,'r');
title('Error vs error Previo')
xlabel('t')
ylabel('error')
grid on;
%%
figure
plot(t,proporcional);
title('Ganancia Proporcional')
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
plot(t,pwm);
title('PWM')
xlabel('t')
ylabel('pwm')
grid on;
%%
datosFiltrados=0;
figure
x=zeros(1,length(esfuerzoPI));
for n =1:length(esfuerzoPI)
    if(esfuerzoPI(n)<-6||esfuerzoPI(n)>6)
        x(n)=0;
    else
        x(n)=esfuerzoPI(n);
        datosFiltrados=datosFiltrados+1;
    end
end
plot(t,x)
title('Voltaje filtrado entre -6 y 6')
xlabel('t')
ylabel('voltaje')
grid on;
length(esfuerzoPI)
datosFiltrados
Porcentaje=datosFiltrados/length(esfuerzoPI)*100
