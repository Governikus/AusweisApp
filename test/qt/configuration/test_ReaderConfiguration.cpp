/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfiguration.h"

#include "Env.h"
#include "MockReaderDetector.h"
#include "ResourceLoader.h"
#include "TestFileHelper.h"

#include <QSharedPointer>
#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(UsbId)

class test_ReaderConfiguration
	: public QObject
{
	Q_OBJECT

	public:
		test_ReaderConfiguration()
			: mUsbIds()
			, mMockReaderDetector(mUsbIds)
		{
			Env::set(ReaderDetector::staticMetaObject, &mMockReaderDetector);
		}

	private:
		static const int cCardReadersInConfigurationFile;

		QList<UsbId> mUsbIds;
		MockReaderDetector mMockReaderDetector;

		void checkPlatformsMinMax(const QJsonObject& pObject, const QLatin1String& pValue)
		{
			const QJsonArray& array = pObject[pValue].toArray();
			for (const QJsonValueConstRef entry : array)
			{
				QVERIFY(entry.isObject());
				auto object = entry.toObject();
				QVERIFY(object.contains(QLatin1String("Platforms")));

				const QJsonArray& platforms = object[QLatin1String("Platforms")].toArray();
				for (const QJsonValueConstRef platform : platforms)
				{
					QVERIFY(platform.isObject());
					auto platformObject = platform.toObject();
					if (platformObject.contains(QLatin1String("min")) && platformObject.contains(QLatin1String("max")))
					{
						auto min = QVersionNumber::fromString(platformObject[QLatin1String("min")].toString());
						auto max = QVersionNumber::fromString(platformObject[QLatin1String("max")].toString());
						QVERIFY(min <= max);
					}
				}
			}
		}

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mUsbIds.clear();
		}


		void checkDefaultReader()
		{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
			const ReaderConfigurationInfo readerSettingsInfo = ReaderConfigurationInfo("crap"_L1);
#else
			const ReaderConfigurationInfo readerSettingsInfo = Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo("crap"_L1);
#endif
			QCOMPARE(readerSettingsInfo.getName(), "crap"_L1);
			QCOMPARE(readerSettingsInfo.getIcon()->getName(), "default_reader.png"_L1);
			QCOMPARE(readerSettingsInfo.getPattern(), QString());
		}


		void checkReaderData_data()
		{
			QTest::addColumn<UsbId>("usbId");
			QTest::addColumn<QString>("readerName");
			QTest::addColumn<QString>("readerViewName");
			QTest::addColumn<QString>("readerIcon");
			QTest::addColumn<QString>("readerPattern");

			QTest::newRow("Remote card reader") << UsbId(0x0000, 0x0000) << "NFC-abcdef1234567890" << QStringLiteral("Smartphone als Kartenleser") << "img_RemoteReader" << "^NFC.*";

			QTest::newRow("PersoSim SaC") << UsbId(0x0000, 0x0001) << "PersoSimHandshake 00 00" << QStringLiteral("PersoSim") << "img_PersoSim" << "^PersoSim.*";
			QTest::newRow("PersoSim native") << UsbId(0x0000, 0x0001) << "PersoSim Virtual Card Reader 0" << QStringLiteral("PersoSim") << "img_PersoSim" << "^PersoSim.*";

			QTest::newRow("Simulator") << UsbId(0x0000, 0x0002) << "Simulator" << "Simulator" << "img_Simulator" << "^Simulator$";

			QTest::newRow("REINER SCT cyberJack RFID komfort") << UsbId(0x0C4B, 0x0501) << "REINER SCT cyberJack RFID komfort" << "REINER SCT cyberJack RFID komfort" << "img_Reiner_SCT_cyberjack_RFID_komfort" << "REINER SCT cyberJack RFID komfort";
			QTest::newRow("REINER SCT cyberJack RFID komfort FON") << UsbId(0x0C4B, 0x2007) << "REINER SCT cyberJack RFID komfort FON" << "REINER SCT cyberJack RFID komfort FON" << "img_Reiner_SCT_cyberjack_RFID_komfort" << "^REINER SCT cyberJack RFID komfort FON";
			QTest::newRow("REINER SCT cyberJack RFID standard") << UsbId(0x0C4B, 0x0500) << "REINER SCT cyberJack RFID standard" << "REINER SCT cyberJack RFID standard" << "img_Reiner_SCT_cyberjack_RFID_standard" << "REINER SCT cyberJack RFID standard";
			QTest::newRow("REINER SCT cyberJack RFID basis") << UsbId(0x0C4B, 0x9102) << "REINER SCT cyberJack RFID basis" << "REINER SCT cyberJack RFID basis" << "img_Reiner_SCT_cyberjack_RFID_basis" << "REINER SCT cyberJack RFID basis";
			QTest::newRow("REINER SCT cyberJack wave") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave" << "REINER SCT cyberJack wave" << "img_cyberjack_wave" << "REINER SCT cyberJack wave";

			QTest::newRow("KOBIL IDToken") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken" << "KOBIL IDToken" << "img_KOBIL_ID_Token" << "^KOBIL (Systems )?IDToken";

			QTest::newRow("Identiv SDI011B") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Contactless Reader" << "Identiv SDI011 Dual Interface Smart Card Reader" << "img_Identive_SDI011" << "^(SCM Microsystems Inc. )?SDI011G? ((Contactless Reader( 0)?)|((USB Smart Card|Contactless) Reader\\([12]\\)))$";
			QTest::newRow("Identiv SDI011C") << UsbId(0x04E6, 0x512C) << "SCM Microsystems Inc. SDI011 Contactless Reader" << "Identiv SDI011 Dual Interface Smart Card Reader" << "img_Identive_SDI011" << "^(SCM Microsystems Inc. )?SDI011G? ((Contactless Reader( 0)?)|((USB Smart Card|Contactless) Reader\\([12]\\)))$";
			QTest::newRow("Identiv SCL011") << UsbId(0x04E6, 0x5292) << "SCM Microsystems Inc. SCL011 Contactless Reader" << "Identiv SCL01x Contactless Smart Card Reader" << "img_Identive_SCL011" << "^(SCM Microsystems Inc. )?SCL011G? Contactless Reader( 0)?$";

			QTest::newRow("ACS ACR1281U") << UsbId(0x072F, 0x0901) << "ACS ACR1281 PICC Reader" << "ACS ACR1281U" << "img_ACS_ACR1281U" << "ACS ACR1281 PICC Reader( 0)?";
			QTest::newRow("ACS ACR1281U-C1") << UsbId(0x072F, 0x2224) << "ACS ACR1281 1S Dual Reader PICC 0" << "ACS ACR1281U-C1" << "img_ACS_ACR1281U" << "ACS ACR1281 1S Dual Reader (PICC 0|\\(1\\))";
			QTest::newRow("ACS-ACR1581U-C1") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader PICC" << "ACS ACR1581U-C1" << "img_ACS_ACR1581U-C1" << "ACS ACR1581 1S Dual Reader (PICC|\\(1\\))( 0)?";

			QTest::newRow("HID OMNIKEY 5022") << UsbId(0x076B, 0x5022) << "HID Global OMNIKEY 5022 Smart Card Reader 0" << "HID OMNIKEY 5022-CL" << "img_HID_Omnikey_Mobile_Reader_502X_CL" << "HID Global OMNIKEY 5022 Smart Card Reader( 0)?$";
			QTest::newRow("HID OMNIKEY 5422") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY 5422CL Smartcard Reader 0" << "HID OMNIKEY 5422" << "img_HID_Omnikey_542x" << R"(HID Global OMNIKEY 5422CL Smartcard Reader 0|HID Global OMNIKEY Smartcard Reader ?(\(1\)|\(2\)))";

			QTest::newRow("FEIG OBID myAXXESS RFID-Reader") << UsbId(0x0AB1, 0x0003) << "FEIG ELECTRONIC GmbH OBID myAXXESS basic Slot:CL 358334430" << "OBID RFID-Reader" << "img_FEIG_myAXXES_basic" << "FEIG ELECTRONIC GmbH OBID myAXXESS basic Slot:CL 358334430";

			QTest::newRow("Gemalto-Prox-DU") << UsbId(0x08E6, 0x5503) << "Gemalto Prox-DU Contactless_" << "Gemalto Prox-DU HID" << "img_Gemalto_Prox_DU" << R"(Gemalto .*Prox(-DU| Dual)( Contactless_| USB PC Link(Reader| Reader)(\(2\)|\(1\))))";
			QTest::newRow("Gemalto-Prox-SU") << UsbId(0x08E6, 0x5504) << "Gemalto Prox-SU Contactless_" << "Gemalto Prox-SU Contactless" << "img_Gemalto_Prox_SU" << R"(Gemalto Prox( |-)SU( Contactless_| USB PC LinkReader(\(1\)|\(2\))))";

			QTest::newRow("Identiv-SCL-3711") << UsbId(0x04E6, 0x5591) << "SCM Microsystems SCL3711 reader & NFC device 0" << "Identiv SCL3711" << "img_Identive_SCL3711" << "(SCM Microsystems SCL3711 reader & NFC device 0|SCL3711 Reader and NFC device)";
			QTest::newRow("Identiv-3700-F") << UsbId(0x04E6, 0x5790) << "Identiv CLOUD 3700 F Contactless Reader 0" << "Identiv 3700 F" << "img_Identive_XXXX_F" << "^Identiv[e]? (uTrust|cloud|CLOUD) 3700 F (Contactless|CL) Reader( 0)?$";
			QTest::newRow("Identiv 3720 F") << UsbId(0x04E6, 0x5612) << "Identiv uTrust 3720 Contactless Reader 0" << "Identiv 3720 F" << "img_Identive_XXXX_F" << "^Identiv (uTrust|cloud|CLOUD) 3720 (Contactless|CL) Reader( 0|\\(1\\))$";
			QTest::newRow("Identiv 3721 F") << UsbId(0x04E6, 0x5613) << "Identiv uTrust 3721 Contactless Reader 0" << "Identiv 3721 F" << "img_Identive_XXXX_F" << "^Identiv (uTrust|cloud|CLOUD) 3721 (Contactless|CL) Reader( 0|\\(1\\))$";
			QTest::newRow("Identiv-Cloud-4700-F") << UsbId(0x04E6, 0x5720) << "Identive CLOUD 4700 F Contactless Reader 1" << "Identiv Cloud 4700 F" << "img_Identive_Cloud_4700_F" << "(Identive CLOUD 4700 F Contactless Reader( 0| 1)|Identiv uTrust 4700 F Dual Interface Reader\\(2\\))";
			QTest::newRow("Identiv 4701 F") << UsbId(0x04E6, 0x5724) << "Identiv CLOUD 4701 F Contactless Reader 0" << "Identiv 4701 F" << "img_Identive_4701_F" << "^Identiv (uTrust|cloud|CLOUD) 4701 F (Contactless|CL|Dual Interface) Reader( 0| 1|\\(1\\)|\\(2\\))?$";

			QTest::newRow("Cherry-sercure Board") << UsbId(0x046A, 0x01A2) << "Cherry GmbH CHERRY SECURE BOARD 1.0" << "Cherry Secure Board 1.0" << "img_Cherry_secure_board" << "Cherry GmbH CHERRY SECURE BOARD 1.0( 0)?$";

			QTest::newRow("Signotec Omega Pad") << UsbId(0x2133, 0x010B) << "NXP PR533" << "Signotec Omega Pad" << "img_Signotec_Omega_Pad" << "NXP PR533( 0)?";
		}


		void checkReaderData()
		{
			QFETCH(UsbId, usbId);
			QFETCH(QString, readerName);
			QFETCH(QString, readerViewName);
			QFETCH(QString, readerIcon);
			QFETCH(QString, readerPattern);

			mUsbIds += usbId;
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
			const ReaderConfigurationInfo readerSettingsInfo = ReaderConfigurationInfo("crap"_L1);
#else
			const ReaderConfigurationInfo readerSettingsInfo = Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo(readerName);
#endif
			QCOMPARE(readerSettingsInfo.getName(), readerViewName);
			QCOMPARE(readerSettingsInfo.getIcon()->getName(), readerIcon + ".png"_L1);
			QCOMPARE(readerSettingsInfo.getPattern(), readerPattern);
		}


		void checkReaderPattern_data()
		{
			QTest::addColumn<UsbId>("usbId");
			QTest::addColumn<QString>("readerName");
			QTest::addColumn<QString>("readerViewName");

			QTest::newRow("UU") << UsbId(0xFFFF, 0xFFFF) << "crap" << "crap";

			QTest::newRow("Remote card reader") << UsbId(0x0000, 0x0000) << "NFC-abcdef1234567890" << QStringLiteral("Smartphone als Kartenleser");

			QTest::newRow("PersoSim SaC") << UsbId(0x0000, 0x0001) << "PersoSimHandshake 00 00" << QStringLiteral("PersoSim");
			QTest::newRow("PersoSim native") << UsbId(0x0000, 0x0001) << "PersoSim Virtual Card Reader 0" << QStringLiteral("PersoSim");

			QTest::newRow("Simulator") << UsbId(0x0000, 0x0002) << "Simulator" << "Simulator";

			QTest::newRow("REINER SCT cyberJack RFID komfort-windows-10-11") << UsbId(0x0C4B, 0x0501) << "REINER SCT cyberJack RFID komfort USB 1" << "REINER SCT cyberJack RFID komfort";
			QTest::newRow("REINER SCT cyberJack RFID komfort-macosx-12-15") << UsbId(0x0C4B, 0x0501) << "REINER SCT cyberJack RFID komfort" << "REINER SCT cyberJack RFID komfort";

			QTest::newRow("REINER SCT cyberJack RFID komfort-FON-windows-7-10") << UsbId(0x0C4B, 0x2007) << "REINER SCT cyberJack RFID komfort FON USB 52" << "REINER SCT cyberJack RFID komfort FON";
			QTest::newRow("REINER SCT cyberJack RFID komfort-FON-macosx-10.13-11.0") << UsbId(0x0C4B, 0x2007) << "REINER SCT cyberJack RFID komfort FON" << "REINER SCT cyberJack RFID komfort FON";

			QTest::newRow("REINER SCT cyberJack RFID standard-windows-10-11") << UsbId(0x0C4B, 0x0500) << "REINER SCT cyberJack RFID standard USB 1" << "REINER SCT cyberJack RFID standard";
			QTest::newRow("REINER SCT cyberJack RFID standard-macosx-12-15") << UsbId(0x0C4B, 0x0500) << "REINER SCT cyberJack RFID standard" << "REINER SCT cyberJack RFID standard";

			QTest::newRow("REINER SCT cyberJack RFID basis-windows-10-11") << UsbId(0x0C4B, 0x9102) << "REINER SCT cyberJack RFID basis 0" << "REINER SCT cyberJack RFID basis";
			QTest::newRow("REINER SCT cyberJack RFID basis-macosx-11-15") << UsbId(0x0C4B, 0x9102) << "REINER SCT cyberJack RFID basis" << "REINER SCT cyberJack RFID basis";

			QTest::newRow("REINER SCT cyberJack wave-windows-10-11-1") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave 0" << "REINER SCT cyberJack wave";
			QTest::newRow("REINER SCT cyberJack wave-windows-10-11-1") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave 1" << "REINER SCT cyberJack wave";
			QTest::newRow("REINER SCT cyberJack wave-windows-10-11-2") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave USB 1" << "REINER SCT cyberJack wave";
			QTest::newRow("REINER SCT cyberJack wave-macosx-12-15") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave" << "REINER SCT cyberJack wave";

			QTest::newRow("KOBIL IDToken-windows-10-11-1") << UsbId(0x0D46, 0x301D) << "KOBIL IDToken 0" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-windows-10-11-2") << UsbId(0x0D46, 0x301D) << "KOBIL IDToken 1" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-windows-10-11-3") << UsbId(0x0D46, 0x301D) << "KOBIL IDToken 2" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-windows-10-11-4") << UsbId(0x0D46, 0x301D) << "KOBIL IDToken" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-macosx-12-15-1") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-macosx-12-15-2") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken 0" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-macosx-12-15-3") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken 1" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-unknown-1") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken (NS1252PT18716) 01 00" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-unknown-2") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken (NS1228PT186BD) 01 00" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-unknown-3") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken (0123456789ABCD) 02 01" << "KOBIL IDToken";

			QTest::newRow("Identiv SDI011B-windows-10-11-1") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Contactless Reader 0" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011B-windows-10-11-2") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Smart Card Reader 0" << "SCM Microsystems Inc. SDI011 Smart Card Reader 0";
			QTest::newRow("Identiv SDI011B-windows-10-11-3") << UsbId(0x04E6, 0x512B) << "SDI011 Contactless Reader" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011B-windows-10-11-4") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011G Contactless Reader 0" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011B-windows-10-11-5") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011G Smart Card Reader 0" << "SCM Microsystems Inc. SDI011G Smart Card Reader 0";
			QTest::newRow("Identiv SDI011B-macosx-12-15") << UsbId(0x04E6, 0x512B) << "SDI011 USB Smart Card Reader(1)" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011B-macosx-12-15") << UsbId(0x04E6, 0x512B) << "SDI011 USB Smart Card Reader(2)" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011B-macosx-12-15-1") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Contactless Reader(1)" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011B-macosx-12-15-2") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Contactless Reader(2)" << "Identiv SDI011 Dual Interface Smart Card Reader";

			QTest::newRow("Identiv SDI011C-windows-10-11-1") << UsbId(0x04E6, 0x512C) << "SCM Microsystems Inc. SDI011 Contactless Reader 0" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011C-windows-10-11-2") << UsbId(0x04E6, 0x512C) << "SCM Microsystems Inc. SDI011 Smart Card Reader 0" << "SCM Microsystems Inc. SDI011 Smart Card Reader 0";
			QTest::newRow("Identiv SDI011C-windows-10-11-3") << UsbId(0x04E6, 0x512C) << "SDI011 Contactless Reader" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011C-windows-10-11-4") << UsbId(0x04E6, 0x512C) << "SCM Microsystems Inc. SDI011G Contactless Reader 0" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011C-windows-10-11-5") << UsbId(0x04E6, 0x512C) << "SCM Microsystems Inc. SDI011G Smart Card Reader 0" << "SCM Microsystems Inc. SDI011G Smart Card Reader 0";
			QTest::newRow("Identiv SDI011C-macosx-12-15") << UsbId(0x04E6, 0x512C) << "SDI011 USB Smart Card Reader(1)" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011C-macosx-12-15") << UsbId(0x04E6, 0x512C) << "SDI011 USB Smart Card Reader(2)" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011C-macosx-12-15-1") << UsbId(0x04E6, 0x512C) << "SCM Microsystems Inc. SDI011 Contactless Reader(1)" << "Identiv SDI011 Dual Interface Smart Card Reader";
			QTest::newRow("Identiv SDI011C-macosx-12-15-2") << UsbId(0x04E6, 0x512C) << "SCM Microsystems Inc. SDI011 Contactless Reader(2)" << "Identiv SDI011 Dual Interface Smart Card Reader";

			QTest::newRow("Identiv SCL011-windows-10-11-1") << UsbId(0x04E6, 0x5292) << "SCL011 Contactless Reader" << "Identiv SCL01x Contactless Smart Card Reader";
			QTest::newRow("Identiv SCL011-windows-10-11-2") << UsbId(0x04E6, 0x5292) << "SCM Microsystems Inc. SCL010 Contactless Reader" << "SCM Microsystems Inc. SCL010 Contactless Reader";
			QTest::newRow("Identiv SCL011-windows-10-11-3") << UsbId(0x04E6, 0x5292) << "SCM Microsystems Inc. SCL011 Contactless Reader 0" << "Identiv SCL01x Contactless Smart Card Reader";
			QTest::newRow("Identiv SCL011-windows-10-11-4") << UsbId(0x04E6, 0x5292) << "SCM Microsystems Inc. SCL011G Contactless Reader 0" << "Identiv SCL01x Contactless Smart Card Reader";
			QTest::newRow("Identiv SCL011-macosx-12-15-1") << UsbId(0x04E6, 0x5292) << "SCL011 Contactless Reader" << "Identiv SCL01x Contactless Smart Card Reader";
			QTest::newRow("Identiv SCL011-macosx-12-15-2") << UsbId(0x04E6, 0x5292) << "SCM Microsystems Inc. SCL011 Contactless Reader" << "Identiv SCL01x Contactless Smart Card Reader";

			QTest::newRow("ACS-ACR1281U-windows-10-11") << UsbId(0x072F, 0x0901) << "ACS ACR1281 PICC Reader 0" << "ACS ACR1281U";
			QTest::newRow("ACS-ACR1281U-macosx-12-15") << UsbId(0x072F, 0x0901) << "ACS ACR1281 PICC Reader" << "ACS ACR1281U";

			QTest::newRow("ACS-ACR1281U-C1-windows-10-11-1") << UsbId(0x072F, 0x2224) << "ACS ACR1281 1S Dual Reader PICC 0" << "ACS ACR1281U-C1";
			QTest::newRow("ACS-ACR1281U-C1-windows-10-11-2") << UsbId(0x072F, 0x2224) << "ACS ACR1281 1S Dual Reader ICC 0" << "ACS ACR1281 1S Dual Reader ICC 0";
			QTest::newRow("ACS-ACR1281U-C1-windows-10-11-3") << UsbId(0x072F, 0x2224) << "ACS ACR1281 1S Dual Reader SAM 0" << "ACS ACR1281 1S Dual Reader SAM 0";
			QTest::newRow("ACS-ACR1281U-C1-macosx-12-15-1") << UsbId(0x072F, 0x2224) << "ACS ACR1281 1S Dual Reader (1)" << "ACS ACR1281U-C1";
			QTest::newRow("ACS-ACR1281U-C1-macosx-12-15-2") << UsbId(0x072F, 0x2224) << "ACS ACR1281 1S Dual Reader (2)" << "ACS ACR1281 1S Dual Reader (2)";
			QTest::newRow("ACS-ACR1281U-C1-macosx-12-15-3") << UsbId(0x072F, 0x2224) << "ACS ACR1281 1S Dual Reader (3)" << "ACS ACR1281 1S Dual Reader (3)";

			QTest::newRow("ACS-ACR1581-windows-10-11-1") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader PICC 0" << "ACS ACR1581U-C1";
			QTest::newRow("ACS-ACR1581-windows-10-11-2") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader ICC 0" << "ACS ACR1581 1S Dual Reader ICC 0";
			QTest::newRow("ACS-ACR1581-windows-10-11-3") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader SAM 0" << "ACS ACR1581 1S Dual Reader SAM 0";
			QTest::newRow("ACS-ACR1581-macosx-12-13-1") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader (1)" << "ACS ACR1581U-C1";
			QTest::newRow("ACS-ACR1581-macosx-12-13-2") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader (2)" << "ACS ACR1581 1S Dual Reader (2)";
			QTest::newRow("ACS-ACR1581-macosx-12-13-3") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader (3)" << "ACS ACR1581 1S Dual Reader (3)";
			QTest::newRow("ACS-ACR1581-macosx-14-15-1") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader PICC" << "ACS ACR1581U-C1";
			QTest::newRow("ACS-ACR1581-macosx-14-15-2") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader ICC" << "ACS ACR1581 1S Dual Reader ICC";
			QTest::newRow("ACS-ACR1581-macosx-14-15-3") << UsbId(0x072F, 0x2301) << "ACS ACR1581 1S Dual Reader SAM" << "ACS ACR1581 1S Dual Reader SAM";

			QTest::newRow("HID OMNIKEY 5022-windows-10-11") << UsbId(0x076B, 0x5022) << "HID Global OMNIKEY 5022 Smart Card Reader 0" << "HID OMNIKEY 5022-CL";
			QTest::newRow("HID OMNIKEY 5022-macosx-12-15") << UsbId(0x076B, 0x5022) << "HID Global OMNIKEY 5022 Smart Card Reader" << "HID OMNIKEY 5022-CL";

			QTest::newRow("HID OMNIKEY 5422-windows-10-11-1") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY 5422 Smartcard Reader 0" << "HID Global OMNIKEY 5422 Smartcard Reader 0";
			QTest::newRow("HID OMNIKEY 5422-windows-10-11-2") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY 5422CL Smartcard Reader 0" << "HID OMNIKEY 5422";
			QTest::newRow("HID OMNIKEY 5422-macosx-12-15-1") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY Smartcard Reader (1)" << "HID OMNIKEY 5422";
			QTest::newRow("HID OMNIKEY 5422-macosx-12-15-2") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY Smartcard Reader (2)" << "HID OMNIKEY 5422";
			QTest::newRow("HID OMNIKEY 5422-macosx-12-15-3") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY Smartcard Reader(1)" << "HID OMNIKEY 5422";
			QTest::newRow("HID OMNIKEY 5422-macosx-12-15-4") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY Smartcard Reader(2)" << "HID OMNIKEY 5422";

			QTest::newRow("FEIG OBID myAXXESS RFID-Reader-windows-10-11") << UsbId(0x0AB1, 0x0003) << "FEIG ELECTRONIC GmbH OBID myAXXESS basic Slot:CL 358334430" << "OBID RFID-Reader";

			QTest::newRow("Gemalto-Prox-DU-windows-10-11-1") << UsbId(0x08E6, 0x5503) << "Gemalto Prox-DU Contactless_10900383 0" << "Gemalto Prox-DU HID";
			QTest::newRow("Gemalto-Prox-DU-windows-10-11-2") << UsbId(0x08E6, 0x5503) << "Gemalto Prox-DU Contact_10900383 0" << "Gemalto Prox-DU Contact_10900383 0";
			QTest::newRow("Gemalto-Prox-DU-windows-macosx-12-15-1") << UsbId(0x08E6, 0x5503) << "Gemalto Prox Dual USB PC Link Reader(1)" << "Gemalto Prox-DU HID";
			QTest::newRow("Gemalto-Prox-DU-windows-macosx-12-15-2") << UsbId(0x08E6, 0x5503) << "Gemalto Prox Dual USB PC Link Reader(2)" << "Gemalto Prox-DU HID";

			QTest::newRow("Gemalto-Prox-SU-windows-10-11-1") << UsbId(0x08E6, 0x5504) << "Gemalto Prox-SU Contactless_10800004 0" << "Gemalto Prox-SU Contactless";
			QTest::newRow("Gemalto-Prox-SU-windows-10-11-2") << UsbId(0x08E6, 0x5504) << "Gemalto Prox-SU Contact_10800004 0" << "Gemalto Prox-SU Contact_10800004 0";
			QTest::newRow("Gemalto-Prox-SU-windows-macosx-12-15-1") << UsbId(0x08E6, 0x5504) << "Gemalto Prox SU USB PC LinkReader(1)" << "Gemalto Prox-SU Contactless";
			QTest::newRow("Gemalto-Prox-SU-windows-macosx-12-15-2") << UsbId(0x08E6, 0x5504) << "Gemalto Prox SU USB PC LinkReader(2)" << "Gemalto Prox-SU Contactless";

			QTest::newRow("Identiv-SCL-3711-windows-10-11") << UsbId(0x04E6, 0x5591) << "SCM Microsystems SCL3711 reader & NFC device 0" << "Identiv SCL3711";
			QTest::newRow("Identiv-SCL-3711--F-macosx-12-15") << UsbId(0x04E6, 0x5591) << "SCL3711 Reader and NFC device" << "Identiv SCL3711";
			QTest::newRow("Identiv-SCL-3711--F-macosx-12-15") << UsbId(0x04E6, 0x5591) << "SCL3711 Reader and NFC device" << "Identiv SCL3711";

			QTest::newRow("Identiv-3700-F-windows-10-11") << UsbId(0x04E6, 0x5790) << "Identiv CLOUD 3700 F Contactless Reader 0" << "Identiv 3700 F";
			QTest::newRow("Identiv-3700-F-macosx-12-15") << UsbId(0x04E6, 0x5790) << "Identiv uTrust 3700 F CL Reader" << "Identiv 3700 F";
			QTest::newRow("Identiv-3700-F-generic-1") << UsbId(0x04E6, 0x5790) << "Identiv uTrust 3700 F CL Reader 0" << "Identiv 3700 F";
			QTest::newRow("Identiv-3700-F-generic-2") << UsbId(0x04E6, 0x5790) << "Identiv cloud 3700 F Contactless Reader 0" << "Identiv 3700 F";
			QTest::newRow("Identiv-3700-F-generic-3") << UsbId(0x04E6, 0x5790) << "Identiv uTrust 3700 F Contactless Reader 0" << "Identiv 3700 F";
			QTest::newRow("Identiv-3700-F-generic-4") << UsbId(0x04E6, 0x5790) << "Identiv CLOUD 3700 F CL Reader" << "Identiv 3700 F";
			QTest::newRow("Identiv-3700-F-generic-5") << UsbId(0x04E6, 0x5790) << "Identiv cloud 3700 F CL Reader" << "Identiv 3700 F";

			QTest::newRow("Identiv-3720-F-windows-10-11") << UsbId(0x04E6, 0x5612) << "Identiv uTrust 3720 Contactless Reader 0" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-macosx-12-15") << UsbId(0x04E6, 0x5612) << "Identiv uTrust 3720 Contactless Reader(1)" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-1") << UsbId(0x04E6, 0x5612) << "Identiv cloud 3720 Contactless Reader 0" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-2") << UsbId(0x04E6, 0x5612) << "Identiv cloud 3720 Contactless Reader(1)" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-3") << UsbId(0x04E6, 0x5612) << "Identiv cloud 3720 CL Reader 0" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-4") << UsbId(0x04E6, 0x5612) << "Identiv cloud 3720 CL Reader(1)" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-5") << UsbId(0x04E6, 0x5612) << "Identiv CLOUD 3720 Contactless Reader 0" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-6") << UsbId(0x04E6, 0x5612) << "Identiv CLOUD 3720 Contactless Reader(1)" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-7") << UsbId(0x04E6, 0x5612) << "Identiv CLOUD 3720 CL Reader 0" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-8") << UsbId(0x04E6, 0x5612) << "Identiv CLOUD 3720 CL Reader(1)" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-9") << UsbId(0x04E6, 0x5612) << "Identiv uTrust 3720 CL Reader 0" << "Identiv 3720 F";
			QTest::newRow("Identiv-3720-F-generic-10") << UsbId(0x04E6, 0x5612) << "Identiv uTrust 3720 CL Reader(1)" << "Identiv 3720 F";

			QTest::newRow("Identiv-uTrust-3721-F-windows-10-11") << UsbId(0x04E6, 0x5613) << "Identiv uTrust 3721 Contactless Reader 0" << "Identiv 3721 F";
			QTest::newRow("Identiv-uTrust-3721-F-macosx-12-15") << UsbId(0x04E6, 0x5613) << "Identiv uTrust 3721 Contactless Reader(1)" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-1") << UsbId(0x04E6, 0x5613) << "Identiv cloud 3721 Contactless Reader 0" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-2") << UsbId(0x04E6, 0x5613) << "Identiv cloud 3721 Contactless Reader(1)" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-3") << UsbId(0x04E6, 0x5613) << "Identiv cloud 3721 CL Reader 0" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-4") << UsbId(0x04E6, 0x5613) << "Identiv cloud 3721 CL Reader(1)" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-5") << UsbId(0x04E6, 0x5613) << "Identiv CLOUD 3721 Contactless Reader 0" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-6") << UsbId(0x04E6, 0x5613) << "Identiv CLOUD 3721 Contactless Reader(1)" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-7") << UsbId(0x04E6, 0x5613) << "Identiv CLOUD 3721 CL Reader 0" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-8") << UsbId(0x04E6, 0x5613) << "Identiv CLOUD 3721 CL Reader(1)" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-9") << UsbId(0x04E6, 0x5613) << "Identiv uTrust 3721 CL Reader 0" << "Identiv 3721 F";
			QTest::newRow("Identiv-3721-F-generic-10") << UsbId(0x04E6, 0x5613) << "Identiv uTrust 3721 CL Reader(1)" << "Identiv 3721 F";

			QTest::newRow("Identiv-Cloud-4700-F-windows-10-11-1") << UsbId(0x04E6, 0x5720) << "Identive CLOUD 4700 F Contact Reader 0" << "Identive CLOUD 4700 F Contact Reader 0";
			QTest::newRow("Identiv-Cloud-4700-F-windows-10-11-2") << UsbId(0x04E6, 0x5720) << "Identive CLOUD 4700 F Contactless Reader 0" << "Identiv Cloud 4700 F";
			QTest::newRow("Identiv-Cloud-4700-F-windows-10-11-3") << UsbId(0x04E6, 0x5720) << "Identive CLOUD 4700 F Contactless Reader 1" << "Identiv Cloud 4700 F";
			QTest::newRow("Identiv-Cloud-4700-F-macosx-12-15-1") << UsbId(0x04E6, 0x5720) << "Identiv uTrust 4700 F Dual Interface Reader(2)" << "Identiv Cloud 4700 F";
			QTest::newRow("Identiv-Cloud-4700-F-macosx-12-15-2") << UsbId(0x04E6, 0x5720) << "Identiv uTrust 4700 F Dual Interface Reader(1)" << "Identiv uTrust 4700 F Dual Interface Reader(1)";

			QTest::newRow("Identiv-4701-F-windows-10-11-1") << UsbId(0x04E6, 0x5724) << "Identiv CLOUD 4701 F Contact Reader 0" << "Identiv CLOUD 4701 F Contact Reader 0";
			QTest::newRow("Identiv-4701-F-windows-10-11-2") << UsbId(0x04E6, 0x5724) << "Identiv CLOUD 4701 F Contactless Reader 1" << "Identiv 4701 F";
			QTest::newRow("Identiv-4701-F-windows-10-11-3") << UsbId(0x04E6, 0x5724) << "Identiv CLOUD 4701 F Contactless Reader 0" << "Identiv 4701 F";
			QTest::newRow("Identiv-4701-F-windows-10-11-4") << UsbId(0x04E6, 0x5724) << "Identiv cloud 4701 F Contactless Reader 1" << "Identiv 4701 F";
			QTest::newRow("Identiv-4701-F-windows-10-11-5") << UsbId(0x04E6, 0x5724) << "Identiv cloud 4701 F Contactless Reader 0" << "Identiv 4701 F";
			QTest::newRow("Identiv-4701-F-windows-10-11-6") << UsbId(0x04E6, 0x5724) << "Identiv uTrust 4701 F CL Reader 0" << "Identiv 4701 F";
			QTest::newRow("Identiv-4701-F-macosx-12-15-1") << UsbId(0x04E6, 0x5724) << "Identiv uTrust 4701 F Dual Interface Reader(1)" << "Identiv 4701 F";
			QTest::newRow("Identiv-4701-F-macosx-12-15-2") << UsbId(0x04E6, 0x5724) << "Identiv uTrust 4701 F Dual Interface Reader(2)" << "Identiv 4701 F";
			QTest::newRow("Identiv-4701-F-macosx-12-15-3") << UsbId(0x04E6, 0x5724) << "Identiv uTrust 4701 F Dual Interface Reader" << "Identiv 4701 F";

			QTest::newRow("Cherry-sercure Board-windows-7-10-1") << UsbId(0x046A, 0x01A2) << "Cherry GmbH CHERRY SECURE BOARD 1.0 0" << "Cherry Secure Board 1.0";
			QTest::newRow("Cherry-sercure Board-macosx-10.13-11.0-1") << UsbId(0x046A, 0x01A2) << "Cherry GmbH CHERRY SECURE BOARD 1.0" << "Cherry Secure Board 1.0";

			QTest::newRow("Signotec Omega Pad-windows-7-10") << UsbId(0x2133, 0x010B) << "NXP PR533 0" << "Signotec Omega Pad";
			QTest::newRow("Signotec Omega Pad-macosx-10.13-11.0") << UsbId(0x2133, 0x010B) << "NXP PR533" << "Signotec Omega Pad";
		}


		void checkReaderPattern()
		{
			QFETCH(UsbId, usbId);
			QFETCH(QString, readerName);
			QFETCH(QString, readerViewName);

			mUsbIds += usbId;
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
			const ReaderConfigurationInfo readerSettingsInfo = ReaderConfigurationInfo("crap"_L1);
#else
			const ReaderConfigurationInfo readerSettingsInfo = Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo(readerName);
#endif
			QCOMPARE(readerSettingsInfo.getName(), readerViewName);
		}


		void uniqueNames()
		{
			QSet<QString> readerNames;
			for (const auto& readerSettingsInfo : std::as_const(Env::getSingleton<ReaderConfiguration>()->getReaderConfigurationInfos()))
			{
				readerNames += readerSettingsInfo.getName();
			}
			QCOMPARE(readerNames.size(), cCardReadersInConfigurationFile);
		}


		void checkOsVersion()
		{
			ResourceLoader::getInstance().init();
			QByteArray data = TestFileHelper::readFile(QStringLiteral(":/updatable-files/supported-readers.json"));
			QJsonParseError jsonError {};
			const QJsonDocument& json = QJsonDocument::fromJson(data, &jsonError);
			QVERIFY(jsonError.error == QJsonParseError::NoError);
			QJsonObject doc = json.object();
			const QJsonArray& array = doc["SupportedDevices"_L1].toArray();
			for (const QJsonValueConstRef entry : array)
			{
				QVERIFY(entry.isObject());
				auto object = entry.toObject();

				checkPlatformsMinMax(object, "Drivers"_L1);
				checkPlatformsMinMax(object, "Information"_L1);
			}
		}


};


const int test_ReaderConfiguration::cCardReadersInConfigurationFile = 27;

QTEST_GUILESS_MAIN(test_ReaderConfiguration)
#include "test_ReaderConfiguration.moc"
