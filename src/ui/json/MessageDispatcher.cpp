/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MessageDispatcher.h"

#include "VolatileSettings.h"
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
#include "messages/MsgHandlerStatus.h"
#include "messages/MsgHandlerUnknownCommand.h"

#ifdef Q_OS_IOS
	#include "ReaderManager.h"
#endif
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
	#include "messages/MsgHandlerPersonalization.h"
#endif

#include <QLoggingCategory>
#include <QScopeGuard>

#include <algorithm>


Q_DECLARE_LOGGING_CATEGORY(json)


#define HANDLE_CURRENT_STATE(msgType, msgHandler) handleCurrentState(requestType, msgType, [&] {return msgHandler;})
#define HANDLE_INTERNAL_ONLY(msgHandler) handleInternalOnly(requestType, [&] {return msgHandler;})


using namespace governikus;


MessageDispatcher::MessageDispatcher()
	: mContext()
#ifndef QT_NO_DEBUG
	, mSkipStateApprovedHook()
#endif
{
}


Msg MessageDispatcher::init(const QSharedPointer<WorkflowContext>& pWorkflowContext)
{
	Q_ASSERT(!mContext.isActiveWorkflow());

	mContext.clear();
	mContext.setWorkflowContext(pWorkflowContext);

#if __has_include("context/PersonalizationContext.h")
	if (mContext.getContext<PersonalizationContext>())
	{
		return MsgHandlerPersonalization();
	}
#endif

	if (mContext.getContext<AuthContext>())
	{
		return MsgHandlerAuth();
	}

	if (mContext.getContext<ChangePinContext>())
	{
		return MsgHandlerChangePin();
	}

	return MsgHandler::Void;
}


void MessageDispatcher::reset()
{
	mContext.clear();
	Env::getSingleton<VolatileSettings>()->setMessages();
	Env::getSingleton<VolatileSettings>()->setHandleInterrupt();
	Env::getSingleton<VolatileSettings>()->setDeveloperMode();
}


Msg MessageDispatcher::finish()
{
	Q_ASSERT(mContext.isActiveWorkflow());

	const auto guard = qScopeGuard([this] {
			reset();
		});

#if __has_include("context/PersonalizationContext.h")
	if (auto personalizationContext = mContext.getContext<PersonalizationContext>())
	{
		return MsgHandlerPersonalization(personalizationContext);
	}
#endif

	if (auto authContext = mContext.getContext<AuthContext>())
	{
		return MsgHandlerAuth(authContext);
	}

	if (auto changePinContext = mContext.getContext<ChangePinContext>())
	{
		return MsgHandlerChangePin(changePinContext);
	}

	return MsgHandler::Void;
}


Msg MessageDispatcher::processStateChange(const QString& pState)
{
	if (!mContext.isActiveWorkflow() || pState.isEmpty())
	{
		qCCritical(json) << "Unexpected condition:" << mContext.getContext() << '|' << pState;
		return MsgHandlerInternalError(QLatin1String("Unexpected condition"));
	}

#ifndef QT_NO_DEBUG
	if (mSkipStateApprovedHook && mSkipStateApprovedHook(pState))
	{
		return MsgHandler::Void;
	}
#endif

	const Msg& msg = createForStateChange(MsgHandler::getStateMsgType(pState, mContext.getContext()->getEstablishPaceChannelType()));
	mContext.addStateMsg(msg);
	return msg;
}


Msg MessageDispatcher::processProgressChange() const
{
	if (mContext.getApiLevel() < MsgLevel::v2)
	{
		return MsgHandler::Void;
	}

	if (!mContext.isActiveWorkflow())
	{
		qCCritical(json) << "Unexpected condition:" << mContext.getContext();
		return MsgHandlerInternalError(QLatin1String("Unexpected condition"));
	}

	return mContext.provideProgressStatus() ? MsgHandlerStatus(mContext) : MsgHandler::Void;
}


QList<Msg> MessageDispatcher::processReaderChange(const ReaderInfo& pInfo)
{
	QList<Msg> messages({MsgHandlerReader(pInfo)});

	const auto& lastStateMsg = mContext.getLastStateMsg();
	if (lastStateMsg == MsgType::INSERT_CARD && !lastStateMsg)
	{
		const MsgHandlerInsertCard msg;
		mContext.addStateMsg(msg);
		messages << msg;
	}

	return messages;
}


Msg MessageDispatcher::createForStateChange(MsgType pStateType)
{
	if (mContext.getContext()->isWorkflowCancelled())
	{
		mContext.getContext()->setStateApproved();
		return MsgHandler::Void;
	}

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


Msg MessageDispatcher::processCommand(const QByteArray& pMsg)
{
	QJsonParseError jsonError {};
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

		case MsgCmdType::INTERRUPT:
			return interrupt();

		case MsgCmdType::GET_API_LEVEL:
			return MsgHandlerApiLevel(mContext);

		case MsgCmdType::SET_API_LEVEL:
			return MsgHandlerApiLevel(pObj, mContext);

		case MsgCmdType::GET_READER:
			return MsgHandlerReader(pObj);

		case MsgCmdType::GET_READER_LIST:
			return MsgHandlerReaderList(mContext);

		case MsgCmdType::GET_STATUS:
			if (mContext.getApiLevel() < MsgLevel::v2)
			{
				return MsgHandlerUnknownCommand(cmd);
			}
			return MsgHandlerStatus(mContext);

		case MsgCmdType::GET_LOG:
			return HANDLE_INTERNAL_ONLY(MsgHandlerLog());

		case MsgCmdType::GET_INFO:
			return MsgHandlerInfo();

		case MsgCmdType::RUN_AUTH:
			return mContext.isActiveWorkflow() ? MsgHandler(MsgHandlerBadState(requestType)) : MsgHandler(MsgHandlerAuth(pObj, mContext));

		case MsgCmdType::RUN_PERSONALIZATION:
#if __has_include("context/PersonalizationContext.h")
			return mContext.isActiveWorkflow() ? MsgHandler(MsgHandlerBadState(requestType)) : MsgHandler(MsgHandlerPersonalization(pObj, mContext));

#else
			return MsgHandlerUnknownCommand(requestType);

#endif

		case MsgCmdType::RUN_CHANGE_PIN:
			return mContext.isActiveWorkflow() ? MsgHandler(MsgHandlerBadState(requestType)) : MsgHandler(MsgHandlerChangePin(pObj, mContext));

		case MsgCmdType::GET_CERTIFICATE:
			return HANDLE_CURRENT_STATE({MsgType::ACCESS_RIGHTS}, MsgHandlerCertificate(mContext));

		case MsgCmdType::SET_CARD:
			if (mContext.getApiLevel() < MsgLevel::v2)
			{
				return MsgHandlerUnknownCommand(cmd);
			}
			return HANDLE_CURRENT_STATE({MsgType::INSERT_CARD}, MsgHandlerInsertCard(pObj, mContext));

		case MsgCmdType::SET_PIN:
			return HANDLE_CURRENT_STATE({MsgType::ENTER_PIN}, MsgHandlerEnterPin(pObj, mContext));

		case MsgCmdType::SET_NEW_PIN:
			return HANDLE_CURRENT_STATE({MsgType::ENTER_NEW_PIN}, MsgHandlerEnterNewPin(pObj, mContext));

		case MsgCmdType::SET_CAN:
			return HANDLE_CURRENT_STATE({MsgType::ENTER_CAN}, MsgHandlerEnterCan(pObj, mContext));

		case MsgCmdType::SET_PUK:
			return HANDLE_CURRENT_STATE({MsgType::ENTER_PUK}, MsgHandlerEnterPuk(pObj, mContext));

		case MsgCmdType::GET_ACCESS_RIGHTS:
			return HANDLE_CURRENT_STATE({MsgType::ACCESS_RIGHTS}, MsgHandlerAccessRights(mContext));

		case MsgCmdType::SET_ACCESS_RIGHTS:
			return HANDLE_CURRENT_STATE({MsgType::ACCESS_RIGHTS}, MsgHandlerAccessRights(pObj, mContext));
	}

	return MsgHandlerInternalError(QLatin1String("Cannot process request"));
}


MsgHandler MessageDispatcher::handleInternalOnly(MsgCmdType pCmdType, const std::function<MsgHandler()>& pFunc) const
{
#ifdef QT_NO_DEBUG
	Q_UNUSED(pFunc)
	return MsgHandlerUnknownCommand(pCmdType);

#else
	Q_UNUSED(pCmdType)
	return pFunc();

#endif
}


MsgHandler MessageDispatcher::handleCurrentState(MsgCmdType pCmdType, std::initializer_list<MsgType> pMsgType, const std::function<MsgHandler()>& pFunc) const
{
	if (std::any_of(pMsgType.begin(), pMsgType.end(), [this](const MsgType pCurrentType)
			{
				return mContext.getLastStateMsg() == pCurrentType;
			}))
	{
		return pFunc();
	}

	return MsgHandlerBadState(pCmdType);
}


MsgHandler MessageDispatcher::cancel()
{
	if (mContext.isActiveWorkflow())
	{
		mContext.getContext()->killWorkflow();
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


MsgHandler MessageDispatcher::interrupt()
{
	const auto cmdType = MsgCmdType::INTERRUPT;
#ifdef Q_OS_IOS
	{
		const auto allowedStates = {MsgType::ENTER_PIN, MsgType::ENTER_CAN, MsgType::ENTER_PUK, MsgType::ENTER_NEW_PIN};
		const auto& workflowContext = mContext.getContext();
		return handleCurrentState(cmdType, allowedStates, [&workflowContext] {
				workflowContext->setInterruptRequested(true);
				switch (workflowContext->getLastPaceResult())
				{
						case CardReturnCode::OK:
						case CardReturnCode::OK_PUK:
							Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPlugInType::NFC); // Null string is interpreted as 'success'
							break;

						default:
							Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPlugInType::NFC, Env::getSingleton<VolatileSettings>()->getMessages().getSessionFailed());
				}

				return MsgHandler::Void;
			});
	}
#else
	return MsgHandlerUnknownCommand(cmdType);

#endif
}


#ifndef QT_NO_DEBUG
void MessageDispatcher::setSkipStateApprovedHook(const SkipStateApprovedHook& pHook)
{
	mSkipStateApprovedHook = pHook;
}


#endif
