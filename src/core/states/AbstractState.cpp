/*!
 * AbstractState.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AbstractState.h"
#include "ReaderManager.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)
Q_DECLARE_LOGGING_CATEGORY(support)


using namespace governikus;


AbstractState::AbstractState(const QSharedPointer<WorkflowContext>& pContext, bool pConnectOnCardRemoved)
	: mContext(pContext)
	, mConnectOnCardRemoved(pConnectOnCardRemoved)
	, mConnections()
{
}


AbstractState::~AbstractState()
{
}


QString AbstractState::getStateName() const
{
	Q_ASSERT(!objectName().isEmpty());
	return objectName();
}


void AbstractState::setStateName(const QString& pName)
{
	setObjectName(pName);
}


QString AbstractState::getClassName(const char* pName)
{
	QString className = QString::fromLatin1(pName);
	if (className.contains(':'))
	{
		className = className.mid(className.lastIndexOf(':') + 1);
	}
	return className;
}


void AbstractState::onStateApprovedChanged()
{
	if (mContext->isStateApproved())
	{
		qCDebug(statemachine) << "Running state" << getStateName();
		run();
	}
}


void AbstractState::onEntry(QEvent* pEvent)
{
	Q_UNUSED(pEvent);
	Q_ASSERT(mConnections.isEmpty());
	if (mConnectOnCardRemoved)
	{
		mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &AbstractState::onCardRemoved);
		mConnections += connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &AbstractState::onCardRemoved);
	}
	mConnections += connect(mContext.data(), &WorkflowContext::fireCancelWorkflow, this, &AbstractState::onUserCancelled);
	mConnections += connect(mContext.data(), &WorkflowContext::fireStateApprovedChanged, this, &AbstractState::onStateApprovedChanged);

	qCDebug(statemachine) << "Next state" << getStateName();
	mContext->setCurrentState(getStateName());
}


void AbstractState::onExit(QEvent* pEvent)
{
	QState::onExit(pEvent);
	for (const auto& connection : qAsConst(mConnections))
	{
		QObject::disconnect(connection);
	}
	mConnections.clear();

	mContext->setStateApproved(false);

	qCDebug(statemachine) << "Leaving state" << getStateName() << "with result:";
	qCDebug(statemachine) << "        " << mContext->getResult().getMajor();
	qCDebug(statemachine) << "        " << mContext->getResult().getMinor();
	qCDebug(statemachine) << "        " << mContext->getResult().getMinorDescription();
	qCDebug(statemachine) << "        " << mContext->getResult().getMessage();
}


bool AbstractState::isCancellationByUser()
{
	return mContext->getResult().getMinor() == Result::Minor::SAL_Cancellation_by_User;
}


void AbstractState::onUserCancelled()
{
	qCInfo(support) << "Cancellation by user";
	setResult(Result::createCancelByUserError());
	Q_EMIT fireCancel();
}


void AbstractState::onCardRemoved(const QString& pReaderName)
{
	if (pReaderName == mContext->getReaderName())
	{
		QString errorMessage = tr("The ID card has been removed. The process is aborted.");
		setResult(Result::createCardRemovedError(errorMessage));
		Q_EMIT fireError();
	}
}


void AbstractState::setResult(const Result& pResult)
{
	if (!pResult.isOk() && mContext->getResult().isOk())
	{
		mContext->setResult(pResult);
	}
}
