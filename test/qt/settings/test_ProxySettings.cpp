/*!
 * \brief Unit tests for \ref ProxySettings
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QCoreApplication>
#include <QFile>
#include <QtTest>

#include "ProxySettings.h"


using namespace governikus;

class test_ProxySettings
	: public QObject
{
	Q_OBJECT
	QScopedPointer<ProxySettings> settings;

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
			settings.reset(new ProxySettings());
		}


		void testEquals()
		{
			QSKIP("Proxy settings are disabled");
			ProxySettings otherSettings;
			otherSettings.load();

			QVERIFY(*settings == otherSettings);

			settings->setProxyType(settings->getProxyType() == QNetworkProxy::NoProxy ? QNetworkProxy::DefaultProxy : QNetworkProxy::NoProxy);
			QVERIFY(*settings != otherSettings);
			otherSettings.setProxyType(settings->getProxyType());
		}


		void testProxyType()
		{
			QSKIP("Proxy settings are disabled");
			auto initial = settings->getProxyType();
			auto newValue = initial == QNetworkProxy::NoProxy ? QNetworkProxy::DefaultProxy : QNetworkProxy::NoProxy;

			settings->setProxyType(newValue);
			QCOMPARE(settings->getProxyType(), newValue);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());


			settings->setProxyType(initial);
			QCOMPARE(settings->getProxyType(), initial);
			settings->save();

		}


};

QTEST_GUILESS_MAIN(test_ProxySettings)
#include "test_ProxySettings.moc"
