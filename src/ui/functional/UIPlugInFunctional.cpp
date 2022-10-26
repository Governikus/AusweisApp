/*!
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInFunctional.h"

#include "AusweisApp2_p.h"

#include "AppSettings.h"
#include "Env.h"
#include "ReaderManager.h"
#include "UILoader.h"

#include <QDebug>
#include <QPluginLoader>

using namespace governikus;

UIPlugInFunctional::UIPlugInFunctional()
	: UIPlugIn()
	, mJson(nullptr)
	, mContext()
{
	if (!Env::getSingleton<UILoader>()->load<UIPlugInJson>())
	{
		qWarning() << "Cannot start functional because JSON-API is missing";
		return;
	}

	mJson = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
	Q_ASSERT(mJson);
	connect(mJson, &UIPlugInJson::fireMessage, this, &UIPlugInFunctional::onJsonMessage);
}


void UIPlugInFunctional::onApplicationStarted()
{
	ausweisapp2_started_internal();
	auto callback = ausweisapp2_get_callback_internal();
	if (callback == nullptr)
	{
		qDebug() << "No callback registered";
		return;
	}

	mJson->setEnabled();
	callback(nullptr);

	Env::getSingleton<ReaderManager>()->startScan(ReaderManagerPlugInType::SIMULATOR);
}


void UIPlugInFunctional::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
	pContext->setReaderPlugInTypes({ReaderManagerPlugInType::NFC, ReaderManagerPlugInType::SIMULATOR});
#else
	pContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC, ReaderManagerPlugInType::SIMULATOR});
#endif

	pContext->claim(this);
	mContext = pContext;

#if !defined(Q_OS_IOS)
	Env::getSingleton<ReaderManager>()->startScanAll();
#endif
}


void UIPlugInFunctional::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)

#if !defined(Q_OS_IOS)
	Env::getSingleton<ReaderManager>()->stopScanAll();
#endif

	mContext.clear();
}


void UIPlugInFunctional::onJsonMessage(const QByteArray& pMessage)
{
	auto callback = ausweisapp2_get_callback_internal();
	if (callback == nullptr)
	{
		qDebug() << "No callback registered";
		return;
	}
	callback(pMessage.constData());
}


void UIPlugInFunctional::doMessageProcessing(const QByteArray& pMsg)
{
	mJson->doMessageProcessing(pMsg);
}


void UIPlugInFunctional::doQuitApplicationRequest()
{
	Q_EMIT fireQuitApplicationRequest();
}


void UIPlugInFunctional::doShutdown()
{
	mJson->setEnabled(false);
}
