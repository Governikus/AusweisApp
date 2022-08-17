/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "pinpad/LengthValue.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_LengthValue
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void empty()
		{
			int offset = 0;
			const auto result = LengthValue::readByteArray<quint16>(QByteArray(), offset);

			QCOMPARE(offset, 0);
			QCOMPARE(result, QByteArray());
		}


		void tooShort()
		{
			int offset = 0;
			const auto result = LengthValue::readByteArray<quint16>(QByteArray::fromHex("00"), offset);

			QCOMPARE(offset, 0);
			QCOMPARE(result, QByteArray());
		}


		void zeroLength()
		{
			int offset = 0;
			const auto result = LengthValue::readByteArray<quint8>(QByteArray::fromHex("00"), offset);

			QCOMPARE(offset, 1);
			QCOMPARE(result, QByteArray());
		}


		void missingValue()
		{
			int offset = 0;
			const auto result = LengthValue::readByteArray<quint8>(QByteArray::fromHex("02"), offset);

			QCOMPARE(offset, 3);
			QCOMPARE(result, QByteArray());
		}


		void singleValue()
		{
			int offset = 0;
			const auto result = LengthValue::readByteArray<quint8>(QByteArray::fromHex("029000"), offset);

			QCOMPARE(offset, 3);
			QCOMPARE(result, QByteArray::fromHex("9000"));
		}


		void doubleValue()
		{
			int offset = 0;
			const QByteArray value = QByteArray::fromHex("029000026300");

			auto result = LengthValue::readByteArray<quint8>(value, offset);
			QCOMPARE(offset, 3);
			QCOMPARE(result, QByteArray::fromHex("9000"));

			result = LengthValue::readByteArray<quint8>(value, offset);
			QCOMPARE(offset, 6);
			QCOMPARE(result, QByteArray::fromHex("6300"));
		}


		void zeroLengthIncluded()
		{
			int offset = 0;
			const QByteArray value = QByteArray::fromHex("02900000026300");

			auto result = LengthValue::readByteArray<quint8>(value, offset);
			QCOMPARE(offset, 3);
			QCOMPARE(result, QByteArray::fromHex("9000"));

			result = LengthValue::readByteArray<quint8>(value, offset);
			QCOMPARE(offset, 4);
			QCOMPARE(result, QByteArray());

			result = LengthValue::readByteArray<quint8>(value, offset);
			QCOMPARE(offset, 7);
			QCOMPARE(result, QByteArray::fromHex("6300"));
		}


		void length16Bit()
		{
			int offset = 0;
			const auto result = LengthValue::readByteArray<quint16>(QByteArray::fromHex("02009000"), offset);

			QCOMPARE(offset, 4);
			QCOMPARE(result, QByteArray::fromHex("9000"));
		}


		void write()
		{
			QByteArray output;

			LengthValue::writeByteArray<quint8>(QByteArray::fromHex("9000"), output);
			QCOMPARE(output, QByteArray::fromHex("029000"));

			LengthValue::writeByteArray<quint8>(QByteArray(), output);
			QCOMPARE(output, QByteArray::fromHex("02900000"));

			LengthValue::writeByteArray<quint8>(QByteArray::fromHex("6300"), output);
			QCOMPARE(output, QByteArray::fromHex("02900000026300"));

			LengthValue::writeByteArray<quint16>(QByteArray::fromHex("0000"), output);
			QCOMPARE(output, QByteArray::fromHex("0290000002630002000000"));
		}


		void writeInvalidLength()
		{
			QByteArray output;
			QByteArray input(256, 'A');

			LengthValue::writeByteArray<quint8>(input, output);
			QCOMPARE(output.size(), 256);

			QByteArray expected;
			expected += QByteArray::fromHex("FF");
			expected += QByteArray(255, 'A');
			QCOMPARE(output, expected);
		}


};

QTEST_GUILESS_MAIN(test_LengthValue)
#include "test_LengthValue.moc"
