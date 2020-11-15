/*
 * SD_Card.c
 *
 *  Created on: 19 Sep 2019
 *      Author: jonga
 */

#include "SD_Card.h"
#include <stdio.h>
#include "stm32l4xx_hal.h"
#include <time.h>

struct tm t;
time_t t_of_day;
int timestampCounter = 0;
uint32_t timestamps[2000];
int pathCounter = 0;
char pathStrings[50][30];
char currPath[50] = "uv";
char yearmonthdayStr[20] = "";
char hourminStr[20] = "";
char secStr[20] = "";
float uvIndexes[50];
int uvCounter = 0;

int sd_card_read(void) {
	FRESULT fr;
	FATFS fs;
	FIL fil;
	char line[100];

	/* Open or create a log file and ready to append */
	int mount = f_mount(&fs, "", 0);
	printf("mount = %d\n\r", mount);
	fr = f_open(&fil, "test.txt", FA_READ);
	if (fr != FR_OK) {
		printf("fr = %d\n\r", fr);
		return fr;
	}
	while (f_gets(line, sizeof line, &fil)) {
		printf(line);
	}

	return 0;
}

/**
  * @brief  Creates files for last measurements, last uv
  * 		index, and last SED calculation
  * @param  Real time clock typedef, Struct for display parameters
  * @retval Void
  */
int sd_card_write(RTC_HandleTypeDef hrtc, displayParameters* params, FATFS fs, SD_HandleTypeDef *hsd) {

	//Re-initialise
	HAL_SD_Init(hsd);

	FRESULT fr;
	//FATFS fs;
	FIL fil;
	//Create new file
	FRESULT mount = f_mount(&fs, "", 1);
	if (mount !=0) {
		printf("mount = %d\n\r", mount);
	}
	char fileName[50];
	char measurementString[50];
	char UVString[50];
	char SEDString[50];

	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	params->years = sDate.Year + 2000; //Function getting year
	params->months = sDate.Month; //Function getting month
	params->days = sDate.Date; //Function getting day
	params->hours = sTime.Hours; //Function getting hour
	params->minutes = sTime.Minutes; //Function getting minute
	params->seconds = sTime.Seconds; //Function getting second

	//sprintf(fileName, "uv-%d-%d-%d-%d-%d-%d.csv", year, month, day, hour, minute, second);
	//printf("filename = %s\n\r", fileName);

	//Create directories
	sprintf(fileName, "uv");
	FRESULT res = f_mkdir(fileName);
	sprintf(fileName, "uv/%d-%02d-%02d", params->years, params->months, params->days);
	res = f_mkdir(fileName);
	sprintf(fileName, "uv/%d-%02d-%02d/%02d-%02d", params->years, params->months, params->days, params->hours, params->minutes);
	res = f_mkdir(fileName);

	//Store last measurement csv file
	sprintf(fileName, "uv/%d-%02d-%02d/%02d-%02d/%02d.csv", params->years, params->months, params->days, params->hours, params->minutes, params->seconds);
	fr = f_open(&fil, fileName, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		printf("Error\r\n");
		return fr;
	}

	sprintf(measurementString, "%.1f, %.3f", params->UVIndex, params->accumulatedDosage);
	f_printf(&fil, "%s", measurementString);
	f_close(&fil);

	//Store most recent UV index
	sprintf(fileName, "uv/latest-index.txt");
	fr = f_open(&fil, fileName, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		return fr;
	}
	sprintf(UVString, "%.1f", params->UVIndex);
	f_printf(&fil, "%s", UVString);
	f_close(&fil);

	//Store most recent SED calculation
	sprintf(fileName, "uv/latest-sed.txt");
	fr = f_open(&fil, fileName, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		return fr;
	}
	sprintf(SEDString, "%.3f", params->accumulatedDosage);
	f_printf(&fil, "%s", SEDString);
	f_close(&fil);

	//Store most MED
	sprintf(fileName, "uv/latest-MED.txt");
	fr = f_open(&fil, fileName, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		return fr;
	}
	f_printf(&fil, "%d", params->MED);
	f_close(&fil);

	//Store most skin type
	sprintf(fileName, "uv/latest-skin-type.txt");
	fr = f_open(&fil, fileName, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		return fr;
	}
	f_printf(&fil, "%d", params->fitzpatrickType);
	f_close(&fil);

	params->totalPoints = 0;
	read_files("uv", params);
//	printf_timestamps();
	printf("Total points = %d\n\r", params->totalPoints);

	//Demount
	f_mount(0, "", 1);

	//De-initialise
	HAL_SD_DeInit(hsd);

//	timestampCounter = 0;
//	pathCounter = 0;
//	uvCounter = 0;
	printf("SD card done\n\r");

	return 0;
}

/**
  * @brief  Recomputes the total SED using all data from SD card
  * @param  Struct for display parameters
  * @retval Void
  */
int recompute_total_SED(displayParameters* params, FATFS fs, SD_HandleTypeDef *hsd) {
	int i;
	float newSED = 0.0;
	int t;
	char fileName[50];
	char measurementString[50];

	printf("recomputing SED\n\r");

	//Re-initialise
	HAL_SD_Init(hsd);

	FRESULT fr;
	FIL fil;
	FRESULT mount = f_mount(&fs, "", 1);
	if (mount !=0) {
		printf("mount = %d\n\r", mount);
	}

	for (i = 0; i < params->totalPoints; i++) {
		if (i == 0) {
			t = 60;
		} else {
			t = timestamps[i] - timestamps[i - 1];
			printf("t = %d\n\r", t);
		}
		newSED += t * 0.25 * uvIndexes[i];
		printf("new SED = %f\n\r", newSED);
		printf("uv index = %f\n\r", uvIndexes[i]);

		sprintf(fileName, "uv/%s", pathStrings[i]);

		fr = f_open(&fil, fileName, FA_WRITE | FA_CREATE_ALWAYS);
		if (fr != FR_OK) {
			printf("Error\r\n");
			return fr;
		}

		sprintf(measurementString, "%.1f, %.3f", uvIndexes[i], newSED);
		f_printf(&fil, "%s", measurementString);
		f_close(&fil);

	}

	//Demount
	f_mount(0, "", 0);

	//De-initialise
	HAL_SD_DeInit(hsd);

	printf("finished recomputing\n\r");
}

void read_files(char* path, displayParameters* params) {

	DIR dir;
	FILINFO fno;
	FRESULT res1;
	FIL fil;
	char newPath[256];
	char line[100];
	uint8_t moreDirectories = 0;

	//Open Directory
	res1 = f_opendir(&dir, path);

	//Still files to be read
	while (moreDirectories != 1) {
		res1 = f_readdir(&dir, &fno);

		if (fno.fattrib & AM_DIR) {
			//Directory
			//printf("DIR - %s\n\r", fno.fname);
			if (count_dashes(fno.fname) == 2) {
				update_year_month_day(fno.fname);
				sprintf(yearmonthdayStr, "%s", fno.fname);
			} else if (count_dashes(fno.fname) == 1) {
				//hour-minute
				update_hour_minute(fno.fname);
				sprintf(hourminStr, "%s", fno.fname);
			}

			sprintf(newPath, "%s/%s", path, fno.fname);
			read_files(newPath, params);

		} else if (fno.fattrib & AM_ARC){
			if (fno.fname[0] == 0) {
				//End of stuff being read
			} else {
				if (strstr(fno.fname, ".txt") == NULL) {
					//File
					//printf("FILE - %s**\n\r", fno.fname);
					update_seconds(fno.fname);
					update_unix_timestamp();
					sprintf(secStr, "%s", fno.fname);
					update_current_path();
					//add_current_path();
					//printf("UNIX TIMESTAMP: %ld\n", (long) t_of_day);
					memset(newPath,0,strlen(newPath));
					sprintf(newPath, "%s/%s", path, fno.fname);
					FRESULT openRes = f_open(&fil, newPath, FA_READ);
					if (openRes == FR_OK) {
						while (f_gets(line, sizeof line, &fil)) {
							//printf("CONTENTS - %s", line);
							update_uv_indexes(line);
						}
						params->totalPoints++;
						f_close(&fil);
					} else {
						printf("openres = %d\n\r", openRes);
					}
				}
			}
		}
		if (fno.fname[0] == 0) {
			moreDirectories = 1;
		}
	}
    f_closedir(&dir);
}

void update_year_month_day(char* path) {
	char yearStr[5] = "";
	char monthStr[5] = "";
	char dayStr[5] = "";
	int i;
	char nextChar;
	int whichParam = 0;
	int yearInt;
	int monthInt;
	int dayInt;

	for (i = 0; i < strlen(path); i++) {
		nextChar = path[i];
		if (nextChar == '-') {
			//next parameter
			whichParam++;
		} else {
			if (whichParam == 0) {
				//year
				sprintf(yearStr, "%s%c", yearStr, nextChar);
			} else if (whichParam == 1) {
				//year
				sprintf(monthStr, "%s%c", monthStr, nextChar);
			} else if (whichParam == 2) {
				//year
				sprintf(dayStr, "%s%c", dayStr, nextChar);
			}
		}
	}

	//Convert strings to ints
	yearInt = atoi(yearStr);
	monthInt = atoi(monthStr);
	dayInt = atoi(dayStr);

	//Place in global time variable
	t.tm_year = yearInt;
	t.tm_mon = monthInt;
	t.tm_mday = dayInt;
}

void update_hour_minute(char* path) {
	char hourStr[5] = "";
	char minuteStr[5] = "";
	int i;
	char nextChar;
	int whichParam = 0;
	int hourInt;
	int minuteInt;

	for (i = 0; i < strlen(path); i++) {
		nextChar = path[i];
		if (nextChar == '-') {
			//next parameter
			whichParam++;
		} else {
			if (whichParam == 0) {
				//hour
				sprintf(hourStr, "%s%c", hourStr, nextChar);
			} else if (whichParam == 1) {
				//minute
				sprintf(minuteStr, "%s%c", minuteStr, nextChar);
			}
		}
	}

	//Convert strings to ints
	hourInt = atoi(hourStr);
	minuteInt = atoi(minuteStr);

	//Place in global time variable
	t.tm_hour = hourInt;
	t.tm_min = minuteInt;
}

void update_seconds(char* path) {
	char secondsStr[5] = "";
	int i;
	char nextChar = 'j';
	int secondsInt;

	for (i = 0; i < strlen(path); i++) {
		nextChar = path[i];
		if (nextChar == '.') {
			//Nothing
		} else {
			sprintf(secondsStr, "%s%c", secondsStr, nextChar);
		}
	}

	//Convert strings to ints
	secondsInt = atoi(secondsStr);

	//Place in global time variable
	t.tm_sec = secondsInt;
}

int count_dashes(char* path) {
	int i;
	int dashCount = 0;
	char nextChar;

	for (i = 0; i < strlen(path); i++) {
		nextChar = path[i];
		if (nextChar == '-') {
			dashCount++;
		}
	}
	return dashCount;
}

void update_unix_timestamp(void) {
	t.tm_isdst = -1;
	t_of_day = mktime(&t);
	timestamps[timestampCounter] = t_of_day;
	timestampCounter++;
}

void printf_timestamps(void) {
	int i;

//	printf("Unsorted:\n\r");
//	for (i = 0; i < timestampCounter; i++) {
//		printf("%d\n\r", (long) timestamps[i]);
//	}

	qsort(timestamps, timestampCounter, sizeof(uint32_t), cmpfunc);
//	printf("Sorted:\n\r");
//	for (i = 0; i < timestampCounter; i++) {
//		//printf("%d : %s\n\r", (long) timestamps[i], ctime(&timestamps[i]));
//		printf("%d	\n\r", (long) timestamps[i]);
//
//	}

	for (i = 0; i < timestampCounter - 1; i++) {
		printf("time between measurements = %d\n\r", timestamps[i + 1] - timestamps[i]);
	}

	for (i = pathCounter; i < 50; i++) {
		sprintf(pathStrings[i], "~");
	}

	printf("sorting file strings\n\r");
	//int stringLen = sizeof(pathStrings[0]) / sizeof(char *);
	qsort(pathStrings, 50, sizeof(pathStrings[0]), cmpfuncStr);
	for (i = 0; i < pathCounter - 1; i++) {
		printf("uv path sorted = %s\n\r", pathStrings[i]);
	}
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int cmpfuncStr (const void* a, const void* b) {
	const char **ia = (const char **)a;
	const char **ib = (const char **)b;

	return strcmp(ia, ib);
}

void update_current_path(void) {

	//memset(currPath,0,strlen(currPath));
	sprintf(currPath, "%s/%s/%s", yearmonthdayStr, hourminStr, secStr);
	strcpy(pathStrings[pathCounter], currPath);
	//pathStrings[pathCounter] = currPath;
	pathCounter++;
}

void update_uv_indexes(char* line) {
	int i;
	char justUV[5] = "";
	char nextChar;
	float uvFloat;

	for (i = 0; i < strlen(line); i++) {
		nextChar = line[i];

		if (nextChar == ',') {
			break;
		} else {
			sprintf(justUV, "%s%c", justUV, nextChar);
		}
	}
	uvFloat = atof(justUV);
	uvIndexes[uvCounter] = uvFloat;
	//printf("uv = %f\n\r", uvIndexes[uvCounter]);
	uvCounter++;
}
