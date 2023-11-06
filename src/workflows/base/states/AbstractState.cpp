/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "AbstractState.h"

#include "ReaderManager.h"
#if defined(Q_OS_IOS)
	#include "VolatileSettings.h"
#endif

#include <QLoggingCategory>
#include <QStateMachine>

Q_DECLARE_LOGGING_CATEGORY(statemachine)
Q_DECLARE_LOGGING_CATEGORY(support)


using namespace governikus;


const char* const AbstractState::cFORCE_START_STOP_SCAN = "FORCE_START_STOP_SCAN";


AbstractState::AbstractState(const QSharedPointer<WorkflowContext>& pContext)
	: mContext(pContext)
	, mConnections()
	, mAbortOnCardRemoved(false)
	, mKeepCardConnectionAlive(false)
{
	Q_ASSERT(mContext);
	connect(this, &AbstractState::fireAbort, this, &AbstractState::onAbort);
}


void AbstractState::setAbortOnCardRemoved()
{
	mAbortOnCardRemoved = true;
}


void AbstractState::setKeepCardConnectionAlive()
{
	mKeepCardConnectionAlive = true;
}


QString AbstractState::getStateName() const
{
	Q_ASSERT(!objectName().isEmpty());
	return objectName();
}


void AbstractState::onAbort(const FailureCode& pFailure) const
{
	if (mContext)
	{
		qCDebug(statemachine) << "Abort with FailureCode" << pFailure;
		mContext->setFailureCode(pFailure);
	}
}


void AbstractState::onStateApprovedChanged(bool pApproved)
{
	if (pApproved)
	{
		qCDebug(statemachine) << "Running state" << getStateName();
		run();
	}
}


void AbstractState::onEntry(QEvent* pEvent)
{
	if (!mConnections.isEmpty())
	{
		dumpObjectInfo();
	}
	Q_ASSERT(mConnections.isEmpty());

	if (mAbortOnCardRemoved)
	{
		const auto readerManager = Env::getSingleton<ReaderManager>();
		mConnections += connect(readerManager, &ReaderManager::fireCardRemoved, this, &AbstractState::onCardRemoved);
		mConnections += connect(readerManager, &ReaderManager::fireReaderRemoved, this, &AbstractState::onCardRemoved);
	}
	mConnections += connect(mContext.data(), &WorkflowContext::fireCancelWorkflow, this, &AbstractState::onUserCancelled);
	mConnections += connect(mContext.data(), &WorkflowContext::fireStateApprovedChanged, this, &AbstractState::onStateApprovedChanged, Qt::QueuedConnection);

	if (const auto& connection = mContext->getCardConnection(); connection && mKeepCardConnectionAlive)
	{
		connection->setKeepAlive(true);
	}

	qCDebug(statemachine) << "Next state is" << getStateName();
	mContext->setCurrentState(getStateName());

	if (mContext->isWorkflowCancelled() && !mContext->isWorkflowCancelledInState())
	{
		QMetaObject::invokeMethod(this, &AbstractState::onUserCancelled, Qt::QueuedConnection);
	}

	QState::onEntry(pEvent);
}


void AbstractState::onExit(QEvent* pEvent)
{
	if (const auto& connection = mContext->getCardConnection(); connection && mKeepCardConnectionAlive)
	{
		connection->setKeepAlive(false);
	}
	clearConnections();
	mContext->setStateApproved(false);
	qCDebug(statemachine) << "Leaving state" << getStateName()
						  << "with status: [" << mContext->getLastPaceResult() << "+" << mContext->getStatus() << "]";

	QState::onExit(pEvent);
}


void AbstractState::operator<<(const QMetaObject::Connection& connection)
{
	mConnections += connection;
}


void AbstractState::clearConnections()
{
	for (const auto& connection : std::as_const(mConnections))
	{
		QObject::disconnect(connection);
	}
	mConnections.clear();
}


bool AbstractState::isCancellationByUser() const
{
	return mContext->getStatus().isCancellationByUser();
}


void AbstractState::onUserCancelled()
{
	qCInfo(support) << "Cancellation by user";
	mContext->setWorkflowCancelledInState();
	updateStatus(GlobalStatus::Code::Workflow_Cancellation_By_User);
	Q_EMIT fireAbort({FailureCode::Reason::User_Cancelled,
					  {FailureCode::Info::State_Name, getStateName()}
			});
}


void AbstractState::onCardRemoved(const ReaderInfo& pInfo)
{
	if (pInfo.getName() == mContext->getReaderName())
	{
		updateStatus(GlobalStatus::Code::Workflow_Card_Removed);
		Q_EMIT fireAbort({FailureCode::Reason::Card_Removed,
						  {FailureCode::Info::State_Name, getStateName()}
				});
	}
}


void AbstractState::updateStatus(const GlobalStatus& pStatus)
{
	if (pStatus.isError() && mContext->getStatus().isNoError())
	{
		mContext->setStatus(pStatus);
	}
}


void AbstractState::updateStartPaosResult(const ECardApiResult& pStartPaosResult)
{
	if (!pStartPaosResult.isOk() && mContext->getStartPaosResult().isOk())
	{
		mContext->setStartPaosResult(pStartPaosResult);
		updateStatus(pStartPaosResult.toStatus());
	}
}


bool AbstractState::isStartStopEnabled() const
{
	return mContext->getReaderPlugInTypes().contains(ReaderManagerPlugInType::NFC)
		   || machine()->property(cFORCE_START_STOP_SCAN).toBool();
}


void AbstractState::stopNfcScanIfNecessary(const QString& pError) const
{
#if defined(Q_OS_IOS)
	const auto& volatileSettings = Env::getSingleton<VolatileSettings>();
	if (volatileSettings->isUsedAsSDK() && !volatileSettings->handleInterrupt())
	{
		return;
	}
	if (isStartStopEnabled())
	{
		Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPlugInType::NFC, pError);
	}
#else
	Q_UNUSED(pError)
#endif
}
