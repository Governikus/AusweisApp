/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "AuthModel.h"
#include "context/AuthContext.h"
#include "ReaderManagerPlugInInfo.h"

using namespace governikus;


AuthModel::AuthModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
{
}


AuthModel::~AuthModel()
{
}


void AuthModel::resetContext(QSharedPointer<AuthContext> pContext)
{
	mContext = pContext;
	if (mContext)
	{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
		mContext->setReaderType(ReaderManagerPlugInType::NFC);
#else
		mContext->setReaderType(ReaderManagerPlugInType::PCSC);
#endif
		connect(mContext.data(), &AuthContext::fireCurrentStateChanged, this, &AuthModel::fireCurrentStateChanged);
		connect(mContext.data(), &WorkflowContext::fireResultChanged, this, &AuthModel::fireResultChanged);
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
	return mContext ? mContext->getResult().getMessage() : QString();
}


bool AuthModel::isError() const
{
	return mContext && !mContext->getResult().isOk();
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


void AuthModel::setReaderType(const QString& pReaderType)
{
	if (mContext)
	{
		mContext->setReaderType(EnumReaderManagerPlugInType::fromString(pReaderType, ReaderManagerPlugInType::UNKNOWN));
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
