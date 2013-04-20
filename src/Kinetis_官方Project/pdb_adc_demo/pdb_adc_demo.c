/*
 * File:		pdb_adc_demo.c
 * Purpose:		
 *
 */



#include "common.h"
#include "adc16.h"

         
#define MAX_SAMPLES   128u

#define DisableInterrupt()  asm("CPSID i \n")
#define EnableInterrupt()  asm("CPSIE i \n")

//globle variable
tADC_Config Master_Adc_Config;  
tADC_Cal_Blk CalibrationStore[2];

volatile uint16  wSamplesBuf[MAX_SAMPLES];
volatile uint16  wSamplesBufOffline[MAX_SAMPLES];

volatile uint16  wSamplesCounter;
volatile uint8   bSamplesReady;
volatile uint32  bSeqErr;

// Prototypes
void adc_init(void);
void pdb_init(void);   
void pdb_seq_error(uint16 channel,uint16 pretrigger);
void process_samples_offline(void);
void handle_pdb_seq_err(void);

// ASSUME: IPBus clock is 48MHz
void main (void)
{
  char  ch;
  
  printf("Hello Kinetis!\r\n");
  printf("Welcome to PDB trigger ADC demo!\r\n");     //��ӭʹ��  �ɱ����ʱģ��PDB ���� ADCģ��ת��
  printf("Please make sure signal on ADC1 channel1 is ready, press any key to continue...!");//������ADC1ͨ��1
  ch = in_char();
  printf("\r\n");
  
  // Initialize global variables  ��ʼ��ȫ�ֱ���
  wSamplesCounter = 0;
  bSamplesReady = 0;
  bSeqErr = 0;
  
  /* Turn on the ADC0 and ADC1 clocks as well as PDB clock  
   */
  //�� ADC0 ��ADC1 ��PDB ʱ��
  SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK | SIM_SCGC6_PDB_MASK);
  SIM_SCGC3 |= (SIM_SCGC3_ADC1_MASK );

  
/*Ƕ�������жϿ����� Nested Vectored Interrupt Controller (NVIC)*/ 
  
  
 //enable adc0 interrupt --73
 //enable adc1 interrupt --74
 /*
  adc0 interrupt vector number 73
 IRQ value: 73-16 = 57
 IRQ VEC reg: 57/32 = 1.xx
 bit to set : 57%32 = 25
 */
 NVICICPR1      |=  (1<<25);        //Enable interrupts for adc0            ����κι���� ADC0 �ж�    NVIC_ICPR0 - NVIC_ICPR7 ���ж��������Ĵ��� Interrupt Clear-Pending Registers
 NVICISER1      |=  (1<<25);        //Clear any pending interrupts on adc0  ʹ�� ADC0 �ж�    �ж�ʹ�ܼĴ���   Interrupt Set-Enable Registers
 NVICICPR1      |=  (1<<26);        //Enable interrupts for adc1       
 NVICISER1      |=  (1<<26);        //Clear any pending interrupts on adc1  
 /*
 ������Щ�Ĵ��������Կ� ��cortex-M4 renference manual.pdf�� ����ĵ� P67
 */
 
 
 
 
 /*
  PDB interrupt vector number 88
 IRQ value: 88-16 = 72
 IRQ VEC reg: 72/32 = 2.xx
 bit to set : 72%32 = 8
 */
 NVICICPR2      =  (1<<8);        //Enable interrupts for PDB             ʹ�� PDB �ж�
 NVICISER2      =  (1<<8);        //Clear any pending interrupts on PDB

 adc_init();          //��ʼ��ADC
 pdb_init();          //��ʼ��PDB
 while(1){
  
   if(bSamplesReady)
   {
     // process samples
     bSamplesReady = 0;     
    // DisableInterrupt();
     process_samples_offline();
    // EnableInterrupt();
   }  
   if(bSeqErr)
   {
     //handle_pdb_seq_err();
   } 
 };
}


void process_samples_offline(void)
{
  int i;
  
 // printf("Output of samples in current sampling period:\r\n");  //�ڵ�ǰ�������ڵ����������
//  printf("=============================================\r\n");
  for(i=0;i<MAX_SAMPLES;i++) {
    printf("\r%#05d",wSamplesBufOffline[i]);
    }  
 // printf("=============================================\r\n");
}

void handle_pdb_seq_err(void)
{
  printf("Error: PDB Sequence error!\r\n"); //����PDB˳�����
}


void adc_init(void)
{
   uint8_t cal_ok;
   
  /* 48MHz IPBus clock
   * ADC clock = 48M/2/8 = 3MHz
   * Total conversion time: 56N+4ADCK
   * Given sampling rate Fs = 6.4K, 156us/sample,  156*3= 468 ADCK
   * the maximum h/w average number = 8
   * use h/w average number = 4
   * Total conversion time: 56*4+4 = 228 ADC clocks,76us
   * There are 468-228 = 240 ADC clocks (ie. 80us) free for post processing
   */
 
  // Initialize ADC1                            ��ʼ��ADC1
  // Do calibration first with 32 h/w averages  ����32�ε�ƽ��ֵУ׼
  Master_Adc_Config.CONFIG1  = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_8) | ADLSMP_LONG | ADC_CFG1_MODE(MODE_16)
                              | ADC_CFG1_ADICLK(ADICLK_BUS_2);  
  Master_Adc_Config.CONFIG2  = MUXSEL_ADCA | ADACKEN_DISABLED | ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20) ;
  Master_Adc_Config.COMPARE1 = 0x1234u ; 
  Master_Adc_Config.COMPARE2 = 0x5678u ;
  Master_Adc_Config.STATUS2  = ADTRG_SW | ACFE_DISABLED | ACFGT_GREATER | ACREN_ENABLED | DMAEN_DISABLED | ADC_SC2_REFSEL(REFSEL_EXT);
  Master_Adc_Config.STATUS3  = CAL_OFF | ADCO_SINGLE | AVGE_ENABLED | ADC_SC3_AVGS(AVGS_32);
  Master_Adc_Config.PGA      = PGAEN_DISABLED | PGACHP_NOCHOP | PGALP_NORMAL | ADC_PGA_PGAG(PGAG_64);
  Master_Adc_Config.STATUS1A = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);       
  Master_Adc_Config.STATUS1B = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);  
  
  ADC_Config_Alt(ADC1_BASE_PTR, &Master_Adc_Config);  // config ADC             //���� ADC
  cal_ok = ADC_Cal(ADC1_BASE_PTR);                    // do the calibration     //У׼
  ADC_Read_Cal(ADC1_BASE_PTR,&CalibrationStore[1]);   // store the cal          //�洢У׼
  
  // Now do normal ADC configuration with 4 h/w averages and h/w trigger from PDB  ������ 4��ƽ��ֵ �� ͨ��PDB����
  Master_Adc_Config.CONFIG1  = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_8) | ADLSMP_LONG | ADC_CFG1_MODE(MODE_16)
                              | ADC_CFG1_ADICLK(ADICLK_BUS_2);  
  Master_Adc_Config.CONFIG2  = MUXSEL_ADCA | ADACKEN_ENABLED | ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20) ;
  Master_Adc_Config.COMPARE1 = 0x1234u ; 
  Master_Adc_Config.COMPARE2 = 0x5678u ;
  Master_Adc_Config.STATUS2  = ADTRG_HW | ACFE_DISABLED | ACFGT_GREATER | ACREN_DISABLED | DMAEN_DISABLED | ADC_SC2_REFSEL(REFSEL_EXT);
  Master_Adc_Config.STATUS3  = CAL_OFF | ADCO_SINGLE | AVGE_ENABLED | ADC_SC3_AVGS(AVGS_4);
  Master_Adc_Config.PGA      = PGAEN_DISABLED | PGACHP_NOCHOP | PGALP_NORMAL | ADC_PGA_PGAG(PGAG_64);

  Master_Adc_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(0); 
  
#define   USE_ADC_SINGLE_MODE  
#if defined(USE_ADC_SINGLE_MODE)
  // use single end for others
  Master_Adc_Config.STATUS1B = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(20);    // ADC1_DM1
#else  
  // use differential mode for channel 1 on tower boards
  Master_Adc_Config.STATUS1B = AIEN_ON | DIFF_DIFFERENTIAL | ADC_SC1_ADCH(1);  
#endif
  
  ADC_Config_Alt(ADC1_BASE_PTR, &Master_Adc_Config);  // config the ADC again to default conditions   ���°�Ĭ�ϵ�����ADC
  
 
 // Initialize ADC0
  // Do calibration first with 32 h/w averages
  Master_Adc_Config.CONFIG1  = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_8) | ADLSMP_LONG | ADC_CFG1_MODE(MODE_16)
                              | ADC_CFG1_ADICLK(ADICLK_BUS_2);  
  Master_Adc_Config.CONFIG2  = MUXSEL_ADCA | ADACKEN_ENABLED | ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20) ;
  Master_Adc_Config.COMPARE1 = 0x1234u ; 
  Master_Adc_Config.COMPARE2 = 0x5678u ;
  Master_Adc_Config.STATUS2  = ADTRG_SW | ACFE_DISABLED | ACFGT_GREATER | ACREN_ENABLED | DMAEN_DISABLED | ADC_SC2_REFSEL(REFSEL_EXT);
  Master_Adc_Config.STATUS3  = CAL_OFF | ADCO_SINGLE | AVGE_ENABLED | ADC_SC3_AVGS(AVGS_32);
  Master_Adc_Config.PGA      = PGAEN_DISABLED | PGACHP_NOCHOP | PGALP_NORMAL | ADC_PGA_PGAG(PGAG_64);
  Master_Adc_Config.STATUS1A = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);       
  Master_Adc_Config.STATUS1B = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);  
  
  ADC_Config_Alt(ADC0_BASE_PTR, &Master_Adc_Config);  // config ADC
  cal_ok = ADC_Cal(ADC0_BASE_PTR);                    // do the calibration
  ADC_Read_Cal(ADC0_BASE_PTR,&CalibrationStore[1]);   // store the cal
  
  // Now do normal ADC configuration with 4 h/w averages and h/w trigger from PDB
  Master_Adc_Config.CONFIG1  = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_8)        | ADLSMP_LONG | ADC_CFG1_MODE(MODE_16) | ADC_CFG1_ADICLK(ADICLK_BUS_2); 
                             //  ��������   | 8��Ƶ��ʱ��Ƶ��Ϊ����ʱ��/8  | ������ʱ��  | 16λת��               | ����ʱ��Ϊ����ʱ�ӳ���2
  
  Master_Adc_Config.CONFIG2  = MUXSEL_ADCA    | ADACKEN_ENABLED | ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20) ;
                            //ѡ�� ADxxa ͨ�� |�첽ʱ�����ʹ�� |  ����ת��ѡ�� |Ĭ�������ʱ�� (20�����ADCK����;�ܹ�24 ADCK����).
  
  Master_Adc_Config.COMPARE1 = 0x1234u ;  //�Ƚ�ֵ1
  Master_Adc_Config.COMPARE2 = 0x5678u ;  //�Ƚ�ֵ2
  
  Master_Adc_Config.STATUS2  = ADTRG_HW | ACFE_DISABLED | ACFGT_GREATER | ACREN_DISABLED | DMAEN_DISABLED | ADC_SC2_REFSEL(REFSEL_EXT);
                            //Ӳ������  | ���αȽϹ���  | ʹ�ܱȽϴ���  |   ֻ�Ƚ�CV1    |      ����DMA   | �ο���ѹѡ�� Ĭ�ϲο���Դ�ܽ�(�ⲿ V REFH �� V REFL )   
  
  Master_Adc_Config.STATUS3  = CAL_OFF | ADCO_SINGLE | AVGE_ENABLED | ADC_SC3_AVGS(AVGS_4);
                            // У׼�ر�|  ������ת�� |ʹ��Ӳ��ƽ��ֵ|   4��������ƽ��ֵ
  
  Master_Adc_Config.PGA      = PGAEN_DISABLED | PGACHP_NOCHOP | PGALP_NORMAL | ADC_PGA_PGAG(PGAG_64);
                            //     ����PGA    |               | ������Դģʽ |     PGA�����趨
  
  Master_Adc_Config.STATUS1A = AIEN_ON        |   DIFF_SINGLE  | ADC_SC1_ADCH(0);
                            // ת����ɺ��ж� |��ת��(�ǲ�ģ)| DADP0��Ϊ����
  
  Master_Adc_Config.STATUS1B = AIEN_ON        | DIFF_SINGLE     | ADC_SC1_ADCH(1);    
                            //                                  | DADP1��Ϊ���� 

  ADC_Config_Alt(ADC0_BASE_PTR, &Master_Adc_Config);  // config the ADC again to default conditions    

}



void pdb_init(void)
{
 int i;

 //use ptd10 11 to indicate program status
 //enable ptd10 11 as rgpio funciton
 PORTD_PCR11  = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;      //��PD10 ��PD11 ��Ϊ ��ͨIO
 PORTD_PCR10  = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;
 
 //enable ptd10 11 as output
 GPIOD_PDDR  |= 0xC00;
 //output 0
 GPIOD_PDOR =0;
 
  //-------------------------pdb setup---------------------------------
 //1.Set to software trigger input                      ���������������
 //2.Enable pdb module                                  ����PDBģʽ
 //3.Prescale set to 0                                  Ԥ��Ƶ��Ϊ0
 //4. LDMOD = 0: load immediately after setting LDOK    ����LDMODΪ0�����ú���������
 //5. enable PDB sequence error interrupt               ���� PDB ˳������ж�
 //Only enable pdb module then you can write in all channel delay value ������ PDB ģʽ ��Ȼ�������д����ͨ����ʱֵ
 
 PDB0_SC = 0x00000000;
 
 // NOTE: PDB must be enabled before writting to buffered registers
 // otherwise, it will be always previous value.
 //Enable PDB
 PDB0_SC |= PDB_SC_PDBEN_MASK;
 
 PDB0_SC |= PDB_SC_TRGSEL(0xF);
   
  //Enable continuous mode
 PDB0_SC |= PDB_SC_CONT_MASK;
 

 //Enable pre-trigger out to ADC0/1
 PDB0_CH0C1 = PDB_C1_TOS(3)|PDB_C1_EN(3);
 PDB0_CH1C1 = PDB_C1_TOS(3)|PDB_C1_EN(3);
 
 //Set CH0 DLY0/1
 PDB0_CH0DLY0 = 0;
 PDB0_CH0DLY1 = 3648;    // delay the conversion time = 76us
 
 //Set CH1 DLY0/1
 PDB0_CH1DLY0 = 0;
 PDB0_CH1DLY1 = 3648;    // delay the conversion time = 76us
 
 //Set PDB_MOD to 156us corresponding to sampling frequency Fs=6.4KHz
 PDB0_MOD = 7488;
 
 //Set interrupt delay value 
 PDB0_IDLY = 7488;
 
 //Load delay value
 PDB0_SC |= PDB_SC_LDOK_MASK;
 
 //Enable interrupt 
 PDB0_SC |= PDB_SC_PDBIE_MASK;
 PDB0_SC |= PDB_SC_PDBEIE_MASK;

 //Set software trigger
 PDB0_SC |= PDB_SC_SWTRIG_MASK;
 }
 

void adc0_isr(void){
  uint16 adc_sample0;
  uint16 adc_sample1;  

  if((ADC0_SC1A & ADC_SC1_COCO_MASK)!= 0){
     GPIOD_PTOR  = 0x400; //toggle 10
     GPIOD_PTOR  = 0x400; //toggle 10
     adc_sample0 = ADC0_RA;
     
  }
 
  if((ADC0_SC1B & ADC_SC1_COCO_MASK)!= 0){
    GPIOD_PTOR  = 0x800; //toggle 11
    GPIOD_PTOR  = 0x800; //toggle 11
    adc_sample1 = ADC0_RB;
  }
     
}

void adc1_isr(void){
  uint16 adc_sample0;
  uint16 adc_sample1;  
  int16  i;
  
  if((ADC1_SC1A&ADC_SC1_COCO_MASK)!= 0){
     GPIOD_PTOR  = 0x400; //toggle 10
     GPIOD_PTOR  = 0x400; //toggle 10
     adc_sample0 = ADC1_RA;
  }
  if((ADC1_SC1B&ADC_SC1_COCO_MASK)!= 0){
    GPIOD_PTOR  = 0x800; //toggle 11
    GPIOD_PTOR  = 0x800; //toggle 11
    adc_sample1 = ADC1_RB;

   
    wSamplesBuf[wSamplesCounter++] = adc_sample1;   
    if(wSamplesCounter == MAX_SAMPLES) {

      // Copy samples to offline backup buf
      for(i=0;i<MAX_SAMPLES;i++) {
        wSamplesBufOffline[i]=wSamplesBuf[i];
      }
   
      bSamplesReady = 1;
      wSamplesCounter = 0;
    }        
  }
}


void pdb_isr(void) 
{
   // Check if pdb sequence error
   if(PDB0_CH0S & PDB_S_ERR(1))
   {
      pdb_seq_error(0,0); // pretrigger 0 sequence error
   }
   if(PDB0_CH0S & PDB_S_ERR(2))
   {
      pdb_seq_error(0,1); // pretrigger 1 sequence error
   }
   if(PDB0_CH1S & PDB_S_ERR(1))
   {
      pdb_seq_error(1,0); // pretrigger 0 sequence error
   }
   if(PDB0_CH1S & PDB_S_ERR(2))
   {
      pdb_seq_error(1,1); // pretrigger 1 sequence error
   }   
   if(PDB0_SC & PDB_SC_PDBIF_MASK)
   {
      PDB0_SC &= ~PDB_SC_PDBIF_MASK;  // write 0 to PDBIF to clear which is different from EM256
   }
}

 
void pdb_seq_error(uint16 channel,uint16 pretrigger)
{
    bSeqErr++;
    if(channel==1)
    {
      PDB0_CH1S &= ~PDB_S_ERR(1<<pretrigger);//clear interrupt flag   
    }
    else if(channel==0)
    {
      PDB0_CH0S &= ~PDB_S_ERR(1<<pretrigger);//clear interrupt flag      
    }
}
/********************************************************************/
