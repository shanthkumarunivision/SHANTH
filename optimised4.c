
#include "header.h"
#define led_red 4
#define led_green 5 
#define key_pin 26
/////////////////////////REGISTER ADDRESSES////////////////////////////////////

///////////////////GLOBEL VARIABLES AND STRUCTURE//////////////
int fd,fd1,r,keyoff=0;
char ch,final_link[100];
char LA[100],LO[100];
char c1=0,c2=1;
//const char link[35]="http://www.google.com/maps/place/";
struct data d1,d2,dr;
struct data
{  
	float ax;
	float ay;
	float az;
	float gx;
	float gy;
	float gz;
	float t;
};

///////////abc() FUNCTION WILL CALL DURING EXIT TIME//////////////

void abc(void)
{
//	system("clear");
	printf("I am in atexit()\n");
//	printf("Turning off after 5 seconds\n");
	delay(1000);
//	system("clear");
	system("./off");
	for(int i=0;i<50;i++)
	{
		if(i<10)
		{
			delay(100);
			system("./on");
			delay(100);
			system("./off");
		}
		if(i==10)
		{
			delay(1000);
			system("./on");
			delay(1000);
			system("./off");
		}
		delay(20);
		system("./on");
		delay(10);
		system("./off");
	}
}
 
/////////////MPU6050 SENCER INITILIZATION FUNCTION//////////////
void MPU6050_Init()
{
	
	wiringPiI2CWriteReg8 (fd, SMPLRT_DIV, 0x07);	/* Write to sample rate register */
	wiringPiI2CWriteReg8 (fd, PWR_MGMT_1, 0x01);	/* Write to power management register */
	wiringPiI2CWriteReg8 (fd, CONFIG, 0);			/* Write to Configuration register */
	wiringPiI2CWriteReg8 (fd, GYRO_CONFIG, 24);		/* Write to Gyro Configuration register */
	wiringPiI2CWriteReg8 (fd, INT_ENABLE, 0x01);	/*Write to interrupt enable register */

} 
///////////FUNCTION FOR READING RAW DATA FROM MPU6050///////////	
short read_raw_data(int addr)
{
	short high_byte,low_byte,value;
	high_byte = wiringPiI2CReadReg8(fd, addr);
	low_byte = wiringPiI2CReadReg8(fd, addr+1);
	value = (high_byte << 8) | low_byte;
	return value;
}
///////////////////////DELAY FUNCTION///////////////////////////
void ms_delay(int val)
{
	int i,j;
	for(i=0;i<=val;i++)
		for(j=0;j<1200;j++);
}
////read() FUNCTION WILL TAKE RAW DATA AND CONVERT IT INTO ACTUAL DATA//////
void read_(struct data *d1)
{
	float Acc_x,Acc_y,Acc_z;
	float Gyro_x,Gyro_y,Gyro_z;
	float Tmp;
//	float Ax=0, Ay=0, Az=0;
//	float Gx=0, Gy=0, Gz=0;
////*Read raw value of Accelerometer and gyroscope from MPU6050*/
	Acc_x = read_raw_data(ACCEL_XOUT_H);
	Acc_y = read_raw_data(ACCEL_YOUT_H);
	Acc_z = read_raw_data(ACCEL_ZOUT_H);
		
	Gyro_x = read_raw_data(GYRO_XOUT_H);
	Gyro_y = read_raw_data(GYRO_YOUT_H);
	Gyro_z = read_raw_data(GYRO_ZOUT_H);
		
	Tmp = read_raw_data(TEMPERATURE);
		
	/* Divide raw value by sensitivity scale factor */
	d1->ax = Acc_x/16384.0;
	d1->ay = Acc_y/16384.0;
	d1->az = Acc_z/16384.0;
		
	d1->gx = Gyro_x/131;
	d1->gy = Gyro_y/131;
	d1->gz = Gyro_z/131;
		
	d1->t=Tmp/340+36.53;
	
	printf("\n Gx=%.3f °/s\tGy=%.3f °/s\tGz=%.3f °/s\tAx=%.3f g\tAy=%.3f g\tAz=%.3f g\tT=%.3f °C\n",d1->gx,d1->gy,d1->gz,d1->ax,d1->ay,d1->az,d1->t);

}

////////////COMPARING PREVIOUS DATA WITH NEW DATA///////////////
//IF THE GAP BETWEEN PREVIOUS DATA AND NEW DATA IS GREATER THEN TOLLERATE LEVEL THEN INCREMENTING r///
int comp(struct data *d1,struct data *d2)
{
	float ax;
	float ay;
	float az;
	float gx;
	float gy;
	float gz;
	float t;
	int i=0;
	ax=d1->ax - d2->ax;
	ay=d1->ay - d2->ay;
	az=d1->az - d2->az;
	gx=d1->gx - d2->gx;
	gy=d1->gy - d2->gy;
	gz=d1->gz - d2->gz;
	t =d1->t  - d2->t;
////////MAKING DATA IN POSITIVE////////	
	ax=ax<0?-ax:ax;
	ay=ay<0?-ay:ay;
	az=az<0?-az:az;
	gx=gx<0?-gx:gx;
	gy=gy<0?-gy:gy;
	gz=gz<0?-gz:gz;
	t =t <0?-t:t;
	
	printf("\n Gx d=%.3f °/s\tGy d=%.3f °/s\tGz d=%.3f °/s\tAx d=%.3f g\tAy d=%.3f g\tAz d=%.3f g\tT d=%.3f °C\n",gx,gy,gz,ax,ay,az,t);
//	ax>0.2?i++:0;
//	ay>0.2?i++:0;
//	az>0.2?i++:0;
//	gx>0.1?i++:0;
//	gy>0.1?i++:0;
//	gz>0.1?i++:0;
//	t =t>5?i++:0;
	ax>0.5?i++:0;
	ay>0.5?i++:0;
	az>0.5?i++:0;
	gx>2.5?i++:0;
	gy>0.5?i++:0;
	gz>0.5?i++:0;
//	if(i>0)
	
	return i;
}
//////////////////function call from latitude or longitude function////////////////////////////////
double convert_to_degree(double lat)
{ 
	double copy;
    copy=lat;
    int f2=((int)lat)/100; ;
//	float decm=(atof(copy+3)/60);
	double decm=copy-(double)(f2*100);
	double final=(double)(f2+decm/60.0);
//	printf("%f",final);
	return final;
}
//////////////////////////////////function for latitude/////////////////////////////////////////////////////
double latitude(char str[])
{int i,j;
char lat1[20];
double latnew,latfinal;
for(i=0,j=24;j<=35;i++,j++)
{
 lat1[i]=str[j];
 
}
latnew=atof(lat1);
latfinal=convert_to_degree(latnew);
return latfinal;
}
///////////////////////////////////function for longitude///////////////////////////////////////////////////////////
double longitude(char str[])
{
int i,j;
char lon1[20];
double lonnew,lonfinal;
for(i=0,j=39;j<=50;i++,j++)
{
 lon1[i]=str[j];
}
lonnew=atof(lon1);
lonfinal=convert_to_degree(lonnew);
return lonfinal;
}
 
 

 
///////////////////////////video()/////////////////////////////
void video(char *t2)			
{	
	char t[100];
	const char *t1="raspivid -o vid1.h264 -t ";
	strcat(t,t1);
	strcat(t,t2);
//	system("clear");
//	delay(50);
	printf("before camera\n");
//	system("raspivid -o vid1.h264 -t 10000");
	system(t2);
//	system("raspistill -o img1.jpg");
	printf("after camera\n");
//	delay(50);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////this function is for checkinmg the signal//////////////////////////////////////////
void *signalckeck(void* arg )
{
	//while(1)
	{
	
	printf("\nsignalckeck thread\n");
	delay(110);
     char stu[50],stu1[50];
     int i=0,j=0;
     int cde_num=0,t;
 serialPuts(fd1,"AT+CSQ\r\n");{
          delay(100);
          while (serialDataAvail(fd1))
          {
          ch=serialGetchar(fd1);
    //      delay(100);
          stu[i++]=ch;
          printf("%c",ch);
        
          fflush(stdout);
          }
          stu[i]='\0';}
 
           printf("%s",stu);
       for(i=5,j=0;i<8;i++)
         { stu1[j++]=stu[i];
           printf("%c",stu1[i]);
             }
             stu1[j]='\0';
             
            //for(i=0,t=0;stu1[i];t=t*10+(stu1[i]-48)); 
             
              // stu1[i]
           //cde_num=int.Parse(stu1);
          // for(i=0;i<8;i++)
           //{
           //printf("%c\n",stu[i]);
           //printf("%s",stu);}
	if(cde_num<10){
		//serialPuts(fd1,"AT+CRESET\r\n");
		
		
		}
	}
}
 
//////////////////////////////////////key check////////////////////////////////////////////////////////////
////////////////////////////checks for the key is inserted or not////////////////////////////////////////	
void *keycheck(void* arg)
{while(1){
	
   printf("\nkeycheck thread\n");
   delay(100);
	if(digitalRead(key_pin==0))
	{printf("key off");
		keyoff=0;
		 
		}
	
	}
	
	}
/////////////////////////////////////sensor function///////////////////////////////////////////////////
///////////////////////////read sensor values if key is off	////////////////////////////////////////////
void *sensorfun(void* arg)
{while(1){if(keyoff==0){
	printf("\nsensorfun thread\n");
	delay(130);
	delay(4000);
	read_(&d1);
	while(1){
		read_(&d2);
		r=comp(&d1,&d2);
//		if(r==0)
		if (r>0)
		{
			d1=d2;
			c1=1;
			c2=1;
			//system("./on");
	}}}}}
/////////////////////////////////////////////////camera///////////////////////////////////	
void *camera(void* arg)
	{while(1){if(c1==1){
		//	system("clear");
//	delay(50);
	printf("before camera\n");
//	system("raspivid -o vid1.h264 -t 10000");
	system("raspistill -o img1.jpg");
	printf("after camera\n");
//	delay(50);

c1=0;	}}}
///////////////////////////////////////4gmodule//////////////////////		
void *gmodule(void* arg)
{ char str[100];
	int i;
	while(1){if(c2==1){
		{serialPuts(fd1,"AT+CGPSINFO\r\n");
	  i=0;
	  while (serialDataAvail(fd1))
	    {
	    ch=serialGetchar(fd1);
	    str[i++]=ch;
	    printf("%c",ch);
	    fflush (stdout) ;
	    delay(20) ;
	    }
	    str[i]='\0';
	  double  lat1=latitude(str);
	  double  lon1=longitude(str);
	  gcvt(lat1,9,LA);
	  gcvt(lon1,9,LO);
	  serialPuts(fd1,"AT+CMGF=1\r\n");
			delay(10);			
			while(serialDataAvail (fd1))
			{
				ch=serialGetchar(fd1);
				printf("%c",ch);
				fflush(stdout);
			}	
			serialPuts(fd1,"AT+CMGS=\"9986149818\"\r\n");//sant
		//	strcat(final_link,link);
			strcat(final_link,"http://www.google.com/maps/place/");
			strcat(final_link,LA);
			strcat(final_link,",");
			strcat(final_link,LO);
			strcat(final_link,"\r\n");
			c2=0;
}}			
	}	
			}
////////////////////////////MAIN///////////////////////////////
int main()
{
//	const char link[]="http://www.google.com/maps/place/";
	fd1=serialOpen("/dev/ttyS0",115200);////
	if(fd1<0)
	{
		perror("serialOpen");
		printf("error\n");
		return 0;
	}
	printf("fd1=%d\n",fd1);
	
	wiringPiSetup();
	fd = wiringPiI2CSetup(Device_Address);   /*Initializes I2C with device Address*/
	pinMode(led_red,OUTPUT);
	pinMode(led_green,OUTPUT);	
	pinMode(key_pin,INPUT);
	MPU6050_Init();	
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	pthread_t thread5;
	pthread_create(&thread1,0,signalckeck,0);
	pthread_create(&thread2,0,keycheck,0);
	pthread_create(&thread3,0,sensorfun,0);
	pthread_create(&thread4,0,camera,0);
	pthread_create(&thread5,0,gmodule,0);
	pthread_exit(0);	                     /* Initializes MPU6050 */
	 
			
			 
 

	
}
