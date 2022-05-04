#include<18f4550.h>
#fuses HS,NOWDT,NOPROTECT,NOMCLR
#use delay(clock=20M,crystal=20M)
#use I2C(master, sda=PIN_B0, scl=PIN_B1, fast)
#use rs232(baud=115200,XMIT=PIN_C6,RCV=PIN_C7,BITS=8,PARITY=N)
#priority int_rb,int_rda
#include <MPU6050.c>

#byte porta = 0xf80
#byte portb = 0xf81
#byte TMR0L = 0xfd6//TMR0L
#byte T0CON = 0xfd5//T0CON

#bit M1a = 0xf80.0//IN1
#bit M1b = 0xf80.1//IN2
#bit M2a = 0xf80.2//IN3
#bit M2b = 0xf80.3//IN4

int16 tope=40;//10=196,5=98,4=79,3=59,2=40,1=20
int8 pwm1;
int8 pwm2;

int1 etapaSerial=1;
int1 pwm1_ON=0;
int1 pwm2_ON=0;

volatile int8 aux;

volatile int8 encoderM1=0;
volatile int8 encoderM1_anterior=0;
volatile int8 cuentasM1=0;

volatile int8 encoderM2=0;
volatile int8 encoderM2_anterior=0;
volatile int8 cuentasM2=0;

#int_rb
void rb_isr(){
   encoderM1=(PORTB&0b00110000)>>4;//0x30=00110000
   aux=encoderM1^encoderM1_anterior;
   if(aux!=0&&aux!=0b00000011)//3=011
      if(((encoderM1_anterior<<1)^encoderM1)&0b00000010)//0x02=10
         cuentasM1--;
      else
         cuentasM1++;
   encoderM1_anterior=encoderM1;

   encoderM2=(PORTB&0b11000000)>>6;//0xC0=1100 0000
   aux=encoderM2^encoderM2_anterior;
   if(aux!=0&&aux!=0b00000011)//3=011
         if(((encoderM2_anterior<<1)^encoderM2)&0b00000010)//0x02=10
            cuentasM2++;
         else
            cuentasM2--;
   encoderM2_anterior=encoderM2;
}

#int_rda
void rda(){
   if(etapaSerial){
      pwm1=getc();
      pwm1_ON=1;
   }
   else {
      pwm2=getc();
      pwm2_ON=1;
   }
   etapaSerial++;
}

void main(){
   set_tris_a(0b11110000);
   set_tris_b(0b11111111);

   enable_interrupts(int_rb);
   enable_interrupts(int_rda);
   enable_interrupts(global);

   T0CON=0xC7;//11000111 prescaler timer0 1:256

   setup_timer_2(T2_DIV_BY_16,255,1);

   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);
   set_pwm1_duty(0);
   set_pwm2_duty(0);

   mpu6050_init();
   cuentasM1=0;
   cuentasM2=0;
   putc(0xAA);//AA=10101010=170

   while(true){
      TMR0L=0;

      putc(mpu6050_read(0x3B));//Acelerometro EjeX:0x3B=LSB,0x3C=MSB
      putc(mpu6050_read(0x3D));//Acelerometro EjeY:0x3D=LSB,0x3E=MSB
      putc(mpu6050_read(0x3F));//Acelerometro EjeZ:0x3F=LSB,0x40=MSB
      putc(mpu6050_read(0x43));//Giroscopio   Ejex:0x43=LSB,0x44=MSB
      putc(cuentasM1);
      putc(cuentasM2);
      putc(TMR0L);

      cuentasM1=0;
      cuentasM2=0;

      while(TMR0L<tope){
         if(pwm1_ON){
            if(pwm1>=128){
               pwm1=pwm1-128;
               M1a=0;
               M1b=1;
            }
            else{
               M1a=1;
               M1b=0;
            }
            pwm1=pwm1<<1;
            set_pwm1_duty(pwm1);
            pwm1_ON=0;
         }//pwm1_ON

         if(pwm2_ON){//pwm2_ON
            if(pwm2>=128){
               pwm2=pwm2-128;
               M2a=0;
               M2b=1;
            }
            else{
               M2a=1;
               M2b=0;
            }
            pwm2=pwm2<<1;
            set_pwm2_duty(pwm2);
            pwm2_ON=0;
         }//pwm2_ON
      }//while timer
   }//loop
}//main
