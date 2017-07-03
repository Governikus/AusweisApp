/*!
 * \brief Dispatch Messages of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "context/WorkflowContext.h"
#include "messages/MsgContext.h"
#include "messages/MsgHandler.h"

#include <QJsonDocument>
#include <QString>

#include <functional>

namespace governikus
{

class MessageDispatcher
{
	private:
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
		QByteArray processCommand(const QByteArray& pMsg);
		QByteArray processStateChange(const QString& pState);

		QByteArray createMsgReader(const QString& pName) const;
};

} /* namespace governikus */
