/*
  ¡¶STM32¶¥²ãÂß¼­¿ØÖÆÄ£¿é¡·
  PART1 £º´®¿ÚUART
  ÔÚmain.hÎÄ¼þÖÐÅäÖÃÓëDSPÍ¨ÐÅµÄ´®¿ÚÎªUART4£¬¶ÔÓ¦µÄµ¥Æ¬»úÒý½ÅPA0:Tx£¬PA1:Rx
  ÔÚmain.cÎÄ¼þmainº¯ÊýÖÐ¶Ô´®¿Ú½øÐÐÊµÀý»¯ÅäÖÃ£¬Ðè×¢ÒâµÄµØ·½ÊÇÅäÖÃµÄ²¨ÌØÂÊÊÇ115200£¬ÒòÎªDSP¶ËÅäÖÃµÄÊÇ115200£¬ÐèÓëÖ®Ò»ÖÂ
  ×îºó³õÊ¼»¯´®¿Ú£¬¼´¿ªÆô´®¿Ú£¬ÕâÀï¿ªÆô´®¿Ú»áµ¼ÖÂºÄµç£¬ËùÒÔÔÚºóÃæµÄÈÎÎñÖÐ¿ÉÒÔÔÚµÈ´ýÈÎÎñÆÚ¼ä¹Ø±Õ´®¿Ú£¨HAL_UART_DeInit£©£¬ÔÚ¿ªÆôÈÎÎñÊ±ÔÙ´ò¿ª´®¿Ú£¨HAL_UART_Init£©
  
  PART2£ºTIMÊ±ÖÓ
  Ê±ÖÓÓÃÓÚ²úÉú¼ÆÊ±£¬¿ÉÒÔÍ¨¹ýºê SECONDS_PER_TIM ¶¨ÒåÃ¿¸öÊ±ÖÓµÄÖÜÆÚÃëÊý£¬¼´Ã¿SECONDS_PER_TIM Ãë²úÉúÒ»¸öÊ±ÖÓÖÐ¶Ï£¬Ö´ÐÐÖÐ¶Ï·þÎñº¯ÊýHAL_TIM_PeriodElapsedCallback
  ÔÚÖÐ¶Ï·þÎñÖÐ¶Ôµ±Ç°Ê±¼äµÈ¼ÆÊ±±äÁ¿½øÐÐ¸üÐÂ£¬²¢ÅÐ¶ÏÊÇ·ñµ½´ïÈÎÎñ¿ªÆôÌõ¼þ£¬¼´ÀÛ»ýÖÐ¶Ï´ÎÊýÊÇ·ñµ½´ïÉè¶¨´ÎÊý
  ¼ÆÊ±±äÁ¿ÓÉ½á¹¹ÌåCurrentTime µÄ¶ÔÏó±íÊ¾£¬°üº¬day,hour,minute,second£¬Ã¿´ÎÖÐ¶ÏÔò½«Ê±¼ä¸üÐÂÀÛ¼ÓSECONDS_PER_TIM Ãë£»µ±Ç°Ê±¼ä±äÁ¿µÄ×÷ÓÃÊÇÓÃÓÚ¸æËßDSPµ±Ç°Ê±¼ä
  ÎªÁË·ÀÖ¹CurrentTimeµÄ¶ÔÏó±äÁ¿ÔÚ¸´Î»ºó¶ªÊ§±äÁ¿Öµ£¬Ðè½«¸Ã±äÁ¿ÉùÃ÷Îª·Ç³õÊ¼»¯ÄÚ´æÇø£¬¶¨ÒåÔÚÌØ¶¨µÄÄÚ´æµØÖ·ÖÐ¡£
  
  PART3£º¿´ÃÅ¹·WATCHDOG
  ¿´ÃÅ¹·¿ÉÒÔ¿´×öÊÇSTM32Íâ²¿µÄÉè±¸£¬Æä×÷ÓÃÊÇÔÚ³ÌÐòÔËÐÐÆÚ¼ä½øÐÐµ¹¼ÆÊ±£¬Ö÷³ÌÐò¿ÉÒÔËæÊ±¶Ô¿´ÃÅ¹·½øÐÐfeed£¬ÈôÔÚÉè¶¨µÄÊ±¼äÄÚ¿´ÃÅ¹·Ã»ÓÐµÃµ½feedÔò¿´ÃÅ¹·¸ºÔð½«STM³ÌÐòÖØÆô
  ÈôÔÚÉè¶¨Ê±¼äÄÚ¿´ÃÅ¹·µÃµ½ÁËfeed£¬Ôòµ¹¼ÆÊ±ÖØÐÂ¿ªÊ¼¼ÆÊ±¡£feed²Ù×÷Í¨¹ýº¯ÊýFeedWDG() Íê³É£¬¿´ÃÅ¹·µÄµ¹¼ÆÊ±Ê±³¤Í¨¹ýºê WDGTIME ¶¨Òå£¬µ¥Î»ÊÇs
  
  PART4£ºÖ÷º¯Êýmain
  mainº¯ÊýµÄ×î¿ªÊ¼ÐèÒª¶Ô³ÌÐòÔËÐÐ×´Ì¬½øÐÐÅÐ¶Ï£¬¼´´Ë´Î½øÈëmainº¯ÊýÊÇ³õ´Î½øÈë»¹ÊÇ¸´Î»ºó½øÈë£¬ÈôÊÇÊ×´Î½øÈëÔò¶Ô¼¸¸ö
  ¶¨ÒåÔÚ·Ç³õÊ¼»¯ÇøÓòµÄ±äÁ¿½øÐÐ³õÊ¼»¯£¬·ñÔò½øÐÐ±äÁ¿Öµ¸üÐÂ¡£
  Ö÷º¯ÊýÖÐÊ×ÏÈ½øÐÐ´®¿Ú£¬Ê±ÖÓºÍ¿´ÃÅ¹·µÄÅäÖÃºÍ³õÊ¼»¯¹¤×÷,È»ºó½øÈëwhileÑ­»·£¬Ö´ÐÐÒµÎñÂß¼­£¬´ËÒµÎñÂß¼­¼´µ¥Æ¬»úÔÚ³¤ÆÚÈÎÎñÖÐËù´¦µÄ×´Ì¬£¬Ö÷Òª¹ý³ÌÈçÏÂ£º
  
  1. ÔÚwhileÑ­»·ÖÐµÈ´ýwaiting_status×´Ì¬Á¿±äÎªfalse£¬·ñÔòÒ»Ö±ÔÚÑ­»·ÖÐfeed¿´ÃÅ¹·£¨ÒòÎª²»feedµÄ»°20sºó¿´ÃÅ¹·¾Í³¬Ê±ÁË£¬»á¸´Î»ÖØÆô³ÌÐò£¬ÔÚ³ÌÐòÔËÐÐµÄ¹ý³ÌÖÐÐèÒªÊ±³¤¼ÇµÃÈ¥feed¿´ÃÅ¹·£©
  waiting_status×´Ì¬Á¿ÔÚTIMÊ±ÖÓµÄÖÐ¶Ï·þÎñº¯ÊýHAL_TIM_PeriodElapsedCallbackÖÐ±»¸Ä±ä£¬µ±TIMÖÐ¶Ï´ÎÊýµ½´ïÖ¸¶¨µÄ´ÎÊý£¬ÔòÐÞ¸ÄÕâ¸ö×´Ì¬Á¿Îªfalse£¬ÔòÔÚÖÐ¶Ïº¯Êý½áÊøºóµ¥Æ¬»ú·µ»Øµ½Ö÷º¯ÊýÖÐÊ±£¬
  whileÑ­»·½áÊø£¬¿ªÆôÒ»´ÎÐÂµÄÌ½²âÈÎÎñ
  2. Ì½²âÈÎÎñÊÇÍ¨¹ýSTM32µ¥Æ¬»úÓëDSPÍ¨¹ý´®¿ÚÍ¨ÐÅµÄ·½Ê½½øÐÐ£¬µ¥Æ¬»úÍ¨¹ý´®¿Ú·¢ËÍATCMD¸øDSP£¬DSPÍê³ÉÖ¸Áîºó·µ»ØÍê³É×´Ì¬£¬µ¥Æ¬»ú³É¹¦½ÓÊÕµ½Õâ¸ö·µ»Ø×´Ì¬ºóÔò¼ÌÐøÏÂÒ»²½Ö¸Áî£¬Èô
  ·µ»ØµÄ×´Ì¬²»·ûºÏÔ¤ÆÚÔòÈÎÎñµ±Ç°Ö¸Áî³ö´í£¬ÐèÒª½øÐÐÏàÓ¦µÄ´¦Àí£¬Õâ¸ö´íÎó´¦ÀíÊÇÉÐ´ý¿ª·¢µÄ
  Ê×ÏÈDSPÍ¨¹ý¿ØÖÆGPIOÒý½ÅPE0,PE1,PE2,PE3µÄµçÆ½½«¼ÌµçÆ÷µÄ³£¿ª½Ó¿Ú±ÕºÏ£¬´Ó¶øÊ¹µÃDSPÓë·¢Éä°åÉÏµç£¬µ¥Æ¬»úµÈ´ý¼¸ÃëÖÓ£¨Õâ¸öÊ±¼äÐèÒª²âÊÔ£¬ÒòÎªDSPÉÏµçºó³õÊ¼»¯ÐèÒªÒ»µãÊ±¼ä£©
  È»ºóÍ¨¹ýATT()º¯Êý¶ÔDSP½øÐÐÍ¨ÐÅ²âÊÔ£¬Èô²âÊÔ³É¹¦Ôò¿ÉÒÔ½øÐÐºóÃæµÄÖ¸ÁîÈÎÎñ·¢ËÍ£¬·ñÔò½øÈë´íÎó´¦Àí£¨¿ÉÒÔÑ¡ÔñµÈ´ý¼¸ÃëÔÙ³¢ÊÔATT()»òÆäËû²Ù×÷£©¡£
  ºóÐøµÄÖ¸Áî²Ù×÷¿ÉÄÜ°üÀ¨£º¼à²âµç³ØµçÑ¹£¬¸ù¾Ýµç³ØµçÑ¹µÄÊ£ÓàÁ¿¿ØÖÆ½ñºóµÄÈÎÎñÖÜÆÚ£»·¢ÉäÌ½²âÉù²¨£»´æ´¢»Ø²¨Êý¾Ýµ½SD¿¨£»µÈµÈ²Ù×÷
  ×îºó½áÊøÁËÒ»´ÎÌ½²âÈÎÎñ£¬Í¨¹ýGPIOÒý½Å¿ØÖÆ¼ÌµçÆ÷¹Ø±ÕDSPºÍ·¢Éä°åµçÔ´£¬È»ºó½øÈëwhile()µÈ´ýÏÂÒ»´ÎÈÎÎñ¡£
  
  PART5£º·¢Éä°å+DSPÖ÷¿Ø°åµçÔ´¹ÜÀí
  Í¨¹ýSTM32Òý½ÅµçÆ½¿ØÖÆ·¢Éä°åºÍDSPÖ÷°åµçÔ´µÄÍ¨¶Ï£¬Òý½ÅÖ±½Ó¿ØÖÆ¼ÌµçÆ÷¡£
  Ô¼¶¨Ê¹ÓÃPE0×÷ÎªDSPµçÔ´¿ØÖÆÂß¼­Òý½Å£¬PE1-PE5¿É×÷Îª·¢Éä°åµÄµçÔ´¿ØÖÆÂß¼­Òý½Å£¬ÔÚmain.hÎÄ¼þÖÐÉùÃ÷Ïà¹Øº¯Êý£¬ÔÚmain.cÎÄ¼þÖÐ¶¨Òå
  
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
b)	ATPING=’d0000001.bin’ 		PING and save RX data to SD using name ‘d0000001.bin’. File name comply with FAT 8.3 format

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

