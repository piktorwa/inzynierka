/*
10.12.2023r
Projekt na pracę dyplomową "Konwerter energii cieplnej ciała ludzkiego do zasilania elementów IoT"
Wiktor Pantak
Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie
AGH University in Cracov

Deinicja serwisów dla aplikacji NRF CONNECT FOR MOBILE
oraz funkcji wysyłania danych do klientów (peerów).
*/

#ifndef BT_LBS_H_
#define BT_LBS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/types.h>

/** @brief LBS Service UUID. */
#define BT_UUID_LBS_VAL BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
#define BT_UUID_LBS_MYSENSOR_VAL BT_UUID_128_ENCODE(0x00001526, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

#define BT_UUID_LBS BT_UUID_DECLARE_128(BT_UUID_LBS_VAL)
#define BT_UUID_LBS_MYSENSOR       BT_UUID_DECLARE_128(BT_UUID_LBS_MYSENSOR_VAL)


/** @brief Send the sensor value as notification.
 *
 * This function sends an uint32_t  value, typically the value
 * of a simulated sensor to all connected peers.
 *
 * @param[in] sensor_value The value of the simulated sensor.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int my_lbs_send_sensor_notify(uint32_t sensor_value);

#ifdef __cplusplus
}
#endif

#endif /* BT_LBS_H_ */
