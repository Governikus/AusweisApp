/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerInfo.h"

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugin.h"
#include "ReaderManager.h"
#include "VersionInfo.h"

#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugin)

using namespace governikus;

Q_DECLARE_METATYPE(ReaderInfo)

class test_MsgHandlerInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void versionInformation()
		{
			const QByteArray msg(R"({"cmd": "GET_INFO"})");
			MessageDispatcher dispatcher;
			auto versionInfo = VersionInfo::getInstance().toJson(QJsonDocument::Compact);

			const auto& result = dispatcher.processCommand(msg);
			QCOMPARE(result, MsgType::INFO);
			const QByteArray data = result;
			QVERIFY(data.contains(versionInfo));
			QVERIFY(data.contains(R"("VersionInfo":{)"));
			QVERIFY(data.contains(R"("msg":"INFO")"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerInfo)
#include "test_MsgHandlerInfo.moc"
