#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include "ti/sysbios/fatfs/ff.h"
#include "soc_C6748.h"
#include "gpio.h"
#include "uartStdio.h"
#include "client.h"

#define ad_upp_frame_size (1024*1024*4)

extern Semaphore_Handle sem1, sem3, sem5 ;
extern Semaphore_Handle semI2C1,semI2C2 ;
extern Semaphore_Handle semMMCSDwr1,semMMCSDwr2 ;
extern Queue_Handle i2cmsgQ, sdWriteQ ;
extern MsgObj i2cMsg ;
extern char HSADC_buffer[] ;
extern char HSDAC_buffer[] ;

unsigned char FPGA_IO(unsigned char regId, unsigned char regVal)
{
	MsgObj  *msg ;

	if(regVal == 0xff) {                    //Read from regId
		i2cMsg.id = I2C_FPGA  ;
		i2cMsg.RWn = I2C_FPGA_READ ;
		i2cMsg.regId = regId ;
		Queue_put(i2cmsgQ,(Queue_Elem *)&i2cMsg) ;
		Semaphore_post(semI2C1) ;
		Semaphore_pend(semI2C2, BIOS_WAIT_FOREVER);
		msg = Queue_get(i2cmsgQ) ;
		return msg->regVal ;
	}
	else {                                  //Write regVal to regId
		i2cMsg.id = I2C_FPGA  ;
		i2cMsg.RWn = I2C_FPGA_WRITE ;
		i2cMsg.regId = regId ;
		i2cMsg.regVal = regVal ;
		Queue_put(i2cmsgQ,(Queue_Elem *)&i2cMsg) ;
		Semaphore_post(semI2C1) ;
	}
}

unsigned char RxStartTime(unsigned char stime)
{
	if(stime == 0) { //Read from FPGA Register 6
		Task_sleep(10);
		return FPGA_IO(6,255) ;
	}
	else { //Write to FPGA Register 6
		Task_sleep(10);
		FPGA_IO(6,stime) ;
	}
}

unsigned char RxEndTime(unsigned char etime)
{
	if(etime == 0) { //Read from FPGA Register 9
		Task_sleep(10);
		return FPGA_IO(9,255) ;
	}
	else { //Write to FPGA Register 6
		Task_sleep(10);
		FPGA_IO(9,etime) ;
	}
}

void setRamp(unsigned char ramp_StartP, unsigned int ramp_Step, unsigned char ramp_Duration)
{
	unsigned char rampH_Step, rampL_Step ;
	RxStartTime(ramp_StartP) ; // Set ramp_StartP
	rampH_Step = (unsigned char)(ramp_Step >> 8) ;
	FPGA_IO(3,rampH_Step) ; // Set rampH_Step
	Task_sleep(10);
	rampL_Step = (unsigned char)(ramp_Step & 0x00ff) ;
	FPGA_IO(4,rampL_Step) ; // Set rampL_Step
	Task_sleep(10);
	FPGA_IO(7,ramp_Duration) ; // Set ramp_Duration
	Task_sleep(10);
}

void swCODAC_CLK(unsigned char swVal)
{
	unsigned char oldVal,newVal ;

	oldVal = FPGA_IO(0,255) ; // Read From FPGA
	if (swVal == 1) // CODAC CLK Turn ON
		newVal = oldVal | (0x03u) ;
	else if (swVal == 0)
		newVal = oldVal & ~(0x03u) ;
	else newVal = oldVal ;
	FPGA_IO(0,newVal) ;
}

void swDAC_CLK(unsigned char swVal)
{
	unsigned char oldVal,newVal ;

	oldVal = FPGA_IO(0,255) ; // Read From FPGA
	if (swVal == 1) // DAC CLK Turn ON
		newVal = oldVal | (0x01u << 0x01u) ;
	else if (swVal == 0)
		newVal = oldVal & ~(0x01u << 0x01u) ;
	else newVal = oldVal ;
	FPGA_IO(0,newVal) ;
}

void swADC_CLK(unsigned char swVal)
{
	unsigned char oldVal,newVal ;

	oldVal = FPGA_IO(0,255) ; // Read From FPGA
	if (swVal == 1) // DAC CLK Turn ON
		newVal = oldVal | (0x01u) ;
	else if (swVal == 0)
		newVal = oldVal & ~(0x01u) ;
	else newVal = oldVal ;
	FPGA_IO(0,newVal) ;
}

void adc2203RD(char *wfileName,int wFlag)
{
	MsgObj  sdWriteMSG ;
	char 	sname[13] ;

	swDAC_CLK(1) ;
	Task_sleep(20);
	swADC_CLK(1) ;
	Semaphore_post(sem5);
	Semaphore_pend(sem3, BIOS_WAIT_FOREVER);
	swCODAC_CLK(0) ;
	printf("Read from HSADC\n") ;
	if(wFlag == 1) { //Save to SD CARD
		memcpy(sname,wfileName+1,12) ;
		sname[12] = '\0' ;
		sdWriteMSG.strName = sname ;
		sdWriteMSG.RWn = 0 ;
		Queue_put(sdWriteQ,(Queue_Elem *)&sdWriteMSG) ;
		Semaphore_post(semMMCSDwr1);
		Semaphore_pend(semMMCSDwr2, BIOS_WAIT_FOREVER);
	}
}

void atCODACLK(unsigned char *cmdArg)
{
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	printf("cmd = CODACLK\n") ;
	if(strcmp(cmdArg,"1") == 0) {
		swCODAC_CLK(1) ;
		UART1Puts("\nOK\n",-1) ;
	}
	else if (strcmp(cmdArg,"0") == 0) {
		swCODAC_CLK(0) ;
		UART1Puts("\nOK\n",-1) ;
	}
	else {
		UART1Puts("\n",-1) ;
		printf("Invalid command argument\n") ;
	}
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

void atDACLK(unsigned char *cmdArg)
{
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	printf("cmd = DACLK\n") ;
	if(strcmp(cmdArg,"1") == 0) {
		swDAC_CLK(1) ;
		UART1Puts("\nOK\n",-1) ;
	}
	else if (strcmp(cmdArg,"0") == 0) {
		swDAC_CLK(0) ;
		UART1Puts("\nOK\n",-1) ;
	}
	else {
		UART1Puts("\n",-1) ;
		printf("Invalid command argument\n") ;
	}
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

void atADCLK(unsigned char *cmdArg)
{
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	printf("cmd = ADCLK\n") ;
	if(strcmp(cmdArg,"1") == 0) {
		swADC_CLK(1) ;
		UART1Puts("\nOK\n",-1) ;
	}
	else if (strcmp(cmdArg,"0") == 0) {
		swADC_CLK(0) ;
		UART1Puts("\nOK\n",-1) ;
	}
	else {
		UART1Puts("\n",-1) ;
		printf("Invalid command argument\n") ;
	}
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

void atPING(unsigned char *cmdArg)
{
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	printf("cmd = PING, Arg = %s \n",cmdArg) ;
	if(*cmdArg == 0)
		adc2203RD(NULL,0) ;
	else
		adc2203RD(cmdArg,1) ;
	UART1Puts("\nOK\n",-1) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

// Turn ON/OFF TGC Mode
void atTGC(unsigned char *cmdArg)
{
	unsigned char oldVal,newVal ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	oldVal = FPGA_IO(0,255) ; // Read From FPGA
	if (strcmp(cmdArg,"0") == 0) // TGC Turn OFF
		newVal = oldVal | (0x01u << 0x04u) ;
	else if (strcmp(cmdArg,"1") == 0) // TGC Turn ON
		newVal = oldVal & ~(0x01u << 0x04u) ;
	else {
		newVal = oldVal ;
		printf("Invalid command argument\n") ;
	}
	FPGA_IO(0,newVal) ;
	UART1Puts("\nOK\n",-1) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

// Set Pulse Interval
void atPUSINT(unsigned char *cmdArg)
{
	unsigned char pInterval ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	pInterval = (unsigned char)atoi(cmdArg) ;
	printf("pulse interval = %d\n",pInterval) ;
	FPGA_IO(1,pInterval) ;
	UART1Puts("\nOK\n",-1) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

// Set Initial Gain
void atIGAIN(unsigned char *cmdArg)
{
	float lG, hG ;
	double iGainV,eGainV ;
	double a,b,c,h1,h2 ;
	unsigned int rampStep ;
	unsigned char iGain,eGain ;
	unsigned char rangeDuration ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	lG = LGain ;
	hG = HGain ;
	iGainV = atof(cmdArg) ;
	iGain = 252*(iGainV-lG)/(hG-lG) + 2 ;
	printf("iGain=%f initGain=%d\n",iGainV,iGain) ;
	FPGA_IO(5,iGain) ; //Set Initial Gain
	Task_sleep(10);
	eGain = FPGA_IO(11,255) ; //Read End Gain
	Task_sleep(10);
	eGainV = (eGain-30)*(hG-lG)/252 + lG ; //Convert to Voltage
	a = 8000 + RxStartTime(0) * 512 ;
	b = 16000+ RxEndTime(0) * 2048 ;
	h1 = 252*(iGainV-lG) / (hG-lG) + 2 ;
	h2 = 252*(eGainV-lG) / (hG-lG) + 2 ;
	c = (b-a) ;
	rampStep = (unsigned int)(c/(8*(h1-h2)))  ;
	rangeDuration = (unsigned char)((c-2000)/2048) ;
	setRamp(RxStartTime(0),rampStep,rangeDuration) ;
	UART1Puts("\nOK\n",-1) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

// Set Final Gain
void atEGAIN(unsigned char *cmdArg)
{
	double eGainV,iGainV,a,b,c ;
	double lG, hG, h1, h2 ;
	unsigned int rampStep ;
	unsigned char eGain, rangeDuration ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	lG = LGain ;
	hG = HGain ;
	eGainV = atof(cmdArg) ;
	eGain = 252*(eGainV-lG)/(hG-lG) + 30 ;
	FPGA_IO(11,eGain) ; //Write eGain
	Task_sleep(10);
	a = 8000 + RxStartTime(0) * 512 ;
	b = 16000+ RxEndTime(0) * 2048 ;
	iGainV = (FPGA_IO(5,255)-2)*(hG-lG)/252 + lG ;
	UART1Puts("\nOK\n",-1) ;
	h1 = 252*(iGainV-lG) / (hG-lG) + 2 ;
	h2 = 252*(eGainV-lG) / (hG-lG) + 2 ;
	c = (b-a) ;
	rampStep = (unsigned int)(-1*c/(8*(h2-h1)))  ;
	rangeDuration = (unsigned char)((c-2000)/2048) ;
	setRamp(RxStartTime(0),rampStep,rangeDuration) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

void atSDLS(void)
{
	FRESULT result ;
	char flname[2400],*pStr ;
	char path[128];
	unsigned int filesLen,fLen ;
	char *pch ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	path[0] = '/';
	path[1] = '\0';
	memset(flname,255,2400) ;
	if ((result = scanFiles(path,flname)) != FR_OK) {
		printf("ERROR scanning files and directories: returned: %d\n", result);
		exit(1);
	}
	pch = (char *)memchr(flname,255,2400) ;
	filesLen = (unsigned int)(pch - flname) ;
	printf("%s endLen = %d\n",flname, filesLen) ;
	pStr = flname ;
	while(filesLen > 0) {
		UART1Puts(pStr,-1) ;
		UART1Puts("\n",-1) ;
		fLen = strlen(pStr)+1 ;
		pStr += fLen ;
		filesLen -= fLen ;
	}
	UART1Puts("OK\n",-1) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

// ATGLFM2=fc,f0,f1,gain,Tsig, ex: ATGLFM2=150,6,14,1,1
void atGLFM2(unsigned char *cmdArg)
{
	unsigned char sVal[20] ;
	char *pch, *pstr ;
	unsigned int slen,n ;
	double fc,f0,f1 ;
	double gain,Tsig, freq, theta1, theta2,t ;
	long Ndat ;
	unsigned char		*ydat ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	pch = (char *)strchr(cmdArg,',') ;
	slen = pch - cmdArg ;
	memcpy(sVal,cmdArg,slen) ;
	sVal[slen] = '\0' ;
	fc = 1000*atof(sVal) ; //fc
	pstr = cmdArg+slen+1 ;
	pch = strchr(pstr,',') ;
	slen = pch - pstr ;
	memcpy(sVal,pstr,slen) ;
	sVal[slen] = '\0' ;
	f0 = 1000*atof(sVal) ; //f0
	pstr += slen+1 ;
	pch = strchr(pstr,',') ;
	slen = pch - pstr ;
	memcpy(sVal,pstr,slen) ;
	sVal[slen] = '\0' ;
	f1 = 1000*atof(sVal) ; //f1
	pstr += slen+1 ;
	pch = strchr(pstr,',') ;
	slen = pch - pstr ;
	memcpy(sVal,pstr,slen) ;
	sVal[slen] = '\0' ;
	gain = -1*atof(sVal) ; //gain
	pstr += slen+1 ;
	strcpy(sVal,pstr) ;
	Tsig = atof(sVal)/1000 ; //Tsig
	printf("fc=%f f0=%f f1=%f gain=%f Tsig=%f\n",fc,f0,f1,gain,Tsig) ;

	Ndat = (long)(Tsig*dacFs) ; //Number of data point
	memset(HSDAC_buffer,128,1024*40) ;
	ydat = (unsigned char *)HSDAC_buffer ;

	for(n=0;n < Ndat;n++ ) {
		t = (double)n/dacFs ;
		freq = f0 + t*(f1-f0)/(2*Tsig) ;
		theta1 = 2*PI*freq*t ;
		theta2 = 2*PI*fc*t ;
		ydat[n+DSHIFT] = 128*(gain * sin(theta1) * sin(theta2))+128 ;
		//ydat[n+DSHIFT] = 128*(gain * cos(2*PI*freq*t) * cos(2*PI*150000*t))+128 ;
		//printf("freq[%d]=%f\n",n,freq) ;
	}

	FPGA_IO(8,(unsigned char)(Ndat/256 + 16)) ; //Write DAC2Gain switch timing register
	UART1Puts("\nOK\n",-1) ;
	Semaphore_post(sem1);
	Semaphore_pend(sem3, BIOS_WAIT_FOREVER);
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

void atGLFM(unsigned char *cmdArg)
{
	unsigned char sVal[20] ;
	char *pch, *pstr ;
	unsigned int f0,f1,slen,n ;
	double gain,Tsig, freq, theta,t ;
	long Ndat ;
	unsigned char		*ydat ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	pch = (char *)strchr(cmdArg,',') ;
	slen = pch - cmdArg ;
	memcpy(sVal,cmdArg,slen) ;
	sVal[slen] = '\0' ;
	f0 = 1000*atoi(sVal) ;
	pstr = cmdArg+slen+1 ;
	pch = strchr(pstr,',') ;
	slen = pch - pstr ;
	memcpy(sVal,pstr,slen) ;
	sVal[slen] = '\0' ;
	f1 = 1000*atoi(sVal) ;
	pstr += slen+1 ;
	pch = strchr(pstr,',') ;
	slen = pch - pstr ;
	memcpy(sVal,pstr,slen) ;
	sVal[slen] = '\0' ;
	gain = -1*atof(sVal) ;
	pstr += slen+1 ;
	strcpy(sVal,pstr) ;
	Tsig = atof(sVal)/1000 ;
	printf("f0=%d f1=%d gain=%f Tsig=%f\n",f0,f1,gain,Tsig) ;

	Ndat = (long)(Tsig*dacFs) ; //Number of data point
	memset(HSDAC_buffer,128,1024*40) ;
	ydat = (unsigned char *)HSDAC_buffer ;

	for(n=0;n < Ndat;n++ ) {
		t = (double)n/dacFs ;
		freq = f0+(f1-f0)/(2*Tsig) * t ;
		theta = 2*PI*freq*t ;
		ydat[n+DSHIFT] = 128*(gain * sin(theta))+128 ;
	}

	FPGA_IO(8,(unsigned char)(Ndat/256 + 16)) ; //Write DAC2Gain switch timing register
	UART1Puts("\nOK\n",-1) ;
	Semaphore_post(sem1);
	Semaphore_pend(sem3, BIOS_WAIT_FOREVER);
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

void atGCW(unsigned char *cmdArg)
{
	unsigned char sVal[20] ;
	char *pch,*pstr ;
	unsigned int nCycle,slen,n ;
	double 	gain,t ;
	long	Ndat,freq ;
	double  theta ;
	unsigned char		*ydat ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	pch = (char *)strchr(cmdArg,',') ;
	slen = pch - cmdArg ;
	memcpy(sVal,cmdArg,slen) ;
	sVal[slen] = '\0' ;
	freq = 1000*atoi(sVal) ;
	pstr = cmdArg+slen+1 ;
	pch = strchr(pstr,',') ;
	slen = pch - pstr ;
	memcpy(sVal,pstr,slen) ;
	sVal[slen] = '\0' ;
	nCycle = atoi(sVal) ;
	pstr += slen+1 ;
	strcpy(sVal,pstr) ;
	gain = -1*atof(sVal) ;
	printf("Frequency=%d kHz Cycle=%d gain=%f \n",freq,nCycle,gain) ;
	Ndat = (long)(nCycle*dacFs/freq) ; //Number of data point
	memset(HSDAC_buffer,128,1024*40) ;
	ydat = (unsigned char *)HSDAC_buffer ;

	for(n=0;n < Ndat;n++ ) {
		t = (double)n/dacFs ;
		theta = 2*PI*freq*t ;
		ydat[n+DSHIFT] = 128*(gain * sin(theta))+128 ;
	}

	FPGA_IO(8,(unsigned char)(Ndat/256 + 16)) ; //Write DAC2Gain switch timing register

	UART1Puts("\nOK\n",-1) ;
	Semaphore_post(sem1);
	Semaphore_pend(sem3, BIOS_WAIT_FOREVER);
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

// Read from LS ADC AD7995
void atLSADC(unsigned char *cmdArg)
{
	unsigned char chnIndex,i ;
	unsigned short Vtg ;
	unsigned char  *pVtg ;
	unsigned char 	v1,v2 ;
	double 	voltage=0 ;
	char buffer [20];
	MsgObj  *msg ;

	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	if (strcmp(cmdArg,"2") == 0)  {// Read LS ADC Ch 2
		printf("Read LS ADC Ch 2\n") ;
		chnIndex = 2 ;
	}
	else if (strcmp(cmdArg,"3") == 0)  { // Read LS ADC Ch 3
		printf("Read LS ADC Ch 3\n") ;
		chnIndex = 3 ;
	}
	else {
		printf("Invalid command argument\n") ;
		UART1Puts("\n",-1) ;
		//return -1 ;
	}

	i2cMsg.id = I2C_AD7995  ;
	i2cMsg.ChnInd = chnIndex ;

	for(i=0;i<20;i++) {
		Queue_put(i2cmsgQ,(Queue_Elem *)&i2cMsg) ;
		Semaphore_post(semI2C1) ;
		Task_sleep(2);
		Semaphore_pend(semI2C2, BIOS_WAIT_FOREVER);
		msg = Queue_get(i2cmsgQ) ;
		Vtg = (msg->voltage_AD7995 & 0xff0f) ;
		pVtg = (unsigned char *)(&Vtg) ;
		v1 = (unsigned char)(*pVtg) ;
		v2 = (unsigned char)(*(pVtg+1)) ;
		voltage += (double)(v1*256+v2)/(1024*1.2) ;
	}

	sprintf(buffer,"%f",voltage/20) ;
	UART1Puts("\n",-1) ;
	UART1Puts(buffer,-1) ;
	UART1Puts("\nOK\n",-1) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

// PA Channel Select
void atPACHN(unsigned char *cmdArg)
{

}

// Test Command
void atT(unsigned char *cmdArg)
{
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	printf("TEST COMMAND\n") ;
	UART1Puts("\nOK\n",-1) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}

void atRESET(void)
{
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_HIGH);
	printf("RESET COMMAND\n") ;
	UART1Puts("\n",-1) ;
	GPIOPinWrite(SOC_GPIO_0_REGS, 2, GPIO_PIN_LOW);
}
