#include <stdbool.h> 
 #include <stdio.h> 
 #include <stdlib.h> 


#define BOARD                 "DE1-SoC"

/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_ONCHIP_END       0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define AV_CONFIG_BASE        0xFF203000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE        0xFF709000
#define HPS_TIMER0_BASE       0xFFC08000
#define HPS_TIMER1_BASE       0xFFC09000
#define HPS_TIMER2_BASE       0xFFD00000
#define HPS_TIMER3_BASE       0xFFD01000
#define FPGA_BRIDGE           0xFFD0501C

/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs



  
volatile int pixel_buffer_start; // global variable  
void erase_character(int x);
void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void draw_character(int x);
void draw_zombie(int startx, int starty, int dy);
void erase_zombie(int startx, int starty, int dy);
void wait_for_vsync();
extern short START [240][320];
extern short END [240][320];


struct bullet{
    int inx;
    int iny;
    int speed;
};


int main(void)
{	
 	
	
	volatile short * pixelbuf = 0xc8000000;
	volatile int * key_base = (int *)0xFF200050;     // point to KEY address

   volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                      // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	int erase =0;
    int zx1 = 20; int zy = 2; int zx2=150; int zx3=280;  
    clear_screen();
	int ii, jj; 
     for (ii=0; ii<240; ii++) {
         for (jj=0; jj<320; jj++) {
         *(pixelbuf + (ii<<0) + (jj<<9)) = START[ii][jj]; 
		 }
	 }
	 bool start = false;
	 while (1){
     if (*key_base & 0x01){
		start = true; 
		break;
		
	 }
	 }
	 clear_screen();
	 wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	 while (start){
		 clear_screen();
    int lane = rand() % 3;
    draw_character(0);
    if (lane == 0){
    	draw_zombie(zx1,zy,0);
    }
    if (lane == 1){
        draw_zombie(zx2,zy,0);
    }
    if (lane == 2){
        draw_zombie(zx3,zy,0);
    }
    int dy1 =1; 
    int by1 =0; 
	int offset = 0;
	int behind = 0;
	int pos = 1;
	
    while(1){
    	erase_zombie(zx1,zy,by1);
        erase_zombie(zx2,zy,by1);
        erase_zombie(zx3,zy,by1);
		erase_character(offset);
		erase_character(behind);
		draw_character(offset);
		
        if (erase == 1){
            erase_zombie(zx1,zy,180);
            erase_zombie(zx2,zy,180);
            erase_zombie(zx3,zy,180);
            erase = 0;
        }
        by1 +=4;
        
		if (zy + dy1 > 140){
			if (lane == 0 && pos == 0){
				break;
			}
			else if (lane == 1 && pos == 1){
				break;
			}
			else if (lane == 2 && pos == 2){
				break;
			}
		}
        if(zy+dy1 > 200){
            erase_zombie(zx1,zy,180);
            erase_zombie(zx2,zy,180);
            erase_zombie(zx3,zy,180);

            lane = rand() % 3;
            dy1 =1; by1 =0; zy =2;
            erase=1;
    	}

    if (lane == 0){
    	draw_zombie(zx1,zy,dy1);
    }
    if (lane == 1){
        draw_zombie(zx2,zy,dy1);
    }
    if (lane == 2){
        draw_zombie(zx3,zy,dy1);
    }
	behind = offset;
	if (*key_base & 0x04){
		offset =0;
		pos =1;
	}
	else if (*key_base & 0x08){
		offset = 140;
		pos = 0;
	}
	else if (*key_base & 0x02){
		offset = -140;
		pos = 2;
	}
	erase_character(behind);
	draw_character(offset);
        dy1+=4; 
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
	start = false;
	 }
	int i, j; 
     for (i=0; i<240; i++) {
         for (j=0; j<320; j++) {
         *(pixelbuf + (i<<0) + (j<<9)) = END[i][j]; 
		 }
	 }
    
}
void wait_for_vsync() {
    volatile int * ctrl =(int *)0XFF203020;
    register int status;
    
    *ctrl = 1;
    
    status = *(ctrl+3);
    while ((status & 0x01) != 0){
        status = *(ctrl + 3);
    }
}

void draw_zombie(int startx, int starty, int dy){
    int row, col;
    //legL
    for (row= startx; row<startx+9; row++){
        for (col = starty+42+dy ; col<starty+56+dy; col++){
            plot_pixel(row,col,0x4800);
        }
    } 
    //legR
    for (row= startx +11; row<startx+20; row++){
        for (col = starty+42+dy ; col<starty+56+dy; col++){
            plot_pixel(row,col,0x4800);
        }
    }
    //belt
    for (row= startx; row<startx+20; row++){
        for (col = starty+36+dy ; col<starty+42+dy; col++){
            plot_pixel(row,col,0x4800);
        }
    }
    //body
    for (row= startx; row<startx+20; row++){
        for (col = starty+13+dy ; col<starty+36+dy; col++){
            plot_pixel(row,col,0x4100);
        }
    }
    //arml
    for (row= startx-4; row<startx; row++){
        for (col = starty+13+dy ; col<starty+20+dy; col++){
            plot_pixel(row,col,0x0200);
        }
    }
    for (row= startx-7; row<startx-2; row++){
        for (col = starty+13+dy ; col<starty+33+dy; col++){
            plot_pixel(row,col,0x0200);
        }
    }
    //armr
    for (row= startx+20; row<startx+24; row++){
        for (col = starty+13+dy ; col<starty+20+dy; col++){
            plot_pixel(row,col,0x4100);
        }
    }
    for (row= startx+22; row<startx+27; row++){
        for (col = starty+13+dy ; col<starty+33+dy; col++){
            plot_pixel(row,col,0x0200);
        }
    }
    //neck
    for (row= startx+6; row<startx+13; row++){
        for (col = starty+10+dy ; col<starty+13+dy; col++){
            plot_pixel(row,col,0x8408);
        }
    }
    //head
    for (row= startx+2; row<startx+17; row++){
        for (col = starty+dy ; col<starty+10+dy; col++){
            plot_pixel(row,col,0x8408);
        }
    }
    
}
void erase_zombie(int startx, int starty, int dy){
    int row, col;
    //legL
    for (row= startx-7; row<startx+27; row++){
        for (col = starty-5; col<starty+56+dy; col++){
            plot_pixel(row,col,0x0000);
        }
    }  
}
void erase_character(int x){
    int row, col;
    //legL
    for (row= 152-x; row<161-x; row++){
        for (col = 215 ; col<240; col++){
            plot_pixel(row,col,0x0000);
        }
    } 
    //legR
    for (row= 163-x; row<172-x; row++){
        for (col = 215 ; col<240; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    //belt
    for (row= 152-x; row<172-x; row++){
        for (col = 210 ; col<215; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    //body
    for (row= 152-x; row<172-x; row++){
        for (col = 183 ; col<210; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    //arml
    for (row= 148-x; row<152-x; row++){
        for (col = 183 ; col<190; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    for (row= 145-x; row<150-x; row++){
        for (col = 183 ; col<203; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    //armr
    for (row= 172-x; row<176-x; row++){
        for (col = 183 ; col<190; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    for (row= 174-x; row<179-x; row++){
        for (col = 183 ; col<203; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    //neck
    for (row= 158-x; row<165-x; row++){
        for (col = 180 ; col<183; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    //head
    for (row= 154-x; row<169-x; row++){
        for (col = 170 ; col<181; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    //hat
    for (row= 154-x; row<169-x; row++){
        for (col = 163 ; col<171; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    for (row= 149-x; row<155-x; row++){
        for (col = 166 ; col<171; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    for (row= 169-x; row<175-x; row++){
        for (col = 166 ; col<171; col++){
            plot_pixel(row,col,0x0000);
        }
    }
    
}
void draw_character(int x){
    int row, col;
    //legL
    for (row= 152-x; row<161-x; row++){
        for (col = 215 ; col<237; col++){
            plot_pixel(row,col,0x4800);
        }
    } 
    //legR
    for (row= 163-x; row<172-x; row++){
        for (col = 215 ; col<237; col++){
            plot_pixel(row,col,0x4800);
        }
    }
    //belt
    for (row= 152-x; row<172-x; row++){
        for (col = 210 ; col<215; col++){
            plot_pixel(row,col,0x4800);
        }
    }
    //body
    for (row= 152-x; row<172-x; row++){
        for (col = 183 ; col<210; col++){
            plot_pixel(row,col,0x02f7);
        }
    }
    //arml
    for (row= 148-x; row<152-x; row++){
        for (col = 183 ; col<190; col++){
            plot_pixel(row,col,0x02f7);
        }
    }
    for (row= 145-x; row<150-x; row++){
        for (col = 183 ; col<203; col++){
            plot_pixel(row,col,0x02f7);
        }
    }
    //armr
    for (row= 172-x; row<176-x; row++){
        for (col = 183 ; col<190; col++){
            plot_pixel(row,col,0x02f7);
        }
    }
    for (row= 174-x; row<179-x; row++){
        for (col = 183 ; col<203; col++){
            plot_pixel(row,col,0x02f7);
        }
    }
    //neck
    for (row= 158-x; row<165-x; row++){
        for (col = 180 ; col<183; col++){
            plot_pixel(row,col,0xdccd);
        }
    }
    //head
    for (row= 154-x; row<169-x; row++){
        for (col = 170 ; col<181; col++){
            plot_pixel(row,col,0xdccd);
        }
    }
    //hat
    for (row= 154-x; row<169-x; row++){
        for (col = 163 ; col<171; col++){
            plot_pixel(row,col,0xc000);
        }
    }
    for (row= 149-x; row<155-x; row++){
        for (col = 166 ; col<171; col++){
            plot_pixel(row,col,0xc000);
        }
    }
    for (row= 169-x; row<175-x; row++){
        for (col = 166 ; col<171; col++){
            plot_pixel(row,col,0xc000);
        }
    }
    
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void clear_screen(){
 	int row, col;
    for (row= 0; row<320; row++){
        for (col = 0 ; col<240; col++){
            plot_pixel(row,col,0);
        }
    }
}


