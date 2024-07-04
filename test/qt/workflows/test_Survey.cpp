/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Survey.h"

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

class test_Survey
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<MockNetworkManager> mNetworkManager;

		void setReaderInfo(Survey& pSurvey, ReaderManagerPluginType pReaderPluginType, CardInfo::TagType pNfcTagType, int pMaxApduLength, bool pHasEid = true)
		{
			CardInfo cardInfo(pHasEid ? CardType::EID_CARD : CardType::NONE);
			cardInfo.setTagType(pNfcTagType);
			ReaderInfo readerInfo("TestReader"_L1, pReaderPluginType, cardInfo);
			readerInfo.setMaxApduLength(pMaxApduLength);
			pSurvey.setReaderInfo(readerInfo);
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
			Survey survey;

			survey.transmitSurvey();
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
			QTest::addColumn<ReaderManagerPluginType>("readerPluginType");
			QTest::addColumn<CardInfo::TagType>("nfcTagType");
			QTest::addColumn<int>("maxNfcPacketLength");
			QTest::addColumn<bool>("authSuccessful");

			QTest::addColumn<bool>("askForDeviceSurvey");
			QTest::addColumn<QString>("nfcTagTypeJson");
			QTest::addColumn<double>("maxNfcPacketLengthJson");

			QTest::addRow("Normal NFC_4A") << false << true << ReaderManagerPluginType::NFC << CardInfo::TagType::NFC_4A << 9999 << true << true << "A" << 9999.;
			QTest::addRow("Normal NFC_4B") << false << true << ReaderManagerPluginType::NFC << CardInfo::TagType::NFC_4B << 500 << true << true << "B" << 500.;

			QTest::addRow("Unknown NFC") << false << true << ReaderManagerPluginType::NFC << CardInfo::TagType::UNKNOWN << 125 << true << true << "U" << 125.;
			QTest::addRow("SDK") << true << true << ReaderManagerPluginType::NFC << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;
			QTest::addRow("No card") << false << false << ReaderManagerPluginType::NFC << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;

			QTest::addRow("PCSC") << false << true << ReaderManagerPluginType::PCSC << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;
			QTest::addRow("REMOTE IFD") << false << true << ReaderManagerPluginType::REMOTE_IFD << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;
			QTest::addRow("LOCAL IFD") << false << true << ReaderManagerPluginType::LOCAL_IFD << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;
			QTest::addRow("SMART") << false << true << ReaderManagerPluginType::SMART << CardInfo::TagType::NFC_4A << 9999 << true << false << "U" << 0.;

			QTest::addRow("Auth Error") << false << true << ReaderManagerPluginType::NFC << CardInfo::TagType::NFC_4A << 9999 << false << false << "A" << 9999.;
		}


		void testOnCardInserted()
		{
			QFETCH(bool, usedAsSdk);
			QFETCH(bool, hasEid);
			QFETCH(ReaderManagerPluginType, readerPluginType);
			QFETCH(CardInfo::TagType, nfcTagType);
			QFETCH(int, maxNfcPacketLength);
			QFETCH(bool, authSuccessful);

			QFETCH(bool, askForDeviceSurvey);
			QFETCH(QString, nfcTagTypeJson);
			QFETCH(double, maxNfcPacketLengthJson);

			SDK_MODE(usedAsSdk);

			Survey survey;

			survey.transmitSurvey();
			mNetworkManager->fireFinished();
			QByteArray json = mNetworkManager->getLastData();
			auto result = QJsonDocument::fromJson(json);
			auto rom = result["Rom"_L1].toObject();
			QCOMPARE(rom["MaximumNfcPacketLength"_L1].toDouble(), 0);
			QCOMPARE(result["NfcTagType"_L1].toString(), "U"_L1);
			QCOMPARE(survey.askForDeviceSurvey(), false);

			setReaderInfo(survey, readerPluginType, nfcTagType, maxNfcPacketLength, hasEid);
			survey.setAuthWasSuccessful(authSuccessful);

			QCOMPARE(survey.askForDeviceSurvey(), askForDeviceSurvey);

			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(true);

			QCOMPARE(survey.askForDeviceSurvey(), false);

			survey.transmitSurvey();
			mNetworkManager->fireFinished();
			json = mNetworkManager->getLastData();
			result = QJsonDocument::fromJson(json);
			rom = result["Rom"_L1].toObject();
			QCOMPARE(survey.isDeviceSurveyPending(), askForDeviceSurvey);
			QCOMPARE(rom["MaximumNfcPacketLength"_L1].toDouble(), maxNfcPacketLengthJson);
			QCOMPARE(result["NfcTagType"_L1].toString(), nfcTagTypeJson);
		}


		void testResetNfc()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);

			Survey survey;

			survey.setAuthWasSuccessful(true);
			setReaderInfo(survey, ReaderManagerPluginType::NFC, CardInfo::TagType::NFC_4A, 9999);

			survey.resetNfcData();

			survey.transmitSurvey();
			mNetworkManager->fireFinished();
			QByteArray json = mNetworkManager->getLastData();
			auto result = QJsonDocument::fromJson(json);
			auto rom = result["Rom"_L1].toObject();
			QCOMPARE(survey.askForDeviceSurvey(), false);
			QCOMPARE(rom["MaximumNfcPacketLength"_L1].toDouble(), 0);
			QCOMPARE(result["NfcTagType"_L1].toString(), "U"_L1);
		}


		void test_DeclineDeviceSurvey()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);

			Survey survey;

			setReaderInfo(survey, ReaderManagerPluginType::NFC, CardInfo::TagType::NFC_4A, 9999);
			survey.setAuthWasSuccessful(true);

			QCOMPARE(survey.askForDeviceSurvey(), true);

			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(false);

			QCOMPARE(survey.askForDeviceSurvey(), false);
			QCOMPARE(survey.isDeviceSurveyPending(), false);
		}


};

QTEST_GUILESS_MAIN(test_Survey)
#include "test_Survey.moc"
