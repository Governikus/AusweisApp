/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QtTest>


// Versions of the Q*() test assertion macros that return an actual value.


#define XVERIFY(statement, returnValue)\
	do {\
		if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__)){\
			return returnValue;}\
	}\
	while (0)


#define XVERIFY2(statement, description, returnValue)\
	do {\
		if (statement){\
			if (!QTest::qVerify(true, #statement, (description), __FILE__, __LINE__)){\
				return returnValue;}\
		}\
		else {\
			if (!QTest::qVerify(false, #statement, (description), __FILE__, __LINE__)){\
				return returnValue;}\
		}\
	}\
	while (0)


#define XCOMPARE(actual, expected, returnValue)\
	do {\
		if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__)){\
			return returnValue;}\
	}\
	while (0)


#define XSKIP(statement, returnValue)\
	do {\
		QTest::qSkip(statement, __FILE__, __LINE__);\
		return returnValue;\
	}\
	while (0)
