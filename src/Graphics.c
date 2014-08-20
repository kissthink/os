#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "Graphics.h"

#include <math.h>
#include "Point.h"
#include "Utilities.h"
#include "VBE.h"

/* Private global variables */
static vbe_mode_info_t vbeInfo;

static char* videoMem; /* Process address to which VRAM is mapped */
static char* mBuffer;
static char* buffer;
static unsigned int videoMemSize;

static unsigned int horResolution; /* Horizontal screen resolution in pixels */
static unsigned int verResolution; /* Vertical screen resolution in pixels */
static unsigned int bitsPerPixel; /* Number of VRAM bits per pixel */
static unsigned int bytesPerPixel;

void* initGraphics(unsigned short mode) {
	mode |= BIT(LINEAR_MODEL_BIT);

	// getting graphics card info
	struct reg86u reg;
	reg.u.w.ax = VBE_CHANGE_VIDEO_MODE;
	reg.u.w.bx = mode;
	reg.u.b.intno = INT_VIDEO;

	// kernel call to return horResolution, verResolution and bitsPerPixel
	if (sys_int86(&reg)) {
		LOG("initGraphics", "sys_int86 failed");
		return NULL;
	}

	if (vbe_get_mode_info(mode, &vbeInfo) < 0) {
		LOG("initGraphics", "vbe_get_mode_info failed");
		return NULL;
	}

	// defining variables
	horResolution = vbeInfo.XResolution;
	verResolution = vbeInfo.YResolution;
	bitsPerPixel = vbeInfo.BitsPerPixel;
	bytesPerPixel = bitsPerPixel / 8;
	LOG_VAR("horResolution", horResolution);
	LOG_VAR("verResolution", verResolution);

	videoMemSize = horResolution * verResolution * bytesPerPixel;

	struct mem_range mr;
	mr.mr_base = vbeInfo.PhysBasePtr;
	mr.mr_limit = mr.mr_base + videoMemSize;
	if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK)
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", reg);

	videoMem = vm_map_phys(SELF, (void*) mr.mr_base, videoMemSize);
	mBuffer = (char*) malloc(videoMemSize);
	buffer = (char*) malloc(videoMemSize);

	return videoMem;
}

char* getVideoMem() {
	return videoMem;
}

char* getGraphicsMBuffer() {
	return mBuffer;
}

char* getGraphicsBuffer() {
	return buffer;
}

int getHorResolution() {
	return horResolution;
}

int getVerResolution() {
	return verResolution;
}

int getBytesPerPixel() {
	return bytesPerPixel;
}

int rgb(unsigned char r, unsigned char g, unsigned char b) {
	if (r < 0 || 255 < r || g < 0 || 255 < g || b < 0 || b > 255)
		return -1;

	int red, green, blue;
	red = r * 31 / 255;
	green = g * 63 / 255;
	blue = b * 31 / 255;

	return (red << 11) | (green << 5) | blue;
}

void flipDisplay() {
	memcpy(videoMem, mBuffer, videoMemSize);
}

void flipMBuffer() {
	memcpy(mBuffer, buffer, videoMemSize);
}

int fillDisplay(int color) {
	char *ptr = buffer;

	// fill one line
	int i;
	for (i = 0; i < horResolution; i++, ptr++) {
		*ptr = (char) (color & 0xFF);
		ptr++;
		*ptr = (char) ((color >> 8) & 0xFF);
	}

	// get pointer to that line
	int screenWidthBytes = horResolution * bytesPerPixel;

	// copy and paste first line
	for (i = 1; i < verResolution; i++)
		memcpy(buffer + i * screenWidthBytes, buffer, screenWidthBytes);

	return 0;
}

int setPixel(long x, long y, int color) {
	if (x < 0 || y < 0 || x >= getHorResolution() || y >= getVerResolution())
		return -1;

	int id = (x + y * getHorResolution()) * getBytesPerPixel();
	buffer[id] = color & 0xFF;
	buffer[id + 1] = (color >> 8) & 0xFF;

	return 0;
}

int setBufferPixel(char* buffer, long x, int color) {
	if (x < 0)
		return -1;

	int id = x * getBytesPerPixel();
	buffer[id] = color & 0xFF;
	buffer[id + 1] = (color >> 8) & 0xFF;

	return 0;
}

unsigned short getPixel(long x, long y) {
	if (x < 0 || y < 0 || x >= getHorResolution() || y >= getVerResolution())
		return -1;

	int id = (x + y * getHorResolution()) * getBytesPerPixel();

	unsigned short color = 0;
	color |= buffer[id] & 0xFF;
	color |= (buffer[id + 1] & 0xFF) << 8;

	return color;
}

int drawLine(long xi, long yi, long xf, long yf, int color) {
	int a = abs(xf - xi);
	int b = abs(yf - yi);

	int invertXY = 0;
	if (b > a) {
		swap(int, xi, yi);
		swap(int, xf, yf);
		swap(int, a, b);
		invertXY = 1;
	}

	if (xf < xi) {
		swap(int, xi, xf);
		swap(int, yi, yf);
	}

	// do not touch this
	int inc2 = 2 * b;
	int d = inc2 - a;	// d = 2*b – a;
	int inc1 = d - a;	// inc1 = 2*(b-a);

	int i, x = xi, y = yi;
	for (i = 0; i < a; i++, x++) {
		invertXY ? setPixel(y, x, color) : setPixel(x, y, color);

		if (d >= 0)
			yi > yf ? y-- : y++, d += inc1;
		else
			d += inc2;
	}

	return 0;
}

int drawRectangle(long xi, long yi, long xf, long yf, int color) {
	int i;

	// y correction
	if (yi > yf)
		swap(int, yi, yf);

	// x correction
	if (xi > xf)
		swap(int, xi, xf);

	// horizontal borders
	for (i = xi; i < xf; i++) {
		setPixel(i, yi, color);
		setPixel(i, yf - 1, color);
	}

	// vertical borders
	for (i = yi + 1; i < yf; i++) {
		setPixel(xi, i, color);
		setPixel(xf - 1, i, color);
	}

	return 0;
}

int drawFilledRectangle(long xi, long yi, long xf, long yf, int color) {
	// y correction
	if (yi > yf)
		swap(int, yi, yf);

	// x correction
	if (xi > xf)
		swap(int, xi, xf);

	// prepare buffer
	char* line = (char*) malloc((xf - xi) * bytesPerPixel);
	int i;
	for (i = 0; i < xf - xi; i++)
		setBufferPixel(line, i, color);

	int screenWidthBytes = horResolution * bytesPerPixel;
	int lineBytes = (xf - xi) * bytesPerPixel;

	// copy and paste first line
	char* start = buffer + xi * bytesPerPixel + yi * screenWidthBytes;
	for (i = 0; i < yf - yi; i++) {
		int t = i * screenWidthBytes;

		if (yi + i >= 0 && yi + i < getVerResolution()) {
			int size;

			if (xi < 0) {
				size = xf * bytesPerPixel;
				memcpy(start + t - xi * bytesPerPixel, line, size);
			} else {
				if (xf > getHorResolution())
					size = (getHorResolution() - xi) * bytesPerPixel;
				else
					size = lineBytes;

				memcpy(start + t, line, size);
			}
		}
	}

	// destroy buffer
	free(line);

	return 0;
}

void expandPixel(long x, long y, int color) {
	// placing pixel at the center
	if (setPixel(x, y, color) == -1)
		return;

	// expand left
	if (getPixel(x - 1, y) != color)
		expandPixel(x - 1, y, color);

	// expand right
	if (getPixel(x + 1, y) != color)
		expandPixel(x + 1, y, color);

	// expand up
	if (getPixel(x, y - 1) != color)
		expandPixel(x, y - 1, color);

	// expand down
	if (getPixel(x, y + 1) != color)
		expandPixel(x, y + 1, color);
}

int drawCircle(long x, long y, double radius, int color) {
	int xi = x, yi = y;

	x = 0, y = radius;
	int p = 1 - radius;
	int incE = 3, incSE = 5 - 2 * radius;

	setPixel(xi - radius, yi, color);
	setPixel(xi + radius, yi, color);
	setPixel(xi, yi - radius, color);
	setPixel(xi, yi + radius, color);

	while (y > x) {
		if (p < 0) {
			p = p + incE;
			incE = incE + 2;
			incSE = incSE + 2;
			x++;
		} else {
			p = p + incSE;
			incE = incE + 2;
			incSE = incSE + 4;
			x++, y--;
		}

		int i, j;
		for (i = 1; i >= -1; i -= 2) {
			for (j = 1; j >= -1; j -= 2) {
				setPixel(xi + i * x, yi + j * y, color);
				setPixel(xi + i * y, yi + j * x, color);
			}
		}
	}

	return 0;
}

int drawFilledCircle(long x, long y, long radius, int color) {
	if (radius <= 0)
		return -1;

	// drawing circle limits
	drawCircle(x, y, radius, color);

	// expanding center
	expandPixel(x, y, color);

	// expanding each pixel which are located inside
	// the circle and right next to the border.
	// this is useful when the circle is drawn outside the screen but
	// due to its radius, part of the circle should be showing up
	if (radius > 1) {
		radius--;
		int xi = x, yi = y;
		x = 0, y = radius;
		int p = 1 - radius;
		int incE = 3, incSE = 5 - 2 * radius;

		expandPixel(xi - radius, yi, color);
		expandPixel(xi + radius, yi, color);
		expandPixel(xi, yi - radius, color);
		expandPixel(xi, yi + radius, color);

		while (y > x) {
			if (p < 0) {
				p = p + incE;
				incE = incE + 2;
				incSE = incSE + 2;
				x++;
			} else {
				p = p + incSE;
				incE = incE + 2;
				incSE = incSE + 4;
				x++, y--;
			}

			int i, j;
			for (i = 1; i >= -1; i -= 2) {
				for (j = 1; j >= -1; j -= 2) {
					expandPixel(xi + i * x, yi + j * y, color);
					expandPixel(xi + i * y, yi + j * x, color);
				}
			}
		}
	}

	return 0;
}

int exitGraphics() {
	struct reg86u reg86;

	// BIOS video services
	reg86.u.b.intno = 0x10;

	// Set Video Mode function
	reg86.u.b.ah = 0x00;

	// 80x25 text mode
	reg86.u.b.al = 0x03;

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}
