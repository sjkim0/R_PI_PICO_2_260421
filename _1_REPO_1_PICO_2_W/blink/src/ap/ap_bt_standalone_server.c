#include "ap_bt_standalone_server.h"

#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

#include "temp_sensor.h"


#define HEARTBEAT_PERIOD_MS     (1000U)
#define ADC_CHANNEL_TEMPSENSOR  (4U)
#define APP_AD_FLAGS            (0x06U)
#define BLE_RX_BUFFER_SIZE      (256U)
#define BLE_RX_BUFFER_ROW_SIZE  (12U)


extern uint8_t const profile_data[]; // generated

typedef struct 
{
    /* data */
    uint32_t count;
    uint8_t ble_received_buff[BLE_RX_BUFFER_ROW_SIZE][BLE_RX_BUFFER_SIZE];
    uint16_t ble_received_buff_row_head;
}ap_bt_standalone_server_t;


static ap_bt_standalone_server_t ap_bt_standalone_server_inst;
static uint8_t adv_data[] = 
{
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS,
    // Name
    0x06, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'P', 'i', 'c', 'o', '2',
    // 16-bit Service UUIDs
    0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x1a, 0x18,
};
static const uint8_t adv_data_len = sizeof(adv_data);
static btstack_timer_source_t heartbeat;
static btstack_packet_callback_registration_t hci_event_callback_registration;
static volatile bool key_pressed;

int le_notification_enabled;
hci_con_handle_t con_handle;
uint16_t current_temp;


static void poll_temp(void);
static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
static void poll_temp(void);
static void heartbeat_handler(struct btstack_timer_source *ts) ;

static void _operateInternalTempSensor(void);
static void _initKeyPressCallback(void);
static void _initCyw43Arch(void);
static void _initBle(void);

void apBtStandaloneServerInit(void)
{
    stdio_init_all();
restart:
    _initCyw43Arch();
    _initKeyPressCallback();
    _operateInternalTempSensor();

    _initBle();

    key_pressed = false;
    while(!key_pressed) 
    {
        async_context_poll(cyw43_arch_async_context());
        async_context_wait_for_work_until(cyw43_arch_async_context(), at_the_end_of_time);
    }

    cyw43_arch_deinit();

    printf("Press the \"S\" key to Start bluetooth\n");
    key_pressed = false;
    while(!key_pressed) 
    {
        sleep_ms(1000);
    }
    goto restart;
}

void apBtStandaloneServerLoop(void)
{
    printf("testing %d\n", ap_bt_standalone_server_inst.count++);
    ap_bt_standalone_server_inst.count %= 1000;
    sleep_ms(1000);
}

void key_pressed_func(void *param) 
{
    int key = getchar_timeout_us(0); // get any pending key press but don't wait
    if (key == 's' || key == 'S') 
    {
        key_pressed = true;
    }
}

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) 
{
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
    if (packet_type != HCI_EVENT_PACKET) return;

    uint8_t event_type = hci_event_packet_get_type(packet);
    switch(event_type)
    {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) 
            {
                return;
            }
            gap_local_bd_addr(local_addr);
            printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));

            // setup advertisements
            uint16_t adv_int_min = 800;
            uint16_t adv_int_max = 800;
            uint8_t adv_type = 0;
            bd_addr_t null_addr;
            memset(null_addr, 0, 6);
            gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
            assert(adv_data_len <= 31); // ble limitation
            gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
            gap_advertisements_enable(1);

            poll_temp();

            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            le_notification_enabled = 0;
            break;
        case ATT_EVENT_CAN_SEND_NOW:
            att_server_notify(con_handle, ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_01_VALUE_HANDLE, (uint8_t*)&current_temp, sizeof(current_temp));
            break;
        default:
            break;
    }
}

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size) 
{
    UNUSED(connection_handle);

    // ble에서 온도 전달하는 부분
    if (att_handle == ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_01_VALUE_HANDLE)
    {
        return att_read_callback_handle_blob((const uint8_t *)&current_temp, sizeof(current_temp), offset, buffer, buffer_size);
    }
    return 0;
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) 
{
    switch(att_handle) 
    {
        // case ATT_CHARACTERISTIC_12345678_1234_1234_1234_1234567890AC_01_VALUE_HANDLE:
        // {
        //     printf("Received over BLE: %s\n", (char *)buffer);
        //     break;
        // }
        case ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_01_CLIENT_CONFIGURATION_HANDLE:
        {
            volatile uint16_t cccd = little_endian_read_16(buffer, 0);
            le_notification_enabled = (cccd == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION) ||
                                      (cccd == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION);

            con_handle = connection_handle;
            if (le_notification_enabled) 
            {
                att_server_request_can_send_now_event(con_handle);
                printf("Client characteristics configure ENABLED\r\n");
            }
            else
            {
                printf("Client characteristics configure not ENABLED\r\n");
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

static void poll_temp(void) 
{
    adc_select_input(ADC_CHANNEL_TEMPSENSOR);
    uint32_t raw32 = adc_read();
    const uint32_t bits = 12;

    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    uint16_t raw16 = raw32 << (16 - bits) | raw32 >> (2 * bits - 16);

    // ref https://github.com/raspberrypi/pico-micropython-examples/blob/master/adc/temperature.py
    const float conversion_factor = 3.3 / (65535);
    float reading = raw16 * conversion_factor;

    // The temperature sensor measures the Vbe voltage of a biased bipolar diode, connected to the fifth ADC channel
    // Typically, Vbe = 0.706V at 27 degrees C, with a slope of -1.721mV (0.001721) per degree.
    float deg_c = 27 - (reading - 0.706) / 0.001721;
    current_temp = deg_c * 100;
    printf("Write temp %.2f degc\n", deg_c);
}

static void heartbeat_handler(struct btstack_timer_source *ts) 
{
    static uint32_t counter = 0;

    // Update the temp every heartbeat
    poll_temp();
    if (le_notification_enabled) 
    {
        att_server_request_can_send_now_event(con_handle);
    }

    // Invert the led
    static int led_on = true;
    led_on = !led_on;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);

    // Restart timer
    btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(ts);
}

// Initialise adc for the temp sensor
static void _operateInternalTempSensor(void)
{
    adc_init();
    adc_select_input(ADC_CHANNEL_TEMPSENSOR);
    adc_set_temp_sensor_enabled(true);
}

static void _initKeyPressCallback(void)
{
    stdio_set_chars_available_callback(key_pressed_func, NULL);
}

static void _initCyw43Arch(void)
{
    if (cyw43_arch_init() != 0) 
    {
        printf("failed to initialise cyw43_arch\n");
    }
}

static void _initBle(void)
{
    l2cap_init();  // Bluetooth 데이터 통로의 기반 계층 준비
    sm_init();  // 보안 매니저 준비
    /*
     * profile_data ->  .gatt 파일으로부터 헤더파일에 생성됨
     * BLE GATT/ATT 서버 준비, profile_data는 GATT 서버의 구조와 특성을 정의하는 데이터입니다.
     * .gatt 파일은 GATT 서버의 서비스, 특성, 디스크립터 등을 정의하는 파일입니다. 이 파일을 기반으로 profile_data가 생성되며, 
     * 이를 att_server_init 함수에 전달하여 GATT/ATT 서버를 초기화합니다.
     * 즉, profile_data는 GATT 서버의 구성과 동작을 정의하는 중요한 역할을 합니다.
     */ 

    att_server_init(profile_data, att_read_callback, att_write_callback);  // BLE GATT/ATT 서버 준비

    // inform about BTstack state
    /*
     * HCI/GAP 이벤트용
     * hci_event_callback_registration 구조체는 HCI 이벤트 콜백을 등록하는 데 사용됩니다. 
     * 이 구조체의 callback 멤버에 packet_handler 함수를 할당하여, HCI 이벤트가 발생할 때마다 packet_handler 함수가 호출되도록 설정합니다.
     * hci_add_event_handler 함수를 호출하여, 등록된 콜백이 실제로 HCI 이벤트를 처리할 수 있도록 합니다. 
     * 이렇게 하면 Bluetooth 스택에서 발생하는 다양한 이벤트(예: 연결, 연결 해제, 데이터 수신 등)에 대해 packet_handler 함수가 적절히 대응할 수 있게 됩니다.
     * 
     * 연결됨, 연결 끊김, 스택 준비 완료, 광고 관련 이벤트, 스캔 결과, 상태 변화 같은 상태/이벤트 통지용 콜백
     */
    hci_event_callback_registration.callback = packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // register for ATT event
    att_server_register_packet_handler(packet_handler);

    // set one-shot btstack timer
    /*
     * HEARTBEAT_PERIOD_MS마다 주기적으로 실행되는 타이머 설정
     * HEARTBEAT_PERIOD_MS마다 heartbeat_handler 함수가 호출되도록 설정합니다.
     */
    heartbeat.process = &heartbeat_handler;
    btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(&heartbeat);

    // turn on bluetooth! 
    // goto BTSTACK_EVENT_STATE with HCI_STATE_WORKING
    hci_power_control(HCI_POWER_ON);
}