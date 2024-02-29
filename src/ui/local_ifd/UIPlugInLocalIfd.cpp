/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInLocalIfd.h"

#include "Env.h"
#include "LocalIfdServer.h"
#include "SecureStorage.h"
#include "context/IfdServiceContext.h"
#include "controller/IfdServiceController.h"

#ifdef Q_OS_ANDROID
	#include "AppSettings.h"
	#include "UILoader.h"

	#include <QJniEnvironment>
	#include <jni.h>
#endif

Q_DECLARE_LOGGING_CATEGORY(ifd)

using namespace governikus;


void UIPlugInLocalIfd::onStateChanged(const QString& pNewState)
{
	Q_UNUSED(pNewState)
	if (mContext)
	{
		mContext->setStateApproved();
	}
}


void UIPlugInLocalIfd::onConnectedChanged(bool pConnected)
{
	if (!pConnected)
	{
		Q_EMIT fireQuitApplicationRequest();
	}
}


void UIPlugInLocalIfd::onSocketError(QAbstractSocket::SocketError pSocketError)
{
	Q_UNUSED(pSocketError)
	Q_EMIT fireQuitApplicationRequest(EXIT_FAILURE);
}


UIPlugInLocalIfd::UIPlugInLocalIfd()
	: UIPlugIn()
	, mContext()
{
}


void UIPlugInLocalIfd::doShutdown()
{

}


void UIPlugInLocalIfd::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	mContext = pRequest->getContext();
	mContext->claim(this);
	connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &UIPlugInLocalIfd::onStateChanged);
}


void UIPlugInLocalIfd::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_ASSERT(mContext == pRequest->getContext());

	disconnect(mContext.data(), &WorkflowContext::fireStateChanged, this, &UIPlugInLocalIfd::onStateChanged);
	mContext.reset();
}


bool UIPlugInLocalIfd::onStartWorkflowRequested(const QString& pPsk)
{
	if (const auto minPskSize = Env::getSingleton<SecureStorage>()->getLocalIfdMinPskSize();
			(pPsk.toUtf8().size() * CHAR_BIT) < minPskSize)
	{
		qCCritical(ifd) << "PSK must be at least" << minPskSize << "bit";
		return false;
	}

	const auto localIfdServer = QSharedPointer<LocalIfdServer>::create();
	localIfdServer->setPsk(pPsk.toUtf8());
	if (!localIfdServer->start(QStringLiteral("LocalIfdServer")))
	{
		return false;
	}

	connect(localIfdServer.data(), &IfdServer::fireConnectedChanged, this, &UIPlugInLocalIfd::onConnectedChanged);
	connect(localIfdServer.data(), &IfdServer::fireSocketError, this, &UIPlugInLocalIfd::onSocketError);
	Q_EMIT fireWorkflowRequested(WorkflowRequest::create<IfdServiceController, IfdServiceContext>(localIfdServer));

	return localIfdServer->isRunning();
}


void UIPlugInLocalIfd::onAbortWorkflowRequested()
{
	if (mContext)
	{
		mContext->killWorkflow();
	}
}


#ifdef Q_OS_ANDROID
extern "C"
{

JNIEXPORT jboolean JNICALL Java_com_governikus_ausweisapp2_AusweisApp2LocalIfdService_notifyStartWorkflow(JNIEnv* pEnv, jobject pObj, jstring pPsk)
{
	Q_UNUSED(pObj)

	const auto* nativeString = pEnv->GetStringUTFChars(pPsk, 0);
	const auto& psk = QString::fromUtf8(nativeString);
	pEnv->ReleaseStringUTFChars(pPsk, nativeString);

	bool initialized = false;
	QMetaObject::invokeMethod(QCoreApplication::instance(), [psk] {
			UIPlugInLocalIfd* const uiPlugIn = Env::getSingleton<UILoader>()->getLoaded<UIPlugInLocalIfd>();
			if (uiPlugIn)
			{
				return uiPlugIn->onStartWorkflowRequested(psk);
			}
			return false;
		}, Qt::BlockingQueuedConnection, &initialized);

	return initialized;
}


JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AusweisApp2LocalIfdService_notifyAbortWorkflow(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pObj)
	Q_UNUSED(pEnv)

	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
			UIPlugInLocalIfd* const uiPlugIn = Env::getSingleton<UILoader>()->getLoaded<UIPlugInLocalIfd>();
			if (uiPlugIn)
			{
				uiPlugIn->onAbortWorkflowRequested();
			}
		}, Qt::BlockingQueuedConnection);
}


JNIEXPORT jboolean JNICALL Java_com_governikus_ausweisapp2_AusweisApp2LocalIfdService_verifyServiceToken(JNIEnv* pEnv, jobject pObj, jstring pServiceToken)
{
	Q_UNUSED(pObj)

	const auto* nativeString = pEnv->GetStringUTFChars(pServiceToken, 0);
	const auto& serviceToken = QString::fromUtf8(nativeString);
	pEnv->ReleaseStringUTFChars(pServiceToken, nativeString);

	const auto& aa2ServiceToken = Env::getSingleton<AppSettings>()->getGeneralSettings().getIfdServiceToken();
	return serviceToken == aa2ServiceToken;
}


}
#endif
