/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Dispatch Messages of JSON API.
 */

#pragma once

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
	friend class ::test_Message;

	private:
		MsgDispatcherContext mContext;
#ifndef QT_NO_DEBUG
		using SkipStateApprovedHook = std::function<bool (const QString& pState)>;
		SkipStateApprovedHook mSkipStateApprovedHook;
#endif

		Msg createForStateChange(MsgType pStateType);
		MsgHandler createForCommand(const QJsonObject& pObj);

		MsgHandler cancel();
		MsgHandler accept();
		MsgHandler interrupt();
		MsgHandler handleCurrentState(MsgCmdType pCmdType, std::initializer_list<MsgType> pMsgType, const std::function<MsgHandler()>& pFunc) const;
		MsgHandler handleInternalOnly(MsgCmdType pCmdType, const std::function<MsgHandler()>& pFunc) const;

	public:
		MessageDispatcher();

		[[nodiscard]] Msg init(const QSharedPointer<WorkflowContext>& pWorkflowContext);
		[[nodiscard]] Msg finish();
		void reset();
		[[nodiscard]] Msg processCommand(const QByteArray& pMsg);
		[[nodiscard]] Msg processStateChange(const QString& pState);
		[[nodiscard]] Msg processProgressChange() const;
		[[nodiscard]] QList<Msg> processReaderChange(const ReaderInfo& pInfo);

#ifndef QT_NO_DEBUG
		void setSkipStateApprovedHook(const SkipStateApprovedHook& pHook);
#endif
};


} // namespace governikus
