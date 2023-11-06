/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UserAgent
 */

#include "paos/element/UserAgent.h"

#include <QCoreApplication>
#include <QtTest>

using namespace governikus;

class test_UserAgent
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getter()
		{
			QCoreApplication::setApplicationVersion("1.32.4");

			UserAgent obj;
			QCOMPARE(obj.getName(), QString("AusweisApp2"));
			QCOMPARE(obj.getVersionMajor(), QString("1"));
			QCOMPARE(obj.getVersionMinor(), QString("32"));
			QCOMPARE(obj.getVersionSubminor(), QString("4"));
		}


};

QTEST_GUILESS_MAIN(test_UserAgent)
#include "test_UserAgent.moc"
