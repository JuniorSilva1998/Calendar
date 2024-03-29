#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 480
#define REFRESH_RATE 60.F

SDL_Window *window = NULL;
SDL_Color *color = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *fontTitle = NULL;
TTF_Font *fontHeader = NULL;
TTF_Font *fontParagraph = NULL;

struct Date {
    int year;
    int month;
    int day;
};
struct Date date;

struct todayDate {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};
struct todayDate todayDate;

struct Local {
    int region;
};
struct Local local;

int init();
void close();

void getTodayDate();
void setDateToTodayDate();
void setLocal(int region);
int leapYearCheck(int year);
int getNumberOfDaysInMonth(int year, int month);
const char* getDayName(int day);
int getDayNumber(int year, int month, int day);
const char* getMonthName(int month);

void previousYear();
void nextYear();
void previousMonth();
void nextMonth();

void renderDayOfWeek(SDL_Surface* textSurface, SDL_Color color);
void renderYear(SDL_Surface* textSurface, SDL_Color color);
void renderMonth(SDL_Surface* textSurface, SDL_Color color);
void renderDate();
void renderTime(SDL_Surface* textSurface, SDL_Color color);

int main(int arg, char *argv[]) {
    /* Start up SDL and create window */
	if (init() < 0) {
		printf("Failed to initialize!\n");
        return -1;
	} else {
        fontTitle = TTF_OpenFont("Arial.ttf", SCREEN_HEIGHT / 13);
		if (!fontHeader) {
			printf("Failed to load Arial font! SDL_ttf Error: %s\n", TTF_GetError());
		}
        fontHeader = TTF_OpenFont("Arial.ttf", SCREEN_HEIGHT / 20);
		if (!fontHeader) {
			printf("Failed to load Arial font! SDL_ttf Error: %s\n", TTF_GetError());
		}
        fontParagraph = TTF_OpenFont("Arial.ttf", SCREEN_HEIGHT / 30);
		if (!fontParagraph) {
			printf("Failed to load Arial font! SDL_ttf Error: %s\n", TTF_GetError());
		}
        setLocal(1);
        getTodayDate();
        setDateToTodayDate();
		int quit = 0;
        while (!quit) {
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);
            SDL_Surface* textSurface;
            SDL_Color color = {0xFF, 0xFF, 0xFF};

            renderDayOfWeek(textSurface, color);

            getTodayDate();
            renderYear(textSurface, color);
            renderMonth(textSurface, color);
            renderDate();
            renderTime(textSurface, color);

			SDL_RenderPresent(renderer);
            SDL_Delay((1.F / 60) * 1000);

            int region = local.region;
            int year = date.year;
            int month = date.month;

            /* Handle events on queue */
			SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
					quit = 1;
				} else if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
                        case SDLK_UP:
							nextYear();
							break;
						case SDLK_DOWN:
							previousYear();
							break;
					    case SDLK_LEFT:
							previousMonth();
							break;
						case SDLK_RIGHT:
							nextMonth();
							break;
                        case SDLK_e:
                            region++;
							setLocal(region);
                            date.year = year;
                            date.month = month;
							break;
						case SDLK_q:
                            region--;
							setLocal(region);
                            date.year = year;
                            date.month = month;
							break;
                        case SDLK_RETURN:
                            setDateToTodayDate();
                            break;
                    }
                }
            }
        }
    }

    close();
    return 0;
}

int init() {
	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return -1;
	} else {
		/* Create window */
		window = SDL_CreateWindow("Calendar", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (!window) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return -1;
		} else {
			/* Create renderer for window */
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (!renderer) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				return -1;
			} else {
				/* Initialize SDL_ttf */
				if (TTF_Init() < 0) {
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					return -1;
				} else {
                    return 0;
                }
			}
		}
	}
}

void close() {
	/* Free global font */
    TTF_CloseFont(fontTitle);
	TTF_CloseFont(fontHeader);
    TTF_CloseFont(fontParagraph);
    fontTitle = NULL;
	fontHeader = NULL;
	fontParagraph = NULL;

	/* Destroy window */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	/* Quit SDL subsystems */
	TTF_Quit();
	SDL_Quit();
}

void getTodayDate() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    todayDate.year = tm.tm_year + 1900;
    todayDate.month = tm.tm_mon + 1;
    todayDate.day = tm.tm_mday;
    todayDate.hour = tm.tm_hour;
    todayDate.minute = tm.tm_min;
    todayDate.second = tm.tm_sec;
}

void setDateToTodayDate() {
    date.year = todayDate.year;
    date.month = todayDate.month;
    date.day = todayDate.day;
}

void setLocal(int region) {
    if (region < 0) {
        region = 2;
    } else if (region > 2) {
        region = 0;
    }
    local.region = region;
}

int isLeapYear(int year) {
    if(year > 1582) {
        if (year % 400 == 0 || year % 100 != 0 && year % 4 == 0) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (year % 4 == 0) {
            return 1;
        } else {
            return 0;
        }
    }
}

int getNumberOfDaysInMonth(int year, int month) {
    switch (month) {
        case 1:
            return 31;
        case 2:
            if(isLeapYear(year)) {
                return 29;
            } else {
                return 28;
            }
        case 3:
            return 31;
        case 4:
            return 30;
        case 5:
            return 31;
        case 6:
            return 30;
        case 7:
            return 31;
        case 8:
            return 31;
        case 9:
            return 30;
        case 10:
            return 31;
        case 11:
            return 30;
        case 12:
            return 31;
        default:
            return -1;
    }
}

const char* getDayName(int day) {
    switch (day) {
        case 0:
            return "Sunday";
        case 1:
            return "Monday";
        case 2:
            return "Tuesday";
        case 3:
            return "Wednesday";
        case 4:
            return "Thursday";
        case 5:
            return "Friday";
        case 6:
            return "Saturday";
        default:
            return("Error in getName(). Invalid argument was passed!");
    }
}

int getDayNumber(int year, int month, int day) {
    if (month < 1 || month > 12) {
        return -1;
    }
    if (day < 1 || day > getNumberOfDaysInMonth(year, month)) {
        return -1;
    }
    int f, k = day, m = month - 2, d = year % 100, c = year / 100;
    if (m < 1) {
        m += 12;
        d--;
        if(d < 0) {
            c--;
            d = 99;
        }
    }
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day > 4)))) {
        f = (k + ((13 * m - 1) / 5) + d + (d / 4) + (c / 4) - (2 * c)) % 7;
    } else {
        f = (k + ((13 * m - 1) / 5) + d + (d / 4) + 5 - c) % 7;
    }
    if (f < 0) {
        f += 7;
    }
    return f;
}

const char* getMonthName(int month) {
    switch (month) {
        case 1:
            return "January";
        case 2:
            return "February";
        case 3:
            return "March";
        case 4:
            return "April";
        case 5:
            return "May";
        case 6:
            return "June";
        case 7:
            return "July";
        case 8:
            return "August";
        case 9:
            return "September";
        case 10:
            return "October";
        case 11:
            return "November";
        case 12:
            return "December";
        default:
            return("Error in getMonthName(). Invalid argument was passed!");
    }
}

void previousYear() {
    date.year--;
}

void nextYear() {
    date.year++;
}

void previousMonth() {
    date.month--;
    if (date.month < 1) {
        date.year--;
        date.month += 12;
    }
}

void nextMonth() {
    date.month++;
    if (date.month > 12) {
        date.year++;
        date.month -= 12;
    }
}

void renderDayOfWeek(SDL_Surface* textSurface, SDL_Color color) {
    SDL_Rect recDayOfWeek;
    recDayOfWeek.x = 40;
    recDayOfWeek.y = 50;
    recDayOfWeek.w = (SCREEN_WIDTH - 80) / 7;
    recDayOfWeek.h = (SCREEN_HEIGHT - 80) / 14;

    SDL_Rect recDayOfWeekText;
    recDayOfWeekText.x = 40;
    recDayOfWeekText.y = 55;
    recDayOfWeekText.w = 0;
    recDayOfWeekText.h = 0;

    int dayOfWeek, textW, textH;
    for (dayOfWeek = 0; dayOfWeek < 7; dayOfWeek++) {
        if ((dayOfWeek == 0 && local.region != 2) || (dayOfWeek == 6 && local.region == 2)) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
			SDL_Color color = {0xFF, 0x00, 0x00};
            SDL_RenderDrawRect(renderer, &recDayOfWeek);
            if (local.region != 2) {
                textSurface = TTF_RenderUTF8_Solid(fontParagraph, getDayName(dayOfWeek), color);
                TTF_SizeUTF8(fontParagraph, getDayName(dayOfWeek), &textW, &textH);
            } else {
                textSurface = TTF_RenderUTF8_Solid(fontParagraph, getDayName(dayOfWeek - 6), color);
                TTF_SizeUTF8(fontParagraph, getDayName(dayOfWeek - 6), &textW, &textH);
            }
            recDayOfWeekText.x += ((SCREEN_WIDTH - 80) / 14) - (textW / 2);
            recDayOfWeekText.w = textW;
            recDayOfWeekText.h = textH;
            
		    SDL_Texture* dayName = SDL_CreateTextureFromSurface(renderer, textSurface);
		    SDL_RenderCopy(renderer, dayName, NULL, &recDayOfWeekText);
		    SDL_DestroyTexture(dayName);
		    SDL_FreeSurface(textSurface);
		    textSurface = NULL;

            recDayOfWeekText.x -= ((SCREEN_WIDTH - 80) / 14) - (textW / 2);
        } else if ((dayOfWeek == 6 && local.region != 2) || (dayOfWeek == 5 && local.region == 2)) {
			SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF);
			SDL_Color color = {0x00, 0xFF, 0xFF};
            SDL_RenderDrawRect(renderer, &recDayOfWeek);
            if (local.region != 2) {
                textSurface = TTF_RenderUTF8_Solid(fontParagraph, getDayName(dayOfWeek), color);
                TTF_SizeUTF8(fontParagraph, getDayName(dayOfWeek), &textW, &textH);
            } else {
                textSurface = TTF_RenderUTF8_Solid(fontParagraph, getDayName(dayOfWeek + 1), color);
                TTF_SizeUTF8(fontParagraph, getDayName(dayOfWeek + 1), &textW, &textH);
	            
            }
            recDayOfWeekText.x += ((SCREEN_WIDTH - 80) / 14) - (textW / 2);
            recDayOfWeekText.w = textW;
            recDayOfWeekText.h = textH;

		    SDL_Texture* dayName = SDL_CreateTextureFromSurface(renderer, textSurface);
		    SDL_RenderCopy(renderer, dayName, NULL, &recDayOfWeekText);
		    SDL_DestroyTexture(dayName);
		    SDL_FreeSurface(textSurface);
		    textSurface = NULL;
            recDayOfWeekText.x -= ((SCREEN_WIDTH - 80) / 14) - (textW / 2);
        } else {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_Color color = {0xFF, 0xFF, 0xFF};
            SDL_RenderDrawRect(renderer, &recDayOfWeek);
            if (local.region != 2) {
                textSurface = TTF_RenderUTF8_Solid(fontParagraph, getDayName(dayOfWeek), color);
                TTF_SizeUTF8(fontParagraph, getDayName(dayOfWeek), &textW, &textH);
            } else {
                textSurface = TTF_RenderUTF8_Solid(fontParagraph, getDayName(dayOfWeek + 1), color);
                TTF_SizeUTF8(fontParagraph, getDayName(dayOfWeek + 1), &textW, &textH);
            }
            recDayOfWeekText.x += ((SCREEN_WIDTH - 80) / 14) - (textW / 2);
            recDayOfWeekText.w = textW;
            recDayOfWeekText.h = textH;

		    SDL_Texture* dayName = SDL_CreateTextureFromSurface(renderer, textSurface);
		    SDL_RenderCopy(renderer, dayName, NULL, &recDayOfWeekText);
		    SDL_DestroyTexture(dayName);
		    SDL_FreeSurface(textSurface);
		    textSurface = NULL;
            recDayOfWeekText.x -= ((SCREEN_WIDTH - 80) / 14) - (textW / 2);
        }
        recDayOfWeek.x += (SCREEN_WIDTH - 80) / 7;
        recDayOfWeekText.x += (SCREEN_WIDTH - 80) / 7;
    }
}

void renderYear(SDL_Surface* textSurface, SDL_Color color) {
    char yearStr[5]; /* The reason yearStr is 5 elements long is due to glitches if 4 was the maximum. */
    itoa(date.year, yearStr, 10);

    textSurface = TTF_RenderUTF8_Solid(fontHeader, yearStr, color);
	SDL_Texture* yearName = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textW, textH;
	TTF_SizeUTF8(fontHeader, yearStr, &textW, &textH);
	SDL_Rect recYear = {((SCREEN_WIDTH - 80) * 2 / 3) + 40 - (textW / 2), 20, textW, textH};

	SDL_RenderCopy(renderer, yearName, NULL, &recYear);
	SDL_DestroyTexture(yearName);
	SDL_FreeSurface(textSurface);
	textSurface = NULL;
}

void renderMonth(SDL_Surface* textSurface, SDL_Color color) {
    textSurface = TTF_RenderUTF8_Solid(fontHeader, getMonthName(date.month), color);
	SDL_Texture* monthName = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textW, textH;
	TTF_SizeUTF8(fontHeader, getMonthName(date.month), &textW, &textH);
	SDL_Rect recMonth = {((SCREEN_WIDTH - 80) / 3) + 40 - (textW / 2), 20, textW, textH};

	SDL_RenderCopy(renderer, monthName, NULL, &recMonth);
	SDL_DestroyTexture(monthName);
	SDL_FreeSurface(textSurface);
	textSurface = NULL;
}

void renderDate() {
    SDL_Rect recDayOfMonth;
    recDayOfMonth.x = 40;
    recDayOfMonth.y = 80;
    recDayOfMonth.w = (SCREEN_WIDTH - 80) / 7;
    recDayOfMonth.h = (SCREEN_HEIGHT - 80) / 8;

    SDL_Rect recDayOfMonthText;
    recDayOfMonthText.x = 40 + ((SCREEN_WIDTH - 80) / 7) / 2;
    recDayOfMonthText.y = 85;
    recDayOfMonthText.w = 0;
    recDayOfMonthText.h = 0;

    SDL_Surface* textSurface;
	SDL_Color color = {0xFF, 0xFF, 0xFF};
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	int daysInMonth = getNumberOfDaysInMonth(date.year, date.month);
	int day, textW, textH;
    char dayStr[2];
    int dayOfWeek = getDayNumber(date.year, date.month, 1);

    if (local.region != 2) {
        recDayOfMonth.x += ((SCREEN_WIDTH - 80) / 7) * dayOfWeek;
        recDayOfMonthText.x += ((SCREEN_WIDTH - 80) / 7) * dayOfWeek;
    } else {
        if (dayOfWeek != 0) {
            recDayOfMonth.x += ((SCREEN_WIDTH - 80) / 7) * (dayOfWeek - 1);
            recDayOfMonthText.x += ((SCREEN_WIDTH - 80) / 7) * (dayOfWeek - 1);
        } else {
            recDayOfMonth.x += ((SCREEN_WIDTH - 80) / 7) * (dayOfWeek + 6);
            recDayOfMonthText.x += ((SCREEN_WIDTH - 80) / 7) * (dayOfWeek + 6);
        }
    }
	for (day = 0; day < daysInMonth; day++) {
        if (date.year == 1582 && date.month == 10 && day > 3 && day < 14) {
            continue;
        }
		itoa(day + 1, dayStr, 10);
		if (date.year == todayDate.year && date.month == todayDate.month && day == todayDate.day - 1) {
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
			SDL_Color color = {0x00, 0xFF, 0x00};
			SDL_RenderDrawRect(renderer, &recDayOfMonth);

			textSurface = TTF_RenderText_Solid(fontTitle, dayStr, color);
            TTF_SizeUTF8(fontTitle, dayStr, &textW, &textH);
            recDayOfMonthText.x -= (textW / 2);
            recDayOfMonthText.w = textW;
            recDayOfMonthText.h = textH;

			SDL_Texture* dayName = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_RenderCopy(renderer, dayName, NULL, &recDayOfMonthText);
			SDL_DestroyTexture(dayName);
			SDL_FreeSurface(textSurface);
			textSurface = NULL;
            recDayOfMonthText.x += (textW / 2);
        } else if (dayOfWeek == 0) {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
			SDL_Color color = {0xFF, 0x00, 0x00};
			SDL_RenderDrawRect(renderer, &recDayOfMonth);

			textSurface = TTF_RenderText_Solid(fontTitle, dayStr, color);
            TTF_SizeUTF8(fontTitle, dayStr, &textW, &textH);
            recDayOfMonthText.x -= (textW / 2);
            recDayOfMonthText.w = textW;
            recDayOfMonthText.h = textH;

			SDL_Texture* dayName = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_RenderCopy(renderer, dayName, NULL, &recDayOfMonthText);
			SDL_DestroyTexture(dayName);
			SDL_FreeSurface(textSurface);
			textSurface = NULL;
            recDayOfMonthText.x += (textW / 2);
		} else if (dayOfWeek == 6) {
			SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF);
			SDL_Color color = {0x00, 0xFF, 0xFF};
			SDL_RenderDrawRect(renderer, &recDayOfMonth);

			textSurface = TTF_RenderText_Solid(fontTitle, dayStr, color);
            TTF_SizeUTF8(fontTitle, dayStr, &textW, &textH);
            recDayOfMonthText.x -= (textW / 2);
            recDayOfMonthText.w = textW;
            recDayOfMonthText.h = textH;

			SDL_Texture* dayName = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_RenderCopy(renderer, dayName, NULL, &recDayOfMonthText);
			SDL_DestroyTexture(dayName);
			SDL_FreeSurface(textSurface);
			textSurface = NULL;
            recDayOfMonthText.x += (textW / 2);
		} else {
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_Color color = {0xFF, 0xFF, 0xFF};
			SDL_RenderDrawRect(renderer, &recDayOfMonth);

			textSurface = TTF_RenderText_Solid(fontTitle, dayStr, color);
            TTF_SizeUTF8(fontTitle, dayStr, &textW, &textH);
            recDayOfMonthText.x -= (textW / 2);
            recDayOfMonthText.w = textW;
            recDayOfMonthText.h = textH;

			SDL_Texture* dayName = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_RenderCopy(renderer, dayName, NULL, &recDayOfMonthText);
			SDL_DestroyTexture(dayName);
			SDL_FreeSurface(textSurface);
            recDayOfMonthText.x += (textW / 2);
		}
		recDayOfMonth.x += (SCREEN_WIDTH - 80) / 7;
        recDayOfMonthText.x += (SCREEN_WIDTH - 80) / 7;
        if (local.region != 2) {
            if (dayOfWeek == 6) {
			    dayOfWeek = 0;
			    recDayOfMonth.x = 40;
			    recDayOfMonth.y += (SCREEN_HEIGHT - 80) / 8;
                recDayOfMonthText.x = 40 + ((SCREEN_WIDTH - 80) / 7) / 2;
                recDayOfMonthText.y += (SCREEN_HEIGHT - 80) / 8;
		    } else {
			    dayOfWeek++;
		    }
        } else {
            if (dayOfWeek == 6) {
			    dayOfWeek = 0;
            } else if (dayOfWeek == 0) {
			    recDayOfMonth.x = 40;
			    recDayOfMonth.y += (SCREEN_HEIGHT - 80) / 8;
                recDayOfMonthText.x = 40 + ((SCREEN_WIDTH - 80) / 7) / 2;
                recDayOfMonthText.y += (SCREEN_HEIGHT - 80) / 8;
                dayOfWeek++;
		    } else {
			    dayOfWeek++;
		    }
        }
	}
}

void renderTime(SDL_Surface* textSurface, SDL_Color color) {
    /*
        International = 0, North America = 1, Europe = 2
    */
    int locale = local.region;

    char date[25] = "";
    char time[25] = "";
    char localeText[25] = "Locale is set to: ";

    char name[4];
    if (local.region == 1) {
        strcat(name, "USA");
    } else if (local.region == 2) {
        strcat(name, "EUR");
    } else {
        strcat(name, "INT");
    }
    strcat(localeText, name);

    char yearStr[5];
    char monthStr[2];
    char dayStr[2];
    char hourStr[3];
    char minStr[2];
    char secStr[2];

    if (locale == 1) {
        itoa(todayDate.month, monthStr, 10);
        strcat(date, monthStr);
        strcat(date, "/");

        itoa(todayDate.day, dayStr, 10);
        strcat(date, dayStr);
        strcat(date, "/");

        itoa(todayDate.year, yearStr, 10);
        strcat(date, yearStr);
    } else if (locale == 2) {
        if (todayDate.day < 10) {
            strcat(date, "0");
        }
        itoa(todayDate.day, dayStr, 10);
        strcat(date, dayStr);
        strcat(date, "/");

        if (todayDate.month < 10) {
            strcat(date, "0");
        }
        itoa(todayDate.month, monthStr, 10);
        strcat(date, monthStr);
        strcat(date, "/");

        itoa(todayDate.year, yearStr, 10);
        strcat(date, yearStr);
    } else {
        itoa(todayDate.year, yearStr, 10);
        strcat(date, yearStr);
        strcat(date, "-");

        if (todayDate.month < 10) {
            strcat(date, "0");
        }
        itoa(todayDate.month, monthStr, 10);
        strcat(date, monthStr);
        strcat(date, "-");

        if (todayDate.day < 10) {
            strcat(date, "0");
        }
        itoa(todayDate.day, dayStr, 10);
        strcat(date, dayStr);
    }
    
    textSurface = TTF_RenderUTF8_Solid(fontHeader, date, color);
	SDL_Texture* displayDate = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textW, textH;
	TTF_SizeUTF8(fontHeader, date, &textW, &textH);
	SDL_Rect recDate = {100 - (textW / 2), (SCREEN_HEIGHT - 80), textW, textH};

	SDL_RenderCopy(renderer, displayDate, NULL, &recDate);
	SDL_DestroyTexture(displayDate);
	SDL_FreeSurface(textSurface);
	textSurface = NULL;

    if (locale == 1) {
        int twelveHour = todayDate.hour;
        if (twelveHour > 12) {
            twelveHour -= 12;
        } else if (twelveHour == 0) {
            twelveHour += 12;
        }
        itoa(twelveHour, hourStr, 10);
    } else {
        if (todayDate.hour < 10) {
            strcat(time, "0");
        }
        itoa(todayDate.hour, hourStr, 10);
    }
    strcat(time, hourStr);
    
    if (SDL_GetTicks() % 1000 < 500) {
        strcat(time, ":");
    } else {
        strcat(time, " ");
    }

    if (todayDate.minute < 10) {
        strcat(time, "0");
    }
    itoa(todayDate.minute, minStr, 10);
    strcat(time, minStr);
    if (SDL_GetTicks() % 1000 < 500) {
        strcat(time, ":");
    } else {
        strcat(time, " ");
    }

    if (todayDate.second < 10) {
        strcat(time, "0");
    }
    itoa(todayDate.second, secStr, 10);
    strcat(time, secStr);
    if (locale == 1) {
        if (todayDate.hour < 12) {
            strcat(time, " AM");
        } else {
            strcat(time, " PM");
        }
    }

    textSurface = TTF_RenderUTF8_Solid(fontHeader, time, color);
	SDL_Texture* displayTime = SDL_CreateTextureFromSurface(renderer, textSurface);

	TTF_SizeUTF8(fontHeader, time, &textW, &textH);
	SDL_Rect recTime = {(SCREEN_WIDTH - 80) / 4 + 100 - (textW / 2), (SCREEN_HEIGHT - 80), textW, textH};

	SDL_RenderCopy(renderer, displayTime, NULL, &recTime);
	SDL_DestroyTexture(displayTime);
	SDL_FreeSurface(textSurface);
	textSurface = NULL;

    textSurface = TTF_RenderUTF8_Solid(fontHeader, localeText, color);
	SDL_Texture* displayLocale = SDL_CreateTextureFromSurface(renderer, textSurface);

	TTF_SizeUTF8(fontHeader, localeText, &textW, &textH);
	SDL_Rect recLocale = {(SCREEN_WIDTH - 80) / 2 + 40, (SCREEN_HEIGHT - 80), textW, textH};

	SDL_RenderCopy(renderer, displayLocale, NULL, &recLocale);
	SDL_DestroyTexture(displayLocale);
	SDL_FreeSurface(textSurface);
	textSurface = NULL;
}
