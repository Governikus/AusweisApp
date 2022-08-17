/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SurveyModel.h"

#include "AppSettings.h"
#include "DeviceInfo.h"
#include "Env.h"
#include "MockNetworkManager.h"
#include "VolatileSettings.h"

#include <QJsonDocument>
#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(CardInfo::TagType)

class test_SurveyModel
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<MockNetworkManager> mNetworkManager;

		void setReaderInfo(SurveyModel& model, ReaderManagerPlugInType readerPlugInType, CardInfo::TagType nfcTagType, int maxApduLength, bool hasEid = true)
		{
			CardInfo cardInfo(hasEid ? CardType::EID_CARD : CardType::NONE);
			cardInfo.setTagType(nfcTagType);
			ReaderInfo readerInfo("TestReader", readerPlugInType, cardInfo);
			readerInfo.setMaxApduLength(maxApduLength);
			model.setReaderInfo(readerInfo);
		}

	private Q_SLOTS:
		void init()
		{
			mNetworkManager.reset(new MockNetworkManager());
			Env::set(NetworkManager::staticMetaObject, mNetworkManager.data());
		}


		void cleanup()
		{
			AbstractSettings::getStore()->clear();
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
			QVERIFY(rom["OsVersion"].isString());
			QCOMPARE(rom["OsVersion"].toString(), DeviceInfo::getOSVersion());
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
			QVERIFY(result["NfcTagType"].isString());
			QCOMPARE(result["NfcTagType"].toString(), "U");
		}


		void testOnCardInserted_data()
		{
			QTest::addColumn<bool>("usedAsSdk");
			QTest::addColumn<bool>("hasEid");
			QTest::addColumn<ReaderManagerPlugInType>("readerPlugInType");
			QTest::addColumn<CardInfo::TagType>("nfcTagType");
			QTest::addColumn<int>("maxNfcPacketLength");
			QTest::addColumn<bool>("authSuccessful");

			QTest::addColumn<bool>("askForDeviceSurvey");
			QTest::addColumn<QString>("nfcTagTypeJson");
			QTest::addColumn<double>("maxNfcPacketLengthJson");

			QTest::addRow("Normal NFC_4A") << false << true << ReaderManagerPlugInType::NFC << CardInfo::TagType::NFC_4A << 9999 << true << true << "A" << 9999.;
			QTest::addRow("Normal NFC_4B") << false << true << ReaderManagerPlugInType::NFC << CardInfo::TagType::NFC_4B << 500 << true << true << "B" << 500.;

			QTest::addRow("Unknown NFC") << false << true << ReaderManagerPlugInType::NFC << CardInfo::TagType::UNKNOWN << 125 << true << true << "U" << 125.;
			QTest::addRow("SDK") << true << true << ReaderManagerPlugInType::NFC << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;
			QTest::addRow("No card") << false << false << ReaderManagerPlugInType::NFC << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;

			QTest::addRow("PCSC") << false << true << ReaderManagerPlugInType::PCSC << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;
			QTest::addRow("REMOTE IFD") << false << true << ReaderManagerPlugInType::REMOTE_IFD << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;
			QTest::addRow("LOCAL IFD") << false << true << ReaderManagerPlugInType::LOCAL_IFD << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;
			QTest::addRow("SMART") << false << true << ReaderManagerPlugInType::SMART << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;

			QTest::addRow("Auth Error") << false << true << ReaderManagerPlugInType::NFC << CardInfo::TagType::NFC_4A << 9999 << false << false << "A" << 9999.;
		}


		void testOnCardInserted()
		{
			QFETCH(bool, usedAsSdk);
			QFETCH(bool, hasEid);
			QFETCH(ReaderManagerPlugInType, readerPlugInType);
			QFETCH(CardInfo::TagType, nfcTagType);
			QFETCH(int, maxNfcPacketLength);
			QFETCH(bool, authSuccessful);

			QFETCH(bool, askForDeviceSurvey);
			QFETCH(QString, nfcTagTypeJson);
			QFETCH(double, maxNfcPacketLengthJson);

			SDK_MODE(usedAsSdk);

			SurveyModel model;

			model.transmitSurvey();
			mNetworkManager->fireFinished();
			QByteArray json = mNetworkManager->getLastData();
			auto result = QJsonDocument::fromJson(json);
			auto rom = result["Rom"].toObject();
			QCOMPARE(rom["MaximumNfcPacketLength"].toDouble(), 0);
			QCOMPARE(result["NfcTagType"].toString(), "U");
			QCOMPARE(model.askForDeviceSurvey(), false);

			setReaderInfo(model, readerPlugInType, nfcTagType, maxNfcPacketLength, hasEid);
			model.setAuthWasSuccessful(authSuccessful);

			QCOMPARE(model.askForDeviceSurvey(), askForDeviceSurvey);

			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(true);

			QCOMPARE(model.askForDeviceSurvey(), false);

			model.transmitSurvey();
			mNetworkManager->fireFinished();
			json = mNetworkManager->getLastData();
			result = QJsonDocument::fromJson(json);
			rom = result["Rom"].toObject();
			QCOMPARE(model.isDeviceSurveyPending(), askForDeviceSurvey);
			QCOMPARE(rom["MaximumNfcPacketLength"].toDouble(), maxNfcPacketLengthJson);
			QCOMPARE(result["NfcTagType"].toString(), nfcTagTypeJson);
		}


		void testResetNfc()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);

			SurveyModel model;

			model.setAuthWasSuccessful(true);
			setReaderInfo(model, ReaderManagerPlugInType::NFC, CardInfo::TagType::NFC_4A, 9999);

			model.resetNfcData();

			model.transmitSurvey();
			mNetworkManager->fireFinished();
			QByteArray json = mNetworkManager->getLastData();
			auto result = QJsonDocument::fromJson(json);
			auto rom = result["Rom"].toObject();
			QCOMPARE(model.askForDeviceSurvey(), false);
			QCOMPARE(rom["MaximumNfcPacketLength"].toDouble(), 0);
			QCOMPARE(result["NfcTagType"].toString(), "U");
		}


		void test_DeclineDeviceSurvey()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);

			SurveyModel model;

			setReaderInfo(model, ReaderManagerPlugInType::NFC, CardInfo::TagType::NFC_4A, 9999);
			model.setAuthWasSuccessful(true);

			QCOMPARE(model.askForDeviceSurvey(), true);

			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(false);

			QCOMPARE(model.askForDeviceSurvey(), false);
			QCOMPARE(model.isDeviceSurveyPending(), false);
		}


};

QTEST_GUILESS_MAIN(test_SurveyModel)
#include "test_SurveyModel.moc"
