/****************************
Written by John Ching 
HKUST Robotics Team
Smartcar 2013
*****************************/

void Flash_init(void);//initializes flash memory for read/write

//get and set memspace 1
void store_u32_to_flashmem1(u32 number);
u32 get_u32_from_flashmem1();

//get and set memspace 2
void store_u32_to_flashmem2(u32 number);
u32 get_u32_from_flashmem2();



u8 Flash_erase_sector(u16 sectorNo);
u8 Flash_write(u16 sectNo,u16 offset,u16 cnt,u8 buf[]);
u32 Flash_cmd_launch(void);
u8 Flash_read(u16 sectNo,u16 offset,u16 cnt,u8*bBuf);
void encode_u32_to_string(u32 number,u8 buf[]);
u32 decode_string_to_u32(u8*bBuf);