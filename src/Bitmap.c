#include "Bitmap.h"

#include <stdio.h>
#include "Graphics.h"
#include "Utilities.h"

Bitmap* loadBitmap(const char *filename) {
	// allocating necessary size
	Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

	// open filename in read binary mode
	LOG("loadBitmap", filename);
	FILE *filePtr;
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	BitmapFileHeader bitmapFileHeader;
	fread(&bitmapFileHeader, 2, 1, filePtr);

	// verify that this is a bmp file by check bitmap id
	if (bitmapFileHeader.type != 0x4D42) {
		fclose(filePtr);
		return NULL;
	}

	int rd;
	do {
		if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
			break;
		if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
			break;
		if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
			break;
	} while (0);

	if (rd = !1) {
		fprintf(stderr, "Error reading file\n");
		exit(-1);
	}

	// read the bitmap info header
	BitmapInfoHeader bitmapInfoHeader;
	fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

	/*
	 // DEBUGGING BLOCK
	 printf("--------------------------------\n");
	 printf("Image size = %d x %d\n", bitmapInfoHeader.width,
	 bitmapInfoHeader.height);
	 printf("Number of colour planes is %d\n", bitmapInfoHeader.planes);
	 printf("Bits per pixel is %d\n", bitmapInfoHeader.bits);
	 printf("Compression type is %d\n", bitmapInfoHeader.compression);
	 printf("Number of colours is %d\n", bitmapInfoHeader.nColors);
	 printf("Number of required colours is %d\n",
	 bitmapInfoHeader.importantColors);
	 printf("--------------------------------\n");
	 */

	// move file pointer to the begining of bitmap data
	fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

	// allocate enough memory for the bitmap image data
	unsigned char* bitmapImage = (unsigned char*) malloc(
			bitmapInfoHeader.imageSize);

	// verify memory allocation
	if (!bitmapImage) {
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL) {
		fclose(filePtr);
		return NULL;
	}

	// close file and return bitmap image data
	fclose(filePtr);

	bmp->bitmapData = bitmapImage;
	bmp->bitmapInfoHeader = bitmapInfoHeader;

	return bmp;
}

void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment) {
	if (bmp == NULL)
		return;

	char* bufferStartPos;
	unsigned int width = bmp->bitmapInfoHeader.width;
	unsigned int height = bmp->bitmapInfoHeader.height;

	if (alignment == ALIGN_CENTER)
		x -= width / 2;
	else if (alignment == ALIGN_RIGHT)
		x -= width;

	int i;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVerResolution())
			continue;

		bufferStartPos = getGraphicsBuffer() + x * 2
				+ pos * getHorResolution() * 2;

		memcpy(bufferStartPos, bmp->bitmapData + i * width * 2, width * 2);
	}
}

void drawFilteredBitmap(Bitmap* bmp, int x, int y, int colorToFilter,
		Alignment alignment) {
	if (bmp == NULL)
		return;

	char* bufferStartPos;
	unsigned int width = bmp->bitmapInfoHeader.width;
	unsigned int height = bmp->bitmapInfoHeader.height;

	if (alignment == ALIGN_CENTER)
		x -= width / 2;
	else if (alignment == ALIGN_RIGHT)
		x -= width;

	int i, j;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVerResolution())
			continue;

		bufferStartPos = getGraphicsBuffer() + x * 2
				+ (y + height - i) * getHorResolution() * 2;

		for (j = 0; j < width * 2; j++, bufferStartPos++) {
			if (x + j < 0 || x * 2 + j >= getHorResolution() * 2)
				continue;

			int pos = j + i * width * 2;
			unsigned short tmp1 = bmp->bitmapData[pos];
			unsigned short tmp2 = bmp->bitmapData[pos + 1];
			unsigned short temp = (tmp1 | (tmp2 << 8));

			//LOG_VAR("pixel color", temp);
			if (temp != colorToFilter) {
				*bufferStartPos = bmp->bitmapData[j + i * width * 2];
				j++;
				bufferStartPos++;
				*bufferStartPos = bmp->bitmapData[j + i * width * 2];
			} else {
				j++;
				bufferStartPos++;
			}
		}
	}
}

void drawFontBitmap(Bitmap* bmp, int x, int y, int color) {
	if (bmp == NULL)
		return;

	unsigned int width = bmp->bitmapInfoHeader.width;
	unsigned int height = bmp->bitmapInfoHeader.height;

	char* bufferStartPos;
	int i, j;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVerResolution())
			continue;

		bufferStartPos = getGraphicsBuffer() + x * 2
				+ (y + height - i) * getHorResolution() * 2;

		for (j = 0; j < width * 2; j++, bufferStartPos++) {
			if (x + j < 0 || x * 2 + j >= getHorResolution() * 2)
				continue;

			int pos = j + i * width * 2;
			unsigned short tmp1 = bmp->bitmapData[pos];
			unsigned short tmp2 = bmp->bitmapData[pos + 1];
			unsigned short temp = (tmp1 | (tmp2 << 8));

			if (temp != WHITE) {
				*bufferStartPos = color & 0xFF;
				j++;
				bufferStartPos++;
				*bufferStartPos = color >> 8;
			} else {
				j++;
				bufferStartPos++;
			}
		}
	}
}

void deleteBitmap(Bitmap* bmp) {
	if (bmp == NULL)
		return;

	free(bmp->bitmapData);
	free(bmp);
}
