/*
 * SD_Card.h
 *
 *  Created on: 19 Sep 2019
 *      Author: jonga
 */

#ifndef SD_CARD_H_
#define SD_CARD_H_

#include "stm32l4xx_hal.h"
#include "displayParameters.h"
#include "fatfs.h"
#include "ff.h"
#include <time.h>

extern struct tm t;
extern time_t t_of_day;
extern int timestampCounter;
extern uint32_t timestamps[2000];
extern char currPath[50];
extern char pathStrings[50][30];
extern int pathCounter;
extern char yearmonthdayStr[20];
extern char hourminStr[20];
extern char secStr[20];
extern float uvIndexes[50];
extern int uvCounter;

int sd_card_read(void);
int sd_card_write(RTC_HandleTypeDef hrtc, displayParameters* params, FATFS fs, SD_HandleTypeDef *hsd);
int recompute_total_SED(displayParameters* params, FATFS fs, SD_HandleTypeDef *hsd);
void read_files(char* path, displayParameters* params);
void update_year_month_day(char* path);
void update_hour_minute(char* path);
void update_seconds(char* path);
void update_unix_timestamp(void);
int count_dashes(char* path);
int cmpfunc (const void * a, const void * b);
int cmpfuncStr (const void * a, const void * b);
static int cmpstringp(const void *p1, const void *p2);
void update_uv_indexes(char* line);

#endif /* SD_CARD_H_ */
