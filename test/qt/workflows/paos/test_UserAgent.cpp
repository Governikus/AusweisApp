/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UserAgent
 */

#include "paos/element/UserAgent.h"

#include <QCoreApplication>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_UserAgent
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getter()
		{
			QCoreApplication::setApplicationVersion("1.32.4"_L1);

			UserAgent obj;
			QCOMPARE(obj.getName(), "AusweisApp2"_L1);
			QCOMPARE(obj.getVersionMajor(), "1"_L1);
			QCOMPARE(obj.getVersionMinor(), "32"_L1);
			QCOMPARE(obj.getVersionSubminor(), "4"_L1);
		}


};

QTEST_GUILESS_MAIN(test_UserAgent)
#include "test_UserAgent.moc"
