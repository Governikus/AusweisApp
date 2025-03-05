/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "controller/DiagnosisController.h"

#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "context/DiagnosisContext.h"

#include "MockReaderDetector.h"
#include "MockReaderManagerPlugin.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_IMPORT_PLUGIN(MockReaderManagerPlugin)

class test_DiagnosisController
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void cleanup()
		{
			MockReaderManagerPlugin::getInstance().removeAllReader();
		}


		void testOnReaderEvent()
		{
			const QList<UsbId> devIds({UsbId(0x0C4B, 0x9102), UsbId(0x076B, 0x5022)}); // cyberJack and OMNIKEY 5022
			MockReaderDetector readerDetector(devIds);
			Env::set(ReaderDetector::staticMetaObject, &readerDetector);

			auto context = QSharedPointer<DiagnosisContext>::create();
			DiagnosisController controller(context);

			MockReaderManagerPlugin::getInstance().addReader(u"REINER SCT cyberJack RFID basis"_s);
			QCOMPARE(context->getReaderInfos().size(), 1); // cyberJack detected
			QCOMPARE(context->getReaderInfosNoDriver().size(), 1); // OMNIKEY 5022 not detected

			MockReaderManagerPlugin::getInstance().addReader(u"Simulator"_s);
			QCOMPARE(context->getReaderInfos().size(), 2);
			QCOMPARE(context->getReaderInfosNoDriver().size(), 1); // OMNIKEY 5022 not detected

			MockReaderManagerPlugin::getInstance().addReader(u"HID Global OMNIKEY 5022 Smart Card Reader"_s);
			QCOMPARE(context->getReaderInfos().size(), 3);
			QCOMPARE(context->getReaderInfosNoDriver().size(), 0);
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisController)
#include "test_DiagnosisController.moc"
