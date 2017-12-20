/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore/QtCore>
#include <QtTest/QtTest>
#include <QThread>

#include "controller/AuthController.h"
#include "paos/retrieve/StartPaosResponse.h"
#include "states/StateBuilder.h"
#include "states/StateStartPaosResponse.h"
#include "TestFileHelper.h"

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
			connect(this, &test_StateStartPaosResponse::fireStateStart, mState.data(), &AbstractState::onEntry);
		}


		void cleanup()
		{
			mState.reset();
			mAuthContext.reset();
		}


		void takeResultFromStartPAOSResponse()
		{
			QSharedPointer<StartPaosResponse> startPAOSResponse(new StartPaosResponse(TestFileHelper::readFile(":/paos/StartPAOSResponse3.xml")));
			mAuthContext->setStartPaosResponse(startPAOSResponse);
			mAuthContext->setStatus(CardReturnCodeUtil::toGlobalStatus(CardReturnCode::CANCELLATION_BY_USER));

			QSignalSpy spy(mState.data(), &StateStartPaosResponse::fireAbort);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			const Result& result = mState->getContext()->getStatus();
			QCOMPARE(result.getMajor(), Result::Major::Error);
			QCOMPARE(result.getMinor(), GlobalStatus::Code::Paos_Error_DP_Timeout_Error);
		}


		void emitErrorIfResultError()
		{
			QSharedPointer<StartPaosResponse> startPAOSResponse(new StartPaosResponse(TestFileHelper::readFile(":/paos/StartPAOSResponse3.xml")));
			mAuthContext->setStartPaosResponse(startPAOSResponse);

			QSignalSpy spy(mState.data(), &StateStartPaosResponse::fireAbort);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void emitSuccessIfResultOk()
		{
			QSharedPointer<StartPaosResponse> startPAOSResponse(new StartPaosResponse(TestFileHelper::readFile(":/paos/StartPAOSResponse1.xml")));
			mAuthContext->setStartPaosResponse(startPAOSResponse);

			QSignalSpy spy(mState.data(), &StateStartPaosResponse::fireContinue);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateStartPaosResponse)
#include "test_StateStartPaosResponse.moc"
