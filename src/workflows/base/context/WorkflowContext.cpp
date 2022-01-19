/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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
	, mCurrentState(QStringLiteral("Initial"))
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
	, mExpectedReader()
	, mLastPaceResult(CardReturnCode::OK)
	, mStatus(GlobalStatus::Code::No_Error)
	, mStartPaosResult(ECardApiResult::createOk())
	, mErrorReportedToUser(true)
	, mPaceResultReportedToUser(false)
	, mWorkflowFinished(false)
	, mWorkflowCancelled(false)
	, mWorkflowCancelledInState(false)
	, mNextWorkflowPending(false)
	, mCurrentReaderHasEidCardButInsufficientApduLength(false)
{
	connect(this, &WorkflowContext::fireCancelWorkflow, this, &WorkflowContext::onWorkflowCancelled);
}


WorkflowContext::~WorkflowContext()
{
#ifndef QT_NO_DEBUG
	if (!QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		Q_ASSERT(getPin().isEmpty() && "PACE passwords must be cleared as soon as possible.");
		Q_ASSERT(getCan().isEmpty() && "PACE passwords must be cleared as soon as possible.");
		Q_ASSERT(getPuk().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	}
#endif
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
		Q_EMIT fireStateApprovedChanged(mStateApproved);
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
	Q_EMIT firePasswordTypeChanged();
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
	mLastPaceResult = pLastPaceResult;
	Q_EMIT firePaceResultUpdated();
}


void WorkflowContext::resetLastPaceResult()
{
	setLastPaceResult(CardReturnCode::OK);
}


void WorkflowContext::rememberReader()
{
	if (mCardConnection)
	{
		mExpectedReader = mCardConnection->getReaderInfo();
	}
	else
	{
		mExpectedReader = ReaderInfo();
	}
}


bool WorkflowContext::isExpectedReader() const
{
	return mExpectedReader.getName() == mReaderName;
}


const ReaderInfo& WorkflowContext::getExpectedReader() const
{
	return mExpectedReader;
}


int WorkflowContext::getExpectedRetryCounter() const
{
	return mExpectedReader.getCardInfo().getRetryCounter();
}


void WorkflowContext::setExpectedRetryCounter(int pExpectedRetryCounter)
{
	mExpectedReader.getCardInfo().setRetryCounter(pExpectedRetryCounter);
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


bool WorkflowContext::isWorkflowCancelledInState() const
{
	return mWorkflowCancelledInState;
}


void WorkflowContext::setWorkflowCancelledInState()
{
	mWorkflowCancelledInState = true;
}


bool WorkflowContext::isCanAllowedMode() const
{
	return false;
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


bool WorkflowContext::currentReaderHasEidCardButInsufficientApduLength() const
{
	return mCurrentReaderHasEidCardButInsufficientApduLength;
}


void WorkflowContext::setCurrentReaderHasEidCardButInsufficientApduLength(bool pState)
{
	if (pState != mCurrentReaderHasEidCardButInsufficientApduLength)
	{
		mCurrentReaderHasEidCardButInsufficientApduLength = pState;
		Q_EMIT fireReaderInfoChanged();
	}
}
