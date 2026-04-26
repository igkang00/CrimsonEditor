#include "stdafx.h"
#include <time.h>
#include "date.h"


#define EPOCH_DATE				19000101
#define DAYS_BEFORE_EPOCH		693961

#define MIN(a, b)				((a) < (b) ? (a) : (b))
#define MAX(a, b)				((a) > (b) ? (a) : (b))

#define SWAPI(a, b)				{ _tmpi = (a); (a) = (b); (b) = _tmpi; }
#define SWAPF(a, b)				{ _tmpf = (a); (a) = (b); (b) = _tmpf; }

#define IS_LEAP_YEAR(year)		((((year) % 4) == 0 && ((year) % 100) != 0) || ((year) % 400) == 0)
#define CHECK_EMR(emr, date)	(((emr) == 1 && (date) == eomdate(YEAR_OF(date), MONTH_OF(date))) ? (emr) : 0)
#define CHECK_FRE(fre)			(((fre) == 1 || (fre) == 2 || (fre) == 4 || (fre) == 6 || (fre) == 12) ? (fre) : 1)
#define PERIOD_OF(fre)			(12 / CHECK_FRE(fre))
#define CHECK_DCB(dcb)			(((dcb) == 0 || (dcb) == 1 || (dcb) == 2 || (dcb) == 3 || (dcb) == 4 || (dcb) == 5) ? (dcb) : 0)


static int _monthdays[] =		{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static int _tmpi;
static double _tmpf;


int today()
{
	time_t t1; struct tm * ptm;
	time( & t1 ); ptm = localtime( & t1 );
	return COMPOSE_DATE(ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
}


int yeardays(int year)
{
	if( ! IS_LEAP_YEAR(year) ) return 365;
	else return 366;
}


int monthdays(int year, int month)
{
	while( month < 1  ) { year--; month += 12; }
	while( month > 12 ) { year++; month -= 12; }
	if( ! IS_LEAP_YEAR(year) || month != 2 ) return _monthdays[month];
	else return _monthdays[month] + 1;
}


int date2days(int date)
{
	int year, month, day, i, j, days;
	DECOMPOSE_DATE( date, year, month, day );

	if( date >= EPOCH_DATE ) {
		days = DAYS_BEFORE_EPOCH;
		for( i = YEAR_OF(EPOCH_DATE); i < year; i++ ) days += yeardays(i);
	} else { /* BEFORE EPOCH */
		days = 0;
		for( i = 0; i < year;  i++ ) days += yeardays(i);
	}

	for( j = 1; j < month; j++ ) days += monthdays(year, j);
	days += day;

	return days;
}


int days2date(int days)
{
	int year, month, day, temp;

	if( days > DAYS_BEFORE_EPOCH ) {
		days -= DAYS_BEFORE_EPOCH;
		year = YEAR_OF(EPOCH_DATE); month = day = 1;
	} else {
		year = 0; month = day = 1;
	}

	while( days > (temp = yeardays(year)) ) { days -= temp; year++; }
	while( days > (temp = monthdays(year, month)) ) { days -= temp; month++; }
	day = days;

	return COMPOSE_DATE(year, month, day);
}


int eomday(int year, int month)
{
	return monthdays(year, month);
}


int eomdate(int year, int month)
{
	return COMPOSE_DATE(year, month, eomday(year, month));
}


int weekday(int date)
{
	return (date2days(date) + 5) % 7;
}


int isbizdate(int date, int hdc)
{
	int weekcode = weekday(date);
	if( hdc == HDC_SUNSAT && weekcode == SATURDAY ) return 0;
	else if( weekcode == SUNDAY ) return 0;
	return 1;
}


int nbizdate(int date, int hdc)
{
	int days = 1, weekcode = weekday(date);
	if( hdc == HDC_SUNSAT && weekcode == FRIDAY ) days = 3;
	else if( weekcode == SATURDAY ) days = 2;
	return days2date(date2days(date) + days);
}


int pbizdate(int date, int hdc)
{
	int days = 1, weekcode = weekday(date);
	if( hdc == HDC_SUNSAT && weekcode == MONDAY ) days = 3;
	else if( (hdc == HDC_SUNSAT && weekcode == SUNDAY) || weekcode == MONDAY ) days = 2;
	return days2date(date2days(date) - days);
}


int adddays(int date, int days)
{
	return days2date(date2days(date) + days);
}


int addmonths(int date, int months, int emr)
{
	int year, month, day, lastday;
	DECOMPOSE_DATE(date, year, month, day); month += months;
	while( month < 1  ) { year--; month += 12; }
	while( month > 12 ) { year++; month -= 12; }
	if( CHECK_EMR(emr, date) == EMR_ENDOFMONTH || day > (lastday = eomday(year, month)) ) day = lastday;
	return COMPOSE_DATE(year, month, day);
}


int addterms(int date, int terms, int fre, int emr)
{
	return addmonths(date, PERIOD_OF(fre) * terms, emr);
}


int days360(int date0, int date1, int eur)
{
	int year0, month0, day0, year1, month1, day1;
	int sign = (date0 < date1) ? 1 : -1;

	if( sign < 0 ) SWAPI(date0, date1);
	DECOMPOSE_DATE(date0, year0, month0, day0);
	DECOMPOSE_DATE(date1, year1, month1, day1);

	if( ! eur && day1 == 31 && day0 < 30 ) { month1++; day1 = 1; }
	if( day0 == 31 ) day0 = 30;
	if( day1 == 31 ) day1 = 30;

	return sign * (360 * (year1 - year0) + 30 * (month1 - month0) + day1 - day0);
}


int days365(int date0, int date1)
{
	int year0, month0, day0, year1, month1, day1;
	int j, monthdays0, monthdays1;

	DECOMPOSE_DATE(date0, year0, month0, day0);
	DECOMPOSE_DATE(date1, year1, month1, day1);

	if( month0 == 2 && day0 == 29 ) day0 = 28;
	if( month1 == 2 && day1 == 29 ) day1 = 28;

	monthdays0 = 0; for( j = 1; j < month0; j++ ) monthdays0 += _monthdays[j];
	monthdays1 = 0; for( j = 1; j < month1; j++ ) monthdays1 += _monthdays[j];

	return 365 * (year1 - year0) + monthdays1 - monthdays0 + day1 - day0;
}


int daysact(int date0, int date1)
{
	return date2days(date1) - date2days(date0);
}


int daysbet(int date0, int date1, int dcb)
{
	switch( CHECK_DCB(dcb) ) {
		case DCB_30360:
			return days360(date0, date1, 0);
		case DCB_30E360:
			return days360(date0, date1, 1);
		case DCB_NL365:
			return days365(date0, date1);
		default: /* DCB_ACTACT, DCB_ACT360, DCB_ACT365 */
			return daysact(date0, date1);
	}
}


int monthsbet(int date0, int date1)
{
	int year0, month0, day0, year1, month1, day1;
	int sign = (date0 < date1) ? 1 : -1;

	if( sign < 0 ) SWAPI(date0, date1);
	DECOMPOSE_DATE(date0, year0, month0, day0);
	DECOMPOSE_DATE(date1, year1, month1, day1);

	if( day0 > day1 ) month0++;
	return sign * (12 * (year1 - year0) + month1 - month0);
}


int termsbet(int date0, int date1, int fre)
{
	return monthsbet(date0, date1) / PERIOD_OF(fre);
}


double termfrac(int date0, int date1, int fre, int dcb, int emr)
{
	int year0, month0, day0, year1, month1, day1;
	int terms, days; double days_in_term;

	DECOMPOSE_DATE(date0, year0, month0, day0);
	DECOMPOSE_DATE(date1, year1, month1, day1);

	terms = termsbet(date0, date1, fre);
	days  = daysbet(addterms(date0, terms, fre, emr), date1, dcb);

	switch( CHECK_DCB(dcb) ) {
		case DCB_ACT360:	case DCB_30360:		case DCB_30E360:
			days_in_term = 360.0 / (double)CHECK_FRE(fre); break;
		case DCB_ACT365:	case DCB_NL365:
			days_in_term = 365.0 / (double)CHECK_FRE(fre); break;
		default: /* DCB_ACTACT */
			days_in_term = (double)daysact( addterms(date1, -1, fre, EMR_ACTUAL), date1 ); break;
	}

	return (double)terms + (double)days / days_in_term;
}


double yearfrac(int date0, int date1, int dcb, int emr)
{
	return termfrac(date0, date1, FRE_ANNUAL, dcb, emr);
}


