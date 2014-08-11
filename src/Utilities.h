#pragma once

#include "stdio.h"

// log data
extern FILE* logfd;
#define LOG_PATH "home/lcom/os/log.txt"
#define LOG(tag, msg) fprintf(logfd, "%s: %s\n", tag, msg)
#define LOG_VAR(tag, var) fprintf(logfd, "%s: %d\n", tag, var)

#define FPS 60
#define BIT(n) (0x01 << n)
#define singleBit(byte) (0x01 & byte)

#define swap(type, i, j) {type t = i; i = j; j = t;}

// functions to deal with files
int fileExists(const char* filename);

int distanceBetweenPoints(int x1, int y1, int x2, int y2);
