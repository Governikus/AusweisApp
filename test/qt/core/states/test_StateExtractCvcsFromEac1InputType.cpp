/*!
 * \brief Tests the StateExtractCvcsFromEac1InputType
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateExtractCvcsFromEac1InputType.h"

#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;


class test_StateExtractCvcsFromEac1InputType
	: public QObject
{
	Q_OBJECT

	QVector<QSharedPointer<const CVCertificate> > mTerminalCvcs, mDvCvcs, mLinkCvcs, mCvcas;
	QScopedPointer<StateExtractCvcsFromEac1InputType> mState;
	QSharedPointer<AuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void initTestCase()
		{
			mTerminalCvcs << CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvat-DE0000024001HW.hex"));
			mTerminalCvcs << CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMOPAA00079.hex"));
			mDvCvcs << CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvdv-DEDVeIDDPST00039.hex"));
			mDvCvcs << CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvdv-DEDVeIDDTR101415.hex"));
			mLinkCvcs << CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DECVCAeID00102_DECVCAeID00103.hex"));
			mCvcas << CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex"));
			mCvcas << CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DECVCAeID00102.hex"));
		}


		void init()
		{
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1_3.xml"));

			mState.reset(new StateExtractCvcsFromEac1InputType(mAuthContext));
			mState->setStateName("StateExtractCvcsFromEac1InputType");

			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.clear();
			connect(this, &test_StateExtractCvcsFromEac1InputType::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void cleanup()
		{
			mState.reset();
			mAuthContext.reset();
		}


		void testNoDvCvc()
		{
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mTerminalCvcs.at(0));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireAbort);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testMoreThanOneDvCvc()
		{
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mTerminalCvcs.at(0));
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mDvCvcs.at(0));
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mDvCvcs.at(1));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireAbort);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testNoTerminalCvc()
		{
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mDvCvcs.at(0));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireAbort);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testMoreThanOneTerminalCvc()
		{
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mDvCvcs.at(0));
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mTerminalCvcs.at(0));
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mTerminalCvcs.at(1));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireAbort);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testSuccess()
		{
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mDvCvcs.at(0));
			mAuthContext->getDidAuthenticateEac1()->mEac1InputType.mCvCertificates.append(mTerminalCvcs.at(0));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireContinue);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(mAuthContext->getTerminalCvc(), mTerminalCvcs.at(0));
			QCOMPARE(mAuthContext->getDvCvc(), mDvCvcs.at(0));
		}


};

QTEST_GUILESS_MAIN(test_StateExtractCvcsFromEac1InputType)
#include "test_StateExtractCvcsFromEac1InputType.moc"
