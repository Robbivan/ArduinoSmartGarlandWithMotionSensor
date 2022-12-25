#include <SoftwareSerial.h>
#include <FastLED.h>
#include <pixeltypes.h>

unsigned long count = 300000; // 5 минут
#define LED 8                 // пин для подключения реле с лампой
#define PIR 2                 // пин для подключения датчика движения
#define NUM_LEDS 50           // количество светодиодов

int val;    // опция ручного режима

// 1 - радуга
// 2 - бегущий огонек
// 3 - радуга с мерцанием
int mode = 0;

byte brightness = 50;  // Яркость
byte hue = 0;          // Оттенок
byte refresh_rate = 0; // Скорость обновления цвета

byte n = 0; // номер цикла

CRGB leds[NUM_LEDS] unsigned long counttime; // массив светодиодов

void setup()
{
    mySerial.begin(9600);
    pinMode(LED, OUTPUT);
    pinMode(PIR, INPUT);
}

void mode_rainbow()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(hue + i * 5, 255, 255);
    }
    hue++;
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(50 - refresh_rate);
}

void mode_running_light()
{
    fadeToBlackBy(leds, NUM_LEDS, (refresh_rate + 1) * 2);
    int pos = beatsin16(12, 0, NUM_LEDS - 1);
    leds[pos] += CHSV(hue++, 255, 192);
    FastLED.setBrightness(brightness);
    FastLED.show();
}

void mode_rainbow_with_fade()
{
    fill_rainbow(leds, NUM_LEDS, hue++, 5);
    if (random8() < 80)
    {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(50 - refresh_rate);
}

void loop()
{
    // Перенастройка 
    if (mySerial.available())
    {
        val = mySerial.read();
        mode = mySerial.read();
        hue = mySerial.read();
        brightness = mySerial.read();
        refresh_rate = mySerial.read();
        n = 1
        digitalWrite(LED, HIGH);
    }

    // Основной цикл 
    while (n == 1)
    {
        // Переход в условие с переназначением
        if (mySerial.available())
        {
            val = mySerial.read();
            if (val == 'M')
            {
                n = 0;
                break;
            }
        }
        // В случае движения - включение света
        if (digitalRead(PIR) == HIGH)
        {
            digitalWrite(LED, HIGH);
            n = 1;
        }
        // В случае простоя - включение счетчика
        if (digitalRead(PIR) == LOW)
        {
            counttime = millis();
            n = 2;
        }

        // Цикл по определению времени простоя
        while (n == 2)
        {
            // Изменение параметров, выход из цикла
            if (mySerial.available())
            {
                val = mySerial.read();
                if (val == 'M')
                {
                    n = 0;
                    break;
                }
            }

            // Если время простоя превысило максимальное - выключение света
            if (millis() - counttime > count)
            {
                digitalWrite(LED, LOW);
                n = 1;
            }
            
            // В случае повторного движения - включение света 
            // и сброс счетчика путем возвращения в верхний цикл
            if (digitalRead(PIR) == HIGH)
            {
                digitalWrite(LED, HIGH);
                n = 1;
            }

            // Запуск одного из режимов подсветки
            if (mode == 1) mode_rainbow();
            if (mode == 2) mode_running_light();
            if (mode == 3) mode_rainbow_with_fade();
        }

        // Запуск одного из режимов подсветки
        if (mode == 1) mode_rainbow();
        if (mode == 2) mode_running_light();
        if (mode == 3) mode_rainbow_with_fade();
    }
}