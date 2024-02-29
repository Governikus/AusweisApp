/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Tests for the class FileCommand.
 */

#include "apdu/FileCommand.h"

#include "LogHandler.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(FileRef)


class test_FileCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void test_data()
		{
			QTest::addColumn<FileRef>("fileRef");
			QTest::addColumn<int>("offset");
			QTest::addColumn<int>("le");
			QTest::addColumn<QByteArray>("buffer");

			QTest::newRow("Elementary file without offset and expected length") << FileRef::efCardAccess() << 0 << 0 << QByteArray::fromHex("00a4020c02011c");
			QTest::newRow("Elementary file with offset") << FileRef::efCardAccess() << 2 << 0 << QByteArray::fromHex("00a4020c02011c");
			QTest::newRow("Elementary file with expected length") << FileRef::efCardAccess() << 0 << 3 << QByteArray::fromHex("00b09c0003");
			QTest::newRow("Elementary file with offset and expected length") << FileRef::efCardAccess() << 2 << 3 << QByteArray::fromHex("00b0000203");

			QTest::newRow("Master file without offset and expected length") << FileRef::masterFile() << 0 << 0 << QByteArray::fromHex("00a4000c023f00");
			QTest::newRow("Master file with offset") << FileRef::masterFile() << 2 << 0 << QByteArray::fromHex("00a4000c023f00");
			QTest::newRow("Master file with expected length") << FileRef::masterFile() << 0 << 3 << QByteArray::fromHex("00b0000003");
			QTest::newRow("Master file with offset and expected length") << FileRef::masterFile() << 2 << 3 << QByteArray::fromHex("00b0000203");

			QTest::newRow("Application without offset and expected length") << FileRef::appEId() << 0 << 0 << QByteArray::fromHex("00a4040c09e80704007f00070302");
			QTest::newRow("Application with offset") << FileRef::appEId() << 2 << 0 << QByteArray::fromHex("00a4040c09e80704007f00070302");
			QTest::newRow("Application with expected length") << FileRef::appEId() << 0 << 3 << QByteArray::fromHex("00b0000003");
			QTest::newRow("Application with offset and expected length") << FileRef::appEId() << 2 << 3 << QByteArray::fromHex("00b0000203");

			QTest::newRow("Unknown without offset and expected length") << FileRef(8, QByteArray("1234")) << 0 << 0 << QByteArray::fromHex("00a4ff0c0431323334");
			QTest::newRow("Unknown with offset") << FileRef(8, QByteArray("1234")) << 2 << 0 << QByteArray::fromHex("00a4ff0c0431323334");
			QTest::newRow("Unknown with expected length") << FileRef(8, QByteArray("1234")) << 0 << 3 << QByteArray::fromHex("00b0000003");
			QTest::newRow("Unknown with offset and expected length") << FileRef(8, QByteArray("1234")) << 2 << 3 << QByteArray::fromHex("00b0000203");
		}


		void test()
		{
			QFETCH(FileRef, fileRef);
			QFETCH(int, offset);
			QFETCH(int, le);
			QFETCH(QByteArray, buffer);

			const FileCommand fileCmd(fileRef, offset, le);
			QCOMPARE(fileCmd.getFileRef().getType(), fileRef.getType());
			QCOMPARE(fileCmd.getFileRef().getIdentifier(), fileRef.getIdentifier());
			QCOMPARE(fileCmd.getOffset(), offset);
			QCOMPARE(fileCmd.getLe(), le);

			const CommandApdu commandApdu(fileCmd);
			QCOMPARE(commandApdu, buffer);

			const FileCommand parsed(commandApdu);
			const bool fileRefExpected = le == 0 || (offset == 0 && fileRef.getType() == FileRef::ELEMENTARY_FILE);
			QCOMPARE(parsed.getFileRef().getType(), fileRefExpected ? fileRef.getType() : FileRef::UNKNOWN);
			QCOMPARE(parsed.getFileRef().getIdentifier(), fileRefExpected && le == 0 ? fileRef.getIdentifier() : QByteArray());
			QCOMPARE(parsed.getFileRef().getShortIdentifier(), fileRefExpected && le != 0 ? fileRef.getShortIdentifier() : QByteArray());
			QCOMPARE(parsed.getOffset(), le == 0 ? 0 : offset);
			QCOMPARE(parsed.getLe(), commandApdu.getINS() == Ins::SELECT ? 0 : le);
		}


};

QTEST_GUILESS_MAIN(test_FileCommand)
#include "test_FileCommand.moc"
