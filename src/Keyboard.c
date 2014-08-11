#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "Keyboard.h"

#include "KBC.h"

static int hook;

int kb_subscribe_int() {
	hook = KB_IRQ;
	int bitmask = BIT(hook);

	if (sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook) != OK
			|| sys_irqenable(&hook) != OK)
		return -1;

	return bitmask;
}

int kb_unsubscribe_int() {
	if (sys_irqdisable(&hook) != OK || sys_irqrmpolicy(&hook) != OK)
		return -1;

	return 0;
}
