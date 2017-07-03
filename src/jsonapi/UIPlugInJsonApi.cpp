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


void UIPlugInJsonApi::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	if (pContext.dynamicCast<AuthContext>())
	{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
		pContext->setReaderType(ReaderManagerPlugInType::NFC);
#else
		pContext->setReaderType(ReaderManagerPlugInType::PCSC);
#endif

		connect(pContext.data(), &WorkflowContext::fireCurrentStateChanged, this, &UIPlugInJsonApi::onCurrentStateChanged);
	}

	const auto& msg = mMessageDispatcher.init(pContext);
	if (!msg.isEmpty())
	{
		Q_EMIT fireMessage(msg);
	}
}


void UIPlugInJsonApi::onWorkflowFinished(QSharedPointer<WorkflowContext> )
{
	const auto& response = mMessageDispatcher.finish();
	if (!response.isEmpty())
	{
		Q_EMIT fireMessage(response);
	}
}


void UIPlugInJsonApi::onReaderEvent(const QString& pName)
{
	Q_EMIT fireMessage(mMessageDispatcher.createMsgReader(pName));
}


void UIPlugInJsonApi::onCurrentStateChanged(const QString& pNewState)
{
	const auto& response = mMessageDispatcher.processStateChange(pNewState);
	if (!response.isEmpty())
	{
		qDebug(jsonapi) << "Fire message:" << response;
		Q_EMIT fireMessage(response);
	}
}


void UIPlugInJsonApi::doMessageProcessing(const QByteArray& pMsg)
{
	const auto& response = mMessageDispatcher.processCommand(pMsg);
	if (!response.isEmpty())
	{
		qDebug(jsonapi) << "Fire message:" << response;
		Q_EMIT fireMessage(response);
	}
}


void UIPlugInJsonApi::doShutdown()
{
}
