/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ApplicationTemplate.h"

#include "FileRef.h"
#include "asn1/ASN1Util.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_ApplicationTemplate
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseCrap()
		{
			QVERIFY(ApplicationTemplate::decode("3003020101") == nullptr);
		}


		void testSingleEidAid()
		{
			QByteArray bytes = QByteArray::fromHex("610B4F09E80704007F00070302");

			auto applicationTemplate = ApplicationTemplate::decode(bytes);
			QVERIFY(!applicationTemplate.isNull());
			auto aid = applicationTemplate->getAid();
			QCOMPARE(aid, FileRef::appEId().getIdentifier());
		}


		void testAppCIA()
		{
			QByteArray bytes = QByteArray::fromHex("61324F0FE828BD080FA000000167455349474E500F434941207A752044462E655369676E5100730C4F0AA000000167455349474E");

			auto applicationTemplate = ApplicationTemplate::decode(bytes);
			QVERIFY(!applicationTemplate.isNull());
			QCOMPARE(applicationTemplate->getAid(), FileRef::appCIA().getIdentifier());
			auto label = Asn1StringUtil::getValue(applicationTemplate->mApplicationLabel);
			QCOMPARE(label, QLatin1String("CIA zu DF.eSign"));
			auto fileRef = Asn1OctetStringUtil::getValue(applicationTemplate->mFileReference);
			QCOMPARE(fileRef, QByteArray());
			auto discretionaryData = Asn1OctetStringUtil::getValue(applicationTemplate->mDiscretionaryData);
			QCOMPARE(discretionaryData, FileRef::appESign().getIdentifier());
		}


		void testAppPassport()
		{
			QByteArray bytes = QByteArray::fromHex("61094F07A0000002471001");

			auto applicationTemplate = ApplicationTemplate::decode(bytes);
			QVERIFY(!applicationTemplate.isNull());
			QCOMPARE(applicationTemplate->getAid(), FileRef::appPassport().getIdentifier());
		}


		void testAppEId()
		{
			QByteArray bytes = QByteArray::fromHex("610B4F09E80704007F00070302");

			auto applicationTemplate = ApplicationTemplate::decode(bytes);
			QVERIFY(!applicationTemplate.isNull());
			QCOMPARE(applicationTemplate->getAid(), FileRef::appEId().getIdentifier());
		}


		void testAppESign()
		{
			QByteArray bytes = QByteArray::fromHex("610C4F0AA000000167455349474E");

			auto applicationTemplate = ApplicationTemplate::decode(bytes);
			QVERIFY(!applicationTemplate.isNull());
			QCOMPARE(applicationTemplate->getAid(), FileRef::appESign().getIdentifier());
		}


};

QTEST_GUILESS_MAIN(test_ApplicationTemplate)
#include "test_ApplicationTemplate.moc"
