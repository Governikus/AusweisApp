/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Test for the chat administration in authentication context.
 */

#include "context/AuthContext.h"

#include "AppSettings.h"
#include "TestAuthContext.h"
#include "VolatileSettings.h"

#include <QSharedPointer>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_AuthContext
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_CanAllowed_data()
		{
			QTest::addColumn<bool>("usedAsSdk");
			QTest::addColumn<bool>("canAllowed");

			QTest::newRow("Sdk/Can") << true << true;
			QTest::newRow("Sdk") << true << false;
			QTest::newRow("Can") << false << true;
			QTest::newRow("None") << false << false;
		}


		void test_CanAllowed()
		{
			QFETCH(bool, usedAsSdk);
			QFETCH(bool, canAllowed);

			const bool enabled = canAllowed | usedAsSdk;

			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(usedAsSdk);
			Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(enabled);
			TestAuthContext context(":/paos/DIDAuthenticateEAC1.xml"_L1);
			Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(false);
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);
			QSignalSpy spy(&context, &TestAuthContext::fireCanAllowedModeChanged);
			QCOMPARE(context.isCanAllowedMode(), enabled);

			*context.getAccessRightManager() += AccessRight::CAN_ALLOWED;
			QCOMPARE(context.isCanAllowedMode(), enabled);
			QCOMPARE(spy.count(), 0);

			*context.getAccessRightManager() -= AccessRight::CAN_ALLOWED;
			QCOMPARE(context.isCanAllowedMode(), false);
			QCOMPARE(spy.count(), enabled ? 1 : 0);

			*context.getAccessRightManager() += AccessRight::CAN_ALLOWED;
			QCOMPARE(context.isCanAllowedMode(), enabled);
			QCOMPARE(spy.count(), enabled ? 2 : 0);
		}


		void test_ErrorReportedToServer()
		{
			AuthContext context;

			QVERIFY(!context.isErrorReportedToServer());

			context.setErrorReportedToServer(true);
			QVERIFY(context.isErrorReportedToServer());

			context.setErrorReportedToServer(false);
			QVERIFY(!context.isErrorReportedToServer());
		}


		void test_DidAuthenticateEacResponse1()
		{
			AuthContext context;
			const QSharedPointer<DIDAuthenticateResponseEAC1> eac(new DIDAuthenticateResponseEAC1());

			QCOMPARE(context.getDidAuthenticateResponseEac1(), nullptr);

			context.setDidAuthenticateResponseEac1(eac);
			QCOMPARE(context.getDidAuthenticateResponseEac1(), eac);
		}


		void test_DidAuthenticateResponseEacAdditionalInputType()
		{
			AuthContext context;
			const QSharedPointer<DIDAuthenticateResponseEAC2> eac(new DIDAuthenticateResponseEAC2());

			QCOMPARE(context.getDidAuthenticateResponseEacAdditionalInputType(), nullptr);

			context.setDidAuthenticateResponseEacAdditionalInputType(eac);
			QCOMPARE(context.getDidAuthenticateResponseEacAdditionalInputType(), eac);
		}


		void test_DidAuthenticateEacAdditional()
		{
			AuthContext context;
			const QSharedPointer<DIDAuthenticateEACAdditional> eac(new DIDAuthenticateEACAdditional());

			QCOMPARE(context.getDidAuthenticateEacAdditional(), nullptr);

			context.setDidAuthenticateEacAdditional(eac);
			QCOMPARE(context.getDidAuthenticateEacAdditional(), eac);
		}


		void test_DidAuthenticateResponseEac2()
		{
			AuthContext context;
			const QSharedPointer<DIDAuthenticateResponseEAC2> eac(new DIDAuthenticateResponseEAC2());

			QCOMPARE(context.getDidAuthenticateResponseEac2(), nullptr);

			context.setDidAuthenticateResponseEac2(eac);
			QCOMPARE(context.getDidAuthenticateResponseEac2(), eac);
		}


		void test_TransmitResponse()
		{
			AuthContext context;
			QSharedPointer<TransmitResponse> response(new TransmitResponse());

			QCOMPARE(context.getTransmitResponse(), nullptr);

			context.setTransmitResponse(response);
			QCOMPARE(context.getTransmitResponse(), response);
		}


		void test_Transmit()
		{
			AuthContext context;
			const QSharedPointer<Transmit> transmit(new Transmit());

			QCOMPARE(context.getTransmit(), nullptr);

			context.setTransmit(transmit);
			QCOMPARE(context.getTransmit(), transmit);
		}


		void test_StartPaos()
		{
			AuthContext context;
			const QByteArray data("paos");
			const QSharedPointer<StartPaos> paos(new StartPaos(data));

			QCOMPARE(context.getStartPaos(), nullptr);

			context.setStartPaos(paos);
			QCOMPARE(context.getStartPaos(), paos);
		}


		void test_SslSession()
		{
			AuthContext context;
			const QByteArray session("session");

			context.setSslSession(session);
			QCOMPARE(context.getSslSession(), session);
		}


		void test_MultiCertificatesUrl()
		{
			AuthContext context;

			QCOMPARE(context.getCertificateList().size(), 0);
			context.addCertificateData(QUrl("https://governikus.de"_L1), QSslCertificate());
			QCOMPARE(context.getCertificateList().size(), 1);
			context.addCertificateData(QUrl("https://governikus.de"_L1), QSslCertificate());
			QCOMPARE(context.getCertificateList().size(), 2);
		}


};

QTEST_GUILESS_MAIN(test_AuthContext)
#include "test_AuthContext.moc"
