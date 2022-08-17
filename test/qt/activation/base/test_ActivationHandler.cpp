/*!
 * \brief Unit tests for \ref ActivationHandler
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ActivationHandler.h"

#include "AppSettings.h"
#include "MockSocket.h"
#include "ResourceLoader.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class TestActivationHandler
	: public ActivationHandler
{
	Q_OBJECT

	public:
		static void checkUrl(const QUrl& pUrl, ActivationType pType, const QString& pValue)
		{
			const auto queryUrl = QUrlQuery(pUrl);
			const auto [type, value] = getRequest(queryUrl);
			QCOMPARE(type, pType);
			QCOMPARE(value, pValue);
		}


		static void checkQueryParams(const QUrl& pUrl)
		{
			handleQueryParams(QUrlQuery(pUrl));
		}


};


class test_ActivationHandler
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getRequest_data()
		{
			QTest::addColumn<QUrl>("url");
			QTest::addColumn<ActivationType>("type");
			QTest::addColumn<QString>("value");

			QTest::newRow("empty") << QUrl("") << ActivationType::UNKNOWN << QString();
			QTest::newRow("no query") << QUrl("example.com") << ActivationType::UNKNOWN << QString();
			QTest::newRow("empty value") << QUrl("?showui=") << ActivationType::SHOWUI << QString();
			QTest::newRow("showui - l") << QUrl("?showui=test") << ActivationType::SHOWUI << QString("test");
			QTest::newRow("showui - h") << QUrl("?SHOWUI=test") << ActivationType::SHOWUI << QString("test");
			QTest::newRow("status - l") << QUrl("?status=test") << ActivationType::STATUS << QString("test");
			QTest::newRow("status - h") << QUrl("?STATUS=test") << ActivationType::STATUS << QString("test");
			QTest::newRow("tctokenurl - l") << QUrl("?tctokenurl=test") << ActivationType::TCTOKENURL << QString("test");
			QTest::newRow("tctokenurl - h") << QUrl("?TCTOKENURL=test") << ActivationType::TCTOKENURL << QString("test");
			QTest::newRow("multi 1") << QUrl("?showui=test1&status=test2") << ActivationType::SHOWUI << QString("test1");
			QTest::newRow("multi 2") << QUrl("?status=test1&showui=test2") << ActivationType::STATUS << QString("test1");
			QTest::newRow("multi 3") << QUrl("?foo=test1&showui=test2") << ActivationType::SHOWUI << QString("test2");
			QTest::newRow("multi 4") << QUrl("?foo=&showui=test2") << ActivationType::SHOWUI << QString("test2");
			QTest::newRow("multi 5") << QUrl("?foo=test1&showui=") << ActivationType::SHOWUI << QString();
			QTest::newRow("multi 6") << QUrl("?showui=&showui=test1") << ActivationType::SHOWUI << QString();
			QTest::newRow("multi 7") << QUrl("?foo=test1&bar=test2") << ActivationType::UNKNOWN << QString();
		}


		void getRequest()
		{
			QFETCH(QUrl, url);
			QFETCH(ActivationType, type);
			QFETCH(QString, value);

			TestActivationHandler::checkUrl(url, type, value);
		}


		void handleQueryParams_data()
		{
			QTest::addColumn<QUrl>("url");
			QTest::addColumn<bool>("useTestUri");
			QTest::addColumn<bool>("enableSimulator");

			QTest::newRow("empty") << QUrl("") << false << false;
			QTest::newRow("useTestUri") << QUrl("?useTestUri=true") << true << false;
			QTest::newRow("!useTestUri") << QUrl("?useTestUri=false") << false << false;
			QTest::newRow("enableSimulator") << QUrl("?enableSimulator=true") << false << true;
			QTest::newRow("!enableSimulator") << QUrl("?enableSimulator=false") << false << false;
			QTest::newRow("multi 1") << QUrl("?useTestUri=true&enableSimulator=true") << true << true;
			QTest::newRow("multi 2") << QUrl("?useTestUri=true&enableSimulator=false") << true << false;
		}


		void handleQueryParams()
		{
			auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			generalSettings.setDeveloperOptions(true);
			generalSettings.setUseSelfauthenticationTestUri(false);
			generalSettings.setSimulatorEnabled(false);

			QCOMPARE(generalSettings.useSelfAuthTestUri(), false);
			QCOMPARE(generalSettings.isSimulatorEnabled(), false);

			QFETCH(QUrl, url);
			QFETCH(bool, useTestUri);
			QFETCH(bool, enableSimulator);

			TestActivationHandler::checkQueryParams(url);

			QCOMPARE(generalSettings.useSelfAuthTestUri(), useTestUri);
			QCOMPARE(generalSettings.isSimulatorEnabled(), enableSimulator);
		}


};

QTEST_GUILESS_MAIN(test_ActivationHandler)
#include "test_ActivationHandler.moc"
