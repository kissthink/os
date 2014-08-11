#pragma once

#define LINEAR_MODEL_BIT 14
#define VBE_GET_INFO_MODE 0x4F01
#define VBE_CHANGE_VIDEO_MODE 0x4F02
#define INT_VIDEO 0x10

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

// user-friendly color names
#define BLACK rgb(0, 0, 0)
#define BLUE rgb(0, 0, 255)
#define GREEN rgb(0, 255, 0)
#define CYAN rgb(0, 255, 255)
#define RED rgb(255, 0, 0)
#define ORANGE rgb(255, 102, 0)
#define YELLOW rgb(255,255, 0)
#define WHITE rgb(255, 255, 255)

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *initGraphics(unsigned short mode);

char* getGraphicsBuffer();

int getHorResolution(); ///< Returns horizontal resolution
int getVerResolution(); ///< Returns vertical resolution

/**
 * @brief Maps RGB color
 *
 * @param Red integer from 0-255
 * @param Green integer from 0-255
 * @param Blue integer from 0-255
 * @return unsigned long with RGB color. -1, upon failure.
 */
int rgb(unsigned char r, unsigned char g, unsigned char b);

/**
 * @brief Copies the graphic buffer to video_mem to print graphics to display
 */
void flipDisplay();

/**
 * @brief Fills the screen with the input color
 * 
 * @param color color to fill the screen with
 * @return 0 on success, non-zero upon failure
 */
int fillDisplay(int color);

/**
 * @brief Sets input pixel with input color
 * 
 * Sets the color of the pixel at the specified position to the input value, 
 *  by writing to the corresponding location in VRAM
 * 
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @param color color to set the pixel
 * @return 0 on success, non-zero otherwise
 */
int setPixel(long x, long y, int color);

/**
 * @brief Returns the color of the input pixel
 * 
 * Returns the color of the pixel at the specified position, 
 *  by reading to the corresponding location in VRAM
 * 
 * @param x horizontal coordinate, starts at 0 (leftmost pixel)
 * @param y vertical coordinate, starts at 0 (top pixel)
 * @return color of the pixel at coordinates (x,y), or -1 if some input argument is not valid
 */
unsigned short getPixel(long x, long y);

/**
 * @brief Draw line segment with specified end points and color
 *
 * Draws a line segment with the specified end points and the input color,
 *  by writing to VRAM
 *
 * @param xi horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)
 * @param yi vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param xf horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)
 * @param yf vertical coordinate of "last" endpoint, starts at 0 (top pixel)
 * @param color color of the line segment to draw
 * @return 0 upon success, non-zero upon failure
 */
int drawLine(long xi, long yi, long xf, long yf, int color);

/**
 * @brief Draw rectangle with specified end points and color
 * 
 * Draws a line segment with the specified end points and the input color, 
 *  by writing to VRAM
 * 
 * @param xi horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)  
 * @param yi vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param xf horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)  
 * @param yf vertical coordinate of "last" endpoint, starts at 0 (top pixel)  
 * @param color color of the line segment to draw
 * @return 0 upon success, non-zero upon failure
 */
int drawRectangle(long xi, long yi, long xf, long yf, int color);

/**
 * @brief Draw filled rectangle with specified end points and color
 *
 * Draws a line segment with the specified end points and the input color,
 *  by writing to VRAM
 *
 * @param xi horizontal coordinate of "first" endpoint, starts at 0 (leftmost pixel)
 * @param yi vertical coordinate of "first" endpoint, starts at 0 (top pixel)
 * @param xf horizontal coordinate of "last" endpoint, starts at 0 (leftmost pixel)
 * @param yf vertical coordinate of "last" endpoint, starts at 0 (top pixel)
 * @param color color of the line segment to draw
 * @return 0 upon success, non-zero upon failure
 */
int drawFilledRectangle(long xi, long yi, long xf, long yf, int color);

/**
 * @brief Paints the pixel at {x, y} with color and expands it left, right, up and down
 *
 * @param x horizontal coordinate
 * @param y vertical coordinate
 * @param color color of the line segment to draw
 */
void expandPixel(long x, long y, int color);

/**
 * @brief Draw circle with specified center, radius and color
 *
 * @param x horizontal coordinate of circle center, starts at 0
 * @param y vertical coordinate of circle center, starts at 0
 * @param circle radius
 * @param color color of the line segment to draw
 * @return 0 upon success, non-zero upon failure
 */
int drawCircle(long x, long y, double radius, int color);

/**
 * @brief Draws a circle with specified center, radius and color using pixel expansion
 *
 * @param x horizontal coordinate of circle center, starts at 0
 * @param y vertical coordinate of circle center, starts at 0
 * @param radius circle radius
 * @param color color of the line segment to draw
 * @return 0 upon success, non-zero upon failure
 */
int drawFilledCircle(long x, long y, long radius, int color);

/**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int exitGraphics(void);
