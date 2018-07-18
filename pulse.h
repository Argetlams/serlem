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

int						ctrl_c_pressed;
typedef char			t_bool;
typedef struct			s_data
{
	int					handle;
	int					lcdhandle;
	int					gpioclk;
	int					gpiodata;
	char				addr;
	char				*bytes;
//	char				rec[10];
	int					n;
	unsigned char		val;
	pthread_t			thread;
}						t_data;

void					set_input_mode(void);
void					reset_input_mode(void);

#endif
