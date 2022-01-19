/*!
 * \brief Workflow context.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnection.h"
#include "ECardApiResult.h"
#include "GlobalStatus.h"
#include "ReaderInfo.h"
#include "SmartCardDefinitions.h"

#include <QElapsedTimer>
#include <QSharedPointer>
#include <QString>

namespace governikus
{

class WorkflowContext
	: public QObject
{
	Q_OBJECT

	private:
		bool mStateApproved;
		bool mWorkflowKilled;
		QString mCurrentState;
		QVector<ReaderManagerPlugInType> mReaderPlugInTypes;
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
		ECardApiResult mStartPaosResult;
		bool mErrorReportedToUser;
		bool mPaceResultReportedToUser;
		bool mWorkflowFinished;
		bool mWorkflowCancelled;
		bool mWorkflowCancelledInState;
		bool mNextWorkflowPending;
		bool mCurrentReaderHasEidCardButInsufficientApduLength;

	private Q_SLOTS:
		void onWorkflowCancelled();

	Q_SIGNALS:
		void fireStateApprovedChanged(bool pApproved);
		void fireStateChanged(const QString& pNewState);
		void fireReaderPlugInTypesChanged();
		void fireReaderInfoChanged();
		void fireReaderNameChanged();
		void fireCardConnectionChanged();
		void fireCanChanged();
		void firePinChanged();
		void firePukChanged();
		void firePaceResultUpdated();
		void fireResultChanged();
		void fireCanAllowedModeChanged();
		void firePasswordTypeChanged();

		void fireCancelWorkflow();

		void fireNextWorkflowPending();

	public:
		WorkflowContext();
		~WorkflowContext() override;

		[[nodiscard]] bool isErrorReportedToUser() const;
		void setErrorReportedToUser(bool pErrorReportedToUser = true);

		[[nodiscard]] bool isPaceResultReportedToUser() const;
		void setPaceResultReportedToUser(bool pReported = true);

		void setStateApproved(bool pApproved = true);
		[[nodiscard]] bool isStateApproved() const;

		void killWorkflow();
		[[nodiscard]] bool isWorkflowKilled() const;

		[[nodiscard]] const QString& getCurrentState() const;
		void setCurrentState(const QString& pNewState);

		[[nodiscard]] const QVector<ReaderManagerPlugInType>& getReaderPlugInTypes() const;
		void setReaderPlugInTypes(const QVector<ReaderManagerPlugInType>& pReaderPlugInTypes);

		[[nodiscard]] const QString& getReaderName() const;
		void setReaderName(const QString& pReaderName);

		[[nodiscard]] const QSharedPointer<CardConnection>& getCardConnection() const;
		void setCardConnection(const QSharedPointer<CardConnection>& pCardConnection);
		void resetCardConnection();

		[[nodiscard]] bool isNpaRepositioningRequired() const;
		void setNpaPositionVerified();
		void handleWrongNpaPosition();

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
		[[nodiscard]] bool isExpectedReader() const;
		[[nodiscard]] const ReaderInfo& getExpectedReader() const;

		[[nodiscard]] int getExpectedRetryCounter() const;
		void setExpectedRetryCounter(int pExpectedRetryCounter);

		[[nodiscard]] const GlobalStatus& getStatus() const;
		void setStatus(const GlobalStatus& pResult);

		[[nodiscard]] const ECardApiResult getStartPaosResult() const;
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
};

} // namespace governikus
