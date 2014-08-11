#pragma once

/** @defgroup timer timer
 * @{
 *
 * Functions for using the i8254 timers
 */

/// Represents a Timer
typedef struct {
	int counter; ///< timer counter
	int ticked; ///< bool indicating if timer has just ticked
} Timer;

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int subscribeTimer(void);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int unsubscribeTimer();

/**
 * @brief Creates a timer
 *
 * @return Non NULL pointer to Timer_t
 */
Timer* newTimer();

/**
 * @brief Returns current count
 *
 * @return int count
 */
int getCount(Timer* timer);

/**
 * @brief Handles a timer on an interruption
 *
 */
void timerHandler(Timer* timer);

/**
 * @brief Resets timer counter to 0
 *
 */
void resetTimer(Timer* Timer);

/**
 * @brief Resets timer ticked to 0
 *
 */
void resetTimerTickedFlag(Timer* Timer);

/**
 * @brief Deletes a timer
 *
 * Integer indicating success or fail
 */
int deleteTimer(Timer* timer);

/**
 * @brief Configures a timer to generate a square wave
 *
 * Does not change the LSB (BCD/binary) of the timer's control word.
 *
 * @param timer Timer to configure. (Ranges from 0 to 2)
 * @param freq Frequency of the square wave to generate
 * @return Return 0 upon success and non-zero otherwise
 */
int timerSetSquare(unsigned long timer, unsigned long freq);

/**
 * @brief Reads the input timer configuration
 *
 * param timer Timer whose config to read (Ranges from 0 to 2)
 * param st    Address of memory position to be filled with the timer config
 * return Return 0 upon success and non-zero otherwise
 */
int getTimerConfig(unsigned long timer, unsigned char *st);

/**
 * @brief Shows timer configuration
 * 
 * Displays in a human friendly way, the configuration of the input
 *  timer, by providing the values (and meanings) of the different
 *  components of a timer configuration 
 *
 * param timer Timer whose config to read (Ranges from 0 to 2)
 * return Return 0 upon success and non-zero otherwise
 */
int showTimerConfig(unsigned long timer);
