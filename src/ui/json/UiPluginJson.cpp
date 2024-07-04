/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginJson.h"

#include "ReaderManager.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "messages/MsgTypes.h"

#include <QLoggingCategory>
#include <QMetaMethod>

Q_DECLARE_LOGGING_CATEGORY(json)

using namespace governikus;

UiPluginJson::UiPluginJson()
	: UiPlugin()
	, mMessageDispatcher()
	, mEnabled(false)
{
}


void UiPluginJson::setEnabled(bool pEnable)
{
	if (mEnabled == pEnable)
	{
		return;
	}

	mEnabled = pEnable;
	const auto* readerManager = Env::getSingleton<ReaderManager>();

	if (mEnabled)
	{
		connect(readerManager, &ReaderManager::fireReaderAdded, this, &UiPluginJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireReaderRemoved, this, &UiPluginJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &UiPluginJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardInserted, this, &UiPluginJson::onCardInserted);
		connect(readerManager, &ReaderManager::fireCardRemoved, this, &UiPluginJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardInfoChanged, this, &UiPluginJson::onCardInfoChanged);
	}
	else
	{
		readerManager->disconnect(this);
	}
}


bool UiPluginJson::isEnabled() const
{
	return mEnabled;
}


void UiPluginJson::callFireMessage(const QByteArray& pMsg, bool pLogging)
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


void UiPluginJson::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (!mEnabled)
	{
		return;
	}

	const auto& context = pRequest->getContext();
	if (context.objectCast<AuthContext>() || context.objectCast<ChangePinContext>())
	{
		connect(context.data(), &WorkflowContext::fireStateChanged, this, &UiPluginJson::onStateChanged);
		connect(context.data(), &WorkflowContext::fireProgressChanged, this, &UiPluginJson::onProgressChanged);
	}

	callFireMessage(mMessageDispatcher.init(context));
}


void UiPluginJson::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)

	if (!mEnabled)
	{
		mMessageDispatcher.reset();
		return;
	}

	callFireMessage(mMessageDispatcher.finish());
}


void UiPluginJson::onCardInfoChanged(const ReaderInfo& pInfo)
{
	if (pInfo.hasEid())
	{
		onReaderEvent(pInfo);
	}
}


void UiPluginJson::onReaderEvent(const ReaderInfo& pInfo)
{
	const auto& messages = mMessageDispatcher.processReaderChange(pInfo);
	for (const auto& msg : messages)
	{
		callFireMessage(msg);
	}
}


void UiPluginJson::onCardInserted(const ReaderInfo& pInfo)
{
	if (pInfo.hasEid() || (pInfo.hasCard() && mMessageDispatcher.getApiLevel() > MsgLevel::v2))
	{
		onReaderEvent(pInfo);
	}
}


void UiPluginJson::onStateChanged(const QString& pNewState)
{
	callFireMessage(mMessageDispatcher.processStateChange(pNewState));
}


void UiPluginJson::onProgressChanged()
{
	callFireMessage(mMessageDispatcher.processProgressChange());
}


void UiPluginJson::doMessageProcessing(const QByteArray& pMsg)
{
	if (!mEnabled)
	{
		return;
	}

	const auto& msg = mMessageDispatcher.processCommand(pMsg);
	callFireMessage(msg, msg != MsgType::LOG);
}


void UiPluginJson::doShutdown()
{
}
