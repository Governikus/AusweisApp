/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UIPlugInScheme
 */

#include "UIPlugInScheme.h"

#include "WorkflowRequest.h"
#include "context/AuthContext.h"

#include <QDesktopServices>
#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<WorkflowRequest>)

class test_UIPlugInScheme
	: public QObject
{
	Q_OBJECT

	private:
		QUrl mReceivedUrl;

	private Q_SLOTS:
		void authentication_data()
		{
			QTest::addColumn<QUrl>("url");

			QTest::newRow(" 1") << QUrl("eid://127.0.0.1:24727/eID-Client?tctokenURL=bla");
			QTest::newRow(" 2") << QUrl("eid://localhost:24727/eID-Client?tctokenURL=bla");
			QTest::newRow(" 3") << QUrl("eid://www.ausweisapp.bund.de/eID-Client?tctokenURL=bla");
			QTest::newRow(" 4") << QUrl("eid://www.ausweisapp.bund.de:24727/eID-Client?tctokenURL=bla");
			QTest::newRow(" 5") << QUrl("http://127.0.0.1:24727/eID-Client?tctokenURL=bla");
			QTest::newRow(" 6") << QUrl("http://localhost:24727/eID-Client?tctokenURL=bla");
			QTest::newRow(" 7") << QUrl("http://www.ausweisapp.bund.de/eID-Client?tctokenURL=bla");
			QTest::newRow(" 8") << QUrl("http://www.ausweisapp.bund.de:24727/eID-Client?tctokenURL=bla");
			QTest::newRow(" 9") << QUrl("https://127.0.0.1:24727/eID-Client?tctokenURL=bla");
			QTest::newRow("10") << QUrl("https://localhost:24727/eID-Client?tctokenURL=bla");
			QTest::newRow("11") << QUrl("https://www.ausweisapp.bund.de/eID-Client?tctokenURL=bla");
			QTest::newRow("12") << QUrl("https://www.ausweisapp.bund.de:24727/eID-Client?tctokenURL=bla");
		}


		void authentication()
		{
			QFETCH(QUrl, url);

			UIPlugInScheme ui;
			QSignalSpy spy(&ui, &UIPlugIn::fireWorkflowRequested);

			QDesktopServices::openUrl(url);
			QCOMPARE(spy.count(), 1);

			const auto request = spy.at(0).at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getContext().objectCast<AuthContext>()->getActivationUrl(), url);
		}


		void showui_data()
		{
			QTest::addColumn<UiModule>("module");
			QTest::addColumn<QString>("urlTemplate");

			const auto& uiModules = Enum<UiModule>::getList();
			for (const auto& module : uiModules)
			{
				QTest::newRow(QString("%1 - eid - ip").arg(getEnumName(module)).toUtf8().constData()) << module << QString("eid://127.0.0.1:24727/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - eid - localhost").arg(getEnumName(module)).toUtf8().constData()) << module << QString("eid://localhost:24727/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - eid - url").arg(getEnumName(module)).toUtf8().constData()) << module << QString("eid://www.ausweisapp.bund.de/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - eid - url - port").arg(getEnumName(module)).toUtf8().constData()) << module << QString("eid://www.ausweisapp.bund.de:24727/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - http - ip").arg(getEnumName(module)).toUtf8().constData()) << module << QString("http://127.0.0.1:24727/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - http - localhost").arg(getEnumName(module)).toUtf8().constData()) << module << QString("http://localhost:24727/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - http - url").arg(getEnumName(module)).toUtf8().constData()) << module << QString("http://www.ausweisapp.bund.de/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - http - url - port").arg(getEnumName(module)).toUtf8().constData()) << module << QString("http://www.ausweisapp.bund.de:24727/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - https - ip").arg(getEnumName(module)).toUtf8().constData()) << module << QString("https://127.0.0.1:24727/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - https - localhost").arg(getEnumName(module)).toUtf8().constData()) << module << QString("https://localhost:24727/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - https - url").arg(getEnumName(module)).toUtf8().constData()) << module << QString("https://www.ausweisapp.bund.de/eID-Client?ShowUI=%1");
				QTest::newRow(QString("%1 - https - url - port").arg(getEnumName(module)).toUtf8().constData()) << module << QString("https://www.ausweisapp.bund.de:24727/eID-Client?ShowUI=%1");
			}
		}


		void showui()
		{
			QFETCH(UiModule, module);
			QFETCH(QString, urlTemplate);

			UIPlugInScheme ui;
			QSignalSpy spy(&ui, &UIPlugIn::fireShowUiRequested);

			const QLatin1String moduleName = Enum<UiModule>::getName(module);

			QDesktopServices::openUrl(QUrl(urlTemplate.arg(moduleName)));
			QCOMPARE(spy.count(), 1);
			QCOMPARE(spy.at(0).at(0).value<UiModule>(), module);

			QDesktopServices::openUrl(QUrl(urlTemplate.arg(QString(moduleName).toLower())));
			QCOMPARE(spy.count(), 2);
			QCOMPARE(spy.at(1).at(0).value<UiModule>(), module);
		}


		void external_data()
		{
			QTest::addColumn<QUrl>("url");

			QTest::newRow("1") << QUrl("https://127.0.0.1/eID-Client?tctokenURL=bla");
			QTest::newRow("2") << QUrl("https://localhost/eID-Client?tctokenURL=bla");
			QTest::newRow("3") << QUrl("https://localhost:24727/eID?tctokenURL=bla");
			QTest::newRow("4") << QUrl("https://www.ausweisapp.bund.de/de/aa2/a11y");
			QTest::newRow("5") << QUrl("https://www.ausweisapp.bund.de/en/aa2/a11y");
		}


		void external()
		{
			QFETCH(QUrl, url);

			UIPlugInScheme ui;
			QSignalSpy spy(&ui, &UIPlugIn::fireWorkflowRequested);

			QTest::ignoreMessage(QtDebugMsg, "Not our business. Using the appropriate mechanism for the user's desktop environment.");
			QDesktopServices::openUrl(url);
			QCOMPARE(spy.count(), 0);
		}


		void unknown_data()
		{
			QTest::addColumn<QUrl>("url");

			QTest::newRow(" 1") << QUrl("eid://127.0.0.1/eID-Client?tctokenURL=bla");
			QTest::newRow(" 2") << QUrl("eid://localhost/eID-Client?tctokenURL=bla");
			QTest::newRow(" 3") << QUrl("eid://localhost:24727/eID?tctokenURL=bla");
			QTest::newRow(" 4") << QUrl("eid://www.ausweisapp.bund.de/de/aa2/a11y");
			QTest::newRow(" 5") << QUrl("eid://www.ausweisapp.bund.de/en/aa2/a11y");
		}


		void unknown()
		{
			QFETCH(QUrl, url);

			UIPlugInScheme ui;
			QSignalSpy spy(&ui, &UIPlugIn::fireWorkflowRequested);

			QTest::ignoreMessage(QtWarningMsg, "Request type: unknown");
			QDesktopServices::openUrl(url);
			QCOMPARE(spy.count(), 0);
		}


		void unknownQuery()
		{
			UIPlugInScheme ui;
			QSignalSpy spyUi(&ui, &UIPlugIn::fireShowUiRequested);
			QSignalSpy spy(&ui, &UIPlugIn::fireWorkflowRequested);

			QTest::ignoreMessage(QtWarningMsg, "Unknown query type: QUrl(\"eid://127.0.0.1:24727/eID-Client?foo=bar\")");
			QDesktopServices::openUrl(QUrl("eid://127.0.0.1:24727/eID-Client?foo=bar"));
			QCOMPARE(spyUi.count(), 0);
			QCOMPARE(spy.count(), 0);
		}


		void onCustomUrl()
		{
			UIPlugInScheme ui;

			QUrl url;
			QSignalSpy spy(&ui, &UIPlugIn::fireWorkflowRequested);
			connect(&ui, &UIPlugIn::fireWorkflowRequested, this, [&url](const QSharedPointer<WorkflowRequest>& pRequest){
					url = pRequest->getContext().objectCast<AuthContext>()->getActivationUrl();
				});

			QTest::ignoreMessage(QtDebugMsg, "Got new request");
			QTest::ignoreMessage(QtDebugMsg, "Request URL: QUrl(\"http://crap\")");
			QTest::ignoreMessage(QtDebugMsg, "Not our business. Using the appropriate mechanism for the user's desktop environment.");
			ui.onCustomUrl(QUrl("http://crap"));
			QCOMPARE(spy.count(), 0);

			const QUrl testUrl("http://localhost:24727/eID-Client?tcTokenURL=http://bla");
			QTest::ignoreMessage(QtDebugMsg, "Got new request");
			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");
			ui.onCustomUrl(testUrl);
			QCOMPARE(spy.count(), 1);
			QCOMPARE(url, testUrl);

			QSignalSpy spyShowUi(&ui, &UIPlugIn::fireShowUiRequested);
			QTest::ignoreMessage(QtDebugMsg, "Got new request");
			QTest::ignoreMessage(QtDebugMsg, "Request type: showui");
			ui.onCustomUrl(QUrl("http://localhost:24727/eID-Client?showUi"));
			QCOMPARE(spy.count(), 1);
			QCOMPARE(spyShowUi.count(), 1);
		}


		void sendRedirectOnCustomUrl(const QUrl& pUrl)
		{
			mReceivedUrl = pUrl;
		}


		void sendRedirect()
		{
			UIPlugInScheme ui;

			QSharedPointer<WorkflowRequest> request;
			QSignalSpy spy(&ui, &UIPlugIn::fireWorkflowRequested);
			connect(&ui, &UIPlugIn::fireWorkflowRequested, this, [&request](const QSharedPointer<WorkflowRequest>& pRequest){
					request = pRequest;
				});

			QTest::ignoreMessage(QtDebugMsg, "Got new request");
			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");
			ui.onCustomUrl(QUrl("http://localhost:24727/eID-Client?tcTokenURL=https://bla"));
			QVERIFY(request);
			QVERIFY(request->getData().value<UIPlugInScheme::RedirectHandler>());

			auto authContext = request->getContext().objectCast<AuthContext>();
			QVERIFY(authContext);
			authContext->setRefreshUrl(QUrl("https://www.example.com"));
			authContext->setStatus(GlobalStatus::Code::No_Error);

			QDesktopServices::setUrlHandler(QStringLiteral("https"), this, "sendRedirectOnCustomUrl");
			QTest::ignoreMessage(QtDebugMsg, "Determined redirect URL QUrl(\"https://www.example.com?ResultMajor=ok\")");
			ui.onWorkflowFinished(request);

			QTRY_COMPARE(mReceivedUrl, QUrl("https://www.example.com?ResultMajor=ok")); // clazy:exclude=qstring-allocations
			QDesktopServices::unsetUrlHandler(QStringLiteral("https"));
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInScheme)
#include "test_UIPlugInScheme.moc"
