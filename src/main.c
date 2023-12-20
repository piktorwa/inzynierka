/*
10.12.2023r
Projekt na pracę dyplomową "Konwerter energii cieplnej ciała ludzkiego do zasilania elementów IoT"
Wiktor Pantak
Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie
AGH University in Cracov
Projekt wykorzystujący nrf52840-dongle do przedstawienia możliwości wykorzystania ciepła
ciała ludzkiego do zasilania elementów IoT.
Mikrokontroler wysyła co pewien czas ustawiany parametrem NOTIFY_INTERVAL
dane typu uint32 do klienta (peera).
*/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include "my_lbs.h"

static struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
	(BT_LE_ADV_OPT_CONNECTABLE |
	 BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
	800, /* Min Advertising Interval 500ms (800*0.625ms) */
	801, /* Max Advertising Interval 500.625ms (801*0.625ms) */
	NULL); /* Set to NULL for undirected advertising */

LOG_MODULE_REGISTER(Lesson4_Exercise2, LOG_LEVEL_INF);

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define STACKSIZE 1024
#define PRIORITY 7

//okres wysyłania danych w miliseknudach
#define NOTIFY_INTERVAL         30000

/* Definicje pinów i konfiguracja flag dla przerwania */
#define PIN 13
#define EDGE GPIO_INT_LEVEL_HIGH
#define BUTTON_PIN (DT_ALIAS_SW0_GPIOS_PIN)

static uint32_t app_sensor_value = 100;

static const struct device *gpio_ct_dev = DEVICE_DT_GET(DT_N_NODELABEL_gpio1);


static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),

};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_LBS_VAL),
};

//wysyłane dane
static void simulate_data(void)
{
	app_sensor_value++;
	if (app_sensor_value == 200) {
		app_sensor_value = 100;
	}
}
//wysyłanie danych
// void send_data_timer_handler(struct k_timer *timer_id) 
// {         
//     simulate_data();
//     my_lbs_send_sensor_notify(app_sensor_value);
// }
// K_TIMER_DEFINE(send_data_timer, send_data_timer_handler, NULL);

//wysyłanie danych
// void send_data_thread(void)
// {
// 	while(1){
// 		/* Simulate data */
// 		simulate_data();
// 		/* Send notification, the function sends notifications only if a client is subscribed */
// 		my_lbs_send_sensor_notify(app_sensor_value);

// 		k_sleep(K_MSEC(NOTIFY_INTERVAL));
// 	}
// }

/* Funkcja obsługująca przerwanie */
void button_pressed(struct device *gpiob, struct gpio_callback *cb,
                    uint32_t pins)
{
    simulate_data();
    my_lbs_send_sensor_notify(app_sensor_value);
}

static void on_connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err %u)\n", err);
		return;
	}

	printk("Connected\n");

}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason %u)\n", reason);

}

struct bt_conn_cb connection_callbacks = {
	.connected = on_connected,
	.disconnected = on_disconnected,
};

void main(void)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)\n", err);
		return;
	}
	//uruchomienie modułu bluetooth
	bt_conn_cb_register(&connection_callbacks);

	LOG_INF("Bluetooth initialized\n");
	err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)\n", err);
		return;
	}

	LOG_INF("Advertising successfully started\n");
	//uruchomienie licznika czasu
	//k_timer_start(&send_data_timer, K_MSEC(NOTIFY_INTERVAL), K_MSEC(NOTIFY_INTERVAL));
	
    struct gpio_callback button_cb;

    /* Inicjalizacja pinu jako wejście z rezystorem podciągającym */
	if(!device_is_ready(gpio_ct_dev)){
		return;
	}

	err = gpio_pin_configure(gpio_ct_dev, 13, GPIO_INPUT | GPIO_PULL_UP);
	if(err != 0){
		return;
	}

	/* Konfiguracja przerwań na pinie */
    gpio_pin_interrupt_configure(gpio_ct_dev, 13, EDGE);

    /* Konfiguracja funkcji obsługującej przerwanie */
    gpio_init_callback(&button_cb, button_pressed, BIT(13));
    gpio_add_callback(gpio_ct_dev, &button_cb);


	while(1){
		k_sleep(K_FOREVER);
	}
	return;
}
//uruchomienie wątku wysyłania danych przez BLE
// K_THREAD_DEFINE(send_data_thread_id, STACKSIZE, send_data_thread, NULL, NULL, NULL, PRIORITY, 0, 0);
