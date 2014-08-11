#include <minix/syslib.h>
#include <minix/drivers.h>
#include "Timer.h"

#include "i8254.h"
#include "Utilities.h"

static int hook = TIMER0_IRQ;

int subscribeTimer(void) {
	int bitmask = BIT(hook);

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook) != OK
			|| sys_irqenable(&hook) != OK)
		return -1;

	return bitmask;
}

int unsubscribeTimer() {
	if (sys_irqdisable(&hook) != OK || sys_irqrmpolicy(&hook) != OK)
		return -1;

	return 0;
}

Timer* newTimer() {
	Timer* timer = (Timer*) malloc(sizeof(Timer));

	timer->counter = 0;
	timer->ticked = 0;

	return timer;
}

int getCount(Timer* timer) {
	return timer->counter;
}

void timerHandler(Timer* Timer) {
	Timer->counter++;
	Timer->ticked = 1;
}

void resetTimer(Timer* Timer) {
	Timer->counter = 0;
	Timer->ticked = 0;
}

void resetTimerTickedFlag(Timer* Timer) {
	Timer->ticked = 0;
}

int deleteTimer(Timer* timer) {
	free(timer);
	return 0;
}

int timerSetSquare(unsigned long timer, unsigned long freq) {
	int port;
	unsigned short freqToSend;
	unsigned char LSB, MSB;
	unsigned long byte = 0;

	freqToSend = TIMER_FREQ / freq;
	LSB = freqToSend & 0xFF;
	MSB = freqToSend >> 8;

	if (timer > 2)
		return -1;

	switch (timer) {
	case 0:
		port = TIMER_0;
		byte = TIMER_SEL0;
		break;
	case 1:
		port = TIMER_1;
		byte = TIMER_SEL1;
		break;
	case 2:
		port = TIMER_2;
		byte = TIMER_SEL2;
		break;
	}

	sys_outb(TIMER_CTRL, TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_BIN);
	sys_outb(port, LSB);
	sys_outb(port, MSB);

	return 0;
}

int getTimerConfig(unsigned long timer, unsigned char *st) {
	if (timer < 0 || timer > 2)
		return -1;

	unsigned char byte = 0;
	unsigned long stl;

	byte |= TIMER_RB_CMD | TIMER_RB_SEL(timer);

	sys_outb(TIMER_CTRL, byte);
	sys_inb(TIMER_0 + timer, &stl);
	*st = stl;

	return 0;
}

int showTimerConfig(unsigned long timer) {
	if (timer < 0 || timer > 2)
		return -1;

	unsigned char st;
	getTimerConfig(timer, &st);
	printf("Word read: %x\n", st);

	unsigned char info;
	if (st & TIMER_LSB_MSB)
		printf("Type of access: TIMER_LSB_MSB\n");
	else if (st & TIMER_LSB)
		printf("Type of access: TIMER_LSB\n");
	else if (st & TIMER_MSB)
		printf("Type of access: TIMER_MSB\n");

	if (st & TIMER_SQR_WAVE)
		printf("Operating mode: TIMER_SQR_WAVE\n");
	else if (st & TIMER_RATE_GEN)
		printf("Operating mode: TIMER_RATE_GEN\n");

	info = st & 1;
	printf("Counting mode: %d\n", info);

	return 0;
}
