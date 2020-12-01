/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "MessageDispatcher.h"

#include "messages/MsgHandlerAccessRights.h"
#include "messages/MsgHandlerApiLevel.h"
#include "messages/MsgHandlerAuth.h"
#include "messages/MsgHandlerBadState.h"
#include "messages/MsgHandlerCertificate.h"
#include "messages/MsgHandlerChangePin.h"
#include "messages/MsgHandlerEnterCan.h"
#include "messages/MsgHandlerEnterNewPin.h"
#include "messages/MsgHandlerEnterPin.h"
#include "messages/MsgHandlerEnterPuk.h"
#include "messages/MsgHandlerInfo.h"
#include "messages/MsgHandlerInsertCard.h"
#include "messages/MsgHandlerInternalError.h"
#include "messages/MsgHandlerInvalid.h"
#include "messages/MsgHandlerLog.h"
#include "messages/MsgHandlerReader.h"
#include "messages/MsgHandlerReaderList.h"
#include "messages/MsgHandlerUnknownCommand.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(json)

#define HANDLE_CURRENT_STATE(msgType, msgHandler) handleCurrentState(requestType, msgType, [&] {return msgHandler;})
#define HANDLE_INTERNAL_ONLY(msgHandler) handleInternalOnly(requestType, [&] {return msgHandler;})

using namespace governikus;


#if !defined(QT_NO_DEBUG) && __has_include(<QTest>)
#include <QTest>
char* governikus::toString(const MessageDispatcher::Msg& pMsg)
{
	return QTest::toString(QByteArray(pMsg));
}


#endif


MessageDispatcher::MessageDispatcher()
	: mContext()
{
}


QByteArray MessageDispatcher::init(const QSharedPointer<WorkflowContext>& pWorkflowContext)
{
	Q_ASSERT(!mContext.isActiveWorkflow());

	reset();
	mContext.setWorkflowContext(pWorkflowContext);

	if (mContext.getContext<AuthContext>())
	{
		return MsgHandlerAuth().getOutput();
	}
	else if (mContext.getContext<ChangePinContext>())
	{
		return MsgHandlerChangePin().getOutput();
	}

	return QByteArray();
}


void MessageDispatcher::reset()
{
	mContext.clear();
}


QByteArray MessageDispatcher::createMsgReader(const ReaderInfo& pInfo) const
{
	return MsgHandlerReader(pInfo).getOutput();
}


QByteArray MessageDispatcher::finish()
{
	Q_ASSERT(mContext.isActiveWorkflow());

	QByteArray result;
	if (auto authContext = mContext.getContext<AuthContext>())
	{
		result = MsgHandlerAuth(authContext).getOutput();
	}
	else if (auto changePinContext = mContext.getContext<ChangePinContext>())
	{
		result = MsgHandlerChangePin(changePinContext).getOutput();
	}

	reset();
	return result;
}


QByteArray MessageDispatcher::processStateChange(const QString& pState)
{
	if (!mContext.isActiveWorkflow() || pState.isEmpty())
	{
		qCCritical(json) << "Unexpected condition:" << mContext.getContext() << '|' << pState;
		return MsgHandlerInternalError(QLatin1String("Unexpected condition")).getOutput();
	}

	const auto& msg = createForStateChange(MsgHandler::getStateMsgType(pState, mContext.getContext()->getEstablishPaceChannelType()));
	mContext.addStateMsg(msg.getType());
	return msg.getOutput();
}


MsgHandler MessageDispatcher::createForStateChange(MsgType pStateType)
{
	switch (pStateType)
	{
		case MsgType::ENTER_PIN:
			return MsgHandlerEnterPin(mContext);

		case MsgType::ENTER_NEW_PIN:
			return MsgHandlerEnterNewPin(mContext);

		case MsgType::ENTER_CAN:
			return MsgHandlerEnterCan(mContext);

		case MsgType::ENTER_PUK:
			return MsgHandlerEnterPuk(mContext);

		case MsgType::ACCESS_RIGHTS:
			return MsgHandlerAccessRights(mContext);

		case MsgType::INSERT_CARD:
			return MsgHandlerInsertCard(mContext);

		default:
			mContext.getContext()->setStateApproved();
			return MsgHandler::Void;
	}
}


MessageDispatcher::Msg MessageDispatcher::processCommand(const QByteArray& pMsg)
{
	QJsonParseError jsonError;
	const auto& json = QJsonDocument::fromJson(pMsg, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		return MsgHandlerInvalid(jsonError);
	}

	const auto& obj = json.object();
	auto msg = createForCommand(obj);
	msg.setRequest(obj);
	return msg;
}


MsgHandler MessageDispatcher::createForCommand(const QJsonObject& pObj)
{
	const auto& cmd = pObj.value(QLatin1String("cmd")).toString();
	if (cmd.isEmpty())
	{
		return MsgHandlerInvalid(QLatin1String("Command cannot be undefined"));
	}

	auto requestType = Enum<MsgCmdType>::fromString(cmd, MsgCmdType::UNDEFINED);
	qCDebug(json) << "Process type:" << requestType;
	switch (requestType)
	{
		case MsgCmdType::UNDEFINED:
			return MsgHandlerUnknownCommand(cmd);

		case MsgCmdType::CANCEL:
			return cancel();

		case MsgCmdType::ACCEPT:
			return accept();

		case MsgCmdType::GET_API_LEVEL:
			return MsgHandlerApiLevel(mContext);

		case MsgCmdType::SET_API_LEVEL:
			return MsgHandlerApiLevel(pObj, mContext);

		case MsgCmdType::GET_READER:
			return MsgHandlerReader(pObj);

		case MsgCmdType::GET_READER_LIST:
			return MsgHandlerReaderList();

		case MsgCmdType::GET_LOG:
			return HANDLE_INTERNAL_ONLY(MsgHandlerLog());

		case MsgCmdType::GET_INFO:
			return MsgHandlerInfo();

		case MsgCmdType::RUN_AUTH:
			return mContext.isActiveWorkflow() ? MsgHandler(MsgHandlerBadState(requestType)) : MsgHandler(MsgHandlerAuth(pObj));

		case MsgCmdType::RUN_CHANGE_PIN:
			return mContext.isActiveWorkflow() ? MsgHandler(MsgHandlerBadState(requestType)) : MsgHandler(MsgHandlerChangePin(pObj));

		case MsgCmdType::GET_CERTIFICATE:
			return HANDLE_CURRENT_STATE(MsgType::ACCESS_RIGHTS, MsgHandlerCertificate(mContext));

		case MsgCmdType::SET_PIN:
			return HANDLE_CURRENT_STATE(MsgType::ENTER_PIN, MsgHandlerEnterPin(pObj, mContext));

		case MsgCmdType::SET_NEW_PIN:
			return HANDLE_CURRENT_STATE(MsgType::ENTER_NEW_PIN, MsgHandlerEnterNewPin(pObj, mContext));

		case MsgCmdType::SET_CAN:
			return HANDLE_CURRENT_STATE(MsgType::ENTER_CAN, MsgHandlerEnterCan(pObj, mContext));

		case MsgCmdType::SET_PUK:
			return HANDLE_CURRENT_STATE(MsgType::ENTER_PUK, MsgHandlerEnterPuk(pObj, mContext));

		case MsgCmdType::GET_ACCESS_RIGHTS:
			return HANDLE_CURRENT_STATE(MsgType::ACCESS_RIGHTS, MsgHandlerAccessRights(mContext));

		case MsgCmdType::SET_ACCESS_RIGHTS:
			return HANDLE_CURRENT_STATE(MsgType::ACCESS_RIGHTS, MsgHandlerAccessRights(pObj, mContext));
	}

	return MsgHandlerInternalError(QLatin1String("Cannot process request"));
}


MsgHandler MessageDispatcher::handleInternalOnly(MsgCmdType pCmdType, const std::function<MsgHandler()>& pFunc) const
{
#ifdef QT_NO_DEBUG
	Q_UNUSED(pFunc)
	return MsgHandlerUnknownCommand(getEnumName(pCmdType));

#else
	Q_UNUSED(pCmdType)
	return pFunc();

#endif
}


MsgHandler MessageDispatcher::handleCurrentState(MsgCmdType pCmdType, MsgType pMsgType, const std::function<MsgHandler()>& pFunc) const
{
	if (mContext.getLastStateMsg() == pMsgType)
	{
		return pFunc();
	}

	return MsgHandlerBadState(pCmdType);
}


MsgHandler MessageDispatcher::cancel()
{
	if (mContext.isActiveWorkflow())
	{
		Q_EMIT mContext.getContext()->fireCancelWorkflow();
		return MsgHandler::Void;
	}

	return MsgHandlerBadState(MsgCmdType::CANCEL);
}


MsgHandler MessageDispatcher::accept()
{
	if (mContext.getLastStateMsg() == MsgType::ACCESS_RIGHTS)
	{
		mContext.getContext()->setStateApproved();
		return MsgHandler::Void;
	}

	return MsgHandlerBadState(MsgCmdType::ACCEPT);
}


MessageDispatcher::Msg::Msg(const MsgHandler& pHandler)
	: mType(pHandler.getType())
	, mData(pHandler.getOutput())
{
}


MessageDispatcher::Msg::operator QByteArray() const
{
	return mData;
}


MessageDispatcher::Msg::operator MsgType() const
{
	return mType;
}
