/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateExtractCvcsFromEac1InputType.h"

#include "states/StateBuilder.h"

#include "Converter.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StateExtractCvcsFromEac1InputType
	: public QObject
{
	Q_OBJECT

	QList<QSharedPointer<const CVCertificate>> mTerminalCvcs, mDvCvcs, mLinkCvcs, mCvcas;
	QScopedPointer<StateExtractCvcsFromEac1InputType> mState;
	QSharedPointer<TestAuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void initTestCase()
		{
			mTerminalCvcs << Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvat-DE0000024001HW.hex"_L1));
			mTerminalCvcs << Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvat-DEDEMOPAA00079.hex"_L1));
			mDvCvcs << Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvdv-DEDVeIDDPST00039.hex"_L1));
			mDvCvcs << Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvdv-DEDVeIDDTR101415.hex"_L1));
			mLinkCvcs << Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DECVCAeID00102_DECVCAeID00103.hex"_L1));
			mCvcas << Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DECVCAeID00103.hex"_L1));
			mCvcas << Converter::certificatefromHex(TestFileHelper::readFile(":/card/cvca-DECVCAeID00102.hex"_L1));
		}


		void init()
		{
			mAuthContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1_3.xml"_L1));

			mState.reset(StateBuilder::createState<StateExtractCvcsFromEac1InputType>(mAuthContext));

			mAuthContext->clearCvCertificates();
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mState.reset();
			mAuthContext.reset();
		}


		void testNoDvCvc()
		{
			mAuthContext->addCvCertificate(mTerminalCvcs.at(0));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireAbort);

			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Extract_Cvcs_From_Eac1_Dv_Missing);
		}


		void testMoreThanOneDvCvc()
		{
			mAuthContext->addCvCertificate(mTerminalCvcs.at(0));
			mAuthContext->addCvCertificate(mDvCvcs.at(0));
			mAuthContext->addCvCertificate(mDvCvcs.at(1));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireAbort);

			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Extract_Cvcs_From_Eac1_No_Unique_Dv);
		}


		void testNoTerminalCvc()
		{
			mAuthContext->addCvCertificate(mDvCvcs.at(0));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireAbort);

			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Extract_Cvcs_From_Eac1_At_Missing);
		}


		void testMoreThanOneTerminalCvc()
		{
			mAuthContext->addCvCertificate(mDvCvcs.at(0));
			mAuthContext->addCvCertificate(mTerminalCvcs.at(0));
			mAuthContext->addCvCertificate(mTerminalCvcs.at(1));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireAbort);

			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Extract_Cvcs_From_Eac1_No_Unique_At);
		}


		void testSuccess()
		{
			mAuthContext->addCvCertificate(mDvCvcs.at(0));
			mAuthContext->addCvCertificate(mTerminalCvcs.at(0));
			QSignalSpy spy(mState.data(), &StateExtractCvcsFromEac1InputType::fireContinue);

			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getAccessRightManager()->getTerminalCvc(), mTerminalCvcs.at(0));
			QCOMPARE(mAuthContext->getDvCvc(), mDvCvcs.at(0));
		}


};

QTEST_GUILESS_MAIN(test_StateExtractCvcsFromEac1InputType)
#include "test_StateExtractCvcsFromEac1InputType.moc"
