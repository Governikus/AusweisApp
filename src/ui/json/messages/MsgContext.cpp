/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#include "MsgContext.h"

#include "MsgHandler.h"

#include <QLoggingCategory>

using namespace governikus;


MsgContext::MsgContext()
	: mApiLevel(MsgHandler::DEFAULT_MSG_LEVEL)
	, mStateMessages()
	, mContext()
{
}


bool MsgContext::isActiveWorkflow() const
{
	return !mContext.isNull();
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
