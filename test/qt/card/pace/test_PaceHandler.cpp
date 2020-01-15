/*!
 * \brief Tests for the PaceHandler
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "pace/PaceHandler.h"

#include "MockCardConnectionWorker.h"
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

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040101"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040102"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040103"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040104"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_GM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040201"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040301"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040302"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040303"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040304"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040401"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040402"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040403"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040404"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_RetryAllowed()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), mEfCardAccessBytes));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(reader.data()));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::RETRY_ALLOWED);
		}


		void establishPaceChannel_KeyAgreementRetryAllowed()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), mEfCardAccessBytes));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(reader.data()));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));

			worker->addResponse(CardReturnCode::UNKNOWN, QByteArray::fromHex("6A80"));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::RETRY_ALLOWED);
		}


		void establishPaceChannel_KeyAgreementCommandFailed()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), mEfCardAccessBytes));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(reader.data()));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));

			worker->addResponse(CardReturnCode::UNKNOWN, QByteArray::fromHex("6A80"));
			worker->addResponse(CardReturnCode::UNKNOWN, QByteArray::fromHex("9000"));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::COMMAND_FAILED);
		}


		// testcase TS_PACE_2.5.1c TR-03105
		void failureOnMseSetAt()
		{
			QVector<TransmitConfig> transmitConfigs;
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("9000"))); // Select file
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6A80"))); // MSE:Set AT
			QScopedPointer<MockReader> reader(MockReader::createMockReader(transmitConfigs, mEfCardAccessBytes));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			CardReturnCode status = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(status, CardReturnCode::PROTOCOL_ERROR);
		}


		void transmitMSESetAT_ErrorMseSetAT_UNKNOWN()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), mEfCardAccessBytes));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(reader.data()));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));

			QTest::ignoreMessage(QtCriticalMsg, "Error on MSE:Set AT | UNKNOWN");
			worker->addResponse(CardReturnCode::UNKNOWN, QByteArray::fromHex("6A80"));
			CardReturnCode status = paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN);

			QCOMPARE(status, CardReturnCode::UNKNOWN);
		}


		void transmitMSESetAT_OK()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), mEfCardAccessBytes));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(reader.data()));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02");

			auto paceInfo = PaceInfo::decode(bytes);
			paceHandler->mPaceInfo = paceInfo;
			QVERIFY(paceHandler->mPaceInfo != nullptr);

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("009000"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("009000"));
			CardReturnCode status = paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN);

			QCOMPARE(status, CardReturnCode::OK);
			QCOMPARE(paceHandler->getStatusMseSetAt(), QByteArray::fromHex("0090"));
		}


		void transmitMSESetAT_ErrorMseSetAT_PROTOCOL_ERROR()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), mEfCardAccessBytes));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(reader.data()));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02");

			auto paceInfo = PaceInfo::decode(bytes);
			paceHandler->mPaceInfo = paceInfo;

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("0090"));
			worker->addResponse(CardReturnCode::CANCELLATION_BY_USER, QByteArray::fromHex("0090"));
			QTest::ignoreMessage(QtCriticalMsg, "Error on MSE:Set AT");
			QCOMPARE(paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(paceHandler->getStatusMseSetAt(), QByteArray::fromHex("0090"));

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("0090"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("006A80"));
			QTest::ignoreMessage(QtCriticalMsg, "Error on MSE:Set AT");
			QCOMPARE(paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(paceHandler->getStatusMseSetAt(), QByteArray::fromHex("006A"));
		}


		void transmitMSESetAT_ErrorMseSetAT_RETRY_ALLOWED()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QVector<TransmitConfig>(), mEfCardAccessBytes));
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(reader.data()));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02");

			auto paceInfo = PaceInfo::decode(bytes);
			paceHandler->mPaceInfo = paceInfo;

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("0090"));
			worker->addResponse(CardReturnCode::UNDEFINED);
			QTest::ignoreMessage(QtCriticalMsg, "Error on MSE:Set AT");
			QCOMPARE(paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN), CardReturnCode::RETRY_ALLOWED);
		}


};

QTEST_GUILESS_MAIN(test_PaceHandler)
#include "test_PaceHandler.moc"
