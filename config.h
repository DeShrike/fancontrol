#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int fanPin;
    int fanOn;
    int fanOff;
} configuration;

void free_config(configuration* config);
configuration* read_config(char* filename);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
