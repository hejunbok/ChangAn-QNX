#ifndef LAYOUT_H
#define LAYOUT_H

/**
 * ZORDER_UNDERGROUND Defines Z order for the window when it should be completely removed from the screen (under all windows on background)
 * @brief ZORDER_UNDERGROUND
 */
static const int ZORDER_UNDERGROUND = -1;

/**
 * ZORDER_UNDERGROUND Defines Z order for camera overlay when it should be completely removed from the screen (under all windows on background)
 * @brief ZORDER_UNDERGROUND
 */
static const int ZORDER_MAIN_HMI = 3;

/**
 * ZORDER_NAVIGATION_OVERLAY Default Z order for map overlay in Navigation screen
 * @brief ZORDER_NAVIGATION_OVERLAY
 */
static const int ZORDER_NAVIGATION_OVERLAY = 10;

/**
 * ZORDER_WEATHER_WIDGET Default Z order for WeatherMini widget in home screen
 * @brief ZORDER_WEATHER_WIDGET
 */
static const int ZORDER_WEATHER_WIDGET = 20;

/**
 * ZORDER_APP_LEVEL Defines Z order for camera overlay when it triggered by applicaiton launch
 * @brief ZORDER_APP_LEVEL
 */
static const int ZORDER_APP_LEVEL = 24;

/**
 * ZORDER_ASR Defines Z order for ASR DUDE overlay
 * @brief ZORDER_ASR
 */
static const int ZORDER_ASR = 25;

/**
 * ZORDER_TOP_LEVEL Defines Z order for camera overlay when it triggered by camera sensor when car is in reverse.
 * @brief ZORDER_TOP_LEVEL
 */
static const int ZORDER_TOP_LEVEL = 26;

/**
 * ZORDER_KEYBOARD Default Z order for keyboard
 * @brief ZORDER_KEYBOARD
 */
static const int ZORDER_KEYBOARD = 300;

#endif // LAYOUT_H
