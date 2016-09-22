void io_hlt(void);
void io_cli(void);
void io_sti(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler21(void);
void asm_inthandler2c(void);

#define ADR_BOOTINFO	0x00000ff0

typedef struct BOOTINFO{
	char cyls;
	char leds;
	char videomode;
	char reserve;
	short screen_xsize;
	short screen_ysize;	
	char* address_vram;
}BOOTINFO;
