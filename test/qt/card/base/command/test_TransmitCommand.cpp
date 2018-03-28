/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "command/TransmitCommand.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_TransmitCommand
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isAcceptable_data()
		{
			QTest::addColumn<QByteArrayList>("acceptable");
			QTest::addColumn<QByteArray>("response");
			QTest::addColumn<bool>("accepted");

			QTest::newRow("all-empty") << QByteArrayList({""}) << QByteArray("") << true;
			QTest::newRow("all-incomplete") << QByteArrayList({""}) << QByteArray("90") << true;
			QTest::newRow("all-code-only") << QByteArrayList({""}) << QByteArray("9000") << true;
			QTest::newRow("all-data+code") << QByteArrayList({""}) << QByteArray("abcd9000") << true;
			QTest::newRow("starting-with-90-empty") << QByteArrayList({"90"}) << QByteArray("") << false;
			QTest::newRow("starting-with-90-incomplete") << QByteArrayList({"90"}) << QByteArray("90") << true;
			QTest::newRow("starting-with-90-code-only") << QByteArrayList({"90"}) << QByteArray("9000") << true;
			QTest::newRow("starting-with-90-data+code") << QByteArrayList({"90"}) << QByteArray("abcd9000") << true;
			QTest::newRow("equal-to-9000-empty") << QByteArrayList({"9000"}) << QByteArray("") << false;
			QTest::newRow("equal-to-9000-incomplete") << QByteArrayList({"9000"}) << QByteArray("90") << false;
			QTest::newRow("equal-to-9000-code-only") << QByteArrayList({"9000"}) << QByteArray("9000") << true;
			QTest::newRow("equal-to-9000-data+code") << QByteArrayList({"9000"}) << QByteArray("abcd9000") << true;
			QTest::newRow("multi-empty") << QByteArrayList({"8000", "9000"}) << QByteArray("") << false;
			QTest::newRow("multi-code-only-wrong") << QByteArrayList({"8000", "9000"}) << QByteArray("7000") << false;
			QTest::newRow("multi-code-only-true-1") << QByteArrayList({"8000", "9000"}) << QByteArray("8000") << true;
			QTest::newRow("multi-code-only-true-2") << QByteArrayList({"8000", "9000"}) << QByteArray("9000") << true;
			QTest::newRow("multi-data+code-wrong") << QByteArrayList({"8000", "9000"}) << QByteArray("abcd7000") << false;
			QTest::newRow("multi-data+code-true-1") << QByteArrayList({"8000", "9000"}) << QByteArray("abcd8000") << true;
			QTest::newRow("multi-data+code-true-2") << QByteArrayList({"8000", "9000"}) << QByteArray("abcd9000") << true;
		}


		void isAcceptable()
		{
			QFETCH(QByteArrayList, acceptable);
			QFETCH(QByteArray, response);
			QFETCH(bool, accepted);

			InputAPDUInfo info;
			for (const auto& code : acceptable)
			{
				info.addAcceptableStatusCode(code);
			}
			ResponseApdu apdu(QByteArray::fromHex(response));
			QCOMPARE(TransmitCommand::isAcceptable(info, apdu), accepted);
		}


};

QTEST_GUILESS_MAIN(test_TransmitCommand)
#include "test_TransmitCommand.moc"
