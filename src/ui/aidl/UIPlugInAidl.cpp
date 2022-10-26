/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInAidl.h"

#include "Env.h"
#include "ReaderManager.h"
#include "UILoader.h"
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


QAtomicPointer<UIPlugInAidl> UIPlugInAidl::instance = nullptr;

UIPlugInAidl::UIPlugInAidl()
	: UIPlugIn()
	, mJson(nullptr)
	, mContext()
	, mWorkflowIsActive()
	, mInitializationSuccessfull(false)
{
	if (Env::getSingleton<UILoader>()->load<UIPlugInJson>())
	{
		mJson = Env::getSingleton<UILoader>()->getLoaded<UIPlugInJson>();
		Q_ASSERT(mJson);
		connect(mJson, &UIPlugInJson::fireMessage, this, &UIPlugInAidl::onToSend, Qt::QueuedConnection);

		mJson->setEnabled();
		mInitializationSuccessfull = true;
	}
	else
	{
		qCWarning(aidl) << "Cannot start AIDL because JSON-API is missing";
	}

	instance = this;
}


UIPlugInAidl* UIPlugInAidl::getInstance(bool pBlock)
{
	// The Java interface thread is ready before our core has booted.
	// Hence we delay access to the UIPlugInAidl.
	if (pBlock)
	{
		while (instance.loadRelaxed() == nullptr)
		{
			QThread::msleep(100);
		}
	}

	return instance.loadRelaxed();
}


bool UIPlugInAidl::isSuccessfullInitialized() const
{
	return mInitializationSuccessfull;
}


void UIPlugInAidl::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	mWorkflowIsActive.lock();
	pContext->setReaderPlugInTypes({ReaderManagerPlugInType::NFC, ReaderManagerPlugInType::LOCAL_IFD, ReaderManagerPlugInType::SIMULATOR});
	mContext = pContext;
	mContext->claim(this);
}


void UIPlugInAidl::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)

	mContext.clear();
	mJson->blockSignals(false);
	mWorkflowIsActive.unlock();
}


void UIPlugInAidl::onReceived(const QByteArray& pMessage)
{
	mJson->doMessageProcessing(pMessage);
}


bool UIPlugInAidl::waitForWorkflowToFinish()
{
	const int fiveSeconds = 5000;
	bool success = mWorkflowIsActive.tryLock(fiveSeconds);
	if (success)
	{
		mWorkflowIsActive.unlock();
	}
	return success;
}


void UIPlugInAidl::reset()
{
	if (mContext)
	{
		mJson->blockSignals(true);
		mContext->killWorkflow();
	}
}


void UIPlugInAidl::startReaderManagerScans() const
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	readerManager->startScan(ReaderManagerPlugInType::NFC);
	readerManager->startScan(ReaderManagerPlugInType::LOCAL_IFD);
	readerManager->startScan(ReaderManagerPlugInType::SIMULATOR);
}


void UIPlugInAidl::onToSend(const QByteArray& pMessage)
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


void UIPlugInAidl::doShutdown()
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

	UIPlugInAidl* plugin = UIPlugInAidl::getInstance();
	if (!plugin->isSuccessfullInitialized())
	{
		qCCritical(aidl) << "Cannot call AIDL plugin";
		return pEnv->NewStringUTF("");
	}
	QMetaObject::invokeMethod(plugin, &UIPlugInAidl::reset, Qt::QueuedConnection);
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

	UIPlugInAidl* plugin = UIPlugInAidl::getInstance();
	if (!plugin->isSuccessfullInitialized())
	{
		qCCritical(aidl) << "Cannot call AIDL plugin";
		return false;
	}

	QMetaObject::invokeMethod(plugin, &UIPlugInAidl::startReaderManagerScans, Qt::QueuedConnection);
	return true;
}


JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AidlBinder_aidlSend(JNIEnv* pEnv, jobject pObj, jstring pJson)
{
	Q_UNUSED(pObj)

	const char* const nativeString = pEnv->GetStringUTFChars(pJson, 0);
	const QString json = QString::fromUtf8(nativeString);
	pEnv->ReleaseStringUTFChars(pJson, nativeString);

	UIPlugInAidl* plugin = UIPlugInAidl::getInstance();
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
