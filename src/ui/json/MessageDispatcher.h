/*!
 * \brief Dispatch Messages of JSON API.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "context/WorkflowContext.h"
#include "messages/MsgContext.h"
#include "messages/MsgHandler.h"

#include <QJsonDocument>
#include <QString>

#include <functional>

class test_Message;

namespace governikus
{

class MessageDispatcher
{
	private:
		friend class ::test_Message;

		MsgDispatcherContext mContext;

		MsgHandler createForStateChange(MsgType pStateType);
		MsgHandler createForCommand(const QJsonObject& pObj);

		MsgHandler cancel();
		MsgHandler accept();
		MsgHandler handleCurrentState(MsgCmdType pCmdType, MsgType pMsgType, const std::function<MsgHandler()>& pFunc);
		MsgHandler handleInternalOnly(MsgCmdType pCmdType, const std::function<MsgHandler()>& pFunc);

	public:
		class Msg final
		{
			friend class MessageDispatcher;
			const MsgType mType;
			const QByteArray mData;

			Msg(const MsgHandler& pHandler);

			public:
				operator QByteArray() const;
				operator MsgType() const;
		};

		MessageDispatcher();

		QByteArray init(const QSharedPointer<WorkflowContext>& pWorkflowContext);
		QByteArray finish();
		void reset();
		Msg processCommand(const QByteArray& pMsg);
		QByteArray processStateChange(const QString& pState);

		QByteArray createMsgReader(const QString& pName) const;
};

} // namespace governikus
