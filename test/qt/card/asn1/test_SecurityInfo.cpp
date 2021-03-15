/*!
 * \brief Unit tests for \ref SecurityInfo
 *
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */


#include <QtCore>
#include <QtTest>

#include "asn1/KnownOIDs.h"
#include "asn1/SecurityInfo.h"


using namespace governikus;


class test_SecurityInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseCrap()
		{
			QVERIFY(SecurityInfo::decode("3003020101") == nullptr);
		}


		void missingOid()
		{
			QByteArray bytes = QByteArray::fromHex("30 06"
												   "            02 01 02"
												   "            02 01 08");

			QVERIFY(SecurityInfo::decode(bytes) == nullptr);
		}


		void missingRequiredData()
		{
			QByteArray bytes = QByteArray::fromHex("30 0C"
												   "            06 0A 04007F00070202040202");

			QVERIFY(SecurityInfo::decode(bytes) == nullptr);
		}


		void withoutOptionalData()
		{
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02");

			auto securityInfo = SecurityInfo::decode(bytes);

			QVERIFY(securityInfo != nullptr);
			QCOMPARE(securityInfo->getProtocol(), QByteArray("0.4.0.127.0.7.2.2.4.2.2"));
		}


		void withOptionalData()
		{
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02"
												   "            02 01 08");

			auto securityInfo = SecurityInfo::decode(bytes);

			QVERIFY(securityInfo != nullptr);
			QCOMPARE(securityInfo->getProtocol(), QByteArray("0.4.0.127.0.7.2.2.4.2.2"));
		}


		void getProtocol()
		{
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02"
												   "            02 01 08");

			auto securityInfo = SecurityInfo::decode(bytes);

			QVERIFY(securityInfo != nullptr);
			QCOMPARE(securityInfo->getProtocol(), QByteArray("0.4.0.127.0.7.2.2.4.2.2"));
			QCOMPARE(securityInfo->getProtocolValueBytes(), QByteArray::fromHex("04007F00070202040202"));
		}


};

QTEST_GUILESS_MAIN(test_SecurityInfo)
#include "test_SecurityInfo.moc"
