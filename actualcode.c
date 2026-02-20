#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
//#include "driver/adc.h"
#include "esp_log.h"

/* ================== CONFIG ================== */
#define TAG "PILL_BOX"
#define BUZZER_GPIO GPIO_NUM_32

/* ---------- Stepper ---------- */
#define IN1 GPIO_NUM_13
#define IN2 GPIO_NUM_12
#define IN3 GPIO_NUM_14
#define IN4 GPIO_NUM_27

#define STEPS_51_DEG 592
#define STEP_DELAY_MS 10

/* ---------- Sensor ---------- */
//#define TCRT_ADC_CHANNEL ADC1_CHANNEL_6   // GPIO 34
//#define PILL_THRESHOLD 1200
#define TCRT_GPIO GPIO_NUM_25   

/* ---------- OLED I2C ---------- */
#define OLED_ADDR 0x3C
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000


static const uint8_t font5x7[][5] = {
{0x00,0x00,0x00,0x00,0x00}, // ' '
{0x00,0x00,0x5F,0x00,0x00}, // !
{0x00,0x07,0x00,0x07,0x00}, // "
{0x14,0x7F,0x14,0x7F,0x14}, // #
{0x24,0x2A,0x7F,0x2A,0x12}, // $
{0x23,0x13,0x08,0x64,0x62}, // %
{0x36,0x49,0x55,0x22,0x50}, // &
{0x00,0x05,0x03,0x00,0x00}, // '
{0x00,0x1C,0x22,0x41,0x00}, // (
{0x00,0x41,0x22,0x1C,0x00}, // )
{0x14,0x08,0x3E,0x08,0x14}, // *
{0x08,0x08,0x3E,0x08,0x08}, // +
{0x00,0x50,0x30,0x00,0x00}, // ,
{0x08,0x08,0x08,0x08,0x08}, // -
{0x00,0x60,0x60,0x00,0x00}, // .
{0x20,0x10,0x08,0x04,0x02}, // /

{0x3E,0x51,0x49,0x45,0x3E}, // 0
{0x00,0x42,0x7F,0x40,0x00}, // 1
{0x42,0x61,0x51,0x49,0x46}, // 2
{0x21,0x41,0x45,0x4B,0x31}, // 3
{0x18,0x14,0x12,0x7F,0x10}, // 4
{0x27,0x45,0x45,0x45,0x39}, // 5
{0x3C,0x4A,0x49,0x49,0x30}, // 6
{0x01,0x71,0x09,0x05,0x03}, // 7
{0x36,0x49,0x49,0x49,0x36}, // 8
{0x06,0x49,0x49,0x29,0x1E}, // 9

{0x00,0x36,0x36,0x00,0x00}, // :
{0x00,0x56,0x36,0x00,0x00}, // ;
{0x08,0x14,0x22,0x41,0x00}, // <
{0x14,0x14,0x14,0x14,0x14}, // =
{0x00,0x41,0x22,0x14,0x08}, // >
{0x02,0x01,0x51,0x09,0x06}, // ?

{0x32,0x49,0x79,0x41,0x3E}, // @
{0x7E,0x11,0x11,0x11,0x7E}, // A
{0x7F,0x49,0x49,0x49,0x36}, // B
{0x3E,0x41,0x41,0x41,0x22}, // C
{0x7F,0x41,0x41,0x22,0x1C}, // D
{0x7F,0x49,0x49,0x49,0x41}, // E
{0x7F,0x09,0x09,0x09,0x01}, // F
{0x3E,0x41,0x49,0x49,0x7A}, // G
{0x7F,0x08,0x08,0x08,0x7F}, // H
{0x00,0x41,0x7F,0x41,0x00}, // I
{0x20,0x40,0x41,0x3F,0x01}, // J
{0x7F,0x08,0x14,0x22,0x41}, // K
{0x7F,0x40,0x40,0x40,0x40}, // L
{0x7F,0x02,0x04,0x02,0x7F}, // M
{0x7F,0x04,0x08,0x10,0x7F}, // N
{0x3E,0x41,0x41,0x41,0x3E}, // O
{0x7F,0x09,0x09,0x09,0x06}, // P
{0x3E,0x41,0x51,0x21,0x5E}, // Q
{0x7F,0x09,0x19,0x29,0x46}, // R
{0x46,0x49,0x49,0x49,0x31}, // S
{0x01,0x01,0x7F,0x01,0x01}, // T
{0x3F,0x40,0x40,0x40,0x3F}, // U
{0x1F,0x20,0x40,0x20,0x1F}, // V
{0x3F,0x40,0x38,0x40,0x3F}, // W
{0x63,0x14,0x08,0x14,0x63}, // X
{0x07,0x08,0x70,0x08,0x07}, // Y
{0x61,0x51,0x49,0x45,0x43}, // Z

{0x00,0x7F,0x41,0x41,0x00}, // [
{0x02,0x04,0x08,0x10,0x20}, // 
{0x00,0x41,0x41,0x7F,0x00}, // ]
{0x04,0x02,0x01,0x02,0x04}, // ^
{0x40,0x40,0x40,0x40,0x40}, // _

{0x00,0x01,0x02,0x04,0x00}, // `
{0x20,0x54,0x54,0x54,0x78}, // a
{0x7F,0x48,0x44,0x44,0x38}, // b
{0x38,0x44,0x44,0x44,0x20}, // c
{0x38,0x44,0x44,0x48,0x7F}, // d
{0x38,0x54,0x54,0x54,0x18}, // e
{0x08,0x7E,0x09,0x01,0x02}, // f
{0x0C,0x52,0x52,0x52,0x3E}, // g
{0x7F,0x08,0x04,0x04,0x78}, // h
{0x00,0x44,0x7D,0x40,0x00}, // i
{0x20,0x40,0x44,0x3D,0x00}, // j
{0x7F,0x10,0x28,0x44,0x00}, // k
{0x00,0x41,0x7F,0x40,0x00}, // l
{0x7C,0x04,0x18,0x04,0x78}, // m
{0x7C,0x08,0x04,0x04,0x78}, // n
{0x38,0x44,0x44,0x44,0x38}, // o
{0x7C,0x14,0x14,0x14,0x08}, // p
{0x08,0x14,0x14,0x18,0x7C}, // q
{0x7C,0x08,0x04,0x04,0x08}, // r
{0x48,0x54,0x54,0x54,0x20}, // s
{0x04,0x3F,0x44,0x40,0x20}, // t
{0x3C,0x40,0x40,0x20,0x7C}, // u
{0x1C,0x20,0x40,0x20,0x1C}, // v
{0x3C,0x40,0x30,0x40,0x3C}, // w
{0x44,0x28,0x10,0x28,0x44}, // x
{0x0C,0x50,0x50,0x50,0x3C}, // y
{0x44,0x64,0x54,0x4C,0x44}  // z
};



/* ================== DELAY ================== */
static inline void delay_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/* ================== I2C ================== */
static void i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

/* ================== OLED LOW LEVEL ================== */
static void oled_write_command(uint8_t cmd) {
    i2c_cmd_handle_t h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (OLED_ADDR << 1), true);
    i2c_master_write_byte(h, 0x00, true);
    i2c_master_write_byte(h, cmd, true);
    i2c_master_stop(h);
    i2c_master_cmd_begin(I2C_MASTER_NUM, h, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(h);
}

static void oled_write_data(uint8_t data) {
    i2c_cmd_handle_t h = i2c_cmd_link_create();
    i2c_master_start(h);
    i2c_master_write_byte(h, (OLED_ADDR << 1), true);
    i2c_master_write_byte(h, 0x40, true);
    i2c_master_write_byte(h, data, true);
    i2c_master_stop(h);
    i2c_master_cmd_begin(I2C_MASTER_NUM, h, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(h);
}

/* ================== OLED INIT ================== */
static void oled_init(void) {
    delay_ms(100);
    oled_write_command(0xAE);
    oled_write_command(0xD5); oled_write_command(0x80);
    oled_write_command(0xA8); oled_write_command(0x3F);
    oled_write_command(0xD3); oled_write_command(0x00);
    oled_write_command(0x40);
    oled_write_command(0x8D); oled_write_command(0x14);
    oled_write_command(0x20); oled_write_command(0x00);
    oled_write_command(0xA1);
    oled_write_command(0xC8);
    oled_write_command(0xDA); oled_write_command(0x12);
    oled_write_command(0x81); oled_write_command(0xCF);
    oled_write_command(0xD9); oled_write_command(0xF1);
    oled_write_command(0xDB); oled_write_command(0x40);
    oled_write_command(0xA4);
    oled_write_command(0xA6);
    oled_write_command(0xAF);
}

static void oled_clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        oled_write_command(0xB0 + page);
        oled_write_command(0x00);
        oled_write_command(0x10);
        for (uint8_t col = 0; col < 128; col++) {
            oled_write_data(0x00);
        }
    }
}

static void oled_set_cursor(uint8_t page, uint8_t col) {
    oled_write_command(0xB0 + page);
    oled_write_command(col & 0x0F);
    oled_write_command(0x10 | (col >> 4));
}

/* ================== FONT ================== */
extern const uint8_t font5x7[][5];

static void oled_draw_char(char c) {
    if (c < 32 || c > 126) c = '?';
    const uint8_t *g = font5x7[c - 32];
    for (int i = 0; i < 5; i++) oled_write_data(g[i]);
    oled_write_data(0x00);
}

static void oled_draw_string(uint8_t page, uint8_t col, const char *s) {
    oled_set_cursor(page, col);
    while (*s) oled_draw_char(*s++);
}

/* ================== STEPPER ================== */
static const int seq[8][4] = {
    {1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},
    {0,0,1,0},{0,0,1,1},{0,0,0,1},{1,0,0,1}
};

static void rotate_51_deg(void) {
    static int idx = 0;
    for (int i = 0; i < STEPS_51_DEG; i++) {
        gpio_set_level(IN1, seq[idx][0]);
        gpio_set_level(IN2, seq[idx][1]);
        gpio_set_level(IN3, seq[idx][2]);
        gpio_set_level(IN4, seq[idx][3]);
        idx = (idx + 1) % 8;
        delay_ms(STEP_DELAY_MS);
    }
}

/* ================== SENSOR ================== */
//static bool pill_present(void) {
//    int v = adc1_get_raw(TCRT_ADC_CHANNEL);
//    ESP_LOGI(TAG, "ADC=%d", v);
//    return (v < PILL_THRESHOLD);
//}
static bool pill_present(void)
{
    int v = gpio_get_level(TCRT_GPIO);
    ESP_LOGI(TAG, "TCRT OUT = %d", v);
    return (v == 0);   // LOW = pill detected
}



/* ================== MAIN ================== */
void app_main(void)
{
    /* ---------- Stepper GPIO ---------- */
    gpio_config_t g = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask =
            (1ULL << IN1) |
            (1ULL << IN2) |
            (1ULL << IN3) |
            (1ULL << IN4)
    };
    gpio_config(&g);

    /* ---------- TCRT5000 DIGITAL INPUT ---------- */
    gpio_config_t s = {
        .pin_bit_mask = (1ULL << TCRT_GPIO),
        .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&s);

    /* ---------- BUZZER OUTPUT ---------- */
    gpio_config_t b = {
        .pin_bit_mask = (1ULL << BUZZER_GPIO),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&b);
    gpio_set_level(BUZZER_GPIO, 0);

    /* ---------- OLED ---------- */
    i2c_master_init();
    oled_init();
    oled_clear();

    /* ---------- TIME & ROTATION SETUP ---------- */
    int rotation_times[6] = {10, 20, 30, 40, 50, 60};  // ONLY pill doses
    int rotation_index = 0;

    int total_doses = 6;
    int successful_doses = 0;
    int missed_doses = 0;

    bool last_missed = false;
    bool first_dose_done = false;
    bool home_rotation_done = false;

    int last_rotation_time = 0;
    int time_sec = 0;

    char line1[40], line2[40];

    /* ---------- MAIN LOOP (ONLY 6 PILL CHAMBERS) ---------- */
    while ((successful_doses + missed_doses) < total_doses)
    {
        int next_rotation_time = rotation_times[rotation_index];

        /* ---------- BUZZER ↔ SENSOR ---------- */
        if (pill_present()) {
            gpio_set_level(BUZZER_GPIO, 1);
        } else {
            gpio_set_level(BUZZER_GPIO, 0);
        }

        oled_clear();

        /* ---------- DISPLAY ---------- */
        if (!first_dose_done) {
            snprintf(line1, sizeof(line1), "FIRST DOSE");
            int nd = next_rotation_time - time_sec;
            if (nd < 0) nd = 0;
            snprintf(line2, sizeof(line2), "Next Dose in %ds", nd);

            oled_draw_string(1, 0, line1);
            oled_draw_string(3, 0, line2);
        }
        else {
            if (last_missed) {
                snprintf(line1, sizeof(line1), "Last Dose MISSED");
            } else {
                int ld = time_sec - last_rotation_time;
                if (ld > 999) ld = 999;
                snprintf(line1, sizeof(line1), "Last Dose %ds ago", ld);
            }

            int nd = next_rotation_time - time_sec;
            if (nd < 0) nd = 0;
            snprintf(line2, sizeof(line2), "Next Dose in %ds", nd);

            oled_draw_string(1, 0, line1);
            oled_draw_string(3, 0, line2);
        }

        /* ---------- DOSE EVENT ---------- */
        if (time_sec == next_rotation_time)
        {
            if (pill_present()) {
                /* ❌ PILL NOT TAKEN */
                last_missed = true;
                missed_doses++;
                first_dose_done = true;

                ESP_LOGW(TAG, "DOSE %d MISSED", rotation_index + 1);
            }
            else {
                /* ✅ PILL TAKEN */
                oled_clear();
                oled_draw_string(4, 0, "ROTATING");

                rotate_51_deg();

                successful_doses++;
                last_rotation_time = time_sec;
                last_missed = false;
                first_dose_done = true;

                ESP_LOGI(TAG, "DOSE %d TAKEN", rotation_index + 1);
            }

            rotation_index++;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
        time_sec++;
    }

    gpio_set_level(BUZZER_GPIO, 0);

    /* ---------- HOME ROTATION (7th) ---------- */
    if (missed_doses == 0)
    {
        oled_clear();
        oled_draw_string(3, 0, "RETURNING HOME");
        ESP_LOGI(TAG, "NO MISSED DOSES → HOME ROTATION");

        rotate_51_deg();   // 7th rotation
        home_rotation_done = true;
    }
    else
    {
        oled_clear();
        oled_draw_string(2, 0, "MISSED DOSE");
        oled_draw_string(4, 0, "NOT HOME");
        ESP_LOGW(TAG, "MISSED DOSE → NO HOME ROTATION");
    }

    /* ---------- FINAL DISPLAY ---------- */
    delay_ms(5000);
    oled_clear();

    if (home_rotation_done) {
        oled_draw_string(2, 0, "ALL DOSES DONE");
    } else {
        oled_draw_string(2, 0, "CHECK BOX");
    }

    ESP_LOGI(TAG, "SYSTEM FINISHED");
}