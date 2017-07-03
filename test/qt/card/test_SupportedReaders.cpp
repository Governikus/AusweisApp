/*!
 * \brief Unit tests for \ref SupportedReaders
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "SupportedReaders.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;


class test_SupportedReaders
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isOnWhiteList()
		{
			const SupportedReaders& reader = SupportedReaders::getInstance();
			QVERIFY(reader.isOnWhiteList("REINER SCT cyberJack RFID komfort"));
			QVERIFY(reader.isOnWhiteList("SCM Microsystems Inc. SCL011 Contactless Reader"));

			QVERIFY(!reader.isOnWhiteList("crap"));
		}


		void getPattern()
		{
			const SupportedReaders& reader = SupportedReaders::getInstance();
			QCOMPARE(reader.getPattern(ReaderType::REINER_cyberJack_RFID_komfort),
					QString("REINER SCT cyberJack RFID komfort"));
			QCOMPARE(reader.getPattern(ReaderType::REINER_cyberJack_RFID_basis), QString("REINER SCT cyberJack RFID basis"));
			QCOMPARE(reader.getPattern(ReaderType::SCM_SCL011_Contactless_Reader),
					QString("(SCM Microsystems Inc. )?SCL011 Contactless Reader"));
		}


		void getReader()
		{
			const SupportedReaders& reader = SupportedReaders::getInstance();

			QCOMPARE(ReaderType::REINER_cyberJack_RFID_komfort, reader.getReader("REINER SCT cyberJack RFID komfort"));
			QCOMPARE(ReaderType::REINER_cyberJack_RFID_standard, reader.getReader("REINER SCT cyberJack RFID standard"));
			QCOMPARE(ReaderType::REINER_cyberJack_RFID_basis, reader.getReader("REINER SCT cyberJack RFID basis"));
			QCOMPARE(ReaderType::REINER_cyberJack_wave, reader.getReader("REINER SCT cyberJack wave"));

			QCOMPARE(ReaderType::KOBIL_IDToken, reader.getReader("KOBIL Systems IDToken"));
			QCOMPARE(ReaderType::KOBIL_IDToken, reader.getReader("KOBIL IDToken"));

			QCOMPARE(ReaderType::SCM_SDI010, reader.getReader("SDI010 USB Contactless Reader"));
			QCOMPARE(ReaderType::SCM_SDI010, reader.getReader("SCM Microsystems Inc. SDI010 Smart Card Reader 0"));
			QCOMPARE(ReaderType::SCM_SDI010, reader.getReader("SDI010 USB Smart Card Reader [Vendor Interface] (21120837205702) 00 00"));
			QCOMPARE(ReaderType::SCM_SDI010, reader.getReader("SCM Microsystems Inc. SDI010 Contactless Reader 0"));
			QCOMPARE(ReaderType::SCM_SDI010, reader.getReader("SDI010 USB Contactless Reader"));

			QCOMPARE(ReaderType::SCM_SDI011, reader.getReader("SDI011 USB Contactless Reader"));

			QCOMPARE(ReaderType::SCM_SCL011_Contactless_Reader, reader.getReader("SCM Microsystems Inc. SCL011 Contactless Reader"));
			QCOMPARE(ReaderType::SCM_SCL011_Contactless_Reader, reader.getReader("SCL011 Contactless Reader"));

			QCOMPARE(ReaderType::ACS_ACR1281_PICC_Reader, reader.getReader("ACS ACR1281 PICC Reader"));

			QCOMPARE(ReaderType::OMNIKEY_CardMan_5x21_CL, reader.getReader("OMNIKEY CardMan 5x21"));
			QCOMPARE(ReaderType::OMNIKEY_CardMan_5x21_CL, reader.getReader("OMNIKEY CardMan 5321"));

			QCOMPARE(ReaderType::FEIG_OBID_myAXXESS_basic, reader.getReader("FEIG ELECTRONIC GmbH OBID myAXXESS basic"));

			QCOMPARE(ReaderType::Gemalto_Prox_SU, reader.getReader("Gemalto Prox SU"));
			QCOMPARE(ReaderType::Gemalto_Prox_SU, reader.getReader("Gemalto Prox-SU"));

			QCOMPARE(ReaderType::Gemalto_Prox_DU, reader.getReader("Gemalto Prox-DU"));
			QCOMPARE(ReaderType::Gemalto_Prox_DU, reader.getReader("Gemalto Prox Dual"));

			QCOMPARE(ReaderType::UNKNOWN, reader.getReader("crap"));
		}


		void consistency()
		{
			const SupportedReaders& reader = SupportedReaders::getInstance();
			for (ReaderType readerType : Enum<ReaderType>::getList())
			{
				if (readerType != ReaderType::UNKNOWN)
				{
					QVERIFY(!reader.getPattern(readerType).isEmpty());
				}
			}
		}


};

QTEST_GUILESS_MAIN(test_SupportedReaders)
#include "test_SupportedReaders.moc"
