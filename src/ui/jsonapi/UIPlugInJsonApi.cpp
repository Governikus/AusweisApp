/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInJsonApi.h"

#include "ReaderManager.h"

#include <QLoggingCategory>
#include <QMetaMethod>

Q_DECLARE_LOGGING_CATEGORY(jsonapi)

using namespace governikus;

UIPlugInJsonApi::UIPlugInJsonApi()
	: UIPlugIn()
	, mMessageDispatcher()
	, mEnabled(false)
{
}


void UIPlugInJsonApi::setEnabled(bool pEnable)
{
	mEnabled = pEnable;
	const auto readerManager = Env::getSingleton<ReaderManager>();

	if (mEnabled)
	{
		connect(readerManager, &ReaderManager::fireReaderAdded, this, &UIPlugInJsonApi::onReaderEvent);
		connect(readerManager, &ReaderManager::fireReaderRemoved, this, &UIPlugInJsonApi::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardInserted, this, &UIPlugInJsonApi::onReaderEvent);
		connect(readerManager, &ReaderManager::fireCardRemoved, this, &UIPlugInJsonApi::onReaderEvent);
	}
	else
	{
		disconnect(readerManager, &ReaderManager::fireReaderAdded, this, &UIPlugInJsonApi::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireReaderRemoved, this, &UIPlugInJsonApi::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireCardInserted, this, &UIPlugInJsonApi::onReaderEvent);
		disconnect(readerManager, &ReaderManager::fireCardRemoved, this, &UIPlugInJsonApi::onReaderEvent);
	}
}


bool UIPlugInJsonApi::isEnabled() const
{
	return mEnabled;
}


void UIPlugInJsonApi::callFireMessage(const QByteArray& pMsg)
{
	if (!pMsg.isEmpty())
	{
		qCDebug(jsonapi).noquote() << "Fire message:" << pMsg;
		Q_EMIT fireMessage(pMsg);
	}
}


void UIPlugInJsonApi::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	if (!mEnabled)
	{
		return;
	}

	if (pContext.objectCast<AuthContext>())
	{
		connect(pContext.data(), &WorkflowContext::fireStateChanged, this, &UIPlugInJsonApi::onStateChanged);
	}

	callFireMessage(mMessageDispatcher.init(pContext));
}


void UIPlugInJsonApi::onWorkflowFinished(QSharedPointer<WorkflowContext> )
{
	if (!mEnabled)
	{
		mMessageDispatcher.reset();
		return;
	}

	callFireMessage(mMessageDispatcher.finish());
}


void UIPlugInJsonApi::onReaderEvent(const QString& pName)
{
	callFireMessage(mMessageDispatcher.createMsgReader(pName));
}


void UIPlugInJsonApi::onStateChanged(const QString& pNewState)
{
	callFireMessage(mMessageDispatcher.processStateChange(pNewState));
}


void UIPlugInJsonApi::doMessageProcessing(const QByteArray& pMsg)
{
	if (!mEnabled)
	{
		return;
	}

	callFireMessage(mMessageDispatcher.processCommand(pMsg));
}


void UIPlugInJsonApi::doShutdown()
{
}
