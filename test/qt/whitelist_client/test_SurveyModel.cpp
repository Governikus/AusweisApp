/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "SurveyModel.h"

#include "DeviceInfo.h"
#include "Env.h"
#include "MockNetworkManager.h"

#include <QJsonDocument>
#include <QtTest>

using namespace governikus;

class test_SurveyModel
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<MockNetworkManager> mNetworkManager;

	private Q_SLOTS:
		void init()
		{
			mNetworkManager.reset(new MockNetworkManager());
			Env::set(NetworkManager::staticMetaObject, mNetworkManager.data());
		}


		void testTransmitSurvey()
		{
			SurveyModel model;

			model.transmitSurvey();
			mNetworkManager->fireFinished();
			const QByteArray json = mNetworkManager->getLastData();
			auto result = QJsonDocument::fromJson(json);

			QVERIFY(result["Rom"].isObject());
			auto rom = result["Rom"].toObject();
			QVERIFY(rom["BuildNumber"].isString());
			QCOMPARE(rom["BuildNumber"].toString(), DeviceInfo::getOSBuildNumber());
			QVERIFY(rom["AndroidVersion"].isString());
			QCOMPARE(rom["AndroidVersion"].toString(), DeviceInfo::getOSVersion());
			QVERIFY(rom["KernelVersion"].isString());
			QCOMPARE(rom["KernelVersion"].toString(), DeviceInfo::getKernelVersion());
			QVERIFY(rom["MaximumNfcPacketLength"].isDouble());
			QCOMPARE(rom["MaximumNfcPacketLength"].toDouble(), 0);

			QVERIFY(result["Vendor"].isString());
			QCOMPARE(result["Vendor"].toString(), DeviceInfo::getVendor());
			QVERIFY(result["ModelNumber"].isString());
			QCOMPARE(result["ModelNumber"].toString(), DeviceInfo::getModelNumber());
			QVERIFY(result["ModelName"].isString());
			QCOMPARE(result["ModelName"].toString(), DeviceInfo::getModelName());
			QVERIFY(result["AusweisAppVersionNumber"].isString());
			QCOMPARE(result["AusweisAppVersionNumber"].toString(), QCoreApplication::applicationVersion());
		}


		void testMaximumPacketLength()
		{
			SurveyModel model;

			model.transmitSurvey();
			mNetworkManager->fireFinished();
			QByteArray json = mNetworkManager->getLastData();
			auto result = QJsonDocument::fromJson(json);
			auto rom = result["Rom"].toObject();
			QCOMPARE(rom["MaximumNfcPacketLength"].toDouble(), 0);

			model.setMaximumNfcPacketLength(9999);
			model.transmitSurvey();
			mNetworkManager->fireFinished();
			json = mNetworkManager->getLastData();
			result = QJsonDocument::fromJson(json);
			rom = result["Rom"].toObject();
			QCOMPARE(rom["MaximumNfcPacketLength"].toDouble(), 9999);
		}


};

QTEST_GUILESS_MAIN(test_SurveyModel)
#include "test_SurveyModel.moc"
