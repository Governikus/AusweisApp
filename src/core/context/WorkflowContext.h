/*!
 * \brief Workflow context.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "CardConnection.h"
#include "GlobalStatus.h"
#include "Result.h"

#include <QSharedPointer>
#include <QString>

namespace governikus
{

class WorkflowContext
	: public QObject
{
	Q_OBJECT

	bool mStateApproved;
	QString mCurrentState;
	ReaderManagerPlugInType mReaderType;
	QString mReaderName;
	QSharedPointer<CardConnection> mCardConnection;
	QString mCan;
	QString mPin;
	QString mPuk;
	QScopedPointer<EstablishPACEChannelOutput> mPaceOutputData;
	int mOldRetryCounter;
	CardReturnCode mLastPaceResult;
	GlobalStatus mStatus;
	bool mErrorReportedToUser;
	bool mWorkflowFinished;

	Q_SIGNALS:
		void fireStateApprovedChanged();
		void fireCurrentStateChanged(const QString& pNewState);
		void fireReaderTypeChanged();
		void fireReaderNameChanged();
		void fireCardConnectionChanged();
		void fireCanChanged();
		void firePinChanged();
		void firePukChanged();
		void fireLastPaceResultChanged();
		void fireResultChanged();

		void fireCancelWorkflow();
		void fireAbortCardSelection();

	public:
		WorkflowContext();

		bool isErrorReportedToUser() const;
		void setErrorReportedToUser(bool pErrorReportedToUser = true);

		void setStateApproved(bool pApproved = true);
		bool isStateApproved();

		const QString& getCurrentState() const;
		void setCurrentState(const QString& pNewState);

		ReaderManagerPlugInType getReaderType() const;
		void setReaderType(ReaderManagerPlugInType pReaderType);

		const QString& getReaderName() const;
		void setReaderName(const QString& pReaderName);

		const QSharedPointer<CardConnection>& getCardConnection() const;
		void setCardConnection(const QSharedPointer<CardConnection>& pCardConnection);

		const QString& getPuk() const;
		void setPuk(const QString& pPuk);

		const QString& getCan() const;
		void setCan(const QString& pCan);

		const QString& getPin() const;
		void setPin(const QString& pPin);

		EstablishPACEChannelOutput* getPaceOutputData() const;
		void setPaceOutputData(const EstablishPACEChannelOutput& pPaceOutputData);

		bool isPinBlocked();
		CardReturnCode getLastPaceResult() const;
		int getOldRetryCounter() const;
		void setLastPaceResultAndRetryCounter(CardReturnCode pLastPaceResult, int pOldRetryCounter);

		const GlobalStatus& getStatus() const;
		void setStatus(const GlobalStatus& pResult);

		bool isWorkflowFinished() const;
		void setWorkflowFinished(bool pWorkflowFinished);
};

} /* namespace governikus */
