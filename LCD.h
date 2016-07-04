#ifndef __LCD__
#define __LCD__

void    LCD_Port(char   a)
{  
    LCD_D4 = (a & 1) ?  1 : 0;
    LCD_D5 = (a & 2) ?  1 : 0;
    LCD_D6 = (a & 4) ?  1 : 0;
    LCD_D7 = (a & 8) ?  1 : 0;
}

void    LCD_Cmd(char    cmd)
{
	LCD_RS = 0;
    LCD_Port(cmd);
    LCD_EN = 1;
    __delay_ms(4);
    LCD_EN = 0;
}

void    LCD_Clear()
{
	LCD_Cmd(0);
	LCD_Cmd(1);
}

void LCD_Set_Cursor(char y, char x) 
{
    char temp, z, t;
    if (y == 1) {
        temp = 0x80 + x - 1;
        z = temp >> 4;
        t = temp & 0x0F;
        LCD_Cmd(z);
        LCD_Cmd(t);
    } else if (y == 2) {
        temp = 0xC0 + x - 1;
        z = temp >> 4;
        t = temp & 0x0F;
        LCD_Cmd(z);
        LCD_Cmd(t);
    }
}

void    LCD_Init() {
    LCD_Port(0x00);
    __delay_ms(20);
    LCD_Cmd(0x03);
    __delay_ms(5);
    LCD_Cmd(0x03);
    __delay_ms(11);
    LCD_Cmd(0x03);
    
    LCD_Cmd(0x02);
    LCD_Cmd(0x02);
    LCD_Cmd(0x08);
    LCD_Cmd(0x00);
    LCD_Cmd(0x0C);
    LCD_Cmd(0x00);
    LCD_Cmd(0x06);
}

void    LCD_Write_Char(char c)
{
   char temp = c & 0x0F;
   char y = c & 0xF0;
   LCD_RS = 1;
   LCD_Port(y >> 4);    //Data transfer
   LCD_EN = 1;
   __delay_us(40);
   LCD_EN = 0;
   LCD_Port(temp);
   LCD_EN = 1;
   __delay_us(40);
   LCD_EN = 0;
}

void    LCD_Write_String(char *str)
{
	int i;
	for(i = 0; str[i] != '\0'; ++i)
	   LCD_Write_Char(str[i]);
}

void    LCD_ShiftRight()
{
	LCD_Cmd(0x01);
	LCD_Cmd(0x0C);
}

void    LCD_ShiftLeft()
{
	LCD_Cmd(0x01);
	LCD_Cmd(0x08);
}

#endif