/*!
 * \brief Dispatch Messages of JSON API.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "context/WorkflowContext.h"
#include "messages/Msg.h"
#include "messages/MsgContext.h"
#include "messages/MsgHandler.h"

#include <QJsonDocument>
#include <QString>

#include <functional>
#include <initializer_list>

class test_Message;

namespace governikus
{

class MessageDispatcher
{
	private:
		friend class ::test_Message;

		MsgDispatcherContext mContext;

		Msg createForStateChange(MsgType pStateType);
		MsgHandler createForCommand(const QJsonObject& pObj);

		MsgHandler cancel();
		MsgHandler accept();
		MsgHandler interrupt();
		MsgHandler handleCurrentState(MsgCmdType pCmdType, std::initializer_list<MsgType> pMsgType, const std::function<MsgHandler()>& pFunc) const;
		MsgHandler handleInternalOnly(MsgCmdType pCmdType, const std::function<MsgHandler()>& pFunc) const;

	public:
		MessageDispatcher();

		QByteArray init(const QSharedPointer<WorkflowContext>& pWorkflowContext);
		QByteArray finish();
		void reset();
		Msg processCommand(const QByteArray& pMsg);
		QByteArray processStateChange(const QString& pState);

		[[nodiscard]] QByteArrayList processReaderChange(const ReaderInfo& pInfo);
};

} // namespace governikus
