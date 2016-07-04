#ifndef __DS1307__
#define	__DS1307__

#include    "I2C.h"

unsigned char   bcd2bin(unsigned char bcd_value)
{
  unsigned char temp;
  temp = bcd_value;
  temp >>= 1;
  temp &= 0x78;
  return(temp + (temp >> 2) + (bcd_value & 0x0f));
}

unsigned char   bin2bcd(unsigned char binary_value)
{
  unsigned char temp;
  unsigned char retval;
  temp = binary_value;
  retval = 0;
  
  while(1)
  {
    // Get the tens digit by doing multiple subtraction
    // of 10 from the binary value.
    if(temp >= 10)
    {
      temp -= 10;
      retval += 0x10;
    }
    else // Get the ones digit by adding the remainder.
    {
      retval += temp;
      break;
    }
  }
  
  return    retval;
}

void    ds1307_Init(void)
{
    char initsec = 0;
    char initmin = 0;
    char inithr = 0;                  
    char initdow = 0;
    char initday = 0;                   
    char initmth = 0;
    char inityear = 0;
    I2CStart(); 
    I2CSend(0xD0);      // WR to RTC 
    I2CSend(0x00);      // REG 0 
    I2CStart(); 
    I2CSend(0xD1);      // RD from RTC 
    initsec  = bcd2bin(I2CRead() & 0x7f); 
    initmin  = bcd2bin(I2CRead() & 0x7f); 
    inithr   = bcd2bin(I2CRead() & 0x3f); 
    initdow  = bcd2bin(I2CRead() & 0x7f);   // REG 3 
    initday  = bcd2bin(I2CRead() & 0x3f);   // REG 4 
    initmth  = bcd2bin(I2CRead() & 0x1f);   // REG 5 
    inityear = bcd2bin(I2CRead());         // REG 6 
    I2CStop(); 
    __delay_us(3);

    I2CStart(); 
    I2CSend(0xD0);      // WR to RTC 
    I2CSend(0x00);      // REG 0 
    I2CSend(bin2bcd(initsec));      // Start oscillator with current "seconds value 
    I2CSend(bin2bcd(initmin));      // REG 1 
    I2CSend(bin2bcd(inithr));       // REG 2 
    I2CSend(bin2bcd(initdow));      // REG 3 
    I2CSend(bin2bcd(initday));      // REG 4 
    I2CSend(bin2bcd(initmth));      // REG 5 
    I2CSend(bin2bcd(inityear));     // REG 6 
    I2CStart(); 
    I2CSend(0xD0);      // WR to RTC 
    I2CSend(0x07);      // Control Register 
    I2CSend(0x90);      //  squarewave output pin 1Hz
    I2CStop(); 
}

void    ds1307_setData(char iData[])
{
    iData[0] &= 0x7F;
    iData[1] &= 0x7F;
    iData[2] &= 0x3F;
    iData[3] &= 0x07;
    iData[4] &= 0x3F;
    iData[5] &= 0x1F;
    iData[6] &= 0xFF;
    
	/* Send Start condition */
	I2CStart();
	/* Send DS1307 slave address with write operation */
	I2CSend(0xD0);
	/* Send subaddress 0x00, we are writing to this location */
	I2CSend(0x00);
 
	/* Loop to write 8 bytes */
	for (int j = 0; j < 8; ++j) {
		/* send I2C data one by one */
		I2CSend(bin2bcd(iData[j]));
	}
 
	/* Send a stop condition - as transfer finishes */
	I2CStop();
}

void    ds1307_getData(char buffer[])
{
    I2CStart();     /* start condition */
    I2CSend(0xD0);  /* Slave address + Write */
    I2CSend(0x00);  /* Starting address of RTC */
    I2CRestart();   /* Repeated start condition */
    I2CSend(0xD1);  /* Slave address + Read */

    /* Read 8 registers from RTC */
    for (int j = 0; j < 8; ++j) {
        buffer[j] = bcd2bin(I2CRead());

        /* check for last byte */
        if (j == 7)
            I2CNak();   /* NAK if last byte */
        else
            I2CAck();   /* ACK for all read bytes */
    }
    /* Reading finished send stop condition */
    I2CStop();
}

#endif

