/*
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "MessageDispatcher.h"

#include "messages/MsgHandlerAccessRights.h"
#include "messages/MsgHandlerApiLevel.h"
#include "messages/MsgHandlerAuth.h"
#include "messages/MsgHandlerBadState.h"
#include "messages/MsgHandlerCertificate.h"
#include "messages/MsgHandlerEnterCan.h"
#include "messages/MsgHandlerEnterPin.h"
#include "messages/MsgHandlerEnterPuk.h"
#include "messages/MsgHandlerInfo.h"
#include "messages/MsgHandlerInsertCard.h"
#include "messages/MsgHandlerInternalError.h"
#include "messages/MsgHandlerInvalid.h"
#include "messages/MsgHandlerReader.h"
#include "messages/MsgHandlerReaderList.h"
#include "messages/MsgHandlerUnknownCommand.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jsonapi)

#define HANDLE_CURRENT_STATE(msgType, msgHandler) handleCurrentState(requestType, msgType, [&] {return msgHandler;});

using namespace governikus;


MessageDispatcher::MessageDispatcher()
	: mContext()
{
}


QByteArray MessageDispatcher::init(const QSharedPointer<WorkflowContext>& pContext)
{
	Q_ASSERT(!mContext.isActiveWorkflow());

	reset();
	mContext.setWorkflowContext(pContext);

	if (mContext.getAuthContext())
	{
		return MsgHandlerAuth().getOutput();
	}

	return QByteArray();
}


void MessageDispatcher::reset()
{
	mContext.clear();
}


QByteArray MessageDispatcher::createMsgReader(const QString& pName) const
{
	return MsgHandlerReader(pName).getOutput();
}


QByteArray MessageDispatcher::finish()
{
	Q_ASSERT(mContext.isActiveWorkflow());

	QByteArray result;
	if (auto authContext = mContext.getAuthContext())
	{
		result = MsgHandlerAuth(authContext).getOutput();
	}

	reset();
	return result;
}


QByteArray MessageDispatcher::processStateChange(const QString& pState)
{
	if (!mContext.isActiveWorkflow() || pState.isEmpty())
	{
		qCritical(jsonapi) << "Unexpected condition:" << mContext.getWorkflowContext() << "|" << pState;
		return MsgHandlerInternalError(QLatin1String("Unexpected condition")).getOutput();
	}

	const auto& msg = createForStateChange(MsgHandler::getStateMsgType(pState, mContext.getWorkflowContext()->getEstablishPaceChannelType()));
	mContext.addStateMsg(msg.getType());
	return msg.getOutput();
}


MsgHandler MessageDispatcher::createForStateChange(MsgType pStateType)
{
	switch (pStateType)
	{
		case MsgType::ENTER_PIN:
			return MsgHandlerEnterPin(mContext);

		case MsgType::ENTER_CAN:
			return MsgHandlerEnterCan(mContext);

		case MsgType::ENTER_PUK:
			return MsgHandlerEnterPuk(mContext);

		case MsgType::ACCESS_RIGHTS:
			return MsgHandlerAccessRights(mContext);

		case MsgType::INSERT_CARD:
			return MsgHandlerInsertCard(mContext);

		default:
			mContext.getWorkflowContext()->setStateApproved();
			return MsgHandler::Void;
	}
}


QByteArray MessageDispatcher::processCommand(const QByteArray& pMsg)
{
	QJsonParseError jsonError;
	const auto& json = QJsonDocument::fromJson(pMsg, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		return MsgHandlerInvalid(jsonError).getOutput();
	}

	const auto& obj = json.object();
	auto msg = createForCommand(obj);
	msg.setRequest(obj);
	return msg.getOutput();
}


MsgHandler MessageDispatcher::createForCommand(const QJsonObject& pObj)
{
	const auto& cmd = pObj.value(QLatin1String("cmd")).toString();
	if (cmd.isEmpty())
	{
		return MsgHandlerInvalid(QLatin1String("Command cannot be undefined"));
	}

	auto requestType = Enum<MsgCmdType>::fromString(cmd, MsgCmdType::UNDEFINED);
	qDebug(jsonapi) << "Process type:" << requestType;
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

		case MsgCmdType::GET_INFO:
			return MsgHandlerInfo();

		case MsgCmdType::RUN_AUTH:
			return mContext.isActiveWorkflow() ? MsgHandler(MsgHandlerBadState(requestType)) : MsgHandler(MsgHandlerAuth(pObj));

		case MsgCmdType::GET_CERTIFICATE:
			return HANDLE_CURRENT_STATE(MsgType::ACCESS_RIGHTS, MsgHandlerCertificate(mContext));

		case MsgCmdType::SET_PIN:
			return HANDLE_CURRENT_STATE(MsgType::ENTER_PIN, MsgHandlerEnterPin(pObj, mContext));

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


MsgHandler MessageDispatcher::handleCurrentState(MsgCmdType pCmdType, MsgType pMsgType, const std::function<MsgHandler()>& pFunc)
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
		Q_EMIT mContext.getWorkflowContext()->fireCancelWorkflow();
		return MsgHandler::Void;
	}

	return MsgHandlerBadState(MsgCmdType::CANCEL);
}


MsgHandler MessageDispatcher::accept()
{
	if (mContext.getLastStateMsg() == MsgType::ACCESS_RIGHTS)
	{
		mContext.getWorkflowContext()->setStateApproved();
		return MsgHandler::Void;
	}

	return MsgHandlerBadState(MsgCmdType::ACCEPT);
}
