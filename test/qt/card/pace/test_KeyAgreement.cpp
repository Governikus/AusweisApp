/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "pace/KeyAgreement.h"

#include "MockCardConnectionWorker.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class KeyAgreementImpl
	: public KeyAgreement
{
	public:
		KeyAgreementImpl(const QSharedPointer<const PaceInfo>& pPaceInfo, const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
			: KeyAgreement(pPaceInfo, pCardConnectionWorker)
		{

		}


		~KeyAgreementImpl() override;

		CardResult determineSharedSecret(const QByteArray& pNonce) override
		{
			Q_UNUSED(pNonce)
			return CardResult();
		}


		QByteArray getUncompressedTerminalPublicKey() override
		{
			return QByteArray();
		}


		QByteArray getUncompressedCardPublicKey() override
		{
			return QByteArray();
		}


		QByteArray getCompressedCardPublicKey() override
		{
			return QByteArray();
		}


};


KeyAgreementImpl::~KeyAgreementImpl() = default;


class test_KeyAgreement
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void mutualAuthenticate_data()
		{
			QTest::addColumn<QByteArray>("response");
			QTest::addColumn<KeyAgreementStatus>("status");

			QTest::newRow("retryAllowed") << QByteArray() << KeyAgreementStatus::RETRY_ALLOWED;
			QTest::newRow("success") << QByteArray::fromHex(("9000")) << KeyAgreementStatus::SUCCESS;
			QTest::newRow("failedRc2") << QByteArray::fromHex(("63c2")) << KeyAgreementStatus::FAILED_RC2;
			QTest::newRow("failedRc1") << QByteArray::fromHex(("63c1")) << KeyAgreementStatus::FAILED_RC1;
			QTest::newRow("failedRc0") << QByteArray::fromHex(("63c0")) << KeyAgreementStatus::FAILED_RC0;
			QTest::newRow("failed") << QByteArray::fromHex(("6300")) << KeyAgreementStatus::FAILED;
			QTest::newRow("protocolError") << QByteArray::fromHex(("6f00")) << KeyAgreementStatus::PROTOCOL_ERROR;
		}


		void mutualAuthenticate()
		{
			QFETCH(QByteArray, response);
			QFETCH(KeyAgreementStatus, status);

			const auto& worker = MockCardConnectionWorker::create();
			worker->addResponse(CardReturnCode::OK, response);

			const auto& efCardAccess = EFCardAccess::fromHex(TestFileHelper::readFile(":/card/efCardAccess.hex"_L1));
			KeyAgreementImpl keyAgreement(efCardAccess->getPaceInfos().at(0), worker);
			QCOMPARE(keyAgreement.performMutualAuthenticate(), status);
		}


		void wrongPassword()
		{
			const auto& worker = MockCardConnectionWorker::create();
			worker->addResponse(CardReturnCode::OK, QByteArray::fromHex(("7C0A8608AFCD013365384BA39000"))); // codespell:ignore

			const auto& efCardAccess = EFCardAccess::fromHex(TestFileHelper::readFile(":/card/efCardAccess.hex"_L1));
			QTest::ignoreMessage(QtCriticalMsg, "Error on mutual authentication");
			KeyAgreementImpl keyAgreement(efCardAccess->getPaceInfos().at(0), worker);
			QCOMPARE(keyAgreement.performMutualAuthenticate(), KeyAgreementStatus::PROTOCOL_ERROR);
		}


};

QTEST_GUILESS_MAIN(test_KeyAgreement)
#include "test_KeyAgreement.moc"
