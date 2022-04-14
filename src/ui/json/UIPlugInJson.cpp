/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInJson.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "messages/MsgTypes.h"
#include "ReaderManager.h"

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
		connect(readerManager, &ReaderManager::fireCardInserted, this, &UIPlugInJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardRemoved, this, &UIPlugInJson::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardRetryCounterChanged, this, &UIPlugInJson::onReaderEvent);
	}
	else
	{
		disconnect(readerManager, &ReaderManager::fireReaderAdded, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireReaderRemoved, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireCardInserted, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireCardRemoved, this, &UIPlugInJson::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireCardRetryCounterChanged, this, &UIPlugInJson::onReaderEvent);
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


void UIPlugInJson::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	if (!mEnabled)
	{
		return;
	}

	if (pContext.objectCast<AuthContext>() || pContext.objectCast<ChangePinContext>())
	{
		connect(pContext.data(), &WorkflowContext::fireStateChanged, this, &UIPlugInJson::onStateChanged);
	}

	callFireMessage(mMessageDispatcher.init(pContext));
}


void UIPlugInJson::onWorkflowFinished(QSharedPointer<WorkflowContext>)
{
	if (!mEnabled)
	{
		mMessageDispatcher.reset();
		return;
	}

	callFireMessage(mMessageDispatcher.finish());
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
