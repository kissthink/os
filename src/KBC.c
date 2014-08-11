#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "KBC.h"

#include "Utilities.h"

int writeToKBC(unsigned long port, unsigned char byte) {
	unsigned long stat;

	while (1) {
		if (sys_inb(STAT_REG, &stat) != OK)
			return -1;

		if ((stat & IBF) == 0) {
			if (sys_outb(port, byte) != OK)
				return -1;

			break;
		}

		tickdelay(micros_to_ticks(DELAY_US));
	}

	return 0;
}

int readKBCState() {
	unsigned long stat, data, counter = 0;

	while (counter < 3) {
		if (sys_inb(STAT_REG, &stat) != OK)
			return -1;

		if ((stat & (OBF | AUX)) != OK) {
			if (sys_inb(DATA_REG, &data) != OK)
				return -1;

			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return data;
			else if (data == 0xFC)
				return data;
			else if (data == 0xFE)
				return data;
			else
				return -1;

			break;
		}

		tickdelay(micros_to_ticks(DELAY_US));
		counter++;
	}

	return data;
}

int checkACK(unsigned long stat) {
	if (readKBCState(&stat) != 0)
		return -1;

	if ((stat & ACK) == 0)
		return -1;

	return 0;
}
