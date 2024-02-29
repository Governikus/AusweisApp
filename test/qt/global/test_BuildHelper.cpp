/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref BuildHelper
 */

#include "BuildHelper.h"

#include <QStringList>
#include <QtTest>

using namespace governikus;

class test_BuildHelper
	: public QObject
{
	Q_OBJECT

	private:
#ifdef Q_OS_ANDROID
		const int mExpectedSize = 11;
#else
		const int mExpectedSize = 10;
#endif

	private Q_SLOTS:
		void processInformationHeader()
		{
			QStringList list;
			BuildHelper::processInformationHeader([&list](const QString& pKey, const QString&){
					list << pKey;
				});
			QCOMPARE(list.size(), mExpectedSize);
		}


		void getInformationHeader()
		{
			const auto& info = BuildHelper::getInformationHeader();

			QCOMPARE(info.size(), mExpectedSize);
			QCOMPARE(info.first().first, QLatin1String("Application"));
			QCOMPARE(info.last().first, QLatin1String("OpenSSL Version"));
		}


};

QTEST_GUILESS_MAIN(test_BuildHelper)
#include "test_BuildHelper.moc"
