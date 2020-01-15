/*!
 * \brief Test for the chat administration in authentication context.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "TestAuthContext.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


class test_AuthContext
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<QSignalSpy> mEac1Changed;
		QScopedPointer<QSignalSpy> mDataChanged;
		QScopedPointer<QSignalSpy> mEffectiveSignal;
		QScopedPointer<TestAuthContext> mAuthContext;

		QSharedPointer<CHAT> getChat(const std::initializer_list<AccessRight>& pList)
		{
			auto chat = newObject<CHAT>();
			chat->setAccessRights(QSet<AccessRight>(pList));
			return chat;
		}


		void clearSignals()
		{
			mEac1Changed->clear();
			mDataChanged->clear();
			mEffectiveSignal->clear();
		}


		void checkChatSizes(const QString& pTestNum, int pRequired, int pOptional, int pEffective)
		{
			qDebug() << "Checking test" << pTestNum;
			QCOMPARE(mAuthContext->getRequiredAccessRights().size(), pRequired);
			QCOMPARE(mAuthContext->getOptionalAccessRights().size(), pOptional);
			QCOMPARE(mAuthContext->getEffectiveAccessRights().size(), pEffective);
		}


		void checkAndClearSignals(const QString& pTestNum, int pEac1Data, int pEffective)
		{
			qDebug() << "Checking test" << pTestNum;
			QCOMPARE(mEac1Changed->count(), pEac1Data);
			QCOMPARE(mDataChanged->count(), pEac1Data);
			QCOMPARE(mEffectiveSignal->count(), pEffective);

			clearSignals();
		}


	private Q_SLOTS:
		void init()
		{
			// Creates a context with 17 access rights in cvc
			// Because required and optional are empty, all 17 rights are optional by default
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
			mAuthContext->setRequiredAccessRights({});
			mAuthContext->setOptionalAccessRights({});

			mEac1Changed.reset(new QSignalSpy(mAuthContext.data(), &AuthContext::fireDidAuthenticateEac1Changed));
			mDataChanged.reset(new QSignalSpy(mAuthContext.data(), &AuthContext::fireAuthenticationDataChanged));
			mEffectiveSignal.reset(new QSignalSpy(mAuthContext.data(), &AuthContext::fireEffectiveChatChanged));
		}


		void cleanup()
		{
			mEac1Changed.reset();
			mEffectiveSignal.reset();
			mAuthContext.reset();
		}


		void test_AuthContext_01()
		{
			// Exists after creation - All cvc rights are optional and selected
			checkChatSizes("01-01", 0, 17, 17);
		}


		void test_AuthContext_02()
		{
			// Try to set required right that is not in cvc
			mAuthContext->setRequiredAccessRights({AccessRight::READ_DG21});
			checkChatSizes("02-01", 0, 17, 17);
			checkAndClearSignals("02-02", 1, 1);

			// Try to set optional right that is not in cvc
			mAuthContext->setOptionalAccessRights({AccessRight::READ_DG21});
			checkChatSizes("02-03", 0, 17, 17);
			checkAndClearSignals("02-04", 1, 1);

			// Try to set a right in required and optional
			mAuthContext->setOptionalAccessRights({AccessRight::READ_DG05});
			checkChatSizes("02-05", 0, 1, 1);
			checkAndClearSignals("02-06", 1, 1);
			mAuthContext->setRequiredAccessRights({AccessRight::READ_DG05});
			checkChatSizes("02-07", 1, 0, 1);
			checkAndClearSignals("02-08", 1, 1);
		}


		void test_AuthContext_03()
		{
			// Clear effective access rights
			mAuthContext->setEffectiveAccessRights({});
			checkChatSizes("03-01", 0, 17, 0);
			checkAndClearSignals("03-02", 0, 1);
		}


		void test_AuthContext_04()
		{
			// Set only one access right as optional, required access rights remain empty
			mAuthContext->setOptionalAccessRights({AccessRight::READ_DG05});
			checkChatSizes("04-01", 0, 1, 1);
			QVERIFY(mAuthContext->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-02", 1, 1);

			// Try to set an optional access right that is already set
			QVERIFY(mAuthContext->addEffectiveAccessRight(AccessRight::READ_DG05));
			checkChatSizes("04-03", 0, 1, 1);
			QVERIFY(mAuthContext->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-04", 0, 0);

			// Try to remove an optional access right
			QVERIFY(mAuthContext->removeEffectiveAccessRight(AccessRight::READ_DG05));
			checkChatSizes("04-05", 0, 1, 0);
			QVERIFY(mAuthContext->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-06", 0, 1);

			// Try to remove an optional access right that is not set
			QVERIFY(mAuthContext->removeEffectiveAccessRight(AccessRight::READ_DG05));
			checkChatSizes("04-07", 0, 1, 0);
			QVERIFY(mAuthContext->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-08", 0, 0);

			// Try to set an optional access right
			QVERIFY(mAuthContext->addEffectiveAccessRight(AccessRight::READ_DG05));
			checkChatSizes("04-09", 0, 1, 1);
			QVERIFY(mAuthContext->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-10", 0, 1);

			// Try to set an access right that is not included in optional or required access rights
			QVERIFY(!mAuthContext->addEffectiveAccessRight(AccessRight::AGE_VERIFICATION));
			checkChatSizes("04-11", 0, 1, 1);
			QVERIFY(mAuthContext->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-12", 0, 0);
		}


		void test_AuthContext_05()
		{
			// Set only one access right as required, optional access rights remain empty
			mAuthContext->setRequiredAccessRights({AccessRight::READ_DG05});
			checkChatSizes("05-01", 1, 0, 1);
			QVERIFY(mAuthContext->getRequiredAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("05-02", 1, 1);

			// Try to set an required access right that is already set
			QVERIFY(mAuthContext->addEffectiveAccessRight(AccessRight::READ_DG05));
			checkChatSizes("05-03", 1, 0, 1);
			QVERIFY(mAuthContext->getRequiredAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("05-04", 0, 0);

			// Try to remove an required access right
			QVERIFY(!mAuthContext->removeEffectiveAccessRight(AccessRight::READ_DG05));
			checkChatSizes("05-05", 1, 0, 1);
			QVERIFY(mAuthContext->getRequiredAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("05-06", 0, 0);

			// Try to remove an access right that is not included in optional or required access rights
			QVERIFY(mAuthContext->removeEffectiveAccessRight(AccessRight::AGE_VERIFICATION));
			checkChatSizes("05-07", 1, 0, 1);
			QVERIFY(mAuthContext->getRequiredAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mAuthContext->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("05-08", 0, 0);
		}


		void test_ErrorReportedToServer()
		{
			AuthContext context(nullptr);

			QVERIFY(!context.isErrorReportedToServer());

			context.setErrorReportedToServer(true);
			QVERIFY(context.isErrorReportedToServer());

			context.setErrorReportedToServer(false);
			QVERIFY(!context.isErrorReportedToServer());
		}


		void test_DidAuthenticateEacResponse1()
		{
			AuthContext context(nullptr);
			const QSharedPointer<DIDAuthenticateResponseEAC1> eac(new DIDAuthenticateResponseEAC1());

			QCOMPARE(context.getDidAuthenticateResponseEac1(), nullptr);

			context.setDidAuthenticateResponseEac1(eac);
			QCOMPARE(context.getDidAuthenticateResponseEac1(), eac);
		}


		void test_DidAuthenticateResponseEacAdditionalInputType()
		{
			AuthContext context(nullptr);
			const QSharedPointer<DIDAuthenticateResponseEAC2> eac(new DIDAuthenticateResponseEAC2());

			QCOMPARE(context.getDidAuthenticateResponseEacAdditionalInputType(), nullptr);

			context.setDidAuthenticateResponseEacAdditionalInputType(eac);
			QCOMPARE(context.getDidAuthenticateResponseEacAdditionalInputType(), eac);
		}


		void test_DidAuthenticateEacAdditional()
		{
			AuthContext context(nullptr);
			const QSharedPointer<DIDAuthenticateEACAdditional> eac(new DIDAuthenticateEACAdditional());

			QCOMPARE(context.getDidAuthenticateEacAdditional(), nullptr);

			context.setDidAuthenticateEacAdditional(eac);
			QCOMPARE(context.getDidAuthenticateEacAdditional(), eac);
		}


		void test_DidAuthenticateResponseEac2()
		{
			AuthContext context(nullptr);
			const QSharedPointer<DIDAuthenticateResponseEAC2> eac(new DIDAuthenticateResponseEAC2());

			QCOMPARE(context.getDidAuthenticateResponseEac2(), nullptr);

			context.setDidAuthenticateResponseEac2(eac);
			QCOMPARE(context.getDidAuthenticateResponseEac2(), eac);
		}


		void test_DidList()
		{
			AuthContext context(nullptr);
			const QByteArray data("data");
			const QSharedPointer<DIDList> list(new DIDList(data));

			QCOMPARE(context.getDidList(), nullptr);

			context.setDidList(list);
			QCOMPARE(context.getDidList(), list);
		}


		void test_DidListResponse()
		{
			AuthContext context(nullptr);
			const QSharedPointer<DIDListResponse> response(new DIDListResponse());

			QCOMPARE(context.getDidListResponse(), nullptr);

			context.setDidListResponse(response);
			QCOMPARE(context.getDidListResponse(), response);
		}


		void test_Disconnect()
		{
			AuthContext context(nullptr);
			const QByteArray data("disconnect");
			const QSharedPointer<Disconnect> discnnct(new Disconnect(data));

			QCOMPARE(context.getDisconnect(), nullptr);

			context.setDisconnect(discnnct);
			QCOMPARE(context.getDisconnect(), discnnct);
		}


		void test_DisconnectResponse()
		{
			AuthContext context(nullptr);
			const QSharedPointer<DisconnectResponse> response(new DisconnectResponse());

			QCOMPARE(context.getDisconnectResponse(), nullptr);

			context.setDisconnectResponse(response);
			QCOMPARE(context.getDisconnectResponse(), response);
		}


		void test_TransmitResponses()
		{
			AuthContext context(nullptr);
			QSharedPointer<TransmitResponse> response(new TransmitResponse());

			QVERIFY(context.getTransmitResponses().isEmpty());

			context.addTransmitResponse(response);
			QVERIFY(context.getTransmitResponses().contains(response));
		}


		void test_Transmits()
		{
			AuthContext context(nullptr);
			const QSharedPointer<Transmit> transmit(new Transmit());

			QVERIFY(context.getTransmits().isEmpty());

			context.addTransmit(transmit);
			QVERIFY(context.getTransmits().contains(transmit));
		}


		void test_StartPaos()
		{
			AuthContext context(nullptr);
			const QByteArray data("paos");
			const QSharedPointer<StartPaos> paos(new StartPaos(data));

			QCOMPARE(context.getStartPaos(), nullptr);

			context.setStartPaos(paos);
			QCOMPARE(context.getStartPaos(), paos);
		}


		void test_SslSession()
		{
			AuthContext context(nullptr);
			const QByteArray session("session");

			context.setSslSession(session);
			QCOMPARE(context.getSslSession(), session);
		}


};

QTEST_GUILESS_MAIN(test_AuthContext)
#include "test_AuthContext.moc"
