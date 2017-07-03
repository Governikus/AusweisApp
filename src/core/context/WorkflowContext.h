/*!
 * \brief Workflow context.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "CardConnection.h"
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
	QScopedPointer<EstablishPACEChannelOutput> mPaceOutputData;
	int mOldRetryCounter;
	ReturnCode mLastPaceResult;
	Result mResult;
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
		void fireLastPaceResultChanged();
		void fireResultChanged();

		void fireCancelWorkflow();
		void fireAbortCardSelection();

	public:
		WorkflowContext();


		bool isErrorReportedToUser() const
		{
			return mErrorReportedToUser;
		}


		void setErrorReportedToUser(bool pErrorReportedToUser = true)
		{
			mErrorReportedToUser = pErrorReportedToUser;
		}


		void setStateApproved(bool pApproved = true)
		{
			if (mStateApproved != pApproved)
			{
				mStateApproved = pApproved;
				Q_EMIT fireStateApprovedChanged();
			}
		}


		bool isStateApproved()
		{
			return mStateApproved;
		}


		const QString& getCurrentState() const
		{
			return mCurrentState;
		}


		void setCurrentState(const QString& pNewState)
		{
			if (mCurrentState != pNewState)
			{
				mCurrentState = pNewState;
				Q_EMIT fireCurrentStateChanged(pNewState);
			}
		}


		ReaderManagerPlugInType getReaderType() const
		{
			return mReaderType;
		}


		void setReaderType(ReaderManagerPlugInType pReaderType)
		{
			if (mReaderType != pReaderType)
			{
				mReaderType = pReaderType;
				Q_EMIT fireReaderTypeChanged();
			}
		}


		const QString& getReaderName() const
		{
			return mReaderName;
		}


		void setReaderName(const QString& pReaderName)
		{
			if (mReaderName != pReaderName)
			{
				mReaderName = pReaderName;
				Q_EMIT fireReaderNameChanged();
			}
		}


		const QSharedPointer<CardConnection>& getCardConnection() const
		{
			return mCardConnection;
		}


		void setCardConnection(const QSharedPointer<CardConnection>& pCardConnection)
		{
			if (mCardConnection != pCardConnection)
			{
				mCardConnection = pCardConnection;
				Q_EMIT fireCardConnectionChanged();
			}
		}


		bool isPinBlocked()
		{
			return mCardConnection != nullptr && mCardConnection->getReaderInfo().getRetryCounter() == 0;
		}


		const QString& getCan() const
		{
			return mCan;
		}


		void setCan(const QString& pCan)
		{
			if (mCan != pCan)
			{
				mCan = pCan;
				Q_EMIT fireCanChanged();
			}
		}


		const QString& getPin() const
		{
			return mPin;
		}


		void setPin(const QString& pPin)
		{
			if (mPin != pPin)
			{
				mPin = pPin;
				Q_EMIT firePinChanged();
			}
		}


		EstablishPACEChannelOutput* getPaceOutputData() const
		{
			return mPaceOutputData.data();
		}


		void setPaceOutputData(const EstablishPACEChannelOutput& pPaceOutputData)
		{
			mPaceOutputData.reset(new EstablishPACEChannelOutput(pPaceOutputData));
		}


		ReturnCode getLastPaceResult() const
		{
			return mLastPaceResult;
		}


		int getOldRetryCounter() const
		{
			return mOldRetryCounter;
		}


		void setLastPaceResultAndRetryCounter(ReturnCode pLastPaceResult, int pOldRetryCounter)
		{
			if (mLastPaceResult != pLastPaceResult || mOldRetryCounter != pOldRetryCounter)
			{
				mLastPaceResult = pLastPaceResult;
				mOldRetryCounter = pOldRetryCounter;
				Q_EMIT fireLastPaceResultChanged();
			}
		}


		const Result& getResult() const
		{
			return mResult;
		}


		void setResult(const Result& pResult)
		{
			mResult = pResult;
			mErrorReportedToUser = false;
			Q_EMIT fireResultChanged();
		}


		bool isWorkflowFinished() const
		{
			return mWorkflowFinished;
		}


		void setWorkflowFinished(bool pWorkflowFinished)
		{
			mWorkflowFinished = pWorkflowFinished;
		}


};

} /* namespace governikus */
