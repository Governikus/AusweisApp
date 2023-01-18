/*!
 * \brief Context of JSON API.
 *
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Msg.h"
#include "MsgTypes.h"
#include "context/WorkflowContext.h"

namespace governikus
{

class MsgContext
{
	Q_DISABLE_COPY(MsgContext)

	private:
		MsgLevel mApiLevel;
		QVector<Msg> mStateMessages;
		bool mProgressStatus;
		QSharedPointer<WorkflowContext> mContext;

	protected:
		void addStateMsg(const Msg& pMsg);
		void clear();
		void setWorkflowContext(const QSharedPointer<WorkflowContext>& pContext);

	public:
		MsgContext();

		void setApiLevel(MsgLevel pApiLevel);
		[[nodiscard]] MsgLevel getApiLevel() const;

		[[nodiscard]] Msg getLastStateMsg() const;

		[[nodiscard]] bool provideProgressStatus() const;
		void setProgressStatus(bool pStatus);

		[[nodiscard]] bool isActiveWorkflow() const;

		template<typename T = WorkflowContext>
		QSharedPointer<const T> getContext() const
		{
			static_assert(std::is_base_of_v<WorkflowContext, T>, "T must derive WorkflowContext");

			if (mContext)
			{
				return mContext.objectCast<const T>();
			}

			return QSharedPointer<const T>();
		}


		template<typename T = WorkflowContext>
		QSharedPointer<T> getContext()
		{
			static_assert(std::is_base_of_v<WorkflowContext, T>, "T must derive WorkflowContext");

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
		using MsgContext::addStateMsg;
		using MsgContext::clear;
		using MsgContext::setWorkflowContext;
};

} // namespace governikus
