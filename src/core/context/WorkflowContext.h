/*!
 * \brief Workflow context.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
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

	private:
		bool mStateApproved;
		QString mCurrentState;
		QVector<ReaderManagerPlugInType> mReaderPlugInTypes;
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

	protected:
		void resetLastPaceResultAndRetryCounter();

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

		const QVector<ReaderManagerPlugInType>& getReaderPlugInTypes() const;
		void setReaderPlugInTypes(const QVector<ReaderManagerPlugInType>& pReaderPlugInTypes);

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
		void setStatus(const GlobalStatus& pResult, bool pReportToUser = true);

		bool isWorkflowFinished() const;
		void setWorkflowFinished(bool pWorkflowFinished);
};

} /* namespace governikus */
