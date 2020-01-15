/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInAidl.h"

#include "UILoader.h"
#ifdef Q_OS_ANDROID
#include "PskManager.h"
#include <QMetaObject>
#endif

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>

#ifdef Q_OS_ANDROID
#include <jni.h>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
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
	if (UILoader::getInstance().load(UIPlugInName::UIPlugInJson))
	{
		mJson = qobject_cast<UIPlugInJson*>(UILoader::getInstance().getLoaded(UIPlugInName::UIPlugInJson));
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


UIPlugInAidl::~UIPlugInAidl()
{
}


UIPlugInAidl* UIPlugInAidl::getInstance(bool pBlock)
{
	// The Java interface thread is ready before our core has booted.
	// Hence we delay access to the UIPlugInAidl.
	if (pBlock)
	{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
		while (instance.loadRelaxed() == nullptr)
#else
		while (instance.load() == nullptr)
#endif
		{
			QThread::msleep(100);
		}
	}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
	return instance.loadRelaxed();

#else
	return instance.load();

#endif
}


bool UIPlugInAidl::isSuccessfullInitialized()
{
	return mInitializationSuccessfull;
}


void UIPlugInAidl::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	mWorkflowIsActive.lock();
	pContext->setReaderPlugInTypes({ReaderManagerPlugInType::NFC});
	mContext = pContext;
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
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


void UIPlugInAidl::onToSend(const QByteArray& pMessage)
{
#ifdef Q_OS_ANDROID
	const QString json = QString::fromUtf8(pMessage);
	QAndroidJniObject jsonAndroidString = QAndroidJniObject::fromString(json);

	QAndroidJniObject aidlBinder = QtAndroid::androidService().callObjectMethod("getAidlBinder", "()Lcom/governikus/ausweisapp2/AidlBinder;");
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

	const auto& finalPsk = PskManager::getInstance().generatePsk();
	return pEnv->NewStringUTF(finalPsk.constData());
}


JNIEXPORT jboolean JNICALL Java_com_governikus_ausweisapp2_AidlBinder_isSecureRandomPsk(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)

	return PskManager::getInstance().isSecureRandomPsk();
}


}

extern "C"
{

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
