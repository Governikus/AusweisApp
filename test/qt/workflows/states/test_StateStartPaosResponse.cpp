/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include <QThread>
#include <QtCore>
#include <QtTest>

#include "TestFileHelper.h"
#include "states/StateBuilder.h"
#include "states/StateStartPaosResponse.h"

using namespace governikus;


class test_StateStartPaosResponse
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateStartPaosResponse> mState;
	QSharedPointer<AuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new AuthContext(nullptr));
			mState.reset(StateBuilder::createState<StateStartPaosResponse>(mAuthContext));
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mState.reset();
			mAuthContext.reset();
		}


		void missingStartPAOSResponse()
		{
			QSignalSpy spyAbort(mState.data(), &StateStartPaosResponse::fireAbort);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spyAbort.count(), 1); // clazy:exclude=qstring-allocations

			const GlobalStatus& status = mState->getContext()->getStatus();
			QCOMPARE(status.getStatusCode(), GlobalStatus::Code::Workflow_Start_Paos_Response_Missing);

			const ECardApiResult& result = mState->getContext()->getStartPaosResult();
			QCOMPARE(result.getMajor(), ECardApiResult::Major::Ok);
			QCOMPARE(result.getMinor(), ECardApiResult::Minor::null);

			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Start_Paos_Response_Missing);
		}


		void takeResultFromStartPAOSResponse()
		{
			QSharedPointer<StartPaosResponse> startPAOSResponse(new StartPaosResponse(TestFileHelper::readFile(":/paos/StartPAOSResponse3.xml")));
			mAuthContext->setStartPaosResponse(startPAOSResponse);
			mAuthContext->setStatus(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER));

			QSignalSpy spy(mState.data(), &StateStartPaosResponse::fireAbort);

			mAuthContext->setStateApproved();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

			const GlobalStatus& status = mState->getContext()->getStatus();
			QCOMPARE(status.getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);

			const ECardApiResult& result = mState->getContext()->getStartPaosResult();
			QCOMPARE(result.getMajor(), ECardApiResult::Major::Error);
			QCOMPARE(result.getMinor(), ECardApiResult::Minor::DP_Timeout_Error);

			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Start_Paos_Response_Error);
		}


		void emitErrorIfResultError()
		{
			QSharedPointer<StartPaosResponse> startPAOSResponse(new StartPaosResponse(TestFileHelper::readFile(":/paos/StartPAOSResponse3.xml")));
			mAuthContext->setStartPaosResponse(startPAOSResponse);

			QSignalSpy spy(mState.data(), &StateStartPaosResponse::fireAbort);

			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Start_Paos_Response_Error);
		}


		void emitSuccessIfResultOk()
		{
			QSharedPointer<StartPaosResponse> startPAOSResponse(new StartPaosResponse(TestFileHelper::readFile(":/paos/StartPAOSResponse1.xml")));
			mAuthContext->setStartPaosResponse(startPAOSResponse);

			QSignalSpy spy(mState.data(), &StateStartPaosResponse::fireContinue);

			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


};

QTEST_GUILESS_MAIN(test_StateStartPaosResponse)
#include "test_StateStartPaosResponse.moc"
