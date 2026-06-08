#include "StdAfx.h"
#include "date.h"
#include <gtest/gtest.h>

TEST(DateTest, ComposeAndDecompose)
{
    int date = COMPOSE_DATE(2024, 3, 15);
    EXPECT_EQ(20240315, date);
    EXPECT_EQ(2024, YEAR_OF(date));
    EXPECT_EQ(3,    MONTH_OF(date));
    EXPECT_EQ(15,   DAY_OF(date));
}

TEST(DateTest, YearDays_LeapYearHas366)
{
    EXPECT_EQ(366, yeardays(2024)); // leap
    EXPECT_EQ(365, yeardays(2023));
    EXPECT_EQ(366, yeardays(2000)); // century divisible by 400
    EXPECT_EQ(365, yeardays(1900)); // century not divisible by 400
}

TEST(DateTest, MonthDays_FebInLeapAndNonLeap)
{
    EXPECT_EQ(29, monthdays(2024, 2));
    EXPECT_EQ(28, monthdays(2023, 2));
    EXPECT_EQ(31, monthdays(2024, 1));
    EXPECT_EQ(30, monthdays(2024, 4));
}

TEST(DateTest, Date2DaysRoundTrip)
{
    int original = COMPOSE_DATE(2024, 6, 8);
    int serial = date2days(original);
    EXPECT_EQ(original, days2date(serial));
}

TEST(DateTest, Weekday_KnownDates)
{
    // 2024-01-01 is Monday
    EXPECT_EQ(MONDAY, weekday(COMPOSE_DATE(2024, 1, 1)));
    // 2024-06-08 is Saturday
    EXPECT_EQ(SATURDAY, weekday(COMPOSE_DATE(2024, 6, 8)));
}

TEST(DateTest, EndOfMonth)
{
    EXPECT_EQ(29, eomday(2024, 2));
    EXPECT_EQ(28, eomday(2023, 2));
    EXPECT_EQ(20240229, eomdate(2024, 2));
}
