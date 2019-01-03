/*!
 * \brief Workflow context.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnection.h"
#include "ECardApiResult.h"
#include "GlobalStatus.h"
#include "SmartCardDefinitions.h"

#include <QElapsedTimer>
#include <QSharedPointer>
#include <QString>

class test_WorkflowContext;

namespace governikus
{

class WorkflowContext
	: public QObject
{
	Q_OBJECT

	private:
		friend class ::test_WorkflowContext;
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
		QString mExpectedReaderName;
		int mExpectedRetryCounter;
		CardReturnCode mLastPaceResult;
		GlobalStatus mStatus;
		ECardApiResult mStartPaosResult;
		bool mErrorReportedToUser;
		bool mPaceResultReportedToUser;
		bool mWorkflowFinished;
		bool mWorkflowCancelled;
		bool mCanAllowedMode;

	private Q_SLOTS:
		void onWorkflowCancelled();

	Q_SIGNALS:
		void fireStateApprovedChanged();
		void fireStateChanged(const QString& pNewState);
		void fireReaderPlugInTypesChanged();
		void fireReaderNameChanged();
		void fireCardConnectionChanged();
		void fireCanChanged();
		void firePinChanged();
		void firePukChanged();
		void fireLastPaceResultChanged();
		void fireResultChanged();
		void fireCanAllowedModeChanged();

		void fireCancelWorkflow();
		void fireAbortCardSelection();

	public:
		WorkflowContext();

		bool isErrorReportedToUser() const;
		void setErrorReportedToUser(bool pErrorReportedToUser = true);

		bool isPaceResultReportedToUser() const;
		void setPaceResultReportedToUser(bool pReported = true);

		void setStateApproved(bool pApproved = true);
		bool isStateApproved() const;

		void killWorkflow();
		bool isWorkflowKilled() const;

		const QString& getCurrentState() const;
		void setCurrentState(const QString& pNewState);

		const QVector<ReaderManagerPlugInType>& getReaderPlugInTypes() const;
		void setReaderPlugInTypes(const QVector<ReaderManagerPlugInType>& pReaderPlugInTypes);

		const QString& getReaderName() const;
		void setReaderName(const QString& pReaderName);

		const QSharedPointer<CardConnection>& getCardConnection() const;
		void setCardConnection(const QSharedPointer<CardConnection>& pCardConnection);
		void resetCardConnection();

		bool isNpaRepositioningRequired() const;
		void setNpaPositionVerified();
		void handleWrongNpaPosition();

		const QString& getPuk() const;
		void setPuk(const QString& pPuk);

		const QString& getCan() const;
		void setCan(const QString& pCan);

		const QString& getPin() const;
		void setPin(const QString& pPin);

		PacePasswordId getEstablishPaceChannelType() const;
		void setEstablishPaceChannelType(PacePasswordId pType);

		virtual void resetPacePasswords();

		EstablishPaceChannelOutput* getPaceOutputData() const;
		void setPaceOutputData(const EstablishPaceChannelOutput& pPaceOutputData);

		bool isPinBlocked();
		CardReturnCode getLastPaceResult() const;
		void setLastPaceResult(CardReturnCode pLastPaceResult);
		void resetLastPaceResult();

		bool isExpectedReader() const;
		void rememberReader();

		int getExpectedRetryCounter() const;
		void setExpectedRetryCounter(int pExpectedRetryCounter);

		const GlobalStatus& getStatus() const;
		void setStatus(const GlobalStatus& pResult);

		const ECardApiResult getStartPaosResult() const;
		void setStartPaosResult(const ECardApiResult& pStartPaosResult);

		bool isWorkflowFinished() const;
		void setWorkflowFinished(bool pWorkflowFinished);

		bool isWorkflowCancelled() const;

		bool isCanAllowedMode() const;
		void setCanAllowedMode(bool pCanAllowedMode);
};

} // namespace governikus
