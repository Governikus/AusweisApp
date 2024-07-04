/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginFunctional.h"

#include "AusweisApp2_p.h"

#include "AppSettings.h"
#include "Env.h"
#include "ReaderManager.h"
#include "UiLoader.h"
#include "WorkflowRequest.h"

#include <QDebug>
#include <QPluginLoader>

using namespace governikus;

UiPluginFunctional::UiPluginFunctional()
	: UiPlugin()
	, mJson(nullptr)
	, mContext()
{
}


bool UiPluginFunctional::initialize()
{
	if (mJson)
	{
		return true;
	}

	if (!Env::getSingleton<UiLoader>()->load<UiPluginJson>())
	{
		qWarning() << "Cannot start functional because JSON-API is missing";
		return false;
	}

	mJson = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
	connect(mJson, &UiPluginJson::fireMessage, this, &UiPluginFunctional::onJsonMessage);
	return true;
}


void UiPluginFunctional::onApplicationStarted()
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

	Env::getSingleton<ReaderManager>()->startScan(ReaderManagerPluginType::SIMULATOR);
}


void UiPluginFunctional::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	mContext = pRequest->getContext();
	mContext->claim(this);

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
	mContext->setReaderPluginTypes({ReaderManagerPluginType::NFC, ReaderManagerPluginType::SIMULATOR});
#else
	mContext->setReaderPluginTypes({ReaderManagerPluginType::PCSC, ReaderManagerPluginType::SIMULATOR});
#endif

#if !defined(Q_OS_IOS)
	Env::getSingleton<ReaderManager>()->startScanAll();
#endif
}


void UiPluginFunctional::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)

#if !defined(Q_OS_IOS)
	Env::getSingleton<ReaderManager>()->stopScanAll();
#endif

	mContext.clear();
}


void UiPluginFunctional::onJsonMessage(const QByteArray& pMessage)
{
	auto callback = ausweisapp2_get_callback_internal();
	if (callback == nullptr)
	{
		qDebug() << "No callback registered";
		return;
	}
	callback(pMessage.constData());
}


void UiPluginFunctional::doMessageProcessing(const QByteArray& pMsg)
{
	mJson->doMessageProcessing(pMsg);
}


void UiPluginFunctional::doQuitApplicationRequest()
{
	Q_EMIT fireQuitApplicationRequest();
}


void UiPluginFunctional::doShutdown()
{
	mJson->setEnabled(false);
}
