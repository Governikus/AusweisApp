/*!
 * \brief Unit tests for \ref StatePreVerification
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateProcessCertificatesFromEac2.h"

#include "ResourceLoader.h"
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
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
			QSharedPointer<DIDAuthenticateEAC2> didAuthEac2(static_cast<DIDAuthenticateEAC2*>(DidAuthenticateEac2Parser().parse(TestFileHelper::readFile(":/paos/DIDAuthenticateEAC2.xml"))));
			mAuthContext->setDidAuthenticateEac2(didAuthEac2);

			QByteArray hexBytes = QByteArray("00000000"
											 "fa00"
											 "9000"
											 "c400 3181c1300d060804007f00070202020201023012060a04007f000702020302020201020201413012060a04007f0007020204020202010202010d301c060904007f000702020302300c060704007f0007010202010d020141302a060804007f0007020206161e687474703a2f2f6273692e62756e642e64652f6369662f6e70612e786d6c303e060804007f000702020831323012060a04007f00070202030202020102020145301c060904007f000702020302300c060704007f0007010202010d020145"
											 "0e 4445544553546549443030303034"
											 "00"
											 "2000 5fdd96ef104815b7e5859b2bbdd64160146fe1ad1fc3152f7534ec339629ddde");
			EstablishPaceChannelOutput paceOutput;
			QVERIFY(paceOutput.parse(QByteArray::fromHex(hexBytes)));
			mAuthContext->setPaceOutputData(paceOutput);

			mState.reset(StateBuilder::createState<StateProcessCertificatesFromEac2>(mAuthContext));
			mState->setStateName("StateProcessCertificatesFromEac2");
			mState->onEntry(nullptr);
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
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(mState->getContext()->hasChainForCertificationAuthority(*mState->getContext()->getPaceOutputData()));
		}


		void testDoNotTakeTerminalCvcFromEac2()
		{
			// move terminal cvc from eac1 to eac2
			mAuthContext->mDIDAuthenticateEAC2->mEac2.mCvCertificates.append(mAuthContext->getDidAuthenticateEac1()->getCvCertificates().at(0));
			mAuthContext->clearCvCertificates();
			mAuthContext->initCvcChainBuilder();

			QSignalSpy spy(mState.data(), &StateProcessCertificatesFromEac2::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
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
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(mState->getContext()->hasChainForCertificationAuthority(*mState->getContext()->getPaceOutputData()));
		}


};

QTEST_GUILESS_MAIN(test_StateProcessCertificatesFromEac2)
#include "test_StateProcessCertificatesFromEac2.moc"
