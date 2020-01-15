/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "command/DidAuthenticateEAC2Command.h"

#include "asn1/CVCertificateChainBuilder.h"

#include "MockCardConnectionWorker.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace governikus;


class test_DidAuthenticateEAC2Command
	: public QObject
{
	Q_OBJECT
	QSharedPointer<MockCardConnectionWorker> mWorker;

	static CVCertificateChain createCVCertificateChain(const QString& pFileName)
	{
		QByteArray data = TestFileHelper::readFile(QString(":/card/").append(pFileName));
		QSharedPointer<const CVCertificate> certificate = CVCertificate::fromHex(data);
		CVCertificateChainBuilder builder(false);
		return builder.getChainStartingWith(certificate);
	}


	private Q_SLOTS:
		void init()
		{
			mWorker.reset(new MockCardConnectionWorker());
		}


		void cleanup()
		{
			mWorker.clear();
		}


		void test_PerformTerminalAuthentication()
		{
			QByteArray input("0000");
			mWorker->addResponse(CardReturnCode::COMMAND_FAILED);
			const CVCertificateChain cvcChain = createCVCertificateChain("cvca-DETESTeID00001.hex");
			DidAuthenticateEAC2Command command(mWorker, cvcChain, QString(), QString(), input);

			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202030202"
												   "            02 01 02"
												   "            02 01 08");

			auto chipAuthenticationInfo = ChipAuthenticationInfo::decode(bytes);
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::COMMAND_FAILED);

			mWorker->addResponse(CardReturnCode::OK);
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtWarningMsg, "CA MSE:Set AT failed: EMPTY");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::PROTOCOL_ERROR);

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			mWorker->addResponse(CardReturnCode::CANCELLATION_BY_USER);
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtDebugMsg, "Performing CA General Authenticate");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::CANCELLATION_BY_USER);

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			mWorker->addResponse(CardReturnCode::OK);
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtDebugMsg, "Performing CA General Authenticate");
			QTest::ignoreMessage(QtWarningMsg, "CA General Authenticate failed: EMPTY");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::PROTOCOL_ERROR);

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtDebugMsg, "Performing CA General Authenticate");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::OK);
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateEAC2Command)
#include "test_DidAuthenticateEAC2Command.moc"
