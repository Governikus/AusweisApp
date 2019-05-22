/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowContext.h"

#include "ReaderManager.h"

#include <QWeakPointer>

using namespace governikus;


void WorkflowContext::onWorkflowCancelled()
{
	mWorkflowCancelled = true;
}


WorkflowContext::WorkflowContext()
	: QObject()
	, mStateApproved(false)
	, mWorkflowKilled(false)
	, mCurrentState()
	, mReaderPlugInTypes()
	, mReaderName()
	, mCardConnection()
	, mCardVanishedDuringPacePinCount(0)
	, mCardVanishedDuringPacePinTimer()
	, mCan()
	, mPin()
	, mPuk()
	, mEstablishPaceChannelType(PacePasswordId::UNKNOWN)
	, mPaceOutputData()
	, mExpectedReaderName()
	, mExpectedRetryCounter(-1)
	, mLastPaceResult(CardReturnCode::OK)
	, mStatus(GlobalStatus::Code::No_Error)
	, mStartPaosResult(ECardApiResult::createOk())
	, mErrorReportedToUser(true)
	, mPaceResultReportedToUser(false)
	, mWorkflowFinished(false)
	, mWorkflowCancelled(false)
	, mCanAllowedMode(false)
	, mNextWorkflowPending(false)
{
	connect(this, &WorkflowContext::fireCancelWorkflow, this, &WorkflowContext::onWorkflowCancelled);
}


bool WorkflowContext::isErrorReportedToUser() const
{
	return mErrorReportedToUser || mWorkflowKilled;
}


void WorkflowContext::setErrorReportedToUser(bool pErrorReportedToUser)
{
	mErrorReportedToUser = pErrorReportedToUser;
}


bool WorkflowContext::isPaceResultReportedToUser() const
{
	return mPaceResultReportedToUser;
}


void WorkflowContext::setPaceResultReportedToUser(bool pReported)
{
	mPaceResultReportedToUser = pReported;
}


void WorkflowContext::setStateApproved(bool pApproved)
{
	if (mStateApproved != pApproved)
	{
		mStateApproved = pApproved;
		Q_EMIT fireStateApprovedChanged();
	}
}


void WorkflowContext::killWorkflow()
{
	qWarning() << "Killing the current workflow.";
	mWorkflowKilled = true;
	mStatus = GlobalStatus::Code::Card_Cancellation_By_User;
	if (!mStateApproved)
	{
		setStateApproved(true);
	}

	Q_EMIT fireCancelWorkflow();
}


bool WorkflowContext::isWorkflowKilled() const
{
	return mWorkflowKilled;
}


bool WorkflowContext::isStateApproved() const
{
	return mStateApproved;
}


const QString& WorkflowContext::getCurrentState() const
{
	return mCurrentState;
}


void WorkflowContext::setCurrentState(const QString& pNewState)
{
	mCurrentState = pNewState;
	Q_EMIT fireStateChanged(pNewState);

	if (mWorkflowKilled)
	{
		setStateApproved(true);
	}
}


const QVector<ReaderManagerPlugInType>& WorkflowContext::getReaderPlugInTypes() const
{
	return mReaderPlugInTypes;
}


void WorkflowContext::setReaderPlugInTypes(const QVector<ReaderManagerPlugInType>& pReaderPlugInTypes)
{
	if (mReaderPlugInTypes != pReaderPlugInTypes)
	{
		mReaderPlugInTypes = pReaderPlugInTypes;
		Q_EMIT fireReaderPlugInTypesChanged();
	}
}


const QString& WorkflowContext::getReaderName() const
{
	return mReaderName;
}


void WorkflowContext::setReaderName(const QString& pReaderName)
{
	if (mReaderName != pReaderName)
	{
		mReaderName = pReaderName;
		Q_EMIT fireReaderNameChanged();
	}
}


const QSharedPointer<CardConnection>& WorkflowContext::getCardConnection() const
{
	return mCardConnection;
}


void WorkflowContext::setCardConnection(const QSharedPointer<CardConnection>& pCardConnection)
{
	if (mCardConnection != pCardConnection)
	{
		mCardConnection = pCardConnection;
		Q_EMIT fireCardConnectionChanged();
	}
}


void WorkflowContext::resetCardConnection()
{
	setCardConnection(QSharedPointer<CardConnection>());
	if (!mReaderName.isEmpty())
	{
		const auto readerManager = Env::getSingleton<ReaderManager>();
		readerManager->updateReaderInfo(mReaderName);
	}
}


bool WorkflowContext::isNpaRepositioningRequired() const
{
	if (mCardVanishedDuringPacePinCount >= 10)
	{
		return true;
	}

	const qint64 fourSeconds = 4000;
	if (mCardVanishedDuringPacePinTimer.isValid() && mCardVanishedDuringPacePinTimer.elapsed() > fourSeconds)
	{
		return true;
	}

	return false;
}


void WorkflowContext::setNpaPositionVerified()
{
	mCardVanishedDuringPacePinCount = 0;
	mCardVanishedDuringPacePinTimer.invalidate();
}


void WorkflowContext::handleWrongNpaPosition()
{
	if (mCardVanishedDuringPacePinCount > 0 && !mCardVanishedDuringPacePinTimer.isValid())
	{
		mCardVanishedDuringPacePinTimer.restart();
	}
	mCardVanishedDuringPacePinCount += 1;
	resetCardConnection();
}


bool WorkflowContext::isPinBlocked()
{
	return mCardConnection != nullptr && mCardConnection->getReaderInfo().getRetryCounter() == 0;
}


const QString& WorkflowContext::getPuk() const
{
	return mPuk;
}


void WorkflowContext::setPuk(const QString& pPuk)
{
	if (mPuk != pPuk)
	{
		mPuk = pPuk;
		Q_EMIT firePukChanged();
	}
}


const QString& WorkflowContext::getCan() const
{
	return mCan;
}


void WorkflowContext::setCan(const QString& pCan)
{
	if (mCan != pCan)
	{
		mCan = pCan;
		Q_EMIT fireCanChanged();
	}
}


const QString& WorkflowContext::getPin() const
{
	return mPin;
}


void WorkflowContext::setPin(const QString& pPin)
{
	if (mPin != pPin)
	{
		mPin = pPin;
		Q_EMIT firePinChanged();
	}
}


PacePasswordId WorkflowContext::getEstablishPaceChannelType() const
{
	return mEstablishPaceChannelType;
}


void WorkflowContext::setEstablishPaceChannelType(PacePasswordId pType)
{
	mEstablishPaceChannelType = pType;
}


void WorkflowContext::resetPacePasswords()
{
	setCan(QString());
	setPin(QString());
	setPuk(QString());
}


EstablishPaceChannelOutput* WorkflowContext::getPaceOutputData() const
{
	return mPaceOutputData.data();
}


void WorkflowContext::setPaceOutputData(const EstablishPaceChannelOutput& pPaceOutputData)
{
	mPaceOutputData.reset(new EstablishPaceChannelOutput(pPaceOutputData));
}


CardReturnCode WorkflowContext::getLastPaceResult() const
{
	return mLastPaceResult;
}


void WorkflowContext::setLastPaceResult(CardReturnCode pLastPaceResult)
{
	mPaceResultReportedToUser = false;
	if (mLastPaceResult != pLastPaceResult)
	{
		mLastPaceResult = pLastPaceResult;
		Q_EMIT fireLastPaceResultChanged();
	}
}


void WorkflowContext::resetLastPaceResult()
{
	setLastPaceResult(CardReturnCode::OK);
}


bool WorkflowContext::isExpectedReader() const
{
	return mExpectedReaderName == mReaderName;
}


void WorkflowContext::rememberReader()
{
	mExpectedReaderName = mReaderName;
}


int WorkflowContext::getExpectedRetryCounter() const
{
	return mExpectedRetryCounter;
}


void WorkflowContext::setExpectedRetryCounter(int pExpectedRetryCounter)
{
	mExpectedRetryCounter = pExpectedRetryCounter;
}


const GlobalStatus& WorkflowContext::getStatus() const
{
	return mStatus;
}


void WorkflowContext::setStatus(const GlobalStatus& pStatus)
{
	mStatus = pStatus;
	mErrorReportedToUser = false;
	Q_EMIT fireResultChanged();
}


const ECardApiResult WorkflowContext::getStartPaosResult() const
{
	return mStartPaosResult;
}


void WorkflowContext::setStartPaosResult(const ECardApiResult& pStartPaosResult)
{
	mStartPaosResult = pStartPaosResult;
}


bool WorkflowContext::isWorkflowFinished() const
{
	return mWorkflowFinished;
}


void WorkflowContext::setWorkflowFinished(bool pWorkflowFinished)
{
	mWorkflowFinished = pWorkflowFinished;
}


bool WorkflowContext::isWorkflowCancelled() const
{
	return mWorkflowCancelled;
}


bool WorkflowContext::isCanAllowedMode() const
{
	return mCanAllowedMode;
}


void WorkflowContext::setCanAllowedMode(bool pCanAllowedMode)
{
	mCanAllowedMode = pCanAllowedMode;
	Q_EMIT fireCanAllowedModeChanged();
}


bool WorkflowContext::hasNextWorkflowPending() const
{
	return mNextWorkflowPending;
}


void WorkflowContext::setNextWorkflowPending(bool pNextWorkflowPending)
{
	if (pNextWorkflowPending != mNextWorkflowPending)
	{
		mNextWorkflowPending = pNextWorkflowPending;
		Q_EMIT fireNextWorkflowPending();
	}
}
