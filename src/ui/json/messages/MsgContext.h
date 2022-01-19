/*!
 * \brief Context of JSON API.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

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
		[[nodiscard]] MsgLevel getApiLevel() const;

		[[nodiscard]] MsgType getLastStateMsg() const;

		[[nodiscard]] bool isActiveWorkflow() const;

		template<typename T = WorkflowContext>
		QSharedPointer<const T> getContext() const
		{
			static_assert(std::is_base_of<WorkflowContext, T>::value, "T must derive WorkflowContext");

			if (mContext)
			{
				return mContext.objectCast<const T>();
			}

			return QSharedPointer<const T>();
		}


		template<typename T = WorkflowContext>
		QSharedPointer<T> getContext()
		{
			static_assert(std::is_base_of<WorkflowContext, T>::value, "T must derive WorkflowContext");

			if (mContext)
			{
				return mContext.objectCast<T>();
			}

			return QSharedPointer<T>();
		}


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
