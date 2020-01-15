/*!
 * \brief Unit tests for \ref StatePreVerification
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateProcessCertificatesFromEac2.h"

#include "asn1/CVCertificateChainBuilder.h"
#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"
#include "paos/retrieve/DidAuthenticateEac2Parser.h"
#include "states/StateBuilder.h"

#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;


class test_StateProcessCertificatesFromEac2
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateProcessCertificatesFromEac2> mState;
	QSharedPointer<TestAuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
			QSharedPointer<DIDAuthenticateEAC2> didAuthEac2(static_cast<DIDAuthenticateEAC2*>(DidAuthenticateEac2Parser().parse(TestFileHelper::readFile(":/paos/DIDAuthenticateEAC2.xml"))));
			mAuthContext->setDidAuthenticateEac2(didAuthEac2);
			EstablishPaceChannelOutput paceOutput;
			paceOutput.parse(QByteArray::fromHex(TestFileHelper::readFile(":/card/EstablishPaceChannelOutput.hex")), PacePasswordId::PACE_PIN);
			mAuthContext->setPaceOutputData(paceOutput);

			mState.reset(StateBuilder::createState<StateProcessCertificatesFromEac2>(mAuthContext));
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

			QSignalSpy spy(mState.data(), &StateProcessCertificatesFromEac2::fireContinue);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(mState->getContext()->hasChainForCertificationAuthority(*mState->getContext()->getPaceOutputData()));
		}


		void testDoNotTakeTerminalCvcFromEac2()
		{
			// move terminal cvc from eac1 to eac2
			mAuthContext->mDIDAuthenticateEAC2->mEac2.mCvCertificates.append(mAuthContext->getDidAuthenticateEac1()->getCvCertificates().at(0));
			mAuthContext->clearCvCertificates();
			mAuthContext->initCvcChainBuilder();

			QSignalSpy spy(mState.data(), &StateProcessCertificatesFromEac2::fireAbort);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(!mState->getContext()->hasChainForCertificationAuthority(*mState->getContext()->getPaceOutputData()));
		}


		void testCvcFromEac2()
		{
			// move terminal dv from eac1 to eac2
			mAuthContext->mDIDAuthenticateEAC2->mEac2.mCvCertificates.append(mAuthContext->getDidAuthenticateEac1()->getCvCertificates().at(1));
			mAuthContext->removeCvCertAt(3);
			mAuthContext->removeCvCertAt(2);
			mAuthContext->removeCvCertAt(1);
			mAuthContext->initCvcChainBuilder();

			QSignalSpy spy(mState.data(), &StateProcessCertificatesFromEac2::fireContinue);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(mState->getContext()->hasChainForCertificationAuthority(*mState->getContext()->getPaceOutputData()));
		}


};

QTEST_GUILESS_MAIN(test_StateProcessCertificatesFromEac2)
#include "test_StateProcessCertificatesFromEac2.moc"
