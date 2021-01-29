#include "msp.h"
#include <driverlib.h> //ti/devices/msp432p4xx/driverlib/
#include <stdint.h>
#include <stdbool.h>

// define port SCL 6.5, SDA 6.4
#define PORT_SDA GPIO_PORT_P6
#define PORT_SCL GPIO_PORT_P6
#define PIN_SDA GPIO_PIN4
#define PIN_SCL GPIO_PIN5


void Set_SDA_High ()  
{
    MAP_GPIO_setAsInputPinWithPullUpResistor (PORT_SDA, PIN_SDA); // Sử dụng đầu vào kéo lên để kéo lên dòng SDA
}
 
void Set_SDA_Low ()  
{
    MAP_GPIO_setAsOutputPin (PORT_SDA, PIN_SDA); // Sử dụng chế độ đầu ra để kéo dòng SDA xuống
    MAP_GPIO_setOutputLowOnPin (PORT_SDA, PIN_SDA);
}
void Set_SCL_High ()  
{
    MAP_GPIO_setAsInputPinWithPullUpResistor (PORT_SCL, PIN_SCL);  // Sử dụng đầu vào kéo lên để kéo lên dòng SCL
}
 
void Set_SCL_Low ()  
{
    MAP_GPIO_setAsOutputPin (PORT_SCL, PIN_SCL); // Sử dụng chế độ đầu ra để kéo dòng SCL xuống
    MAP_GPIO_setOutputLowOnPin (PORT_SCL, PIN_SCL);
}

void Set_SDA_HighwhenNack () //  MCU chủ động kéo mức SDA lên khi nó không phản hồi
{
    MAP_GPIO_setAsOutputPin (PORT_SDA, PIN_SDA); // Sử dụng chế độ đầu ra để kéo dòng SDA xuống
    MAP_GPIO_setOutputHighOnPin (PORT_SDA, PIN_SDA);
}

void Start_I2C ()  
{
    Set_SDA_High ();
    Set_SCL_High (); // Dòng SDA và dòng SCL được đặt ở mức cao, làm cho bus I2C đi vào trạng thái rỗi
    Set_SDA_Low ();
    Set_SCL_Low ();
}
 
void Stop_I2C ()  
{
    Set_SDA_Low (); // Đặt SDA ở mức thấp để đảm bảo rằng cạnh tăng có thể xảy ra sau này
    Set_SCL_High ();
    Set_SDA_High ();
}

bool Wait_Ack ()  
{
    Set_SDA_High ();
    Set_SCL_High ();
    Set_SCL_Low ();
    return 1 ;  
}

void Send_Byte ( uint8_t  data) //  Gửi byte
{
    uint8_t i;
    for (i = 0 ; i < 8 ; i ++)
    {
        Set_SCL_Low (); // Kéo xuống dòng đồng hồ
        if(data & 0x80 ) // các data một . 8 -bit ghi tuần tự từ bit quan trọng nhất
        {
            Set_SDA_High ();
        }
        else
            Set_SDA_Low (); // câu lệnh if-else được sử dụng để xác định xem thông tin được truyền bởi dòng dữ liệu là 1 hay 0
        data <<= 1 ; // Logic dữ liệu dịch sang trái một bit
        Set_SCL_High ();
        Set_SCL_Low (); // SCL tạo xung sóng vuông để tạo dữ liệu mẫu 
    }
 
}

// Gửi một byte
// Ghi một byte vào SSD1306 .
// mode: data / command flag  0, nghĩa là lệnh ; 1, có nghĩa là dữ liệu ;
void OLED_WR_Byte ( uint8_t  dat, uint8_t  mode)  
{
  Start_I2C ();
  Send_Byte ( 0x78 );
  Wait_Ack ();
  if(mode) {Send_Byte ( 0x40 );}
  else {Send_Byte ( 0x00 );}
  Wait_Ack ();
  Send_Byte (dat);
  Wait_Ack ();
  Stop_I2C ();
}
 
// Cài đặt tọa độ
void OLED_Set_Pos ( uint8_t  x,  uint8_t  y)  
{
  OLED_WR_Byte ( 0xb0 + y, OLED_CMD);
  OLED_WR_Byte (((x & 0xf0 ) >> 4 ) | 0x10 , OLED_CMD);
  OLED_WR_Byte ((x & 0x0f ), OLED_CMD);
}
 
// Bật OLED hiển thị
void OLED_Display_On ( void )  
{
  OLED_WR_Byte ( 0X8D , OLED_CMD);   // Lệnh SET DCDC
  OLED_WR_Byte ( 0X14 , OLED_CMD);   // DCDC BẬT
  OLED_WR_Byte ( 0XAF , OLED_CMD);   // BẬT MÀN HÌNH
}
 
// Tắt OLED hiển thị
void OLED_Display_Off ( void )  
{
  OLED_WR_Byte ( 0X8D , OLED_CMD);   // Lệnh SET DCDC
  OLED_WR_Byte ( 0X10 , OLED_CMD);   // DCDC TẮT
  OLED_WR_Byte ( 0XAE , OLED_CMD);   // TẮT HIỂN THỊ
}
 
// Chức năng xóa màn hình , sau khi xóa màn hình thì toàn bộ màn hình đen (không sáng)
void OLED_Clear ( void )  
{
  uint8_t  i, n;
  for (i = 0 ; i < 8 ; i ++)
  {
    OLED_WR_Byte ( 0xb0 + i, OLED_CMD);     // Đặt địa chỉ trang ( 0 ~ 7 )
    OLED_WR_Byte ( 0x00 , OLED_CMD);       // Dược bố trí đối diện với vị trí của - dịa chỉ cột thấp
    OLED_WR_Byte ( 0x10 , OLED_CMD);       // Đặt vị trí hiển thị - địa chỉ cột cao
    for (n = 0 ; n < 128 ; n ++) OLED_WR_Byte ( 0 , OLED_DATA);
  }  // Cập nhật màn hình
}
 
// Hiển thị một ký tự ở vị trí được chỉ định , bao gồm một số ký tự
// x: 0 ~ 127
// y: 0 ~ 63
// sizey: chọn phông chữ  6x8 8x16
void OLED_ShowChar ( uint8_t  x, uint8_t  y, uint8_t  chr, uint8_t  sizey)  
{
  uint8_t  c = 0 , sizex = sizey / 2 ;
  uint16_t  i = 0 , size1;
  if (sizey == 8 ) size1 = 6 ;
  else  size1 = (sizey / 8 + ((sizey% 8 )? 1 : 0 )) * (sizey / 2 );
  c = chr- '' ; // Lấy giá trị bù
  OLED_Set_Pos (x, y);
  for (i = 0 ; i <size1; i ++)
  {
    if (i% sizex == 0 && sizey != 8 ) OLED_Set_Pos (x, y++);
    if (sizey == 8 ) OLED_WR_Byte (asc2_0806[c][i], OLED_DATA); // Kích thước phông chữ 6X8
    else if (sizey == 16 ) OLED_WR_Byte (asc2_1608[c][i], OLED_DATA); //  cỡ chữ 8x16
    else return ;  
  }
}
// m ^ n chức năng
uint32_t oled_pow ( uint8_t  m, uint8_t  n)  
{
  uint32_t result = 1 ;
  while (n-) result *= m;
  return result;
}
// Hiển thị số
// x, y: tọa độ điểm bắt đầu
// num: số được hiển thị
// len: số chữ số
// sizey: cỡ chữ
void OLED_ShowNum ( uint8_t  x, uint8_t  y, uint32_t  num, uint8_t  len, uint8_t  sizey)  
{
  uint8_t  t, temp, m = 0 ;
  uint8_t  enshow = 0 ;
  if (sizey == 8 ) m = 2 ;
  for (t = 0 ; t <len; t ++)
  {
    temp = (num / oled_pow ( 10 , len-t -1 ))% 10 ;
    if (enshow == 0 && t <(len -1 ))
    {
      if (temp == 0 )
      {
        OLED_ShowChar (x + (sizey / 2 + m) * t, y, '' , sizey);
        return ;
      } else  enshow = 1 ;
    }
    OLED_ShowChar (x + (sizey / 2 + m) * t, y, temp + '0' , sizey);
  }
}
// Hiển thị một chuỗi ký tự
void OLED_ShowString ( uint8_t  x, uint8_t  y, uint8_t  * chr, uint8_t  sizey)  
{
  uint8_t  j = 0 ;
  while(chr[j]! = '\0' )
  {
    OLED_ShowChar (x, y, chr[j++], sizey);
    if (sizey == 8 ) x += 6 ;
    return x += sizey / 2 ;
  }
}
 
// Hiển thị hình ảnh
// tọa độ hiển thị x, y
// sizex, sizey, chiều dài và chiều rộng hình ảnh
// BMP : hình ảnh được hiển thị
void OLED_DrawBMP ( uint8_t x, uint8_t y, uint8_t sizex,  uint8_t sizey, uint8_t BMP[])  
{
  uint16_t  j = 0 ;
  uint8_t  i, m;
  sizey = sizey / 8 + ((sizey % 8 ) ? 1 : 0 );
  for (i = 0 ; i < sizey; i++)
  {
    OLED_Set_Pos (x, i + y);
    for (m = 0 ; m < sizex; m ++)
    {
      OLED_WR_Byte (BMP[j++], OLED_DATA);
    }
  }
}
 
 
// Khởi tạo SSD1306
void OLED_Init ( void )  
{
  delay_ms ( 200 );
  OLED_WR_Byte ( 0xAE , OLED_CMD); // - TẮT bảng OLED
  OLED_WR_Byte ( 0x00 , OLED_CMD); // --- đặt địa chỉ cột thấp
  OLED_WR_Byte ( 0x10 , OLED_CMD); // --- đặt địa chỉ cột cao
  OLED_WR_Byte ( 0x40 , OLED_CMD); // - đặt địa chỉ dòng bắt đầu Đặt ánh xạ RAM Hiển thị dòng bắt đầu (0x00 ~ 0x3F)
  OLED_WR_Byte ( 0x81 , OLED_CMD); // - đặt thanh ghi điều khiển độ tương phản
  OLED_WR_Byte ( 0xCF , OLED_CMD); // thiết lập độ sáng SEG hiện tại của ngõ  ra
  OLED_WR_Byte ( 0xA1 , OLED_CMD); // - Đặt SEG / Ánh xạ cột 0xa0 đảo trái và phải  0xa1 là bình thường
  OLED_WR_Byte ( 0xC8 , OLED_CMD); // Đặt COM / Hướng quét hàng 0xc0 lên và xuống đã đảo ngược  0xc8 bình thường
  OLED_WR_Byte ( 0xA6 , OLED_CMD); // - đặt màn hình bình thường
  OLED_WR_Byte ( 0xA8 , OLED_CMD); // - đặt tỷ lệ ghép kênh (1 đến 64)
  OLED_WR_Byte ( 0x3f , OLED_CMD); // - nhiệm vụ 1/64
  OLED_WR_Byte ( 0xD3 , OLED_CMD); // - THIẾT LẬP màn hình Ánh xạ RAM bù đắp Bộ đếm Dịch chuyển (0x00 ~ 0x3F)
  OLED_WR_Byte ( 0x00 , OLED_CMD); // - không bù đắp
  OLED_WR_Byte ( 0xd5 , OLED_CMD); // - đặt tỷ lệ phân chia đồng hồ hiển thị / tần số dao động
  OLED_WR_Byte ( 0x80 , OLED_CMD); // - đặt tỷ lệ phân chia, Đặt đồng hồ là 100 khung hình / giây
  OLED_WR_Byte ( 0xD9 , OLED_CMD); // - đặt khoảng thời gian sạc trước
  OLED_WR_Byte ( 0xF1 , OLED_CMD); // Đặt Sạc trước là 15 Đồng hồ & Xả làm 1 Đồng hồ
  OLED_WR_Byte ( 0xDA , OLED_CMD); // - đặt cấu hình phần cứng chân com
  OLED_WR_Byte ( 0x12 , OLED_CMD);
  OLED_WR_Byte ( 0xDB , OLED_CMD); // - đặt vcomh
  OLED_WR_Byte ( 0x40 , OLED_CMD); // Đặt mức Bỏ chọn VCOM
  OLED_WR_Byte ( 0x20 , OLED_CMD); // - Đặt chế độ định địa chỉ trang (0x00 / 0x01 / 0x02)
  OLED_WR_Byte ( 0x02 , OLED_CMD); //
  OLED_WR_Byte ( 0x8D , OLED_CMD); // - đặt bật / tắt Charge Pump
  OLED_WR_Byte ( 0x14 , OLED_CMD); // - vô hiệu hóa set (0x10)
  OLED_WR_Byte ( 0xA4 , OLED_CMD); // Tắt Toàn bộ Hiển thị Bật (0xa4 / 0xa5)
  OLED_WR_Byte ( 0xA6 , OLED_CMD); // Tắt Hiển thị Đảo ngược Bật (0xa6 / a7)
  OLED_Clear ();
  OLED_WR_Byte ( 0xAF , OLED_CMD);  // * hiển thị BẬT * /
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    OLED_Init();
   // uint8_t t= ' ';
    OLED_ShowString(8,2,"Kiensyiu",16);
    OLED_ShowString(20,4,"2020/09/06",16);
    while(1)
    {
    }
}