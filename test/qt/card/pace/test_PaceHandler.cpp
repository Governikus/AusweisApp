/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "pace/PaceHandler.h"

#include "MockCardConnectionWorker.h"
#include "MockReader.h"
#include "TestFileHelper.h"
#include "asn1/Oid.h"

#include <QPointer>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_PaceHandler
	: public QObject
{
	Q_OBJECT
	QByteArray mEfCardAccessBytes;

	private Q_SLOTS:
		void initTestCase()
		{
			mEfCardAccessBytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/efCardAccess.hex"_L1));
		}


		void getPaceProtocol_uninitialized()
		{
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(QSharedPointer<CardConnectionWorker>()));
			QVERIFY(paceHandler->getPaceProtocol().getOid().isUndefined());
		}


		void getPaceProtocol()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), mEfCardAccessBytes));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			paceHandler->initialize(reader->getReaderInfo().getCardInfo().getEfCardAccess());

			QCOMPARE(paceHandler->getPaceProtocol(), SecurityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128));
		}


		void establishPaceChannel_getEfCardAccessFailed()
		{
			QScopedPointer<MockReader> reader(MockReader::createMockReader());
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040101"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040102"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040103"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_GM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040104"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_GM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040201"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040301"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040302"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040303"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_DH_IM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040304"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_3DES_CBC_CBC_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040401"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_128_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040402"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_192_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040403"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_id_PACE_ECDH_IM_AES_CBC_CMAC_256_unsupported()
		{
			QByteArray efCardAccess = QByteArray::fromHex(mEfCardAccessBytes.toHex().replace("04007f00070202040202", "04007f00070202040404"));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(QList<TransmitConfig>(), efCardAccess));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void establishPaceChannel_RetryAllowed()
		{
			QPointer<MockReader> reader = MockReader::createMockReader(QList<TransmitConfig>(), mEfCardAccessBytes);
			const auto& worker = MockCardConnectionWorker::create(reader);
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::RESPONSE_EMPTY);
		}


		void establishPaceChannel_KeyAgreementRetryAllowed()
		{
			QPointer<MockReader> reader = MockReader::createMockReader(QList<TransmitConfig>(), mEfCardAccessBytes);
			const auto& worker = MockCardConnectionWorker::create(reader);
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::RESPONSE_EMPTY);
		}


		// testcase TS_PACE_2.5.1c TR-03105
		void failureOnMseSetAt()
		{
			QList<TransmitConfig> transmitConfigs;
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("9000"))); // Select file
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6A80"))); // MSE:Set AT
			QScopedPointer<MockReader> reader(MockReader::createMockReader(transmitConfigs, mEfCardAccessBytes));
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(reader->createCardConnectionWorker()));

			const auto& output = paceHandler->establishPaceChannel(PacePasswordId::PACE_PIN, "123456");

			QCOMPARE(output.getReturnCode(), CardReturnCode::PROTOCOL_ERROR);
		}


		void transmitMSESetAT_OK()
		{
			QPointer<MockReader> reader = MockReader::createMockReader(QList<TransmitConfig>(), mEfCardAccessBytes);
			const auto& worker = MockCardConnectionWorker::create(reader);
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02");

			auto paceInfo = PaceInfo::decode(bytes);
			paceHandler->mPaceInfo = paceInfo;
			QVERIFY(paceHandler->mPaceInfo != nullptr);

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("009000"));
			QCOMPARE(paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN), CardReturnCode::OK);
			QCOMPARE(paceHandler->mStatusMseSetAt, QByteArray::fromHex("9000"));

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			QCOMPARE(paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN), CardReturnCode::OK);
			QCOMPARE(paceHandler->mStatusMseSetAt, QByteArray::fromHex("9000"));
		}


		void transmitMSESetAT_ErrorMseSetAT_PROTOCOL_ERROR()
		{
			QPointer<MockReader> reader = MockReader::createMockReader(QList<TransmitConfig>(), mEfCardAccessBytes);
			const auto& worker = MockCardConnectionWorker::create(reader);
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02");

			auto paceInfo = PaceInfo::decode(bytes);
			paceHandler->mPaceInfo = paceInfo;

			worker->addResponse(CardReturnCode::CANCELLATION_BY_USER, QByteArray::fromHex("0090"));
			QTest::ignoreMessage(QtCriticalMsg, "Error on MSE:Set AT");
			QCOMPARE(paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(paceHandler->mStatusMseSetAt, QByteArray::fromHex("0090"));

			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("006A"));
			QTest::ignoreMessage(QtCriticalMsg, "Error on MSE:Set AT");
			QCOMPARE(paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(paceHandler->mStatusMseSetAt, QByteArray::fromHex("006A"));
		}


		void transmitMSESetAT_ErrorMseSetAT_RETRY_ALLOWED()
		{
			QPointer<MockReader> reader = MockReader::createMockReader(QList<TransmitConfig>(), mEfCardAccessBytes);
			const auto& worker = MockCardConnectionWorker::create(reader);
			QScopedPointer<PaceHandler> paceHandler(new PaceHandler(worker));
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02");

			auto paceInfo = PaceInfo::decode(bytes);
			paceHandler->mPaceInfo = paceInfo;

			worker->addResponse(CardReturnCode::UNDEFINED);
			QTest::ignoreMessage(QtCriticalMsg, "Error on MSE:Set AT");
			QCOMPARE(paceHandler->transmitMSESetAT(PacePasswordId::PACE_PIN), CardReturnCode::RESPONSE_EMPTY);
		}


		void fullRun_data()
		{
			QTest::addColumn<QByteArray>("mseSetAt");
			QTest::addColumn<QByteArray>("response");
			QTest::addColumn<PacePasswordId>("passwordId");
			QTest::addColumn<CardReturnCode>("returnCode");

			QTest::newRow("empty") << QByteArray::fromHex(("9000")) << QByteArray() << PacePasswordId::PACE_PIN << CardReturnCode::RESPONSE_EMPTY;
			QTest::newRow("failed") << QByteArray::fromHex(("9000")) << QByteArray::fromHex(("7c0a860832c7e8df7c3cee579000")) << PacePasswordId::PACE_PIN << CardReturnCode::PROTOCOL_ERROR;
			QTest::newRow("failedRc2") << QByteArray::fromHex(("9000")) << QByteArray::fromHex(("63c2")) << PacePasswordId::PACE_PIN << CardReturnCode::OK;
			QTest::newRow("failedRc1") << QByteArray::fromHex(("63c2")) << QByteArray::fromHex(("63c1")) << PacePasswordId::PACE_PIN << CardReturnCode::OK;
			QTest::newRow("failedRc0") << QByteArray::fromHex(("63c1")) << QByteArray::fromHex(("63c0")) << PacePasswordId::PACE_PIN << CardReturnCode::OK;
			QTest::newRow("failedCan") << QByteArray::fromHex(("9000")) << QByteArray::fromHex(("6300")) << PacePasswordId::PACE_CAN << CardReturnCode::OK;
			QTest::newRow("failedPuk") << QByteArray::fromHex(("9000")) << QByteArray::fromHex(("6300")) << PacePasswordId::PACE_PUK << CardReturnCode::OK;
			QTest::newRow("protocolError") << QByteArray::fromHex(("9000")) << QByteArray::fromHex(("6f00")) << PacePasswordId::PACE_PIN << CardReturnCode::PROTOCOL_ERROR;
		}


		void fullRun()
		{
			QFETCH(QByteArray, mseSetAt);
			QFETCH(QByteArray, response);
			QFETCH(PacePasswordId, passwordId);
			QFETCH(CardReturnCode, returnCode);

			QPointer<MockReader> reader = MockReader::createMockReader(QList<TransmitConfig>(), mEfCardAccessBytes);
			const auto& worker = MockCardConnectionWorker::create(reader);
			PaceHandler paceHandler(worker);
			paceHandler.initialize(reader->getReaderInfo().getCardInfo().getEfCardAccess());

			worker->addResponse(CardReturnCode::OK, mseSetAt);
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("7c128010eb57ead6b00688a984fd3330defce40d9000"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("7c438241041679ccd805e8d063cca0972ae6cec1edeecb4d3dec440d659573e3bc719ade9d4bb9f363405a40b2c0761eefa3e5eb1b34c5eb069793826d283b86f5849a17fb9000"));
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex("7c438441047184f73be4b409ea4299806f1e785e80d4fde03799e4265d091165cccf16a70a5c653ad9214df5eaed32f69d4414ba9e7ec6f71b97b40c577f6861549d480a479000"));
			worker->addResponse(CardReturnCode::OK, response);
			const auto& output = paceHandler.establishPaceChannel(passwordId, QByteArray("123456"));

			QCOMPARE(output.getReturnCode(), returnCode);
		}


};

QTEST_GUILESS_MAIN(test_PaceHandler)
#include "test_PaceHandler.moc"
