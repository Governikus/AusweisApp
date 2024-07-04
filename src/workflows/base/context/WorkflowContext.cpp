/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowContext.h"

#include "ReaderManager.h"

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	#include "VolatileSettings.h"
#endif

#include <QWeakPointer>

using namespace governikus;

WorkflowContext::WorkflowContext(const Action pAction, bool pActivateUi)
	: QObject()
	, mAction(pAction)
	, mActivateUi(pActivateUi)
	, mStateApproved(false)
	, mWorkflowKilled(false)
	, mCurrentState()
	, mReaderPluginTypes()
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
	, mFailureCode()
	, mStartPaosResult(ECardApiResult::createOk())
	, mWorkflowFinished(false)
	, mWorkflowCancelled(false)
	, mWorkflowCancelledInState(false)
	, mNextWorkflowPending(false)
	, mCurrentReaderHasEidCardButInsufficientApduLength(false)
	, mSkipStartScan(false)
	, mProgressValue(0)
	, mProgressMessage()
	, mShowRemoveCardFeedback(false)
	, mClaimedBy()
	, mInterruptRequested(false)
	, mInitialInputErrorShown(false)
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


bool WorkflowContext::wasClaimed() const
{
	return !mClaimedBy.isNull();
}


void WorkflowContext::claim(const QObject* pClaimant)
{
	const auto& claimant = QString::fromLatin1(pClaimant->metaObject()->className());
	qDebug() << "Claim workflow by" << claimant;

	if (wasClaimed())
	{
		// just a warning... UiAutomatic and UiQml are greedy but it looks ok to see an automated Qml.
		qWarning() << "Workflow already claimed by" << mClaimedBy;
		return;
	}

	mClaimedBy = claimant;
}


void WorkflowContext::onWorkflowCancelled()
{
	mWorkflowCancelled = true;
}


void WorkflowContext::setStateApproved(bool pApproved)
{
	if (mStateApproved != pApproved)
	{
		mStateApproved = pApproved;
		Q_EMIT fireStateApprovedChanged(mStateApproved);
	}
}


void WorkflowContext::killWorkflow(GlobalStatus::Code pCode)
{
	qWarning() << "Killing the current workflow.";
	mWorkflowKilled = true;
	setStatus(pCode);
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


const QList<ReaderManagerPluginType>& WorkflowContext::getReaderPluginTypes() const
{
	return mReaderPluginTypes;
}


void WorkflowContext::setReaderPluginTypes(const QList<ReaderManagerPluginType>& pReaderPluginTypes)
{
	if (mReaderPluginTypes != pReaderPluginTypes)
	{
#ifdef Q_OS_IOS
		const bool usedAsSdk = Env::getSingleton<VolatileSettings>()->isUsedAsSDK();
		const bool containsNfc = pReaderPluginTypes.contains(ReaderManagerPluginType::NFC);
		setSkipStartScan(containsNfc && !usedAsSdk);
#endif

		mReaderPluginTypes = pReaderPluginTypes;
		Q_EMIT fireReaderPluginTypesChanged();
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
		Q_EMIT fireEidTypeMismatchChanged();
	}
}


void WorkflowContext::resetCardConnection()
{
	setCardConnection(QSharedPointer<CardConnection>());
	if (!mReaderName.isEmpty())
	{
		Env::getSingleton<ReaderManager>()->updateReaderInfo(mReaderName);
	}
}


bool WorkflowContext::isSmartCardUsed() const
{
	return getCardConnection() && getCardConnection()->getReaderInfo().getCardType() == CardType::SMART_EID;
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


bool WorkflowContext::isRequestTransportPin() const
{
	return false;
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


bool WorkflowContext::remembersReader() const
{
	return !mExpectedReader.getName().isEmpty();
}


bool WorkflowContext::isExpectedReader() const
{
	return remembersReader() && mExpectedReader.getName() == mReaderName;
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
	Q_EMIT fireResultChanged();
}


std::optional<FailureCode> WorkflowContext::getFailureCode() const
{
	return mFailureCode;
}


void WorkflowContext::setFailureCode(const FailureCode& pFailure)
{
	if (mFailureCode.has_value())
	{
		qWarning() << "FailureCode already set to" << mFailureCode.value() << "- ignoring" << pFailure;
		return;
	}

	mFailureCode = pFailure;
	Q_EMIT fireResultChanged();
}


const ECardApiResult& WorkflowContext::getStartPaosResult() const
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


bool WorkflowContext::skipStartScan() const
{
	return mSkipStartScan;
}


void WorkflowContext::setSkipStartScan(bool pState)
{
	mSkipStartScan = pState;
}


void WorkflowContext::setProgress(int pValue, const QString& pMessage)
{
	if (pValue != mProgressValue || pMessage != mProgressMessage)
	{
		mProgressValue = pValue;
		mProgressMessage = pMessage;

		const auto& connection = getCardConnection();
		if (connection)
		{
			// Card interaction makes up about 80 % of the entire workflow's duration,
			// "correct" the relative progress value accordingly.
			connection->setProgressMessage(pMessage, static_cast<int>(1.25 * pValue));
		}

		Q_EMIT fireProgressChanged();
	}
}


void WorkflowContext::setRemoveCardFeedback(bool pEnabled)
{
	if (pEnabled != mShowRemoveCardFeedback)
	{
		mShowRemoveCardFeedback = pEnabled;
		Q_EMIT fireRemoveCardFeedbackChanged();
	}
}


bool WorkflowContext::eidTypeMismatch() const
{
	if (!getCardConnection())
	{
		return false;
	}

	const auto& acceptedEidTypes = getAcceptedEidTypes();
	const auto& cardType = getCardConnection()->getReaderInfo().getCardInfo().getCardType();
	const auto& mobileEidType = getCardConnection()->getReaderInfo().getCardInfo().getMobileEidType();

	if (cardType == CardType::NONE)
	{
		return false;
	}

	if (cardType == CardType::EID_CARD)
	{
		return !acceptedEidTypes.contains(AcceptedEidType::CARD_CERTIFIED);
	}

	return !isMobileEidTypeAllowed(mobileEidType);
}


bool WorkflowContext::isMobileEidTypeAllowed(const MobileEidType& mobileEidType) const
{
	const auto& acceptedEidTypes = getAcceptedEidTypes();
	switch (mobileEidType)
	{
		case MobileEidType::SE_CERTIFIED:
			return acceptedEidTypes.contains(AcceptedEidType::SE_CERTIFIED);

		case MobileEidType::SE_ENDORSED:
			return acceptedEidTypes.contains(AcceptedEidType::SE_ENDORSED);

		case MobileEidType::HW_KEYSTORE:
			return acceptedEidTypes.contains(AcceptedEidType::HW_KEYSTORE);

		case MobileEidType::UNKNOWN:
			return false;
	}
	Q_UNREACHABLE();
}


void WorkflowContext::setInterruptRequested(bool pInterruptRequested)
{
	mInterruptRequested = pInterruptRequested;
}


bool WorkflowContext::isInitialInputErrorShown() const
{
	return mInitialInputErrorShown;
}


void WorkflowContext::setInitialInputErrorShown()
{
	mInitialInputErrorShown = true;
	Q_EMIT fireInitialInputErrorShownChanged();
}
