/*
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgContext.h"

#include "MsgHandler.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jsonapi)

using namespace governikus;


MsgContext::MsgContext()
	: mApiLevel(MsgHandler::DEFAULT_MSG_LEVEL)
	, mStateMessages()
	, mContext()
{
}


bool MsgContext::isActiveWorkflow() const
{
	return mContext;
}


QSharedPointer<AuthContext> MsgContext::getAuthContext()
{
	if (mContext)
	{
		return mContext.dynamicCast<AuthContext>();
	}

	return QSharedPointer<AuthContext>();
}


QSharedPointer<const AuthContext> MsgContext::getAuthContext() const
{
	if (mContext)
	{
		return mContext.dynamicCast<const AuthContext>();
	}

	return QSharedPointer<const AuthContext>();
}


QSharedPointer<WorkflowContext> MsgContext::getWorkflowContext()
{
	return mContext;
}


QSharedPointer<const WorkflowContext> MsgContext::getWorkflowContext() const
{
	return mContext;
}


void MsgContext::setApiLevel(MsgLevel pApiLevel)
{
	mApiLevel = pApiLevel;
}


MsgLevel MsgContext::getApiLevel() const
{
	return mApiLevel;
}


MsgType MsgContext::getLastStateMsg() const
{
	if (mStateMessages.isEmpty())
	{
		return MsgType::INTERNAL_ERROR;
	}

	return mStateMessages.last();
}


void MsgDispatcherContext::setWorkflowContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
}


void MsgDispatcherContext::addStateMsg(MsgType pMsgType)
{
	mStateMessages += pMsgType;
}


void MsgDispatcherContext::clear()
{
	mStateMessages.clear();
	mContext.clear();
}
