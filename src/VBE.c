#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include "VBE.h"

#include "LmLib.h"
#include "Utilities.h"

#define LINEAR_MODEL_BIT 14
#define VBE_MODE_INFO_BLOCK_SIZE sizeof(vbe_mode_info_t)

#define VBE_MODE 0x4F
#define RET_VBE_MODE_INFO 0x01
#define BIOS_VIDEO_SERVICE 0x10

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t* vmi_p) {
	char TAG[30] = "vbe_get_mode_info";

	struct reg86u r;
	mmap_t map;

	if (mode < 0x100) {
		LOG(TAG, "non-VBE mode");
		return -1;
	}

	if (lm_init()) {
		LOG(TAG, "lm_init failed");
		return -1;
	}

	lm_alloc(VBE_MODE_INFO_BLOCK_SIZE, &map);

	r.u.b.ah = VBE_MODE;
	r.u.b.al = RET_VBE_MODE_INFO;
	r.u.w.es = PB2BASE(map.phys);
	r.u.w.di = PB2OFF(map.phys);
	r.u.w.cx = mode | BIT(LINEAR_MODEL_BIT);
	r.u.b.intno = BIOS_VIDEO_SERVICE;

	if (sys_int86(&r)) {
		LOG(TAG, "sys_int86 failed");
		return -1;
	}

	*vmi_p = *(vbe_mode_info_t*) map.virtual;

	lm_free(&map);

	return r.u.w.ax;
}
