/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ApplicationTemplates.h"

#include "FileRef.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_ApplicationTemplates
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseCrap()
		{
			QVERIFY(ApplicationTemplates::decode("3003020101") == nullptr);
		}


		void testStandardEfDir()
		{
			QByteArray bytes = QByteArray::fromHex("61324F0FE828BD080FA000000167455349474E500F434941207A752044462E655369676E5100730C4F0AA000000167455349474E61094F07A0000002471001610B4F09E80704007F00070302610C4F0AA000000167455349474E");

			auto applicationTemplates = ApplicationTemplates::decode(bytes);
			QVERIFY(!applicationTemplates.isNull());
			QVERIFY(applicationTemplates->contains(FileRef::appEId().getIdentifier()));
			QVERIFY(applicationTemplates->contains(FileRef::appPassport().getIdentifier()));
			QVERIFY(applicationTemplates->contains(FileRef::appESign().getIdentifier()));
			QVERIFY(applicationTemplates->contains(FileRef::appCIA().getIdentifier()));
		}


};

QTEST_GUILESS_MAIN(test_ApplicationTemplates)
#include "test_ApplicationTemplates.moc"
