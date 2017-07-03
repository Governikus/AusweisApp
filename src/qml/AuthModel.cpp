/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "AuthModel.h"

#include "ReaderManagerPlugInInfo.h"
#include "context/AuthContext.h"

using namespace governikus;


AuthModel::AuthModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
	, mTransactionInfo()
{
}


AuthModel::~AuthModel()
{
}


void AuthModel::resetContext(const QSharedPointer<AuthContext>& pContext)
{
	if (!mTransactionInfo.isEmpty())
	{
		mTransactionInfo.clear();

		Q_EMIT fireTransactionInfoChanged();
	}

	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &AuthContext::fireCurrentStateChanged, this, &AuthModel::fireCurrentStateChanged);
		connect(mContext.data(), &WorkflowContext::fireResultChanged, this, &AuthModel::fireResultChanged);
		connect(mContext.data(), &AuthContext::fireDidAuthenticateEac1Changed, this, &AuthModel::onDidAuthenticateEac1Changed);
	}

	/*
	 * Only this state change is emitted when the context is reset, i.e. after the end of the workflow
	 */
	Q_EMIT fireCurrentStateChanged(getCurrentState());
}


QString AuthModel::getCurrentState() const
{
	return mContext ? mContext->getCurrentState() : QString();
}


QString AuthModel::getResultString() const
{
	return mContext ? mContext->getStatus().toErrorDescription(true) : QString();
}


bool AuthModel::isError() const
{
	return mContext && mContext->getStatus().isError();
}


const QString& AuthModel::getTransactionInfo() const
{
	return mTransactionInfo;
}


void AuthModel::continueWorkflow()
{
	if (mContext)
	{
		mContext->setStateApproved();
	}
}


void AuthModel::cancelWorkflow()
{
	if (mContext)
	{
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


void AuthModel::cancelWorkflowOnPinBlocked()
{
	if (mContext)
	{
		mContext->setStatus(GlobalStatus::Code::Workflow_Pin_Blocked_And_Puk_Objectionable);
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


void AuthModel::setReaderType(const QString& pReaderType)
{
	if (mContext)
	{
		mContext->setReaderType(Enum<ReaderManagerPlugInType>::fromString(pReaderType, ReaderManagerPlugInType::UNKNOWN));
	}
}


bool AuthModel::isBasicReader()
{
	if (mContext)
	{
		return mContext->getCardConnection()->getReaderInfo().isBasicReader();
	}

	return true;
}


void AuthModel::abortCardSelection()
{
	if (mContext)
	{
		Q_EMIT mContext->fireAbortCardSelection();
	}
}


void AuthModel::onDidAuthenticateEac1Changed()
{
	if (mContext)
	{
		const QSharedPointer<DIDAuthenticateEAC1>& didAuthenticateEAC1 = mContext->getDidAuthenticateEac1();
		const QString newTransactionInfo = didAuthenticateEAC1.isNull() ? QString() : didAuthenticateEAC1->getTransactionInfo();
		if (newTransactionInfo != mTransactionInfo)
		{
			mTransactionInfo = newTransactionInfo;

			Q_EMIT fireTransactionInfoChanged();
		}
	}
}
