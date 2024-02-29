/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SurveyModel.h"

#include "AppSettings.h"
#include "DeviceInfo.h"
#include "Env.h"
#include "MockNetworkManager.h"
#include "VolatileSettings.h"

#include <QJsonDocument>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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
			ReaderInfo readerInfo("TestReader"_L1, readerPlugInType, cardInfo);
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

			QVERIFY(result["Rom"_L1].isObject());
			auto rom = result["Rom"_L1].toObject();
			QVERIFY(rom["BuildNumber"_L1].isString());
			QCOMPARE(rom["BuildNumber"_L1].toString(), DeviceInfo::getOSBuildNumber());
			QVERIFY(rom["OsVersion"_L1].isString());
			QCOMPARE(rom["OsVersion"_L1].toString(), DeviceInfo::getOSVersion());
			QVERIFY(rom["KernelVersion"_L1].isString());
			QCOMPARE(rom["KernelVersion"_L1].toString(), DeviceInfo::getKernelVersion());
			QVERIFY(rom["MaximumNfcPacketLength"_L1].isDouble());
			QCOMPARE(rom["MaximumNfcPacketLength"_L1].toDouble(), 0);

			QVERIFY(result["Vendor"_L1].isString());
			QCOMPARE(result["Vendor"_L1].toString(), DeviceInfo::getVendor());
			QVERIFY(result["ModelNumber"_L1].isString());
			QCOMPARE(result["ModelNumber"_L1].toString(), DeviceInfo::getModelNumber());
			QVERIFY(result["ModelName"_L1].isString());
			QCOMPARE(result["ModelName"_L1].toString(), DeviceInfo::getModelName());
			QVERIFY(result["AusweisAppVersionNumber"_L1].isString());
			QCOMPARE(result["AusweisAppVersionNumber"_L1].toString(), QCoreApplication::applicationVersion());
			QVERIFY(result["NfcTagType"_L1].isString());
			QCOMPARE(result["NfcTagType"_L1].toString(), "U"_L1);
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
			auto rom = result["Rom"_L1].toObject();
			QCOMPARE(rom["MaximumNfcPacketLength"_L1].toDouble(), 0);
			QCOMPARE(result["NfcTagType"_L1].toString(), "U"_L1);
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
			rom = result["Rom"_L1].toObject();
			QCOMPARE(model.isDeviceSurveyPending(), askForDeviceSurvey);
			QCOMPARE(rom["MaximumNfcPacketLength"_L1].toDouble(), maxNfcPacketLengthJson);
			QCOMPARE(result["NfcTagType"_L1].toString(), nfcTagTypeJson);
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
			auto rom = result["Rom"_L1].toObject();
			QCOMPARE(model.askForDeviceSurvey(), false);
			QCOMPARE(rom["MaximumNfcPacketLength"_L1].toDouble(), 0);
			QCOMPARE(result["NfcTagType"_L1].toString(), "U"_L1);
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
