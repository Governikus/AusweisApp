/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "UIPlugInJsonApi.h"

#include "ReaderManager.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(jsonapi)

using namespace governikus;

UIPlugInJsonApi::UIPlugInJsonApi()
	: UIPlugIn()
	, mMessageDispatcher()
{
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &UIPlugInJsonApi::onReaderEvent);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &UIPlugInJsonApi::onReaderEvent);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &UIPlugInJsonApi::onReaderEvent);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &UIPlugInJsonApi::onReaderEvent);
}


UIPlugInJsonApi::~UIPlugInJsonApi()
{
}


void UIPlugInJsonApi::callFireMessage(const QByteArray& pMsg) const
{
	if (!pMsg.isEmpty())
	{
		qCDebug(jsonapi).noquote() << "Fire message:" << pMsg;
		Q_EMIT fireMessage(pMsg);
	}
}


void UIPlugInJsonApi::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	if (pContext.objectCast<AuthContext>())
	{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
		pContext->setReaderType(ReaderManagerPlugInType::NFC);
#else
		pContext->setReaderType(ReaderManagerPlugInType::PCSC);
#endif

		connect(pContext.data(), &WorkflowContext::fireCurrentStateChanged, this, &UIPlugInJsonApi::onCurrentStateChanged);
	}

	callFireMessage(mMessageDispatcher.init(pContext));
}


void UIPlugInJsonApi::onWorkflowFinished(QSharedPointer<WorkflowContext> )
{
	callFireMessage(mMessageDispatcher.finish());
}


void UIPlugInJsonApi::onReaderEvent(const QString& pName)
{
	callFireMessage(mMessageDispatcher.createMsgReader(pName));
}


void UIPlugInJsonApi::onCurrentStateChanged(const QString& pNewState)
{
	callFireMessage(mMessageDispatcher.processStateChange(pNewState));
}


void UIPlugInJsonApi::doMessageProcessing(const QByteArray& pMsg)
{
	callFireMessage(mMessageDispatcher.processCommand(pMsg));
}


void UIPlugInJsonApi::doShutdown()
{
}
