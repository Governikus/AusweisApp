/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "FileRef.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(FileRef)
Q_DECLARE_METATYPE(FileRef::TYPE)


class test_FileRef
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void knownFiles_data()
		{
			QTest::addColumn<FileRef>("file");
			QTest::addColumn<FileRef::TYPE>("type");
			QTest::addColumn<QByteArray>("identifier");
			QTest::addColumn<QByteArray>("shortIdentifier");
			QTest::addColumn<QLatin1String>("name");

			QTest::newRow("masterFile") << FileRef::masterFile() << FileRef::TYPE::MASTER_FILE << QByteArray("3F00") << QByteArray() << "MasterFile"_L1;
			QTest::newRow("efDir") << FileRef::efDir() << FileRef::TYPE::ELEMENTARY_FILE << QByteArray("2F00") << QByteArray("1E") << "ElementaryFile"_L1;
			QTest::newRow("efCardAccess") << FileRef::efCardAccess() << FileRef::TYPE::ELEMENTARY_FILE << QByteArray("011C") << QByteArray("1C") << "ElementaryFile"_L1;
			QTest::newRow("efCardSecurity") << FileRef::efCardSecurity() << FileRef::TYPE::ELEMENTARY_FILE << QByteArray("011D") << QByteArray("1D") << "ElementaryFile"_L1;
			QTest::newRow("appCIA") << FileRef::appCIA() << FileRef::TYPE::APPLICATION << QByteArray("E828BD080FA000000167455349474E") << QByteArray() << "eSign CIA"_L1;
			QTest::newRow("appEId") << FileRef::appEId() << FileRef::TYPE::APPLICATION << QByteArray("E80704007F00070302") << QByteArray() << "eID"_L1;
			QTest::newRow("appESign") << FileRef::appESign() << FileRef::TYPE::APPLICATION << QByteArray("A000000167455349474E") << QByteArray() << "eSign"_L1;
			QTest::newRow("appPassport") << FileRef::appPassport() << FileRef::TYPE::APPLICATION << QByteArray("A0000002471001") << QByteArray() << "Passport"_L1;
			QTest::newRow("appPersosim") << FileRef::appPersosim() << FileRef::TYPE::APPLICATION << QByteArray("F04E66E75C02D8") << QByteArray() << "PersoSim"_L1;
		}


		void knownFiles()
		{
			QFETCH(FileRef, file);
			QFETCH(FileRef::TYPE, type);
			QFETCH(QByteArray, identifier);
			QFETCH(QByteArray, shortIdentifier);
			QFETCH(QLatin1String, name);

			QCOMPARE(file.getType(), type);
			QCOMPARE(file.getIdentifier(), QByteArray::fromHex(identifier));
			QCOMPARE(file.getShortIdentifier(), QByteArray::fromHex(shortIdentifier));
			QCOMPARE(file.getName(), name);
		}


		void unknownType()
		{
			QCOMPARE(FileRef(0x01, QByteArray()).getType(), FileRef::TYPE::UNKNOWN);
			QCOMPARE(FileRef(0x03, QByteArray()).getType(), FileRef::TYPE::UNKNOWN);
			QCOMPARE(FileRef(0x05, QByteArray()).getType(), FileRef::TYPE::UNKNOWN);
			QCOMPARE(FileRef(0xFF, QByteArray()).getType(), FileRef::TYPE::UNKNOWN);
		}


		void unknownName()
		{
			QCOMPARE(FileRef(FileRef::TYPE::APPLICATION, QByteArray()).getName(), "Unknown"_L1);
			QCOMPARE(FileRef(FileRef::TYPE::APPLICATION, QByteArray::fromHex("1234")).getName(), "Unknown"_L1);
			QCOMPARE(FileRef(FileRef::TYPE::APPLICATION, QByteArray::fromHex("ABCD")).getName(), "Unknown"_L1);
		}


};

QTEST_GUILESS_MAIN(test_FileRef)
#include "test_FileRef.moc"
