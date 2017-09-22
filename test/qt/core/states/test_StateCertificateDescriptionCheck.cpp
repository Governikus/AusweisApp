/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StateCertificateDescriptionCheck.h"

#include "asn1/ASN1Util.h"
#include "asn1/CVCertificate.h"
#include "context/AuthContext.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "Result.h"
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
			mAuthContext.reset(new AuthContext(nullptr));
			QSharedPointer<DIDAuthenticateEAC1> didAuthEac1(dynamic_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml"))));
			mAuthContext->setDidAuthenticateEac1(didAuthEac1);
			mAuthContext->setTerminalCvc(didAuthEac1->getCvCertificates().at(0));
			mAuthContext->setDvCvc(didAuthEac1->getCvCertificates().at(1));
			mAuthContext->setTcTokenUrl(QUrl("https://dev-demo.governikus-eid.de:8443/Autent-DemoApplication/RequestServlet;jsessionid=14w5aKuENyd2D4ZsMmuaeX2g"));

			mState.reset(new StateCertificateDescriptionCheck(mAuthContext));
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
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCertificateDescription = QSharedPointer<CertificateDescription>(new CertificateDescription());
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCertificateDescriptionAsBinary.clear();

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void noSubjectUrl()
		{

			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCertificateDescription->setSubjectUrl(QString());

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void nonMatchingDescription()
		{
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCertificateDescription = QSharedPointer<CertificateDescription>(new CertificateDescription());
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCertificateDescriptionAsBinary = QByteArray::fromHex("1234567890abcdef");

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void matchingDescription()
		{
			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void subjectUrlNoSameOrigin()
		{
			mAuthContext->setTcTokenUrl(QUrl("https://dev-demo.governikus-eid.de:8442/Autent-DemoApplication/RequestServlet;jsessionid=14w5aKuENyd2D4ZsMmuaeX2g"));

			QSignalSpy spy(mState.data(), &StateCertificateDescriptionCheck::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateCertificateDescriptionCheck)
#include "test_StateCertificateDescriptionCheck.moc"
