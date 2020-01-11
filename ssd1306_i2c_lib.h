
#ifndef SSD1306_I2C_LIB_H_
#define SSD1306_I2C_LIB_H_
#define INTERFACE_IIC
void i2c_init ();
void i2c_transmit (unsigned char *params, unsigned char flag);

#endif
