#ifndef __DATE_H_
#define __DATE_H_


/* CONSTANT DEFINITION */
#define FRE_ANNUAL					1
#define FRE_SEMIANNUAL				2
#define FRE_QUARTERLY				4
#define FRE_BIMONTHLY				6
#define FRE_MONTHLY					12

#define DCB_ACTACT					0x00
#define DCB_ACT360					0x01
#define DCB_ACT365					0x02
#define DCB_30360					0x03
#define DCB_30E360					0x04
#define DCB_NL365					0x05

#define EMR_ACTUAL					0x00
#define EMR_ENDOFMONTH				0x01

#define HDC_SUNDAY					0x00
#define HDC_SUNSAT					0x06

#define SUNDAY						0
#define MONDAY						1
#define TUESDAY						2
#define WEDSDAY						3
#define THURSDAY					4
#define FRIDAY						5
#define SATURDAY					6


/* MACRO FUNCTIONS */
#define YEAR_OF(date)				((date) / 10000)
#define MONTH_OF(date)				((date) % 10000 / 100)
#define DAY_OF(date)				((date) % 100)

#define COMPOSE_DATE(year, month, day)			(10000 * (year) + 100 * (month) + (day))
#define DECOMPOSE_DATE(date, year, month, day)	{ \
	year  = YEAR_OF(date); \
	month = MONTH_OF(date); \
	day   = DAY_OF(date); \
}


/* FUNCTION PROTOTYPES */
int today(); /* current date */

int yeardays(int year); /* number of days in year */
int monthdays(int year, int month); /* number of days in month */

int date2days(int date); /* date to serial date number conversion */
int days2date(int days); /* serial date number to date conversion */

int eomday(int year, int month); /* last day of month */
int eomdate(int year, int month); /* last date of month */
int weekday(int date); /* day of week */

int isbizdate(int date, int hdc); /* true for date that is business day */
int nbizdate(int date, int hdc); /* next business day */
int pbizdate(int date, int hdc); /* prev business day */

int adddays(int date, int days);
int addmonths(int date, int months, int emr);
int addterms(int date, int terms, int fre, int emr);

int days360(int date0, int date1, int eur); /* days between dates based on 360-day year */
int days365(int date0, int date1); /* days between dates based on 365-day year */
int daysact(int date0, int date1); /* actual number of days between dates */
int daysbet(int date0, int date1, int dcb); /* days between dates for any day-count basis */

int monthsbet(int date0, int date1); /* months between dates */
int termsbet(int date0, int date1, int fre); /* terms between dates */

double termfrac(int date0, int date1, int fre, int dcb, int emr); /* fraction of term between dates */
double yearfrac(int date0, int date1, int dcb, int emr); /* fraction of year between dates */


#endif /* __DATE_H_ */

