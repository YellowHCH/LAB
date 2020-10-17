/*
  ��STM32�����߼�����ģ�顷
  PART1 ������UART
  ��main.h�ļ���������DSPͨ�ŵĴ���ΪUART4����Ӧ�ĵ�Ƭ������PA0:Tx��PA1:Rx
  ��main.c�ļ�main�����жԴ��ڽ���ʵ�������ã���ע��ĵط������õĲ�������115200����ΪDSP�����õ���115200������֮һ��
  ����ʼ�����ڣ����������ڣ����￪�����ڻᵼ�ºĵ磬�����ں���������п����ڵȴ������ڼ�رմ��ڣ�HAL_UART_DeInit�����ڿ�������ʱ�ٴ򿪴��ڣ�HAL_UART_Init��
  
  PART2��TIMʱ��
  ʱ�����ڲ�����ʱ������ͨ���� SECONDS_PER_TIM ����ÿ��ʱ�ӵ�������������ÿSECONDS_PER_TIM �����һ��ʱ���жϣ�ִ���жϷ�����HAL_TIM_PeriodElapsedCallback
  ���жϷ����жԵ�ǰʱ��ȼ�ʱ�������и��£����ж��Ƿ񵽴����������������ۻ��жϴ����Ƿ񵽴��趨����
  ��ʱ�����ɽṹ��CurrentTime �Ķ����ʾ������day,hour,minute,second��ÿ���ж���ʱ������ۼ�SECONDS_PER_TIM �룻��ǰʱ����������������ڸ���DSP��ǰʱ��
  Ϊ�˷�ֹCurrentTime�Ķ�������ڸ�λ��ʧ����ֵ���轫�ñ�������Ϊ�ǳ�ʼ���ڴ������������ض����ڴ��ַ�С�
  
  PART3�����Ź�WATCHDOG
  ���Ź����Կ�����STM32�ⲿ���豸�����������ڳ��������ڼ���е���ʱ�������������ʱ�Կ��Ź�����feed�������趨��ʱ���ڿ��Ź�û�еõ�feed���Ź�����STM��������
  �����趨ʱ���ڿ��Ź��õ���feed���򵹼�ʱ���¿�ʼ��ʱ��feed����ͨ������FeedWDG() ��ɣ����Ź��ĵ���ʱʱ��ͨ���� WDGTIME ���壬��λ��s
  
  PART4��������main
  main�������ʼ��Ҫ�Գ�������״̬�����жϣ����˴ν���main�����ǳ��ν��뻹�Ǹ�λ����룬�����״ν�����Լ���
  �����ڷǳ�ʼ������ı������г�ʼ����������б���ֵ���¡�
  �����������Ƚ��д��ڣ�ʱ�ӺͿ��Ź������úͳ�ʼ������,Ȼ�����whileѭ����ִ��ҵ���߼�����ҵ���߼�����Ƭ���ڳ���������������״̬����Ҫ�������£�
  
  1. ��whileѭ���еȴ�waiting_status״̬����Ϊfalse������һֱ��ѭ����feed���Ź�����Ϊ��feed�Ļ�20s���Ź��ͳ�ʱ�ˣ��Ḵλ���������ڳ������еĹ�������Ҫʱ���ǵ�ȥfeed���Ź���
  waiting_status״̬����TIMʱ�ӵ��жϷ�����HAL_TIM_PeriodElapsedCallback�б��ı䣬��TIM�жϴ�������ָ���Ĵ��������޸����״̬��Ϊfalse�������жϺ���������Ƭ�����ص���������ʱ��
  whileѭ������������һ���µ�̽������
  2. ̽��������ͨ��STM32��Ƭ����DSPͨ������ͨ�ŵķ�ʽ���У���Ƭ��ͨ�����ڷ���ATCMD��DSP��DSP���ָ��󷵻����״̬����Ƭ���ɹ����յ��������״̬���������һ��ָ���
  ���ص�״̬������Ԥ��������ǰָ�������Ҫ������Ӧ�Ĵ���������������д�������
  ����DSPͨ������GPIO����PE0,PE1,PE2,PE3�ĵ�ƽ���̵����ĳ����ӿڱպϣ��Ӷ�ʹ��DSP�뷢����ϵ磬��Ƭ���ȴ������ӣ����ʱ����Ҫ���ԣ���ΪDSP�ϵ���ʼ����Ҫһ��ʱ�䣩
  Ȼ��ͨ��ATT()������DSP����ͨ�Ų��ԣ������Գɹ�����Խ��к����ָ�������ͣ�����������������ѡ��ȴ������ٳ���ATT()��������������
  ������ָ��������ܰ���������ص�ѹ�����ݵ�ص�ѹ��ʣ�������ƽ����������ڣ�����̽���������洢�ز����ݵ�SD�����ȵȲ���
  ��������һ��̽������ͨ��GPIO���ſ��Ƽ̵����ر�DSP�ͷ�����Դ��Ȼ�����while()�ȴ���һ������
  
  PART5�������+DSP���ذ��Դ����
  ͨ��STM32���ŵ�ƽ���Ʒ�����DSP�����Դ��ͨ�ϣ�����ֱ�ӿ��Ƽ̵�����
  Լ��ʹ��PE0��ΪDSP��Դ�����߼����ţ�PE1-PE5����Ϊ�����ĵ�Դ�����߼����ţ���main.h�ļ���������غ�������main.c�ļ��ж���
  
 */
 Deep Sea Parametric Array AT commands
1.	Turn on/off DAC/ADC Clock
a)	ATCODACLK=1 		Turn ON
b)	ATCODACLK=0 		Turn OFF

2.	Turn on/off DAC Clock
a)	ATDACLK=1 		Turn ON
b)	ATDACLK=0 		Turn OFF

3.	Turn on/off ADC Clock
a)	ATADCLK=1 		Turn ON
b)	ATADCLK=0 		Turn OFF

4.	Signal PIN
a)	ATPING					PING only
b)	ATPING=�d0000001.bin� 		PING and save RX data to SD using name �d0000001.bin�. File name comply with FAT 8.3 format

5.	Turn on/off Time Gain Control (TGC)
a)	ATTGC=1			Turn on TGC
b)	ATTGC=0			Turn off TGC

6.	 Set Pulse Interval
a)	ATPUSINT=N		Set pulse interval = N (ms)

7.	Set Initial Gain (in volts)
a)	ATIGAIN=k			Set Initial Gain = k (volts)  (in general 0.2 < k < 1.8)

8.	Set End Gain (in volts)
a)	ATEGAIN=p		Set End Gain = p (volts) (in general k < p < 1.8)

9.	List files in SD Card
a)	ATSDLS

10.	Generate CW Pulse
a)	ATGCW=150,20,1	Generate 150kHz CW pulse, 20 cycles, Gain=1

11.	Generate LFM Pulse
a)	ATGLFM=f0,f1,gain,Tsig	Generate LFM with start frequency f0 kHz, end frequency f1 kHz, Signal Gain = gain, Signal Duration = Tsig (ms)
EX: ATGLFM=130,170,1,1

12.	Generate LFM2 Pulse
a)	ATGLFM2=fc,f0,f1,gain,Tsig(in ms)
EX: ATGLFM2=150,3,7,1,2

13.	Read Low Speed ADC
a)	ATLSADC=k		Read LS ADC channel k

14.	Source Select
a)	ATSRSEL=0			Select Primary Channel (HF)
b)	ATSRSEL=1			Select Secondary Channel (LF)

15.	PA Channel Select
a)	ATPACSEL=0		Turn off Both Channel
b)	ATPACSEL=1		Turn on Channel 1
c)	ATPACSEL=2		Turn on Channel 2
d)	ATPACSEL=3		Turn on Both PA Channel

16.	Test Command			Test Command, Return OK indicate system ready
a)	ATT
17.	System Reset
a)	ATRESET			Not implement Yet

