/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateCertificateDescriptionCheck.h"

#include "asn1/ASN1Util.h"
#include "asn1/CVCertificate.h"
#include "context/AuthContext.h"
#include "states/StateBuilder.h"

#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>
#include <QThread>

using namespace governikus;


class test_StateCertificateDescriptionCheck
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateCertificateDescriptionCheck> mState;
	QSharedPointer<AuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
			mAuthContext->setTcTokenUrl(QUrl("https://test.governikus-eid.de:443/Autent-DemoApplication/RequestServlet?provider=demo_epa_can&redirect=true"));

			mState.reset(StateBuilder::createState<StateCertificateDescriptionCheck>(mAuthContext));
			mState->setStateName("StateCertificateDescriptionCheck");

			connect(this, &test_StateCertificateDescriptionCheck::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void cleanup()
		{
			mState.reset();
			mAuthContext.reset();
		}


		void noDescription()
		{
			const QSharedPointer<DIDAuthenticateEAC1> eac1(new DIDAuthenticateEAC1());
			mAuthContext->setDidAuthenticateEac1(eac1);
			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireAbort);

			Q_EMIT fireStateStart(nullptr);

			QTest::ignoreMessage(QtCriticalMsg, "No certificate description available");
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void noSubjectUrl()
		{
			const auto& desc = qSharedPointerConstCast<CertificateDescription>(mAuthContext->getDidAuthenticateEac1()->getCertificateDescription());
			desc->setSubjectUrl(QString());

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireAbort);
			Q_EMIT fireStateStart(nullptr);

			QTest::ignoreMessage(QtCriticalMsg, "No subject url available in certificate description");
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void nonMatchingDescription()
		{
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex");
			auto cert = CVCertificate::fromHex(source);
			mAuthContext->initAccessRightManager(cert);

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireAbort);
			Q_EMIT fireStateStart(nullptr);

			QTest::ignoreMessage(QtCriticalMsg, "\"The certificate description does not match the certificate.\"");
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void matchingDescription()
		{
			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireContinue);
			Q_EMIT fireStateStart(nullptr);

			QTest::ignoreMessage(QtDebugMsg, "SOP-Check succeeded.");
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void subjectUrlNoSameOrigin()
		{
			mAuthContext->setTcTokenUrl(QUrl("https://dev-demo.governikus-eid.de:8442/Autent-DemoApplication/RequestServlet;jsessionid=14w5aKuENyd2D4ZsMmuaeX2g"));

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireAbort);
			Q_EMIT fireStateStart(nullptr);

			QTest::ignoreMessage(QtDebugMsg, "SOP-Check failed.");
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateCertificateDescriptionCheck)
#include "test_StateCertificateDescriptionCheck.moc"
