#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "fancontrol.h"
#include "config.h"

static volatile int keepRunning = 1;

// sudo apt-get install wiringpi
// gcc -lwiringPi
// gcc -lwiringPiDev

// Checkout https://www.youtube.com/watch?v=gymfmJIrc3g

#define CONFIG_FILE         "/etc/fancontrol.ini"

#define delayMilliseconds   delay

#define PINMODE(p, m)           if (p != 0) { pinMode(p, m); }
#define PULLUPDNCONTROL(p, m)   if (p != 0) { pullUpDnControl(p, m); }
#define DIGITALWRITE(p, m)      if (p != 0) { digitalWrite(p, m); }
#define DIGITALREAD(p)          (p != 0 ? digitalRead(p) : 0)

#define STEPDELAY          3000      // Delay between temperature checks in ms

configuration* config;

int state = STATE_OFF;

float prev_systemp = 0.0f;

struct timespec time_start;
struct timespec time_stop;

void intHandler(int dummy)
{
    keepRunning = 0;
}

void cleanup()
{
    printf("Cleanup\n");

    DIGITALWRITE(config->fanPin, LOW);
    delayMilliseconds(100);
    PINMODE(config->fanPin, INPUT);
}

void loop()
{
    float systemp, millideg;
    FILE *thermal;

    thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    fscanf(thermal, "%f", &millideg);
    fclose(thermal);
    systemp = millideg / 1000;

    if (systemp != prev_systemp)
    {
        printf("CPU temperature is %f degrees C\n", systemp);
        prev_systemp = systemp;
    }

    if (state == STATE_OFF)
    {
        if (systemp >= (float)config->fanOn)
        {
            printf("Starting fan !\n");
            clock_gettime(CLOCK_REALTIME, &time_start);
            // DIGITALWRITE(config->fanPin, HIGH);
            state = STATE_ON;
        }
    }
    else if (state == STATE_ON)
    {
        if (systemp <= (float)config->fanOff)
        {
            printf("Stopping fan !\n");
            clock_gettime(CLOCK_REALTIME, &time_stop);
            // DIGITALWRITE(config->fanPin, LOW);
            state = STATE_OFF;
        }
    }

    delayMilliseconds(STEPDELAY);
}

void start()
{
    signal(SIGKILL, intHandler);
    signal(SIGINT, intHandler);
    signal(SIGTERM, intHandler);
    signal(SIGHUP, intHandler);

    wiringPiSetupGpio();  // setup wih Broadcom numbering

    PINMODE(config->fanPin, OUTPUT);

    printf("Running! Press Ctrl+C to quit. \n");

    while (keepRunning)
    {
        loop();
    }

    printf("\nStopping\n");
    cleanup();
}

void show_config(configuration* config)
{
    printf("Config loaded from '%s':\n", CONFIG_FILE);

    printf("  Fan Pin: %d \n", config->fanPin);
    printf("  Fan  ON: %d °C\n", config->fanOn);
    printf("  Fan OFF: %d °C\n", config->fanOff);
}

int main(int argc, char* argv[])
{
    config = read_config(CONFIG_FILE);
    if (config == NULL)
    {
        fprintf(stderr, "Can't load '%s'\n", CONFIG_FILE);
        return EXIT_FAILURE;
    }

    show_config(config);

    start();

    free_config(config);

    return EXIT_SUCCESS;
}
