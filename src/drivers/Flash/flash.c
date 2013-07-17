/****************************
Written by John Ching 
HKUST Robotics Team
Smartcar 2013
*****************************/

#include "common.h"
#include  "flash.h"

void store_u32_to_flashmem1(u32 number){
  u8 Send_Flash[6];
  Flash_erase_sector(100);
  encode_u32_to_string(number,Send_Flash);
  Flash_write(100,0,6,Send_Flash);  
}
  
u32 get_u32_from_flashmem1(){
  u8 Receive_Flash[6];
  Flash_read(100,0,6,Receive_Flash);
  return decode_string_to_u32(Receive_Flash);
}

void store_u32_to_flashmem2(u32 number){
  u8 Send_Flash[6];
  Flash_erase_sector(101);
  encode_u32_to_string(number,Send_Flash);
  Flash_write(101,0,6,Send_Flash);  
}
  
u32 get_u32_from_flashmem2(){
  u8 Receive_Flash[6];
  Flash_read(101,0,6,Receive_Flash);
  return decode_string_to_u32(Receive_Flash);
}


void encode_u32_to_string(u32 number,u8 buf[]){
    buf[0]=number>>24;
    buf[1]=number>>16;
    buf[2]=number>>8;
    buf[3]=number;
}

u32 decode_string_to_u32(u8*bBuf){
  u32 decoded=0;
  decoded+=bBuf[0]<<24;
  decoded+=bBuf[1]<<16;
  decoded+=bBuf[2]<<8;
  decoded+=bBuf[3];
  
  return decoded;
}

void Flash_init(void)
{
    FMC_PFB0CR |= FMC_PFB0CR_S_B_INV_MASK;       // ??Flash??????
    
    while(!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK));    // ??????
    
    FTFL_FSTAT = (0 | FTFL_FSTAT_ACCERR_MASK      // ?????????
                    | FTFL_FSTAT_FPVIOL_MASK);    
}


u8 Flash_erase_sector(u16 sectorNo)
{
    union
    {
        u32  word;
        u8   byte[4];
    } dest;
    
    dest.word    = (u32)(sectorNo*(1<<11));

    FTFL_FCCOB0 = 0x09; //????
    
    // ??????
    FTFL_FCCOB1 = dest.byte[2];
    FTFL_FCCOB2 = dest.byte[1];
    FTFL_FCCOB3 = dest.byte[0];
    
    // ??????
    if(1 == Flash_cmd_launch())    //?????????
        return 1;     //??????
   
    // ???sector0?,?????
    if(dest.word <= 0x800)
    {
        
        FTFL_FCCOB0 = 0x06; // ??4??
        // ??????
        FTFL_FCCOB1 = 0x00;
        FTFL_FCCOB2 = 0x04;
        FTFL_FCCOB3 = 0x0C;
        // ??
        FTFL_FCCOB4 = 0xFF;
        FTFL_FCCOB5 = 0xFF;
        FTFL_FCCOB6 = 0xFF;
        FTFL_FCCOB7 = 0xFE;
        // ??????
        if(1 == Flash_cmd_launch())  //?????????
            return 2;   //??????
    }  
    
    return 0;  //????
}



u8 Flash_read(u16 sectNo,u16 offset,u16 cnt,u8*bBuf)
{
    u32 wAddr = 0;
    wAddr = sectNo * 2048 + offset;
    while (cnt--)
        *bBuf++=*(u8*)wAddr++;
   return TRUE;
}


u8 Flash_write(u16 sectNo,u16 offset,u16 cnt,u8 buf[])
{
    u32 size;
    u32 destaddr;
    
    union
    {
        u32   word;
        u8  byte[4];
    } dest;
    
    if(offset%4 != 0)
        return 1;   //??????,??????(4????)
    
    // ??????
    FTFL_FCCOB0 = 0x06;
    destaddr = (u32)(sectNo*(1<<11) + offset);//????
    dest.word = destaddr;
    for(size=0; size<cnt; size+=4, dest.word+=4, buf+=4)
    {
        // ??????
        FTFL_FCCOB1 = dest.byte[2];
        FTFL_FCCOB2 = dest.byte[1];
        FTFL_FCCOB3 = dest.byte[0];
 
        // ????
        FTFL_FCCOB4 = buf[3];
        FTFL_FCCOB5 = buf[2];
        FTFL_FCCOB6 = buf[1];
        FTFL_FCCOB7 = buf[0];
        
        if(1 == Flash_cmd_launch()) 
            return 2;  //??????
    }
    
    return 0;  //????
}


u32 Flash_cmd_launch(void)
{
    // ?????????????????
    FTFL_FSTAT = (1<<5) | (1<<4);
    
    // ????
    FTFL_FSTAT = (1<<7);

    // ??????
    while(!(FTFL_FSTAT &(1<<7)));

    // ??????
    if(FTFL_FSTAT & ((1<<5) | (1<<4) | 1))
        return 1 ; //??????
  
    return 0; //??????
}