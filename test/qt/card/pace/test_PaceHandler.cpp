/*!
 * \brief Tests for the PaceHandler
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "pace/PaceHandler.h"

#include "MockReader.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_PaceHandler
	: public QObject
{
	Q_OBJECT
	QByteArray mEfCardAccessBytes;

	private Q_SLOTS:
		void initTestCase()
		{
			mEfCardAccessBytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/efCardAccess.hex"));
		}


		void getPaceProtocol_uninitiaized()
		{
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(QSharedPointer<CardConnectionWorker>()));
			QCOMPARE(paceHandler->getPaceProtocol(), QByteArray(""));
		}


		void getPaceProtocol()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), mEfCardAccessBytes));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			paceHandler->initialize(reader->getReaderInfo().getCardInfo().getEfCardAccess());

			QCOMPARE(paceHandler->getPaceProtocol(), QByteArray("0.4.0.127.0.7.2.2.4.2.2"));
		}


		void establishPaceChannel_getEfCardAccessFailed()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader());
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040101"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040102"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040103"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040104"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_GM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040201"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040301"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040302"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040303"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040304"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040401"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040402"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040403"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040404"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		// testcase TS_PACE_2.5.1c TR-03105
		void failureOnMseSetAt()
		{
			QVector<TransmitConfig> transmitConfigs;
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("9000"))); // Select file
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6A80"))); // MSE:Set AT
			QScopedPointer<MockReader> reader(MockReader::createMockReader(transmitConfigs, mEfCardAccessBytes));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PACE_PASSWORD_ID::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


};

QTEST_GUILESS_MAIN(test_PaceHandler)
#include "test_PaceHandler.moc"
