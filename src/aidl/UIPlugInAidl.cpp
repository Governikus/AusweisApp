/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "UIPlugInAidl.h"

#include "view/UILoader.h"
#ifdef Q_OS_ANDROID
#include "PskManager.h"
#include <QMetaObject>
#endif

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QSignalBlocker>
#include <QThread>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#include <jni.h>
#endif


Q_DECLARE_LOGGING_CATEGORY(aidl)

using namespace governikus;


QAtomicPointer<UIPlugInAidl> UIPlugInAidl::instance = nullptr;

UIPlugInAidl::UIPlugInAidl()
	: UIPlugIn()
	, mJsonApi(nullptr)
	, mContext()
	, mInitializationSuccessfull(false)
{
	if (UILoader::getInstance().load(UIPlugInName::UIPlugInJsonApi))
	{
		mJsonApi = qobject_cast<UIPlugInJsonApi*>(UILoader::getInstance().getLoaded(UIPlugInName::UIPlugInJsonApi));
		Q_ASSERT(mJsonApi);
		connect(mJsonApi, &UIPlugInJsonApi::fireMessage, this, &UIPlugInAidl::onToSend);

		mInitializationSuccessfull = true;
	}
	else
	{
		qWarning(aidl) << "Cannot start AIDL because JSON-API is missing";
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
		while (instance.load() == nullptr)
		{
			QThread::msleep(100);
		}
	}

	return instance.load();
}


bool UIPlugInAidl::isSuccessfullInitialized()
{
	return mInitializationSuccessfull;
}


void UIPlugInAidl::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	mContext = pContext;
}


void UIPlugInAidl::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	mContext = pContext;
}


void UIPlugInAidl::onReceived(const QByteArray& pMessage)
{
	mJsonApi->doMessageProcessing(pMessage);
}


void UIPlugInAidl::reset()
{
	if (mContext)
	{
		const QSignalBlocker blocker(mJsonApi);
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


void UIPlugInAidl::onToSend(const QByteArray& pMessage)
{
#ifdef Q_OS_ANDROID
	const QString json = QString::fromUtf8(pMessage);
	QAndroidJniObject jsonAndroidString = QAndroidJniObject::fromString(json);

	QAndroidJniObject aidlBinder = QtAndroid::androidService().callObjectMethod("getAidlBinder", "()Lcom/governikus/ausweisapp2/AidlBinder;");
	// FIXME This function is meant to return void istead of java.lang.Object but JNI does not like void
	aidlBinder.callObjectMethod("aidlReceive", "(Ljava/lang/String;)Ljava/lang/Object;", jsonAndroidString.object<jstring>());
#else
	Q_UNUSED(pMessage);
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

JNIEXPORT jstring JNICALL Java_com_governikus_ausweisapp2_AidlBinder_resetValidSessionID(JNIEnv* pEnv, jobject pObj, jstring pClientPartialPsk)
{
	Q_UNUSED(pObj);

	const char* nativeString = pEnv->GetStringUTFChars(pClientPartialPsk, 0);
	const QString clientPartialPsk = QString::fromUtf8(nativeString);
	pEnv->ReleaseStringUTFChars(pClientPartialPsk, nativeString);


	UIPlugInAidl* plugin = UIPlugInAidl::getInstance();
	if (!plugin->isSuccessfullInitialized())
	{
		qCritical(aidl) << "Cannot call AIDL plugin";
		return pEnv->NewStringUTF(QString().toUtf8().constData());
	}
	QMetaObject::invokeMethod(plugin, "reset", Qt::BlockingQueuedConnection);


	const QString finalPsk = PskManager::getInstance().generatePsk(clientPartialPsk);
	return pEnv->NewStringUTF(finalPsk.toUtf8().constData());
}


JNIEXPORT jboolean JNICALL Java_com_governikus_ausweisapp2_AidlBinder_isSecureRandomPsk(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv);
	Q_UNUSED(pObj);

	return PskManager::getInstance().isSecureRandomPsk();
}


}

extern "C"
{

JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AidlBinder_aidlSend(JNIEnv* pEnv, jobject pObj, jstring pJson)
{
	Q_UNUSED(pObj);

	const char* nativeString = pEnv->GetStringUTFChars(pJson, 0);
	const QString json = QString::fromUtf8(nativeString);
	pEnv->ReleaseStringUTFChars(pJson, nativeString);

	UIPlugInAidl* plugin = UIPlugInAidl::getInstance();
	if (!plugin->isSuccessfullInitialized())
	{
		qCritical(aidl) << "Cannot call AIDL plugin";
		return;
	}

	QMetaObject::invokeMethod(plugin, "onReceived", Qt::QueuedConnection, Q_ARG(QByteArray, json.toUtf8()));
}


}
#endif
