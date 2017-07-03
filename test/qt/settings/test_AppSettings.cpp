/*!
 * \brief Unit tests for \ref AppSettings
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QCoreApplication>
#include <QFile>
#include <QtTest>

#include "AppSettings.h"


using namespace governikus;

class test_AppSettings
	: public QObject
{
	Q_OBJECT

	private:
		AppSettings* settings = nullptr;

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
			settings = new AppSettings;
		}


		void cleanup()
		{
			delete settings;
		}


		void testEquals()
		{
			AppSettings* otherSettings = new AppSettings;

			QVERIFY(*settings == *otherSettings);

			settings->getGeneralSettings().setShowSetupAssistant(!settings->getGeneralSettings().isShowSetupAssistant());
			QVERIFY(*settings != *otherSettings);
			otherSettings->getGeneralSettings().setShowSetupAssistant(settings->getGeneralSettings().isShowSetupAssistant());
			QVERIFY(*settings == *otherSettings);

			settings->getPreVerificationSettings().addLinkCertificate(QByteArray("123456"));
			QVERIFY(*settings != *otherSettings);
			otherSettings->getPreVerificationSettings().addLinkCertificate(QByteArray("123456"));
			QVERIFY(*settings == *otherSettings);

			settings->getProviderSettings().setIssueDate(QDateTime(QDate(1989, 11, 9)));
			QVERIFY(*settings != *otherSettings);
			otherSettings->getProviderSettings().setIssueDate(QDateTime(QDate(1989, 11, 9)));
			QVERIFY(*settings == *otherSettings);

			auto proxyType = settings->getProxySettings().getProxyType() == QNetworkProxy::DefaultProxy ? QNetworkProxy::NoProxy : QNetworkProxy::DefaultProxy;
			settings->getProxySettings().setProxyType(proxyType);
			QVERIFY(*settings != *otherSettings);
			otherSettings->getProxySettings().setProxyType(proxyType);
			QVERIFY(*settings == *otherSettings);

			settings->getSecureStorage().load();
			QVERIFY(*settings != *otherSettings);
			otherSettings->getSecureStorage().load();
			QVERIFY(*settings == *otherSettings);

			delete otherSettings;
		}


};

QTEST_GUILESS_MAIN(test_AppSettings)
#include "test_AppSettings.moc"
