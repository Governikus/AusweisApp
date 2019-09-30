/*!
 * \brief Unit tests for \ref ReaderConfiguration
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderConfiguration.h"

#include "Env.h"
#include "MockReaderDetector.h"
#include "ResourceLoader.h"

#include <QSharedPointer>
#include <QtCore>
#include <QtTest>


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

		QVector<UsbId> mUsbIds;
		MockReaderDetector mMockReaderDetector;

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
			const ReaderConfigurationInfo readerSettingsInfo = Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo("crap");
			QCOMPARE(readerSettingsInfo.getName(), QString("crap"));
			QCOMPARE(readerSettingsInfo.getIcon()->getName(), QString("default_reader.png"));
			QCOMPARE(readerSettingsInfo.getIconWithNPA()->getName(), QString("default_reader_mit_ausweis.png"));
			QCOMPARE(readerSettingsInfo.getPattern(), QString(""));
		}


		void checkReaderData_data()
		{
			QTest::addColumn<UsbId>("usbId");
			QTest::addColumn<QString>("readerName");
			QTest::addColumn<QString>("readerViewName");
			QTest::addColumn<QString>("readerIcon");
			QTest::addColumn<QString>("readerPattern");

			QTest::newRow("Remote card reader") << UsbId(0x0000, 0x0000) << "NFC-abcdef1234567890" << QStringLiteral("Smartphone als Kartenleser") << "img_RemoteReader" << "^NFC.*";

			QTest::newRow("REINER SCT cyberJack RFID komfort") << UsbId(0x0C4B, 0x0501) << "REINER SCT cyberJack RFID komfort" << "REINER SCT cyberJack RFID komfort" << "img_Reiner_SCT_cyberjack_RFID_komfort" << "REINER SCT cyberJack RFID komfort";
			QTest::newRow("REINER SCT cyberJack RFID standard") << UsbId(0x0C4B, 0x0500) << "REINER SCT cyberJack RFID standard" << "REINER SCT cyberJack RFID standard" << "img_Reiner_SCT_cyberjack_RFID_standard" << "REINER SCT cyberJack RFID standard";
			QTest::newRow("REINER SCT cyberJack RFID basis") << UsbId(0x0C4B, 0x9102) << "REINER SCT cyberJack RFID basis" << "REINER SCT cyberJack RFID basis" << "img_Reiner_SCT_cyberjack_RFID_basis" << "REINER SCT cyberJack RFID basis";
			QTest::newRow("REINER SCT cyberJack wave") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave" << "REINER SCT cyberJack wave" << "img_cyberjack_wave" << "REINER SCT cyberJack wave";

			QTest::newRow("KOBIL IDToken") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken" << "KOBIL IDToken" << "img_KOBIL_ID_Token" << "KOBIL (Systems )?IDToken";

			QTest::newRow("SCM SDI011") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Contactless Reader" << "SDI011 Contactless Reader" << "img_Identive_SDI011" << "SDI011 Contactless Reader|SDI011 USB Smart Card Reader(\\(1\\)|\\(2\\))";
			QTest::newRow("SCM SCL011") << UsbId(0x04E6, 0x5292) << "SCM Microsystems Inc. SCL011 Contactless Reader" << "SCL01x Contactless Reader" << "img_Identive_SCL011" << "^(SCM Microsystems Inc. )?SCL011 Contactless Reader( 0)?$";

			QTest::newRow("ACS-ACR1281U") << UsbId(0x072F, 0x0901) << "ACS ACR1281 PICC Reader" << "ACS ACR1281U" << "img_ACS_ACR1281U" << "ACS ACR1281 PICC Reader( 0)?";
			QTest::newRow("ACS-ACR1252U") << UsbId(0x072F, 0x223B) << "ACS ACR1252 1S CL Reader PICC 0" << "ACS ACR1252U" << "img_ACS_ACR1252U" << "ACS ACR1252 1S CL Reader PICC 0|ACS ACR1252 Dual Reader\\(2\\)";

			QTest::newRow("OMNIKEY 5021") << UsbId(0x076B, 0x5340) << "OMNIKEY CardMan 5x21-CL 0" << "OMNIKEY 5021-CL" << "img_HID_Omnikey_Mobile_Reader_502X_CL" << "OMNIKEY CardMan 5x21-CL 0|OMNIKEY CardMan \\(076B:5340\\) 5021 CL";
			QTest::newRow("OMNIKEY 5022") << UsbId(0x076B, 0x5022) << "HID Global OMNIKEY 5022 Smart Card Reader 0" << "OMNIKEY 5022-CL" << "img_HID_Omnikey_Mobile_Reader_502X_CL" << "HID Global OMNIKEY 5022 Smart Card Reader 0|HID Global OMNIKEY 5022 Smart Card Reader";
			QTest::newRow("OMNIKEY 5321 v2") << UsbId(0x076B, 0x5321) << "OOMNIKEY CardMan 5x21-CL 0" << "OMNIKEY 5321 v2" << "img_HID_Global_OMNIKEY_5321_V2" << R"(OMNIKEY CardMan 5x21-CL 0|OMNIKEY CardMan \(076B:5321\) 5321(\(1\)|\(2\)))";
			QTest::newRow("OMNIKEY 5421") << UsbId(0x076B, 0x5421) << "OMNIKEY CardMan 5x21-CL 0" << "OMNIKEY 5421" << "img_HID_Omnikey_542x" << R"(OMNIKEY CardMan 5x21-CL 0|OMNIKEY Smart Card Reader USB 0|OMNIKEY CardMan \(076B:5421\) 5421(\(1\)|\(2\)))";
			QTest::newRow("OMNIKEY 5422") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY 5422CL Smartcard Reader 0" << "OMNIKEY 5422" << "img_HID_Omnikey_542x" << R"(HID Global OMNIKEY 5422CL Smartcard Reader 0|HID Global OMNIKEY Smartcard Reader (\(1\)|\(2\)))";

			QTest::newRow("FEIG OBID myAXXESS RFID-Reader") << UsbId(0x0AB1, 0x0003) << "FEIG ELECTRONIC GmbH OBID myAXXESS basic Slot:CL 358334430" << "OBID RFID-Reader" << "img_FEIG_myAXXES_basic" << "FEIG ELECTRONIC GmbH OBID myAXXESS basic Slot:CL 358334430";

			QTest::newRow("Gemalto-Prox-DU") << UsbId(0x08E6, 0x5503) << "Gemalto Prox-DU Contactless_" << "Prox-DU HID" << "img_Gemalto_Prox_DU" << R"(Gemalto .*Prox(-DU| Dual)( Contactless_| USB PC Link(Reader| Reader)(\(2\)|\(1\))))";
			QTest::newRow("Gemalto-Prox-SU") << UsbId(0x08E6, 0x5504) << "Gemalto Prox-SU Contactless_" << "Gemalto Prox-SU Contactless" << "img_Gemalto_Prox_SU" << R"(Gemalto Prox( |-)SU( Contactless_| USB PC LinkReader(\(1\)|\(2\))))";

			QTest::newRow("Identiv-SCL-3711") << UsbId(0x04E6, 0x5591) << "SCM Microsystems SCL3711 reader & NFC device 0" << "Identiv SCL3711" << "img_Identive_SCL3711" << "(SCM Microsystems SCL3711 reader & NFC device 0|SCL3711 reader and NFC device)";
			QTest::newRow("Identiv-Cloud-3700-F") << UsbId(0x04E6, 0x5790) << "Identiv CLOUD 3700 F Contactless Reader 0" << "Identiv Cloud 3700 F" << "img_Identive_Cloud_3700_F" << "(Identiv CLOUD 3700 F Contactless Reader 0|Identiv uTrust 3700 F CL Reader)";
			QTest::newRow("Identiv-Cloud-4700-F") << UsbId(0x04E6, 0x5720) << "Identive CLOUD 4700 F Contactless Reader 1" << "Identiv Cloud 4700 F" << "img_Identive_Cloud_4700_F" << "(Identive CLOUD 4700 F Contactless Reader( 0| 1)|Identiv uTrust 4700 F Dual Interface Reader\\(2\\))";
			QTest::newRow("Identiv-Cloud-4701-F") << UsbId(0x04E6, 0x5724) << "Identiv CLOUD 4701 F Contactless Reader 0" << "Identiv Cloud 4701 F" << "img_Identive_Cloud_4701_F" << "(Identiv CLOUD 4701 F Contactless Reader( 0| 1)|Identiv uTrust 4701 F Dual Interface Reader)";

			QTest::newRow("Cherry-TC-1200-data") << UsbId(0x046A, 0x0091) << "Cherry TC 1200" << "Cherry TC-1200" << "img_Cherry_TC_1200" << "(Cherry TC 1200($|[^-])|TC 12xx-CL 0|Cherry SC Reader \\(046A:0091\\))";
			QTest::newRow("Cherry-TC-1300-data") << UsbId(0x046A, 0x0092) << "Cherry TC 1300" << "Cherry TC-1300" << "img_Cherry_TC_1300" << "(Cherry TC 1300|Cherry Smartcard Terminal TC 13xx-CL 0|Cherry SC Reader \\(046A:0092\\))";
			QTest::newRow("Cherry-ST-1275-data") << UsbId(0x046A, 0x0072) << "Cherry SmartTerminal XX7X-RF 0" << "Cherry ST-1275" << "img_Cherry_ST_1275" << "(Cherry ST-1275|Cherry SmartTerminal XX7X-RF 0)";

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
			const ReaderConfigurationInfo readerSettingsInfo = Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo(readerName);
			QCOMPARE(readerSettingsInfo.getName(), readerViewName);
			QCOMPARE(readerSettingsInfo.getIcon()->getName(), readerIcon + ".png");
			QCOMPARE(readerSettingsInfo.getIconWithNPA()->getName(), readerIcon + "_mit_ausweis.png");
			QCOMPARE(readerSettingsInfo.getPattern(), readerPattern);
		}


		void checkReaderPattern_data()
		{
			QTest::addColumn<UsbId>("usbId");
			QTest::addColumn<QString>("readerName");
			QTest::addColumn<QString>("readerViewName");
			QTest::addColumn<QString>("readerIcon");
			QTest::addColumn<QString>("readerPattern");

			QTest::newRow("UU") << UsbId(0xFFFF, 0xFFFF) << "crap" << "crap";

			QTest::newRow("Remote card reader") << UsbId(0x0000, 0x0000) << "NFC-abcdef1234567890" << QStringLiteral("Smartphone als Kartenleser");

			QTest::newRow("REINER SCT cyberJack RFID komfort-windows-7-10") << UsbId(0x0C4B, 0x0501) << "REINER SCT cyberJack RFID komfort USB 1" << "REINER SCT cyberJack RFID komfort";
			QTest::newRow("REINER SCT cyberJack RFID komfort-macosx-10.11-10.14") << UsbId(0x0C4B, 0x0501) << "REINER SCT cyberJack RFID komfort" << "REINER SCT cyberJack RFID komfort";

			QTest::newRow("REINER SCT cyberJack RFID standard-windows-7-10") << UsbId(0x0C4B, 0x0500) << "REINER SCT cyberJack RFID standard USB 1" << "REINER SCT cyberJack RFID standard";
			QTest::newRow("REINER SCT cyberJack RFID standard-macosx-10.11-10.14") << UsbId(0x0C4B, 0x0500) << "REINER SCT cyberJack RFID standard" << "REINER SCT cyberJack RFID standard";

			QTest::newRow("REINER SCT cyberJack RFID basis-windows-7-10") << UsbId(0x0C4B, 0x9102) << "REINER SCT cyberJack RFID basis 0" << "REINER SCT cyberJack RFID basis";
			QTest::newRow("REINER SCT cyberJack RFID basis-macosx-10.11-10.14") << UsbId(0x0C4B, 0x9102) << "REINER SCT cyberJack RFID basis" << "REINER SCT cyberJack RFID basis";

			QTest::newRow("REINER SCT cyberJack wave-windows-7-10-1") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave 0" << "REINER SCT cyberJack wave";
			QTest::newRow("REINER SCT cyberJack wave-windows-7-10-2") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave USB 1" << "REINER SCT cyberJack wave";
			QTest::newRow("REINER SCT cyberJack wave-macosx-10.11-10.14") << UsbId(0x0C4B, 0x0505) << "REINER SCT cyberJack wave" << "REINER SCT cyberJack wave";

			QTest::newRow("KOBIL IDToken-windows-7-10") << UsbId(0x0D46, 0x301D) << "KOBIL IDToken 0" << "KOBIL IDToken";
			QTest::newRow("KOBIL IDToken-macosx-10.11-10.14") << UsbId(0x0D46, 0x301D) << "KOBIL Systems IDToken" << "KOBIL IDToken";

			QTest::newRow("SCM SDI011-windows-7-10-1") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Contactless Reader 0" << "SDI011 Contactless Reader";
			QTest::newRow("SCM SDI011-windows-7-10-2") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Smart Card Reader 0" << "SCM Microsystems Inc. SDI011 Smart Card Reader 0";
			QTest::newRow("SCM SDI011-windows-7-10-3") << UsbId(0x04E6, 0x512B) << "SDI011 Contactless Reader" << "SDI011 Contactless Reader";
			QTest::newRow("SCM SDI011-macosx-10.11-10.14-1") << UsbId(0x04E6, 0x512B) << "SDI011 USB Smart Card Reader(1)" << "SDI011 Contactless Reader";
			QTest::newRow("SCM SDI011-macosx-10.11-10.14-2") << UsbId(0x04E6, 0x512B) << "SDI011 USB Smart Card Reader(2)" << "SDI011 Contactless Reader";
			QTest::newRow("SCM SDI011-macosx-10.10-10:14-3") << UsbId(0x04E6, 0x512B) << "SCM Microsystems Inc. SDI011 Contactless Reader(1)" << "SDI011 Contactless Reader";

			QTest::newRow("SCM SCL011-windows-7-10-1") << UsbId(0x04E6, 0x5292) << "SCL011 Contactless Reader" << "SCL01x Contactless Reader";
			QTest::newRow("SCM SCL011-windows-7-10-2") << UsbId(0x04E6, 0x5292) << "SCM Microsystems Inc. SCL011 Contactless Reader 0" << "SCL01x Contactless Reader";
			QTest::newRow("SCM SCL011-macosx-10.11-10.14-1") << UsbId(0x04E6, 0x5292) << "SCL011 Contactless Reader" << "SCL01x Contactless Reader";
			QTest::newRow("SCM SCL011-macosx-10.11-10-14-2") << UsbId(0x04E6, 0x5292) << "SCM Microsystems Inc. SCL011 Contactless Reader" << "SCL01x Contactless Reader";

			QTest::newRow("ACS-ACR1281U-windows-7-10") << UsbId(0x072F, 0x0901) << "ACS ACR1281 PICC Reader 0" << "ACS ACR1281U";
			QTest::newRow("ACS-ACR1281U-macosx-10.11-10.14") << UsbId(0x072F, 0x0901) << "ACS ACR1281 PICC Reader" << "ACS ACR1281U";

			QTest::newRow("ACS-ACR1252U-windows-7-10-1") << UsbId(0x072F, 0x223B) << "ACS ACR1252 1S CL Reader PICC 0" << "ACS ACR1252U";
			QTest::newRow("ACS-ACR1252U-windows-7-10-2") << UsbId(0x072F, 0x223B) << "ACS ACR1252 1S CL Reader SAM 0" << "ACS ACR1252 1S CL Reader SAM 0";
			QTest::newRow("ACS-ACR1252U-macosx-10.11-10.14-1") << UsbId(0x072F, 0x223B) << "ACS ACR1252 Dual Reader(1)" << "ACS ACR1252 Dual Reader(1)";
			QTest::newRow("ACS-ACR1252U-macosx-10.11-10.14-2") << UsbId(0x072F, 0x223B) << "ACS ACR1252 Dual Reader(2)" << "ACS ACR1252U";

			QTest::newRow("OMNIKEY 5021-windows-7-10") << UsbId(0x076B, 0x5340) << "OMNIKEY CardMan 5x21-CL 0" << "OMNIKEY 5021-CL";
			QTest::newRow("OMNIKEY 5021-macosx-10.11-10.14") << UsbId(0x076B, 0x5340) << "OMNIKEY CardMan (076B:5340) 5021 CL" << "OMNIKEY 5021-CL";

			QTest::newRow("OMNIKEY 5022-windows-7-10") << UsbId(0x076B, 0x5022) << "HID Global OMNIKEY 5022 Smart Card Reader 0" << "OMNIKEY 5022-CL";
			QTest::newRow("OMNIKEY 5022-macosx-10.11-10.14") << UsbId(0x076B, 0x5022) << "HID Global OMNIKEY 5022 Smart Card Reader" << "OMNIKEY 5022-CL";

			QTest::newRow("OMNIKEY 5321 v2-windows-7-10-1") << UsbId(0x076B, 0x5321) << "OMNIKEY CardMan 5x21 0" << "OMNIKEY CardMan 5x21 0";
			QTest::newRow("OMNIKEY 5321 v2-windows-7-10-2") << UsbId(0x076B, 0x5321) << "OMNIKEY CardMan 5x21-CL 0" << "OMNIKEY 5321 v2";
			QTest::newRow("OMNIKEY 5321 v2-macosx-10.11-10.11-1") << UsbId(0x076B, 0x5321) << "OMNIKEY CardMan (076B:5321) 5321(1)" << "OMNIKEY 5321 v2";
			QTest::newRow("OMNIKEY 5321 v2-macosx-10.11-10.14-2") << UsbId(0x076B, 0x5321) << "OMNIKEY CardMan (076B:5321) 5321(2)" << "OMNIKEY 5321 v2";

			QTest::newRow("OMNIKEY 5421-windows-7-10-1") << UsbId(0x076B, 0x5421) << "OMNIKEY CardMan 5x21 0" << "OMNIKEY CardMan 5x21 0";
			QTest::newRow("OMNIKEY 5421-windows-7-10-2") << UsbId(0x076B, 0x5421) << "OMNIKEY CardMan 5x21-CL 0" << "OMNIKEY 5421";
			QTest::newRow("OMNIKEY 5421-windows-7-10-3") << UsbId(0x076B, 0x5421) << "OMNIKEY Smart Card Reader USB 0" << "OMNIKEY 5421";
			QTest::newRow("OMNIKEY 5421-macosx-10.11-10.14-1") << UsbId(0x076B, 0x5421) << "OMNIKEY CardMan (076B:5421) 5421(1)" << "OMNIKEY 5421";
			QTest::newRow("OMNIKEY 5421-macosx-10.11-10.14-2") << UsbId(0x076B, 0x5421) << "OMNIKEY CardMan (076B:5421) 5421(2)" << "OMNIKEY 5421";

			QTest::newRow("OMNIKEY 5422-windows-7-10-1") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY 5422 Smartcard Reader 0" << "HID Global OMNIKEY 5422 Smartcard Reader 0";
			QTest::newRow("OMNIKEY 5422-windows-7-10-2") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY 5422CL Smartcard Reader 0" << "OMNIKEY 5422";
			QTest::newRow("OMNIKEY 5421-macosx-10.13-10.14-1") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY Smartcard Reader (1)" << "OMNIKEY 5422";
			QTest::newRow("OMNIKEY 5421-macosx-10.13-10.14-2") << UsbId(0x076B, 0x5422) << "HID Global OMNIKEY Smartcard Reader (2)" << "OMNIKEY 5422";

			QTest::newRow("FEIG OBID myAXXESS RFID-Reader-windows-7-10") << UsbId(0x0AB1, 0x0003) << "FEIG ELECTRONIC GmbH OBID myAXXESS basic Slot:CL 358334430" << "OBID RFID-Reader";

			QTest::newRow("Gemalto-Prox-DU-windows-7-10-1") << UsbId(0x08E6, 0x5503) << "Gemalto Prox-DU Contactless_10900383 0" << "Prox-DU HID";
			QTest::newRow("Gemalto-Prox-DU-windows-7-10-2") << UsbId(0x08E6, 0x5503) << "Gemalto Prox-DU Contact_10900383 0" << "Gemalto Prox-DU Contact_10900383 0";
			QTest::newRow("Gemalto-Prox-DU-windows-macosx-10.11-10.14-1") << UsbId(0x08E6, 0x5503) << "Gemalto Prox Dual USB PC Link Reader(1)" << "Prox-DU HID";
			QTest::newRow("Gemalto-Prox-DU-windows-macosx-10.11-10.14-2") << UsbId(0x08E6, 0x5503) << "Gemalto Prox Dual USB PC Link Reader(2)" << "Prox-DU HID";

			QTest::newRow("Gemalto-Prox-SU-windows-7-10-1") << UsbId(0x08E6, 0x5504) << "Gemalto Prox-SU Contactless_10800004 0" << "Gemalto Prox-SU Contactless";
			QTest::newRow("Gemalto-Prox-SU-windows-7-10-2") << UsbId(0x08E6, 0x5504) << "Gemalto Prox-SU Contact_10800004 0" << "Gemalto Prox-SU Contact_10800004 0";
			QTest::newRow("Gemalto-Prox-SU-windows-macosx-10.11-10.14-1") << UsbId(0x08E6, 0x5504) << "Gemalto Prox SU USB PC LinkReader(1)" << "Gemalto Prox-SU Contactless";
			QTest::newRow("Gemalto-Prox-SU-windows-macosx-10.11-10.14-2") << UsbId(0x08E6, 0x5504) << "Gemalto Prox SU USB PC LinkReader(2)" << "Gemalto Prox-SU Contactless";

			QTest::newRow("Identiv-SCL-3711-windows-7-10") << UsbId(0x04E6, 0x5591) << "SCM Microsystems SCL3711 reader & NFC device 0" << "Identiv SCL3711";

			QTest::newRow("Identiv-Cloud-3700-F-windows-7-10") << UsbId(0x04E6, 0x5790) << "Identiv CLOUD 3700 F Contactless Reader 0" << "Identiv Cloud 3700 F";
			QTest::newRow("Identiv-Cloud-3700-F-macosx-10.11-10.14") << UsbId(0x04E6, 0x5790) << "Identiv uTrust 3700 F CL Reader" << "Identiv Cloud 3700 F";

			QTest::newRow("Identiv-Cloud-4700-F-windows-7-10-1") << UsbId(0x04E6, 0x5720) << "Identive CLOUD 4700 F Contact Reader 0" << "Identive CLOUD 4700 F Contact Reader 0";
			QTest::newRow("Identiv-Cloud-4700-F-windows-7-10-2") << UsbId(0x04E6, 0x5720) << "Identive CLOUD 4700 F Contactless Reader 0" << "Identiv Cloud 4700 F";
			QTest::newRow("Identiv-Cloud-4700-F-windows-7-10-3") << UsbId(0x04E6, 0x5720) << "Identive CLOUD 4700 F Contactless Reader 1" << "Identiv Cloud 4700 F";
			QTest::newRow("Identiv-Cloud-4700-F-macosx-10.11-10.14-1") << UsbId(0x04E6, 0x5720) << "Identiv uTrust 4700 F Dual Interface Reader(2)" << "Identiv Cloud 4700 F";
			QTest::newRow("Identiv-Cloud-4700-F-macosx-10.11-10.14-2") << UsbId(0x04E6, 0x5720) << "Identiv uTrust 4700 F Dual Interface Reader(1)" << "Identiv uTrust 4700 F Dual Interface Reader(1)";

			QTest::newRow("Identiv-Cloud-4701-F-windows-7-10-1") << UsbId(0x04E6, 0x5724) << "Identiv CLOUD 4701 F Contact Reader 0" << "Identiv CLOUD 4701 F Contact Reader 0";
			QTest::newRow("Identiv-Cloud-4701-F-windows-7-10-2") << UsbId(0x04E6, 0x5724) << "Identiv CLOUD 4701 F Contactless Reader 1" << "Identiv Cloud 4701 F";
			QTest::newRow("Identiv-Cloud-4701-F-windows-7-10-3") << UsbId(0x04E6, 0x5724) << "Identiv CLOUD 4701 F Contactless Reader 0" << "Identiv Cloud 4701 F";
			QTest::newRow("Identiv-Cloud-4701-F-macosx-10.11-10.14-1") << UsbId(0x04E6, 0x5724) << "Identiv uTrust 4701 F Dual Interface Reader(1)" << "Identiv Cloud 4701 F";
			QTest::newRow("Identiv-Cloud-4701-F-macosx-10.11-10.14-2") << UsbId(0x04E6, 0x5724) << "Identiv uTrust 4701 F Dual Interface Reader(2)" << "Identiv Cloud 4701 F";

			QTest::newRow("Cherry-TC-1200-windows-7-10-1") << UsbId(0x046A, 0x0091) << "Cherry Smartcard Terminal TC 12xx-CL 0" << "Cherry TC-1200";
			QTest::newRow("Cherry-TC-1200-windows-10") << UsbId(0x046A, 0x0091) << "Cherry Smartcard Terminal TC 12xx 0" << "Cherry Smartcard Terminal TC 12xx 0";
			QTest::newRow("Cherry-TC-1200-macosx-10.13-10.14") << UsbId(0x046A, 0x0091) << "Cherry SC Reader (046A:0091)" << "Cherry TC-1200";
			QTest::newRow("Cherry-TC-1200-linux") << UsbId(0x046A, 0x0091) << "Cherry TC 1200" << "Cherry TC-1200";

			QTest::newRow("Cherry-TC-1300-windows-7-10-1") << UsbId(0x046A, 0x0092) << "Cherry Smartcard Terminal TC 13xx-CL 0" << "Cherry TC-1300";
			QTest::newRow("Cherry-TC-1300-windows-7-10-2") << UsbId(0x046A, 0x0092) << "Cherry Smartcard Terminal TC 13xx 0" << "Cherry Smartcard Terminal TC 13xx 0";
			QTest::newRow("Cherry-TC-1300-macosx-10.11-10.14-1") << UsbId(0x046A, 0x0092) << "Cherry SC Reader (046A:0092)(1)" << "Cherry TC-1300";
			QTest::newRow("Cherry-TC-1300-macosx-10.11-10.14-2") << UsbId(0x046A, 0x0092) << "Cherry SC Reader (046A:0092)(2)" << "Cherry TC-1300";
			QTest::newRow("Cherry-TC-1300-linux") << UsbId(0x046A, 0x0092) << "Cherry TC 1300" << "Cherry TC-1300";

			QTest::newRow("Cherry-ST-1275-windows-7-10-1") << UsbId(0x046A, 0x0072) << "Cherry SmartTerminal XX7X 0" << "Cherry SmartTerminal XX7X 0";
			QTest::newRow("Cherry-ST-1275-windows-7-10-2") << UsbId(0x046A, 0x0072) << "Cherry SmartTerminal XX7X-RF 0" << "Cherry ST-1275";

			QTest::newRow("Signotec Omega Pad-windows-7-10") << UsbId(0x2133, 0x010B) << "NXP PR533 0" << "Signotec Omega Pad";
			QTest::newRow("Signotec Omega Pad-macosx-10.11-10.14") << UsbId(0x2133, 0x010B) << "NXP PR533" << "Signotec Omega Pad";
		}


		void checkReaderPattern()
		{
			QFETCH(UsbId, usbId);
			QFETCH(QString, readerName);
			QFETCH(QString, readerViewName);

			mUsbIds += usbId;
			const ReaderConfigurationInfo readerSettingsInfo = Env::getSingleton<ReaderDetector>()->getReaderConfigurationInfo(readerName);
			QCOMPARE(readerSettingsInfo.getName(), readerViewName);
		}


		void uniqueNames()
		{
			QSet<QString> readerNames;
			for (const auto& readerSettingsInfo : qAsConst(Env::getSingleton<ReaderConfiguration>()->getReaderConfigurationInfos()))
			{
				readerNames += readerSettingsInfo.getName();
			}
			QCOMPARE(readerNames.size(), cCardReadersInConfigurationFile);
		}


		void forbidEmptyFields()
		{
			for (const auto& readerSettingsInfo : qAsConst(Env::getSingleton<ReaderConfiguration>()->getReaderConfigurationInfos()))
			{
				QVERIFY(!readerSettingsInfo.getName().isEmpty());
				QVERIFY(!readerSettingsInfo.getPattern().isEmpty());
				QVERIFY(!readerSettingsInfo.getIcon()->getName().isEmpty());
				QVERIFY(!readerSettingsInfo.getIconWithNPA()->getName().isEmpty());
			}
		}


};


const int test_ReaderConfiguration::cCardReadersInConfigurationFile = 26;

QTEST_GUILESS_MAIN(test_ReaderConfiguration)
#include "test_ReaderConfiguration.moc"
