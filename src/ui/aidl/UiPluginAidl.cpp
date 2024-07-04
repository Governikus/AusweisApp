/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginAidl.h"

#include "Env.h"
#include "ReaderManager.h"
#include "UiLoader.h"
#include "WorkflowRequest.h"
#ifdef Q_OS_ANDROID
	#include "Randomizer.h"
	#include <QMetaObject>
#endif

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>

#ifdef Q_OS_ANDROID
	#include <QJniEnvironment>
	#include <QJniObject>
	#include <jni.h>
#endif


Q_DECLARE_LOGGING_CATEGORY(aidl)

using namespace governikus;


QAtomicPointer<UiPluginAidl> UiPluginAidl::instance = nullptr;

UiPluginAidl::UiPluginAidl()
	: UiPlugin()
	, mJson(nullptr)
	, mContext()
	, mWorkflowIsActive()
{
	instance = this;
}


bool UiPluginAidl::initialize()
{
	if (mJson)
	{
		return true;
	}

	if (!Env::getSingleton<UiLoader>()->load<UiPluginJson>())
	{
		qCWarning(aidl) << "Cannot start AIDL because JSON-API is missing";
		return false;
	}

	mJson = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
	connect(mJson, &UiPluginJson::fireMessage, this, &UiPluginAidl::onToSend, Qt::QueuedConnection);
	mJson->setEnabled();
	return true;
}


UiPluginAidl* UiPluginAidl::getInstance(bool pBlock)
{
	// The Java interface thread is ready before our core has booted.
	// Hence we delay access to the UiPluginAidl.
	if (pBlock)
	{
		while (instance.loadRelaxed() == nullptr)
		{
			QThread::msleep(100);
		}
	}

	return instance.loadRelaxed();
}


bool UiPluginAidl::isSuccessfullInitialized() const
{
	return mJson;
}


void UiPluginAidl::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	mWorkflowIsActive.lock();
	mContext = pRequest->getContext();
	mContext->setReaderPluginTypes({ReaderManagerPluginType::NFC, ReaderManagerPluginType::LOCAL_IFD, ReaderManagerPluginType::SIMULATOR});
	mContext->claim(this);
}


void UiPluginAidl::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)

	mContext.clear();
	mJson->blockSignals(false);
	mWorkflowIsActive.unlock();
}


void UiPluginAidl::onReceived(const QByteArray& pMessage)
{
	mJson->doMessageProcessing(pMessage);
}


bool UiPluginAidl::waitForWorkflowToFinish()
{
	const int fiveSeconds = 5000;
	bool success = mWorkflowIsActive.tryLock(fiveSeconds);
	if (success)
	{
		mWorkflowIsActive.unlock();
	}
	return success;
}


void UiPluginAidl::reset()
{
	if (mContext)
	{
		mJson->blockSignals(true);
		mContext->killWorkflow();
	}
}


void UiPluginAidl::startReaderManagerScans() const
{
	auto* readerManager = Env::getSingleton<ReaderManager>();
	readerManager->startScan(ReaderManagerPluginType::NFC);
	readerManager->startScan(ReaderManagerPluginType::LOCAL_IFD);
	readerManager->startScan(ReaderManagerPluginType::SIMULATOR);
}


void UiPluginAidl::onToSend(const QByteArray& pMessage) const
{
#ifdef Q_OS_ANDROID
	const QString json = QString::fromUtf8(pMessage);
	QJniObject jsonAndroidString = QJniObject::fromString(json);

	QJniObject service = QNativeInterface::QAndroidApplication::context();
	QJniObject aidlBinder = service.callObjectMethod("getAidlBinder", "()Lcom/governikus/ausweisapp2/AidlBinder;");
	aidlBinder.callMethod<void>("aidlReceive", "(Ljava/lang/String;)V", jsonAndroidString.object<jstring>());
#else
	Q_UNUSED(pMessage)
#endif
}


void UiPluginAidl::doShutdown()
{
}


#ifdef Q_OS_ANDROID
extern "C"
{

// These functions need to be explicitly exported so that the JVM can bind to them.
// At the moment only the Q_Plugins seem to be appropriate locations.

JNIEXPORT jstring JNICALL Java_com_governikus_ausweisapp2_AidlBinder_resetValidSessionID(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pObj)

	UiPluginAidl* plugin = UiPluginAidl::getInstance();
	if (!plugin->isSuccessfullInitialized())
	{
		qCCritical(aidl) << "Cannot call AIDL plugin";
		return pEnv->NewStringUTF("");
	}
	QMetaObject::invokeMethod(plugin, &UiPluginAidl::reset, Qt::QueuedConnection);
	if (!plugin->waitForWorkflowToFinish())
	{
		qCCritical(aidl) << "Cannot acquire workflow mutex";
		return pEnv->NewStringUTF("");
	}

	const auto& finalPsk = Randomizer::getInstance().createUuid();
	return pEnv->NewStringUTF(finalPsk.toByteArray().constData());
}


JNIEXPORT jboolean JNICALL Java_com_governikus_ausweisapp2_AidlBinder_isSecureRandomPsk(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)

	return Randomizer::getInstance().isSecureRandom();
}


JNIEXPORT jboolean JNICALL Java_com_governikus_ausweisapp2_AidlBinder_startReaderManagerScans(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)

	UiPluginAidl* plugin = UiPluginAidl::getInstance();
	if (!plugin->isSuccessfullInitialized())
	{
		qCCritical(aidl) << "Cannot call AIDL plugin";
		return false;
	}

	QMetaObject::invokeMethod(plugin, &UiPluginAidl::startReaderManagerScans, Qt::QueuedConnection);
	return true;
}


JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AidlBinder_aidlSend(JNIEnv* pEnv, jobject pObj, jstring pJson)
{
	Q_UNUSED(pObj)

	const char* const nativeString = pEnv->GetStringUTFChars(pJson, 0);
	const QString json = QString::fromUtf8(nativeString);
	pEnv->ReleaseStringUTFChars(pJson, nativeString);

	UiPluginAidl* plugin = UiPluginAidl::getInstance();
	if (!plugin->isSuccessfullInitialized())
	{
		qCCritical(aidl) << "Cannot call AIDL plugin";
		return;
	}

	QMetaObject::invokeMethod(plugin, [ = ] {
			plugin->onReceived(json.toUtf8());
		}, Qt::QueuedConnection);
}


}
#endif
