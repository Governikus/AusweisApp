/*!
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MulticastLock.h"

#include <QLoggingCategory>
#include <QtGlobal>

#if defined(Q_OS_ANDROID)
	#include <QAndroidJniEnvironment>
	#include <QAndroidJniObject>
	#include <QtAndroid>
#endif

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;


MulticastLock::MulticastLock()
{
#if defined(Q_OS_ANDROID)
	invokeJniMethod("acquire");
#endif
}


MulticastLock::~MulticastLock()
{
#if defined(Q_OS_ANDROID)
	invokeJniMethod("release");
#endif
}


void MulticastLock::invokeJniMethod(const char* const pMethodName)
{
#if defined(Q_OS_ANDROID)
	QAndroidJniEnvironment env;
	const QAndroidJniObject context(QtAndroid::androidContext());
	if (!context.isValid())
	{
		qCCritical(network) << "Cannot determine android context.";
		return;
	}

	QAndroidJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/MulticastLockJniBridgeUtil",
			pMethodName,
			"(Landroid/content/Context;)V",
			context.object<jobject>());

	if (env->ExceptionCheck())
	{
		qCCritical(network) << "Cannot call MulticastLockJniBridgeUtil." << pMethodName << "()";
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
#else
	Q_UNUSED(pMethodName)
#endif
}
