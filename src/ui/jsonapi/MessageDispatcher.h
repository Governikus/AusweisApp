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

	public:
		MessageDispatcher();

		QByteArray init(const QSharedPointer<WorkflowContext>& pContext);
		QByteArray finish();
		void reset();
		QByteArray processCommand(const QByteArray& pMsg);
		QByteArray processStateChange(const QString& pState);

		QByteArray createMsgReader(const QString& pName) const;
};

} // namespace governikus
