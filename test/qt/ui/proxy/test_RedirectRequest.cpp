/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RedirectRequest.h"

#include "MockSocket.h"
#include "PortFile.h"
#include "ResourceLoader.h"

#include <QtTest>

using namespace governikus;

class test_RedirectRequest
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void getterSetter()
		{
			const auto httpRequest = QSharedPointer<HttpRequest>::create(new MockSocket());

			QTest::ignoreMessage(QtWarningMsg, "No port found");
			RedirectRequest request(httpRequest);
			QVERIFY(!request.isAnswerReceived());

			request.answerReceived();
			QVERIFY(request.isAnswerReceived());
		}


		void getPortFile()
		{
#ifdef Q_OS_WIN
			QSKIP("Windows does not use PortFile");
#endif

			PortFile portFile;
			portFile.handlePort(123);
			QVERIFY(portFile.getFile().exists());

			const auto httpRequest = QSharedPointer<HttpRequest>::create(new MockSocket());

			QTest::ignoreMessage(QtDebugMsg, "Redirect to port: 123");
			RedirectRequest request(httpRequest);
		}


};

QTEST_GUILESS_MAIN(test_RedirectRequest)
#include "test_RedirectRequest.moc"
