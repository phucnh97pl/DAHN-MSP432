#include "msp.h"
#include "driverlib.h"
#include "spi.h"
#include "math.h"
#include "stdio.h"
#include "string.h"
// LIGHT, GND   : GND
// VCC          : 3.3V
// CLK          : P1.5
// DIN          : P1.6
// DC           : P6.7
// CE           : P6.0
// RST          : P6.6

#define CE    0x01
#define RESET 0x40
#define DC    0x80

/* define the pixel size of display */
#define GLCD_WIDTH  84
#define GLCD_HEIGHT 48

void GLCD_setCursor(unsigned char x, unsigned char y);
void GLCD_clear(void);
void GLCD_init(void);
void GLCD_data_write(unsigned char data);
void GLCD_command_write(unsigned char data);
void SPI_init(void);
void SPI_write(unsigned char data);
void init_matrix(void);
void set_point(int x, int y);
void rectangle(int x1, int x2, int y1, int y2);
void GLCD_putchar(void);
void GLCD(int c);
void circle(int x0, int y0, int r);
void duong_nam_doc(int dodai, int x, int y);
void duong_cheo(int dodai, int x, int y);
void duong_nam_ngang(int dodai, int x, int y);
void rectangle(int x1, int y1, int x2, int y2);
void circle(int x0, int y0, int r);
void write_string(char abc[]);
void write_char(char a);
void delay(int n);

uint8_t m[6][GLCD_WIDTH];

const char font_table[][6] =
{{0x7e, 0x11, 0x11, 0x11, 0x7e, 0}, // 41 A
{0x7f, 0x49, 0x49, 0x49, 0x36, 0}, // 42 B
{0x3e, 0x41, 0x41, 0x41, 0x22, 0}, // 43 C
{0x7f, 0x41, 0x41, 0x22, 0x1c, 0}, // 44 D
{0x7f, 0x49, 0x49, 0x49, 0x41, 0}, // 45 E
{0x7f, 0x09, 0x09, 0x09, 0x01, 0}, // 46 F
{0x3e, 0x41, 0x49, 0x49, 0x7a, 0}, // 47 G
{0x7f, 0x08, 0x08, 0x08, 0x7f, 0}, // 48 H
{0x00, 0x41, 0x7f, 0x41, 0x00, 0}, // 49 I
{0x20, 0x40, 0x41, 0x3f, 0x01, 0}, // 4a J
{0x7f, 0x08, 0x14, 0x22, 0x41, 0}, // 4b K
{0x7f, 0x40, 0x40, 0x40, 0x40, 0}, // 4c L
{0x7f, 0x02, 0x0c, 0x02, 0x7f, 0}, // 4d M
{0x7f, 0x04, 0x08, 0x10, 0x7f, 0}, // 4e N
{0x3e, 0x41, 0x41, 0x41, 0x3e, 0}, // 4f O
{0x7f, 0x09, 0x09, 0x09, 0x06, 0}, // 50 P
{0x3e, 0x41, 0x51, 0x21, 0x5e, 0}, // 51 Q
{0x7f, 0x09, 0x19, 0x29, 0x46, 0}, // 52 R
{0x46, 0x49, 0x49, 0x49, 0x31, 0}, // 53 S
{0x01, 0x01, 0x7f, 0x01, 0x01, 0}, // 54 T
{0x3f, 0x40, 0x40, 0x40, 0x3f, 0}, // 55 U
{0x1f, 0x20, 0x40, 0x20, 0x1f, 0}, // 56 V
{0x3f, 0x40, 0x38, 0x40, 0x3f, 0}, // 57 W
{0x63, 0x14, 0x08, 0x14, 0x63, 0}, // 58 X
{0x07, 0x08, 0x70, 0x08, 0x07, 0}, // 59 Y
{0x61, 0x51, 0x49, 0x45, 0x43, 0}};// 5a Z

int main(void) {
    GLCD_init();        /* initialize the GLCD controller */
    GLCD_clear();      /* clear display and home the cursor */
    init_matrix();
//    duong_cheo(40,3,8);
//    duong_nam_ngang(40,6,8);
//    rectangle(2,2, 24, 47);
//    duong_nam_doc(32, 3, 8);
    int i=0;
    int j;
    while(1) {
        for (;i<48;i++){
            if (i==47){
                i=0;
            }
            circle(i, 16, 5);
            GLCD_putchar();
            delay(10);
            GLCD_clear();
        }
    }
}

void delay(int n){
    int i;
    int j;
    for (i=0;i<n; i++ ){
        for (j =0; j<700; j++){
            int z=0;
        }
    }
}

void init_matrix(void){
    int i;
    int j;
    for (i= 0 ; i <6; i++){
        for (j = 0; j < GLCD_WIDTH; j++){
            m[i][j] = 0x00;
        }
    }
}

void set_point(int x, int y){
    int a = x/8;
    int b = x%8;
    m[a][y] = m[a][y] | (uint8_t)(1<<b);
}


void GLCD_putchar(void) {
    int i;
    int j;
    for(j = 0; j <6; j++ )
        for(i = 0; i<GLCD_WIDTH; i++)
            GLCD_data_write( m[j][i]);
}

void GLCD_setCursor(unsigned char x, unsigned char y) {
    GLCD_command_write(0x80 | x);  /* column */
    GLCD_command_write(0x40 | y);  /* bank (8 rows per bank) */
}

/* clears the GLCD by writing zeros to the entire screen */
void GLCD_clear(void) {
    int32_t index;
    for (index = 0 ; index < (GLCD_WIDTH * GLCD_HEIGHT / 8) ; index++)
        GLCD_data_write(0x00);

    GLCD_setCursor(0, 0); /* return to the home position */
}

/* send the initialization commands to PCD8544 GLCD controller */
void GLCD_init(void) {
    SPI_init();

    /* hardware reset of GLCD controller */
    P6->OUT |= RESET;               /* deassert reset */

    GLCD_command_write(0x21);       /* set extended command mode */
    GLCD_command_write(0xB8);       /* set LCD Vop for contrast */
    GLCD_command_write(0x04);       /* set temp coefficient */
    GLCD_command_write(0x14);       /* set LCD bias mode 1:48 */
    GLCD_command_write(0x20);       /* set normal command mode */
    GLCD_command_write(0x0C);       /* set display normal mode */
}

/* write to GLCD controller data register */
void GLCD_data_write(unsigned char data) {
    P6->OUT |= DC;                  /* select data register */
    SPI_write(data);                /* send data via SPI */
}

/* write to GLCD controller command register */
void GLCD_command_write(unsigned char data) {
    P6->OUT &= ~DC;                 /* select command register */
    SPI_write(data);                /* send data via SPI */
}

void SPI_init(void) {
    EUSCI_B0->CTLW0 = 0x0001;       /* put UCB0 in reset mode */
    EUSCI_B0->CTLW0 = 0x69C1;       /* PH=0, PL=1, MSB first, Master, SPI, SMCLK */
    EUSCI_B0->BRW = 3;              /* 3 MHz / 3 = 1 MHz */
    EUSCI_B0->CTLW0 &= ~0x0001;     /* enable UCB0 after config */

    P1->SEL0 |= 0x60;               /* P1.5, P1.6 for UCB0 */
    P1->SEL1 &= ~0x60;

    P6->DIR |= CE | RESET | DC;     /* P6.7, P6.6, P6.0 set as output */
    P6->OUT |= CE;                  /* CE idle high */
    P6->OUT &= ~RESET;              /* assert reset */
}

void SPI_write(unsigned char data) {
    P6->OUT &= ~CE;                 /* assert /CE */
    EUSCI_B0->TXBUF = data;         /* write data */
    while(EUSCI_B0->STATW & 0x01) ; /* wait for transmit done */
    P6->OUT |= CE;                  /* deassert /CE */
}

void duong_nam_doc(int dodai, int x, int y){//hang,cot
    int i;
    for (i = x; i < (x+dodai); i++){
        set_point(i,y);
    }
}

void duong_cheo(int dodai, int x, int y){//hang,cot
    int i;
    for (i = x; i < (x+dodai); i++){
        set_point(x+i,y+i);
    }

}

void duong_nam_ngang(int dodai, int x, int y){// hang, cot
    int i;
    for (i = y; i < (y+dodai); i++){
        set_point(x,i);
    }
}

void rectangle(int x1, int y1, int x2, int y2){
    int i, j;
    for (i = x1; i <= x2; i++){
        set_point(i,y1);
        set_point(i,y2);
    }
    for (j = y1; j <= y2; j++){
        set_point(x1,j);
        set_point(x2,j);
    }
}

void circle(int x0, int y0, int r){
    int x1 = ((x0-r)>0) ? x0-r:0;
    int x2 = ((x0+r)<GLCD_HEIGHT) ? x0+r:GLCD_HEIGHT;
    int y1 = ((y0-r)>0) ? y0-r:0;
    int y2 = ((y0+r)<GLCD_WIDTH) ? y0+r:GLCD_WIDTH;
    int i,j;
    for( i=x1; i<=x2; i++){
        for( j=y1; j<=y2; j++){
            if((int)(sqrt(pow((i-x0),2)+pow((j-y0),2)) + 0.5) == r){
                set_point(i,j);
            }
        }
    }
}

void write_string(char abc[]){
    int a;
    for (a = 0; a < strlen(abc); a++){
        write_char(abc[a]);
    }
}

void write_char(char a){
    if (a == 'a' || a == 'A'){
        GLCD(0);
    }
    else if (a == 'b' || a == 'B'){
        GLCD(1);
    }
}

void GLCD(int c) {
    int i;
    for (i = 0; i < 6; i++)
        GLCD_data_write(font_table[c][i]);
}
