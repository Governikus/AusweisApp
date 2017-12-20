/*!
 * \brief Unit tests for \ref FileDestination
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
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
		void getPathCharPtr()
		{
			QString path = FileDestination::getPath("qtlogging.ini");
			QVERIFY(path.endsWith("qtlogging.ini"));
			QVERIFY(QFile::exists(path));
		}


		void getPath()
		{
			QString path = FileDestination::getPath(QStringLiteral("qtlogging.ini"));
			QVERIFY(path.endsWith("qtlogging.ini"));
			QVERIFY(QFile::exists(path));
		}


};

QTEST_GUILESS_MAIN(test_FileDestination)
#include "test_FileDestination.moc"
