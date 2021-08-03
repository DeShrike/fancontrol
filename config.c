#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"
#include "config.h"
#include "fancontrol.h"

void free_config(configuration* config)
{
    free(config);
}

int handler(void* config, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)config;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("fan", "pin"))
    {
        pconfig->fanPin = atoi(value);
    }
    else if (MATCH("fan", "on"))
    {
        pconfig->fanOn = atoi(value);
    }
    else if (MATCH("fan", "off"))
    {
        pconfig->fanOff = atoi(value);
    }

    return 1;
}

configuration* read_config(char* filename)
{
    configuration* config;
    config = (configuration*)malloc(sizeof(configuration));
    // printf("Alloc Config %p - %d bytes\n", (void*)config, sizeof(configuration));

    if (ini_parse(filename, handler, config) < 0)
    {
        return NULL;
    }

    return config;
}
