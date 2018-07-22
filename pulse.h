#ifndef PULSE_H
#define PULSE_H

#include <pthread.h>
#include <signal.h>
#include <time.h>
#define ADDR 0x48
#define BASE 64
#define RS BASE + 0
#define RW BASE + 1
#define EN BASE + 2
#define LED BASE + 3
#define D4 BASE + 4
#define D5 BASE + 5
#define D6 BASE + 6
#define D7 BASE + 7
#define HIGH 1
#define LOW 0
#define READGPIO 1
#define WRITEGPIO 0
#define IT_MAX 10
#define ADDRBITS 8
#define REGBITS 8
#define OUTPUTPIN 64

int						ctrl_c_pressed;
int						a_is_pressed;

typedef char			t_bool;

typedef struct			s_bytes_com
{
	char				*addr;
	char				*reg;
	char				*data;
}						t_bytes_com;

typedef struct			s_data
{
	int					handle;
	int					lcdhandle;
	int					gpioclk;
	int					gpiodata;
	char				addr;
	char				bytes[5];
	int					res;
	int					*av;
	char				rec[10];
	int					n;
	unsigned char		val;
	pthread_t			thread;
}						t_data;

void					set_input_mode(void);
void					reset_input_mode(void);
/*take one bit from gpio*/
int						rec_one_bit(int gpioclk, int gpiodata);
/*Send bit to gpio*/
void					send_one_bit(int gpioclk, int gpiodata, int bit);
/*send the stop bit signaling end of transaction*/
void					send_stop(int gpioclk, int gpiodata);
/*send the start signal to gpio*/
void					send_start(int gpioclk, int gpiodata);
/*send nbits to the gpio, *octets must contain addresses for all the required bits one after the other*/
void					send_bits(int nbits, char *octets, int gpiodata, int gpioclk);
/*return an int containing up to 32 bits*/
int						rec_bits(int nbits, int gpiodata, int gpioclk);
/*return bits read from "pin" on the gpio*/
int						read_analog(char pin, int gpioclk, int gpiodata, int nbits);
int						average(int *mesure, unsigned int size);
/*Send value to the ana output pin*/
void		write_analog(char pin, int gpioclk, int gpiodata, char value);
#endif
