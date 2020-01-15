/*!
 * \brief Context of JSON API.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "context/WorkflowContext.h"
#include "MsgTypes.h"

namespace governikus
{

class MsgContext
{
	Q_DISABLE_COPY(MsgContext)

	protected:
		MsgLevel mApiLevel;
		QList<MsgType> mStateMessages;
		QSharedPointer<WorkflowContext> mContext;

	public:
		MsgContext();

		void setApiLevel(MsgLevel pApiLevel);
		MsgLevel getApiLevel() const;

		MsgType getLastStateMsg() const;

		bool isActiveWorkflow() const;

		QSharedPointer<AuthContext> getAuthContext();
		QSharedPointer<const AuthContext> getAuthContext() const;

		QSharedPointer<WorkflowContext> getWorkflowContext();
		QSharedPointer<const WorkflowContext> getWorkflowContext() const;
};

class MsgDispatcherContext
	: public MsgContext
{
	public:
		void clear();
		void addStateMsg(MsgType pMsgType);
		void setWorkflowContext(const QSharedPointer<WorkflowContext>& pContext);
};

} // namespace governikus
