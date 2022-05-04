#include <windows.h>
#include <stdio.h>
#include <math.h>
using namespace std;

const float anguloDeseado_grados= -0.2;//5;
#define Ts 0.002//Tiempo de muestreo
#define VNM 6//voltaje nominal minimo
#define VM 7.5//voltaje máximo
#define VelNM 18.0956//aprox 2.88 Vueltas/s

#define kpM 1000//700-1000
#define kdM 70
#define kiM 0

#define kp1 .018
#define ki1 7

#define kp2 .018
#define ki2 7

#define pi 3.141593
#define CPR 1496//Cuentas por Revolucion = 11*4*34=1496

int etapa=0;
float t=0.0;
float CPR_toRads=2*pi/CPR;//escalamiento de entrada
float Voltage_toPWM=127/VM;//escalamiento de salida

unsigned char timer;
unsigned char acelerometroX;
unsigned char acelerometroY;
unsigned char acelerometroZ;
unsigned char giroscopioX;
float Xa;
float Ya;
float Za;
float Xg;
float aceleracionX;

float anguloDeseado_rads=anguloDeseado_grados*3.141593/180;//angulo deseado en radianes
float anguloPrevio=anguloDeseado_grados;
float angulo=0;//angulo de entrada en grados
float radianes=0;//angulo de entrada en radianes
float errorM_previo=0;//error anterior angulo
float errorM=0;//error actual angulo
float diferenciaErrorM=0;
float proporcionalM=0;
float derivativaM=0;
float integralM=0;
float esfuerzoPD=0;

float diferenciaPosicion1=0;
float signedCuentasM1;
float velocidad1=0;
float velocidad1_deseado=0;//suma de P+I+D del maestro
float error1=0;
float error1_previo=0;
float proporcional1=0;
float integral1=0;
float voltaje1=0;
float esfuerzoPI1=0;

float diferenciaPosicion2=0;
float signedCuentasM2;
float velocidad2=0;
float velocidad2_deseado=0;//suma de P+I+D del maestro
float error2=0;
float error2_previo=0;
float proporcional2=0;
float integral2=0;
float voltaje2=0;
float esfuerzoPI2=0;

unsigned char pwm1=0;
unsigned char pwm2=0;
unsigned char cuentasM1=0;
unsigned char cuentasM2=0;

HANDLE h; /*handler, sera el descriptor del puerto*/
DCB dcb; /*estructura de configuracion*/
FILE *balancin_txt;
FILE *angulo_txt;
FILE *motor1_txt;
FILE *motor2_txt;
DWORD n;
unsigned char fromPIC;

int main(){
  if((balancin_txt=fopen("selfBalancingRobot_stats.txt","w+"))==NULL){
	  printf("No se puede abrir el archivo.\n");
	  exit(1);
	}
  if((angulo_txt=fopen("angle.txt","w+"))==NULL){
	  printf("No se puede abrir el archivo.\n");
	  exit(1);
	}
	if((motor1_txt=fopen("motor1.txt","w+"))==NULL){
	  printf("No se puede abrir el archivo.\n");
	  exit(1);
	}
	if((motor2_txt=fopen("motor2.txt","w+"))==NULL){
	  printf("No se puede abrir el archivo.\n");
	  exit(1);
	}
  h=CreateFile("COM4",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);/*abrimos el puerto*/
  if(h == INVALID_HANDLE_VALUE){/*ocurrio un error al intentar abrir el puerto*/}
	if(!GetCommState(h, &dcb)){printf("Error: no se puede obtener la configuracion");}

	dcb.BaudRate =115200;
  dcb.ByteSize = 8;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  dcb.fBinary = TRUE;
  dcb.fParity = TRUE;
  if(!SetCommState(h, &dcb)){printf("Error al configurar el puerto o dispositivo no conectado\n\n");}

  printf("En espera de vinculacion con Balancin...\n\n");
  do ReadFile(h, &fromPIC, 1, &n, NULL);
  while(fromPIC!=170);
  printf("Balancin vinculado correctamente\n");

  do{
    ReadFile(h, &fromPIC, 1, &n, NULL);
    switch (etapa) {
      case 0:{//Recibe parte baja acelerometroX
        acelerometroX=fromPIC;
        break;
      }
      case 1:{//Recibe parte baja acelerometroY
        acelerometroY=fromPIC;
        break;
      }
      case 2:{//Recibe parte baja acelerometroZ
        acelerometroZ=fromPIC;
        break;
      }
      case 3:{//Recibe parte baja giroscopioX
        giroscopioX=fromPIC;
        break;
      }
      case 4:{//Recibe cuentas motor1
        cuentasM1=fromPIC;
        break;
      }
      case 5:{//Recibe cuentas motor 2
        cuentasM2=fromPIC;
        break;
      }
      case 6:{
        timer=fromPIC;

        Xa=(signed char)acelerometroX;
        Ya=(signed char)acelerometroY;
        Za=(signed char)acelerometroZ;
        Xg=(signed char)giroscopioX;

        aceleracionX=atan(Xa/sqrt(pow(Ya,2)+pow(Za,2)))*180/3.141593;//grados
        angulo=0.995*(anguloPrevio+(Xg/131)*Ts)+0.005*aceleracionX;//{0.995,0.005}
        anguloPrevio=angulo;

        printf("%.3f\t%.3f\t%u\n",t,angulo,timer);
        radianes=angulo*pi/180;
        errorM=-1*(anguloDeseado_rads-radianes);//error en radianes
        proporcionalM=kpM*errorM;
        diferenciaErrorM=errorM-errorM_previo;
        derivativaM=kdM*diferenciaErrorM/Ts;
        if(integralM<VelNM&&integralM>-VelNM)integralM=integralM+kiM*Ts*errorM;
        else{
          if(integralM>VelNM)integralM=0.95*VelNM;
          else integralM=-0.95*VelNM;
        }

        esfuerzoPD=proporcionalM+derivativaM+integralM;
        if(esfuerzoPD<-VelNM)velocidad1_deseado=-VelNM;
        else{
          if(esfuerzoPD>VelNM)velocidad1_deseado=VelNM;
          else velocidad1_deseado=esfuerzoPD;
        }
        velocidad2_deseado=velocidad1_deseado;
        velocidad1_deseado=6.28*sin(6.28*t);//////////////////////////////////////////////velocidad1 FORZADO
        velocidad2_deseado=6.28*sin(6.28*t);//////////////////////////////////////////////velocidad2 FORZADO
        fprintf(angulo_txt,"%.3f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\n",t,angulo,radianes,errorM_previo,errorM,diferenciaErrorM,proporcionalM,derivativaM,integralM,esfuerzoPD,velocidad1_deseado);
        errorM_previo=errorM;

        signedCuentasM1=(signed char)cuentasM1;
        diferenciaPosicion1=signedCuentasM1*CPR_toRads;
        if(abs(diferenciaPosicion1)<60)velocidad1=diferenciaPosicion1/Ts;
        error1=velocidad1_deseado-velocidad1;
        proporcional1=kp1*error1;
        //integral1=integral1+ki1*Ts*error1;///////////////////////////////////////////////////////NO Windup
        if(integral1<VNM&&integral1>-VNM)integral1=integral1+ki1*Ts*error1;
        else{
          if(integral1>=VNM)integral1=0.95*VNM;
          if(integral1<=-VNM)integral1=-0.95*VNM;
        }
        esfuerzoPI1=proporcional1+integral1;
        if(esfuerzoPI1<-VNM)voltaje1=-VNM;
        else{
          if(esfuerzoPI1>VNM)voltaje1=VNM;
          else voltaje1=esfuerzoPI1;
        }
        //voltaje1=0;////////////////////////////////////////////////////////////////////voltaje1 FORZADO
        pwm1=(unsigned char)fabs(voltaje1*Voltage_toPWM);
        if(voltaje1<0)pwm1=pwm1+128;
        if(!WriteFile(h,&pwm1,1,&n,NULL)){}
        fprintf(motor1_txt,"%.3f\t%.1f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%u\n",t,signedCuentasM1,diferenciaPosicion1,velocidad1_deseado,velocidad1,error1_previo,error1,proporcional1,integral1,esfuerzoPI1,voltaje1,pwm1);
        error1_previo=error1;

        signedCuentasM2=(signed char)cuentasM2;
        diferenciaPosicion2=signedCuentasM2*CPR_toRads;
        if(abs(diferenciaPosicion2)<60)velocidad2=diferenciaPosicion2/Ts;
        error2=velocidad2_deseado-velocidad2;
        proporcional2=kp2*error2;
        //integral2=integral2+ki2*Ts*error2;///////////////////////////////////////////////////////NO Windup
        if(integral2<VNM&&integral2>-VNM)integral2=integral2+ki2*Ts*error2;
        else{
          if(integral2>=VNM)integral2=0.95*VNM;
          if(integral2<=-VNM)integral2=-0.95*VNM;
        }
        esfuerzoPI2=proporcional2+integral2;
        if(esfuerzoPI2<-VNM)voltaje2=-VNM;
        else{
          if(esfuerzoPI2>VNM)voltaje2=VNM;
          else voltaje2=esfuerzoPI2;
        }
        //voltaje2=0;////////////////////////////////////////////////////////////////////voltaje2 FORZADO
        pwm2=(unsigned char)fabs(voltaje2*Voltage_toPWM);
        if(voltaje2<0)pwm2=pwm2+128;
        if(!WriteFile(h,&pwm2,1,&n,NULL)){}
        fprintf(motor2_txt,"%.3f\t%.1f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%u\n",t,signedCuentasM2,diferenciaPosicion2,velocidad2_deseado,velocidad2,error2_previo,error2,proporcional2,integral2,esfuerzoPI2,voltaje2,pwm2);
        error2_previo=error2;

        fprintf(balancin_txt,"%.3f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%.5f\t%u\n",t,anguloDeseado_rads,anguloDeseado_grados,radianes,angulo,velocidad1_deseado,velocidad1,voltaje1,velocidad2_deseado,velocidad2,voltaje2,timer);
        t=t+Ts;
        break;
      }//etapa 6
    }//termina switch
    if(etapa==6)etapa=0;
    else etapa++;
  }//termina do
  while(n);
  fclose(balancin_txt);
	fclose(angulo_txt);
	fclose(motor1_txt);
	fclose(motor2_txt);
  return 0;
}//termina main
