/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInJson.h"

#include "ReaderManager.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "messages/MsgTypes.h"

#include <QLoggingCategory>
#include <QMetaMethod>

Q_DECLARE_LOGGING_CATEGORY(json)

using namespace governikus;

UIPlugInJson::UIPlugInJson()
	: UIPlugIn()
	, mMessageDispatcher()
	, mEnabled(false)
{
}


void UIPlugInJson::setEnabled(bool pEnable)
{
	mEnabled = pEnable;
	const auto readerManager = Env::getSingleton<ReaderManager>();

	if (mEnabled)
	{
		connect(readerManager, &ReaderManager::fireReaderAdded, this, &UIPlugInJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireReaderRemoved, this, &UIPlugInJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &UIPlugInJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardInserted, this, &UIPlugInJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardRemoved, this, &UIPlugInJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardInfoChanged, this, &UIPlugInJson::onCardInfoChanged);
	}
	else
	{
		disconnect(readerManager, &ReaderManager::fireReaderAdded, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireReaderRemoved, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireCardInserted, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireCardRemoved, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireCardInfoChanged, this, &UIPlugInJson::onCardInfoChanged);
	}
}


bool UIPlugInJson::isEnabled() const
{
	return mEnabled;
}


void UIPlugInJson::callFireMessage(const QByteArray& pMsg, bool pLogging)
{
	if (!pMsg.isEmpty())
	{
		if (Q_LIKELY(pLogging))
		{
			qCDebug(json).noquote() << "Fire message:" << pMsg;
		}
		Q_EMIT fireMessage(pMsg);
	}
}


void UIPlugInJson::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (!mEnabled)
	{
		return;
	}

	const auto& context = pRequest->getContext();
	if (context.objectCast<AuthContext>() || context.objectCast<ChangePinContext>())
	{
		connect(context.data(), &WorkflowContext::fireStateChanged, this, &UIPlugInJson::onStateChanged);
		connect(context.data(), &WorkflowContext::fireProgressChanged, this, &UIPlugInJson::onProgressChanged);
	}

	callFireMessage(mMessageDispatcher.init(context));
}


void UIPlugInJson::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)

	if (!mEnabled)
	{
		mMessageDispatcher.reset();
		return;
	}

	callFireMessage(mMessageDispatcher.finish());
}


void UIPlugInJson::onCardInfoChanged(const ReaderInfo& pInfo)
{
	if (pInfo.hasEid())
	{
		onReaderEvent(pInfo);
	}
}


void UIPlugInJson::onReaderEvent(const ReaderInfo& pInfo)
{
	const auto& messages = mMessageDispatcher.processReaderChange(pInfo);
	for (const auto& msg : messages)
	{
		callFireMessage(msg);
	}
}


void UIPlugInJson::onStateChanged(const QString& pNewState)
{
	callFireMessage(mMessageDispatcher.processStateChange(pNewState));
}


void UIPlugInJson::onProgressChanged()
{
	callFireMessage(mMessageDispatcher.processProgressChange());
}


void UIPlugInJson::doMessageProcessing(const QByteArray& pMsg)
{
	if (!mEnabled)
	{
		return;
	}

	const auto& msg = mMessageDispatcher.processCommand(pMsg);
	callFireMessage(msg, msg != MsgType::LOG);
}


void UIPlugInJson::doShutdown()
{
}
