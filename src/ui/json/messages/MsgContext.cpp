/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MsgContext.h"

#include "MsgHandler.h"

#include <QLoggingCategory>

using namespace governikus;


MsgContext::MsgContext()
	: mApiLevel(MsgHandler::DEFAULT_MSG_LEVEL)
	, mStateMessages()
	, mProgressStatus(true)
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


Msg MsgContext::getLastStateMsg() const
{
	if (mStateMessages.isEmpty())
	{
		return Msg();
	}

	return mStateMessages.last();
}


bool MsgContext::provideProgressStatus() const
{
	return mProgressStatus;
}


void MsgContext::setProgressStatus(bool pStatus)
{
	mProgressStatus = pStatus;
}


void MsgContext::setWorkflowContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
}


void MsgContext::addStateMsg(const Msg& pMsg)
{
	mStateMessages += pMsg;
}


void MsgContext::clear()
{
	mStateMessages.clear();
	mContext.clear();
}
