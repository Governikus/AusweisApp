/*!
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MulticastLock.h"

#include <QLoggingCategory>
#include <QtGlobal>

#if defined(Q_OS_ANDROID)
	#include <QCoreApplication>
	#include <QJniEnvironment>
	#include <QJniObject>
#endif

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;


#if defined(Q_OS_ANDROID)
MulticastLock::MulticastLock()
{
	invokeJniMethod("acquire");
}


MulticastLock::~MulticastLock()
{
	invokeJniMethod("release");
}


#else
MulticastLock::MulticastLock() = default;


MulticastLock::~MulticastLock() = default;


#endif


void MulticastLock::invokeJniMethod(const char* const pMethodName)
{
#if defined(Q_OS_ANDROID)
	QJniEnvironment env;
	const QJniObject context(QNativeInterface::QAndroidApplication::context());
	if (!context.isValid())
	{
		qCCritical(network) << "Cannot determine android context.";
		return;
	}

	QJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/MulticastLockJniBridgeUtil",
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
