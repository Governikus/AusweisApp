/*!
 * \brief Unit tests for \ref StatePreVerification
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StateProcessCertificatesFromEac2.h"

#include "asn1/CVCertificateChainBuilder.h"
#include "Commands.h"
#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "paos/retrieve/DidAuthenticateEac2Parser.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;


class test_StateProcessCertificatesFromEac2
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateProcessCertificatesFromEac2> mState;
	QSharedPointer<AuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new AuthContext(nullptr));
			QSharedPointer<DIDAuthenticateEAC1> didAuthEac1(dynamic_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml"))));
			mAuthContext->setDidAuthenticateEac1(didAuthEac1);
			QSharedPointer<DIDAuthenticateEAC2> didAuthEac2(dynamic_cast<DIDAuthenticateEAC2*>(DidAuthenticateEac2Parser().parse(TestFileHelper::readFile(":/paos/DIDAuthenticateEAC2.xml"))));
			mAuthContext->setDidAuthenticateEac2(didAuthEac2);
			EstablishPACEChannelOutput paceOutput;
			paceOutput.parse(QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPACEChannelOutput.hex")), PACE_PIN_ID::PACE_PIN);
			mAuthContext->setPaceOutputData(paceOutput);

			mState.reset(new StateProcessCertificatesFromEac2(mAuthContext));
			mState->setStateName("StateProcessCertificatesFromEac2");
			connect(this, &test_StateProcessCertificatesFromEac2::fireStateStart, mState.data(), &AbstractState::onEntry);
		}


		void cleanup()
		{
			mAuthContext.clear();
			mState.reset();
		}


		void testDoNotProcessIfCvcChainAvailable()
		{
			mAuthContext->initCvcChainBuilder();

			QSignalSpy spy(mState.data(), &StateProcessCertificatesFromEac2::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(mState->getContext()->hasChainForCertificationAuthority(*mState->getContext()->getPaceOutputData()));
		}


		void testDoNotTakeTerminalCvcFromEac2()
		{
			// move terminal cvc from eac1 to eac2
			mAuthContext->mDIDAuthenticateEAC2->mEac2.mCvCertificates.append(mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.at(0));
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.removeAt(0);
			mAuthContext->initCvcChainBuilder();

			QSignalSpy spy(mState.data(), &StateProcessCertificatesFromEac2::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(!mState->getContext()->hasChainForCertificationAuthority(*mState->getContext()->getPaceOutputData()));
		}


		void testCvcFromEac2()
		{
			// move terminal dv from eac1 to eac2
			mAuthContext->mDIDAuthenticateEAC2->mEac2.mCvCertificates.append(mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.at(1));
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.removeAt(3);
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.removeAt(2);
			mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.removeAt(1);
			mAuthContext->initCvcChainBuilder();

			QSignalSpy spy(mState.data(), &StateProcessCertificatesFromEac2::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(mState->getContext()->hasChainForCertificationAuthority(*mState->getContext()->getPaceOutputData()));
		}


};

QTEST_GUILESS_MAIN(test_StateProcessCertificatesFromEac2)
#include "test_StateProcessCertificatesFromEac2.moc"
