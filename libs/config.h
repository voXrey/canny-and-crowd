#ifndef CONFIG_H
#define CONFIG_H


extern int DEBUG_MODE; // 0 = no debug, 1 = debug, 2 = verbose debug

// Charger une configuration à partir d'un fichier
void config_load(const char* filename);

#endif