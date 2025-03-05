/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginScheme.h"

#include "WorkflowRequest.h"
#include "context/AuthContext.h"

#include <QDesktopServices>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<WorkflowRequest>)

class test_UiPluginScheme
	: public QObject
{
	Q_OBJECT

	private:
		QUrl mReceivedUrl;

	private Q_SLOTS:
		void authentication_data()
		{
			QTest::addColumn<QUrl>("url");

			QTest::newRow(" 1") << QUrl("eid://127.0.0.1:24727/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 2") << QUrl("eid://localhost:24727/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 3") << QUrl("eid://www.ausweisapp.bund.de/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 4") << QUrl("eid://www.ausweisapp.bund.de:24727/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 5") << QUrl("http://127.0.0.1:24727/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 6") << QUrl("http://localhost:24727/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 7") << QUrl("http://www.ausweisapp.bund.de/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 8") << QUrl("http://www.ausweisapp.bund.de:24727/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 9") << QUrl("https://127.0.0.1:24727/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow("10") << QUrl("https://localhost:24727/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow("11") << QUrl("https://www.ausweisapp.bund.de/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow("12") << QUrl("https://www.ausweisapp.bund.de:24727/eID-Client?tctokenURL=bla"_L1);
		}


		void authentication()
		{
			QFETCH(QUrl, url);

			UiPluginScheme ui;
			QSignalSpy spy(&ui, &UiPlugin::fireWorkflowRequested);

			QDesktopServices::openUrl(url);
			QCOMPARE(spy.count(), 1);

			const auto request = spy.at(0).at(0).value<QSharedPointer<WorkflowRequest>>();
			QCOMPARE(request->getContext().objectCast<AuthContext>()->getActivationUrl(), url);
		}


		void showui_data()
		{
			QTest::addColumn<UiModule>("module");
			QTest::addColumn<QLatin1String>("urlTemplate");

			const auto& uiModules = Enum<UiModule>::getList();
			for (const auto& module : uiModules)
			{
				QTest::newRow(QStringLiteral("%1 - eid - ip").arg(getEnumName(module)).toUtf8().constData()) << module << "eid://127.0.0.1:24727/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - eid - localhost").arg(getEnumName(module)).toUtf8().constData()) << module << "eid://localhost:24727/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - eid - url").arg(getEnumName(module)).toUtf8().constData()) << module << "eid://www.ausweisapp.bund.de/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - eid - url - port").arg(getEnumName(module)).toUtf8().constData()) << module << "eid://www.ausweisapp.bund.de:24727/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - http - ip").arg(getEnumName(module)).toUtf8().constData()) << module << "http://127.0.0.1:24727/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - http - localhost").arg(getEnumName(module)).toUtf8().constData()) << module << "http://localhost:24727/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - http - url").arg(getEnumName(module)).toUtf8().constData()) << module << "http://www.ausweisapp.bund.de/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - http - url - port").arg(getEnumName(module)).toUtf8().constData()) << module << "http://www.ausweisapp.bund.de:24727/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - https - ip").arg(getEnumName(module)).toUtf8().constData()) << module << "https://127.0.0.1:24727/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - https - localhost").arg(getEnumName(module)).toUtf8().constData()) << module << "https://localhost:24727/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - https - url").arg(getEnumName(module)).toUtf8().constData()) << module << "https://www.ausweisapp.bund.de/eID-Client?ShowUI=%1"_L1;
				QTest::newRow(QStringLiteral("%1 - https - url - port").arg(getEnumName(module)).toUtf8().constData()) << module << "https://www.ausweisapp.bund.de:24727/eID-Client?ShowUI=%1"_L1;
			}
		}


		void showui()
		{
			QFETCH(UiModule, module);
			QFETCH(QLatin1String, urlTemplate);

			UiPluginScheme ui;
			QSignalSpy spy(&ui, &UiPlugin::fireShowUiRequested);

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

			QTest::newRow("1") << QUrl("https://127.0.0.1/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow("2") << QUrl("https://localhost/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow("3") << QUrl("https://localhost:24727/eID?tctokenURL=bla"_L1);
			QTest::newRow("4") << QUrl("https://www.ausweisapp.bund.de/de/aa2/a11y"_L1);
			QTest::newRow("5") << QUrl("https://www.ausweisapp.bund.de/en/aa2/a11y"_L1);
		}


		void external()
		{
			QFETCH(QUrl, url);

			UiPluginScheme ui;
			QSignalSpy spy(&ui, &UiPlugin::fireWorkflowRequested);

			QTest::ignoreMessage(QtDebugMsg, "Not our business. Using the appropriate mechanism for the user's desktop environment.");
			QDesktopServices::openUrl(url);
			QCOMPARE(spy.count(), 0);
		}


		void unknown_data()
		{
			QTest::addColumn<QUrl>("url");

			QTest::newRow(" 1") << QUrl("eid://127.0.0.1/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 2") << QUrl("eid://localhost/eID-Client?tctokenURL=bla"_L1);
			QTest::newRow(" 3") << QUrl("eid://localhost:24727/eID?tctokenURL=bla"_L1);
			QTest::newRow(" 4") << QUrl("eid://www.ausweisapp.bund.de/de/aa2/a11y"_L1);
			QTest::newRow(" 5") << QUrl("eid://www.ausweisapp.bund.de/en/aa2/a11y"_L1);
		}


		void unknown()
		{
			QFETCH(QUrl, url);

			UiPluginScheme ui;
			QSignalSpy spy(&ui, &UiPlugin::fireWorkflowRequested);

			QTest::ignoreMessage(QtWarningMsg, "Request type: unknown");
			QDesktopServices::openUrl(url);
			QCOMPARE(spy.count(), 0);
		}


		void unknownQuery()
		{
			UiPluginScheme ui;
			QSignalSpy spyUi(&ui, &UiPlugin::fireShowUiRequested);
			QSignalSpy spy(&ui, &UiPlugin::fireWorkflowRequested);

			QTest::ignoreMessage(QtWarningMsg, "Unknown query type: QUrl(\"eid://127.0.0.1:24727/eID-Client?foo=bar\")");
			QDesktopServices::openUrl(QUrl("eid://127.0.0.1:24727/eID-Client?foo=bar"_L1));
			QCOMPARE(spyUi.count(), 0);
			QCOMPARE(spy.count(), 0);
		}


		void onCustomUrl()
		{
			UiPluginScheme ui;

			QUrl url;
			QSignalSpy spy(&ui, &UiPlugin::fireWorkflowRequested);
			connect(&ui, &UiPlugin::fireWorkflowRequested, this, [&url](const QSharedPointer<WorkflowRequest>& pRequest){
						url = pRequest->getContext().objectCast<AuthContext>()->getActivationUrl();
					});

			QTest::ignoreMessage(QtDebugMsg, "Got new request");
			QTest::ignoreMessage(QtDebugMsg, "Request URL: QUrl(\"http://crap\")");
			QTest::ignoreMessage(QtDebugMsg, "Not our business. Using the appropriate mechanism for the user's desktop environment.");
			ui.onCustomUrl(QUrl("http://crap"_L1));
			QCOMPARE(spy.count(), 0);

			const QUrl testUrl("http://localhost:24727/eID-Client?tcTokenURL=http://bla"_L1);
			QTest::ignoreMessage(QtDebugMsg, "Got new request");
			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");
			ui.onCustomUrl(testUrl);
			QCOMPARE(spy.count(), 1);
			QCOMPARE(url, testUrl);

			QSignalSpy spyShowUi(&ui, &UiPlugin::fireShowUiRequested);
			QTest::ignoreMessage(QtDebugMsg, "Got new request");
			QTest::ignoreMessage(QtDebugMsg, "Request type: showui");
			ui.onCustomUrl(QUrl("http://localhost:24727/eID-Client?showUi"_L1));
			QCOMPARE(spy.count(), 1);
			QCOMPARE(spyShowUi.count(), 1);
		}


		void sendRedirectOnCustomUrl(const QUrl& pUrl)
		{
			mReceivedUrl = pUrl;
		}


		void sendRedirect()
		{
			UiPluginScheme ui;

			QSharedPointer<WorkflowRequest> request;
			QSignalSpy spy(&ui, &UiPlugin::fireWorkflowRequested);
			connect(&ui, &UiPlugin::fireWorkflowRequested, this, [&request](const QSharedPointer<WorkflowRequest>& pRequest){
						request = pRequest;
					});

			QTest::ignoreMessage(QtDebugMsg, "Got new request");
			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");
			ui.onCustomUrl(QUrl("http://localhost:24727/eID-Client?tcTokenURL=https://bla"_L1));
			QVERIFY(request);
			QCOMPARE(request->getData().metaType(), QMetaType::fromType<UiPluginScheme::Referrer>());

			auto authContext = request->getContext().objectCast<AuthContext>();
			QVERIFY(authContext);
			authContext->setRefreshUrl(QUrl("https://www.example.com?ResultMajor=ok"_L1));
			authContext->setStatus(GlobalStatus::Code::No_Error);

			QDesktopServices::setUrlHandler(QStringLiteral("https"), this, "sendRedirectOnCustomUrl");
			QTest::ignoreMessage(QtDebugMsg, "Perform redirect to URL QUrl(\"https://www.example.com?ResultMajor=ok\")");
			ui.onWorkflowFinished(request);

			QTRY_COMPARE(mReceivedUrl, QUrl("https://www.example.com?ResultMajor=ok"_L1)); // clazy:exclude=qstring-allocations
			QDesktopServices::unsetUrlHandler(QStringLiteral("https"));
		}


};

QTEST_GUILESS_MAIN(test_UiPluginScheme)
#include "test_UiPluginScheme.moc"
