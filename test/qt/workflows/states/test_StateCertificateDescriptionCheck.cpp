/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "states/StateCertificateDescriptionCheck.h"

#include "asn1/ASN1Util.h"
#include "context/AuthContext.h"
#include "states/StateBuilder.h"

#include "Converter.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StateCertificateDescriptionCheck
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateCertificateDescriptionCheck> mState;
	QSharedPointer<AuthContext> mAuthContext;

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1.xml"_L1));
			mAuthContext->setTcTokenUrl(QUrl("https://test.governikus-eid.de/Autent-DemoApplication/api/eid/request"_L1));

			mState.reset(StateBuilder::createState<StateCertificateDescriptionCheck>(mAuthContext));

			mState->onEntry(nullptr);
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

			QTest::ignoreMessage(QtCriticalMsg, "No certificate description available");
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Certificate_Check_Failed_No_Description);
		}


		void noSubjectUrl()
		{
			const auto& desc = qSharedPointerConstCast<CertificateDescription>(mAuthContext->getDidAuthenticateEac1()->getCertificateDescription());

			Asn1StringUtil::setValue(QString(), desc->mSubjectURL);

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireAbort);

			QTest::ignoreMessage(QtCriticalMsg, "No subject url available in certificate description");
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Certificate_Check_Failed_No_SubjectUrl_In_Description);
		}


		void nonMatchingDescription()
		{
			QByteArray source = TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex"_L1);
			auto cert = Converter::certificatefromHex(source);
			mAuthContext->initAccessRightManager(cert);

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireAbort);

			QTest::ignoreMessage(QtCriticalMsg, "\"The certificate description does not match the certificate.\"");
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Certificate_Check_Failed_Hash_Mismatch);
		}


		void matchingDescription()
		{
			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "SOP-Check succeeded.");
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void subjectUrlNoSameOrigin()
		{
			mAuthContext->setTcTokenUrl(QUrl("https://other.governikus-eid.de/Autent-DemoApplication/api/eid/request"_L1));

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "SOP-Check failed.");
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Certificate_Check_Failed_Same_Origin_Policy_Violation);
		}


};

QTEST_GUILESS_MAIN(test_StateCertificateDescriptionCheck)
#include "test_StateCertificateDescriptionCheck.moc"
