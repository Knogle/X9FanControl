

/*
 * chairman/temp.c
 *
 * Temperature control for X9 based Supermicro boards.
 * Using Bang–bang control with hysteresis and f=1/s
 * Δx = 2*1K
 * t=1s
 * PT1 first order lag element.
 *
 * Relational approach g(x) = e^((x - 17.33793493) / 15) + 7.65
 *
 * Copyright (c) Fabian Druschke 2023
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the project's author nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_VALUES 24
#define MAX_FANSPEED 12000
#define MAX_PWM_VAL 255
#define FAN_DIFF (MAX_FANSPEED / MAX_PWM_VAL)
#define MIN_INTERVAL 1

int tachoControl;  // PWM frequency control in kHz (range: 0 - 255).
int sensorValues[MAX_VALUES]; // Maximum number of sensors we want to read.
int interval;      // Interval to repeat execution every X seconds.
int debug;          // Debug mode indicator.

// Function prototypes
void printTemperatureTable();
float calculateFanSpeed(int temperature);
int interpolateFanSpeed(float pwm_frequency);
void swap(int *arr, int n, int m);
void heapSort(int *arr, int n);
void setFanSpeed();

void setFanSpeed() {
    // Function to set fan speed based on temperature sensors.
    // ... (existing code)
    char *discardChar = (char *)malloc(sizeof(path));
    char discard[sizeof(path)];

    // ... (existing code)

    free(discardChar);
    // ... (existing code)
}

void printTemperatureTable() {
    // Function to print a table of temperature values and their corresponding fan speeds.
    for (float i = 0.0; i <= 100.0; i++) // Temperature range from 0 to 100 degrees Celsius
    {
        float pwm_frequency = calculateFanSpeed(i);
        printf("Fan Speed for %0.f°C: %f = %f 1/60s\n", i, pwm_frequency, (pwm_frequency * FAN_DIFF));
    }
}

float calculateFanSpeed(int temperature) {
    // Function to calculate fan speed based on a temperature value.
    // Fan speed calculation based on: g(x) = e^((x - 17.33793493) / 15) + 7.65
    return exp((temperature - 17.33793493) / 15) + 7.65;
}

int interpolateFanSpeed(float pwm_frequency) {
    // Function to interpolate fan speed based on the PWM frequency.
    return tachoControl = (int)(pwm_frequency < 0 ? (pwm_frequency - 0.5) : (pwm_frequency + 0.5));
}

void swap(int *arr, int n, int m) {
    // Function to swap elements in an array.
    int tmp = arr[n];
    arr[n] = arr[m];
    arr[m] = tmp;
}

void heapSort(int *arr, int n) {
    // Function to perform heap sort on an array.
    arr -= 1;

    for (int last = 1; last <= n; ++last) {
        int current = last;
        while (current > 1) {
            int parent = current / 2;
            if (arr[parent] > arr[current])
                break;

            swap(arr, parent, current);
            current = parent;
        }
    }

    for (int last = n - 1; last >= 1; --last) {
        swap(arr, 1, last + 1);
        int current = 1;

        while (1) {
            int max = current;
            int left = current * 2;
            int right = left + 1;

            if (left <= last && arr[left] > arr[max])
                max = left;
            if (right <= last && arr[right] > arr[max])
                max = right;

            if (current == max)
                break;

            swap(arr, max, current);
            current = max;
        }
    }
}

int main(int argc, char *argv[]) {
    debug = 0;
    interval = MIN_INTERVAL;

    if (argc == 1) {
        printf("Setting Fan Speed.\n");
        setFanSpeed();
    } else if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            printf("Usage: %s [INTERVAL] [OPTIONS]\n", argv[0]);
            printf("  --table  Print scaled fan speed information\n");
            printf("  --debug  Enable debug mode\n");
        } else if (strcmp(argv[1], "--table") == 0) {
            printTemperatureTable();
        } else if (strcmp(argv[1], "--debug") == 0) {
            debug = 1;
            printf("Setting Fan Speed.\n");
            setFanSpeed();
        } else {
            printf("Invalid input specified.\n");
        }
    } else if (argc >= 2 && argc <= 4) {
        char *endPtr;
        long intervalValue = strtol(argv[1], &endPtr, 10);

        if (endPtr[0] == '\0' && intervalValue >= MIN_INTERVAL) {
            interval = (int)intervalValue;

            if (argc == 4 && strcmp(argv[3], "--debug") == 0) {
                debug = 1;
            }

            printf("Hysteresis: %d seconds. This program will continue until being interrupted.\n", interval);

            while (1) {
                setFanSpeed();
                sleep(interval);
            }
        } else {
            printf("Invalid interval specified.\n");
        }
    } else {
        printf("Invalid arguments. Use '%s --help' for usage information.\n", argv[0]);
    }

    return 0;
}

