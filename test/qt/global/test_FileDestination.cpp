/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref FileDestination
 */

#include "FileDestination.h"

#include <QFile>
#include <QtTest>

using namespace governikus;

class test_FileDestination
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getPath()
		{
			const auto filename = QStringLiteral("AusweisApp.rcc");
			QString path = FileDestination::getPath(filename);
			QVERIFY(path.endsWith(filename));
			QVERIFY(QFile::exists(path));
		}


};

QTEST_GUILESS_MAIN(test_FileDestination)
#include "test_FileDestination.moc"
