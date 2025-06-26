/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnection.h"
#include "ECardApiResult.h"
#include "FailureCode.h"
#include "GlobalStatus.h"
#include "ReaderInfo.h"
#include "SmartCardDefinitions.h"

#include <QElapsedTimer>
#include <QSharedPointer>
#include <QString>
#include <optional>

namespace governikus
{
defineEnumType(Action,
		AUTH,
		SELF_AUTH,
		CHANGE_PIN,
		PERSONALIZATION,
		REMOTE_SERVICE)

class WorkflowContext
	: public QObject
{
	Q_OBJECT

	private:
		const Action mAction;
		const bool mActivateUi;
		bool mStateApproved;
		bool mWorkflowKilled;
		QString mCurrentState;
		QList<ReaderManagerPluginType> mReaderPluginTypes;
		QString mReaderName;
		QSharedPointer<CardConnection> mCardConnection;
		int mCardVanishedDuringPacePinCount;
		QElapsedTimer mCardVanishedDuringPacePinTimer;
		QString mCan;
		QString mPin;
		QString mPuk;
		PacePasswordId mEstablishPaceChannelType;
		QScopedPointer<EstablishPaceChannelOutput> mPaceOutputData;
		ReaderInfo mExpectedReader;
		CardReturnCode mLastPaceResult;
		GlobalStatus mStatus;
		std::optional<FailureCode> mFailureCode;
		ECardApiResult mStartPaosResult;
		bool mWorkflowFinished;
		bool mWorkflowCancelled;
		bool mWorkflowCancelledInState;
		bool mNextWorkflowPending;
		bool mCurrentReaderHasEidCardButInsufficientApduLength;
		int mProgressValue;
		QString mProgressMessage;
		bool mShowRemoveCardFeedback;
		QString mClaimedBy;
		bool mInterruptRequested;
		bool mInitialInputErrorShown;
		bool mCardInitiallyAppeared;

	private Q_SLOTS:
		void onWorkflowCancelled();

	Q_SIGNALS:
		void fireStateApprovedChanged(bool pApproved);
		void fireStateChanged(const QString& pNewState);
		void fireReaderPluginTypesChanged(bool pExplicitStart = false);
		void fireReaderInfoChanged();
		void fireReaderNameChanged();
		void fireCardConnectionChanged();
		void fireAcceptedEidTypesChanged();
		void fireEidTypeMismatchChanged();
		void fireCanChanged();
		void firePinChanged();
		void firePukChanged();
		void firePaceResultUpdated();
		void fireResultChanged();
		void fireCanAllowedModeChanged();
		void firePasswordTypeChanged();
		void fireProgressChanged();
		void fireRemoveCardFeedbackChanged();
		void fireInitialInputErrorShownChanged();

		void fireCancelWorkflow();

		void fireNextWorkflowPending();

	public:
		explicit WorkflowContext(const Action mAction, bool pActivateUi = true);
		~WorkflowContext() override;

		[[nodiscard]] Action getAction() const
		{
			return mAction;
		}


		[[nodiscard]] bool isActivateUi() const
		{
			return mActivateUi;
		}


		[[nodiscard]] bool wasClaimed() const;
		void claim(const QObject* pClaimant);

		void setStateApproved(bool pApproved = true);
		[[nodiscard]] bool isStateApproved() const;

		void killWorkflow(GlobalStatus::Code pCode = GlobalStatus::Code::Workflow_Cancellation_By_User);
		[[nodiscard]] bool isWorkflowKilled() const;

		[[nodiscard]] const QString& getCurrentState() const;
		void setCurrentState(const QString& pNewState);

		[[nodiscard]] bool isSmartCardUsed() const;

		[[nodiscard]] const QList<ReaderManagerPluginType>& getReaderPluginTypes() const;
		void setReaderPluginTypes(const QList<ReaderManagerPluginType>& pReaderPluginTypes);

		[[nodiscard]] const QString& getReaderName() const;
		void setReaderName(const QString& pReaderName);

		[[nodiscard]] const QSharedPointer<CardConnection>& getCardConnection() const;
		[[nodiscard]] bool getCardInitiallyAppeared() const;
		void resetCardInitiallyAppeared();
		void setCardConnection(const QSharedPointer<CardConnection>& pCardConnection);
		void resetCardConnection();
		void setCardInitiallyAppeared();

		[[nodiscard]] bool isNpaRepositioningRequired() const;
		void setNpaPositionVerified();
		void handleWrongNpaPosition();

		[[nodiscard]] virtual bool isRequestTransportPin() const;

		[[nodiscard]] const QString& getPuk() const;
		void setPuk(const QString& pPuk);

		[[nodiscard]] const QString& getCan() const;
		void setCan(const QString& pCan);

		[[nodiscard]] const QString& getPin() const;
		void setPin(const QString& pPin);

		[[nodiscard]] PacePasswordId getEstablishPaceChannelType() const;
		void setEstablishPaceChannelType(PacePasswordId pType);

		virtual void resetPacePasswords();

		[[nodiscard]] EstablishPaceChannelOutput* getPaceOutputData() const;
		void setPaceOutputData(const EstablishPaceChannelOutput& pPaceOutputData);

		bool isPinBlocked();
		[[nodiscard]] CardReturnCode getLastPaceResult() const;
		void setLastPaceResult(CardReturnCode pLastPaceResult);
		void resetLastPaceResult();

		void rememberReader();
		[[nodiscard]] bool remembersReader() const;
		[[nodiscard]] bool isExpectedReader() const;
		[[nodiscard]] const ReaderInfo& getExpectedReader() const;

		[[nodiscard]] int getExpectedRetryCounter() const;
		void setExpectedRetryCounter(int pExpectedRetryCounter);

		[[nodiscard]] const GlobalStatus& getStatus() const;
		void setStatus(const GlobalStatus& pResult);

		[[nodiscard]] std::optional<FailureCode> getFailureCode() const;
		void setFailureCode(const FailureCode& pFailure);

		[[nodiscard]] const ECardApiResult& getStartPaosResult() const;
		void setStartPaosResult(const ECardApiResult& pStartPaosResult);

		[[nodiscard]] bool isWorkflowFinished() const;
		void setWorkflowFinished(bool pWorkflowFinished);

		[[nodiscard]] bool isWorkflowCancelled() const;
		[[nodiscard]] bool isWorkflowCancelledInState() const;
		void setWorkflowCancelledInState();

		[[nodiscard]] virtual bool isCanAllowedMode() const;

		[[nodiscard]] bool hasNextWorkflowPending() const;
		void setNextWorkflowPending(bool pNextWorkflowPending);

		[[nodiscard]] bool currentReaderHasEidCardButInsufficientApduLength() const;
		void setCurrentReaderHasEidCardButInsufficientApduLength(bool pState);

		[[nodiscard]] int getProgressValue() const
		{
			return mProgressValue;
		}


		[[nodiscard]] const QString& getProgressMessage() const
		{
			return mProgressMessage;
		}


		void setProgress(int pValue, const QString& pMessage);

		[[nodiscard]] bool showRemoveCardFeedback() const
		{
			return mShowRemoveCardFeedback;
		}


		void setInterruptRequested(bool pInterruptRequested);
		[[nodiscard]] bool interruptRequested() const
		{
			return mInterruptRequested;
		}


		void setRemoveCardFeedback(bool pEnabled);

		[[nodiscard]] virtual QList<AcceptedEidType> getAcceptedEidTypes() const = 0;

		[[nodiscard]] bool eidTypeMismatch() const;
		[[nodiscard]] bool isMobileEidTypeAllowed(const MobileEidType& mobileEidType) const;

		[[nodiscard]] bool isInitialInputErrorShown() const;
		void setInitialInputErrorShown();
};

} // namespace governikus
