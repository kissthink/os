#pragma once

/** @defgroup kbc kbc
 * @{
 * Functions to manipulate keyboard driver
 */

#define DELAY_US 20000

#define KB_IRQ		1
#define MOUSE_IRQ	12

#define DATA_REG	0x60
#define STAT_REG	0x64

#define SET_LEDS	0xED
#define ACK			0xFA

#define OBF			BIT(0)
#define IBF     	BIT(1)
#define AUX			BIT(5)
#define TO_ERR		BIT(6)
#define PAR_ERR		BIT(7)

/**
 * @brief Writes byte to KBC
 *
 * @param port Port to write to
 * @param byte Byte to be written
 * @return int Integer indicating success or fail
 */
int writeToKBC(unsigned long port, unsigned char byte);

/**
 * @brief Reads byte from KBC
 *
 * @return KBC read state
 */
int readKBCState();

/**
 * @brief Checks ACK
 *
 * @param stat Byte to be written
 * @return int Integer indicating success or fail
 */
int checkACK(unsigned long stat);

/**@}*/
