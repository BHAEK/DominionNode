
// GPIO PINS
#define GPIO_LED_RED     19
#define GPIO_LED_BLUE    18
#define GPIO_BTN_RED     5
#define GPIO_BTN_BLUE    4

#define GPIO_7SEG_RED_CLK   13
#define GPIO_7SEG_RED_SDO   14
#define GPIO_7SEG_BLUE_CLK  15
#define GPIO_7SEG_BLUE_SDO  16

#define GPIO_BUZZER         21

// GENERIC
#define DEBOUNCE_DELAY_MS       200
#define SETTINGS_HOLD_TIME_MS   3000

// EVENT BITS
#define BTN_RED_EVENT   (1 << 0)
#define BTN_BLUE_EVENT  (1 << 1)

// TASKS STACK DEPTH
#define BUTTON_TASK_STACK_DEPTH     2048
#define APP_TASK_STACK_DEPTH        2048
#define DISPLAY_TASK_STACK_DEPTH    2048
#define BUZZER_TASK_STACK_DEPTH     2048

// TASK PRIORITY
#define BUTTON_TASK_PRIORITY        5
#define APP_TASK_PRIORITY           3
#define DISPLAY_TASK_PRIORITY       3
#define BUZZER_TASK_PRIORITY        3     

// OTHER
#define FW_VERSION                  1.0