/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include <QtTest>

#include "SmartReaderManagerPlugIn.h"


using namespace governikus;


class test_SmartReaderManagerPlugIn
	: public QObject
{
	Q_OBJECT

	private:
		SmartReaderManagerPlugIn mPlugin;

	private Q_SLOTS:
		void initTestCase()
		{
			mPlugin.init();
		}


		void getInfo()
		{
			const auto& info = mPlugin.getInfo();
			QCOMPARE(info.isAvailable(), false);
		}


		void getReader()
		{
			QCOMPARE(mPlugin.getReaders().size(), 0);
		}


};

QTEST_GUILESS_MAIN(test_SmartReaderManagerPlugIn)
#include "test_SmartReaderManagerPlugIn.moc"
