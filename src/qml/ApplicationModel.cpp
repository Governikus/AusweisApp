/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "ApplicationModel.h"

#include "ReaderInfo.h"
#include "ReaderManager.h"
#include "SingletonHelper.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthenticationContext.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
#include <QBluetoothLocalDevice>
#endif

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

using namespace governikus;

defineSingleton(ApplicationModel)


void ApplicationModel::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() == ReaderManagerPlugInType::BLUETOOTH)
	{
		Q_EMIT fireBluetoothEnabledChanged();
	}
	else if (pInfo.getPlugInType() == ReaderManagerPlugInType::NFC)
	{
		Q_EMIT fireNfcEnabledChanged();
	}
}


ApplicationModel::ApplicationModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
{
	connect(&ReaderManager::getInstance(), &ReaderManager::fireStatusChanged, this, &ApplicationModel::onStatusChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderConnected, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &ApplicationModel::fireBluetoothReaderChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireStatusChanged, this, &ApplicationModel::fireBluetoothReaderChanged);
}


ApplicationModel::~ApplicationModel()
{
}


ApplicationModel& ApplicationModel::getWidgetInstance()
{
	return *Instance;
}


void ApplicationModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	Q_EMIT fireCurrentWorkflowChanged();
}


ReaderManagerPlugInInfo ApplicationModel::getFirstPlugInInfo(ReaderManagerPlugInType pType) const
{
	const auto pluginInfos = ReaderManager::getInstance().getPlugInInfos();
	for (const auto& info : pluginInfos)
	{
		if (info.getPlugInType() == pType)
		{
			return info;
		}
	}
	return ReaderManagerPlugInInfo();
}


bool ApplicationModel::isNfcAvailable() const
{
	return getFirstPlugInInfo(ReaderManagerPlugInType::NFC).isAvailable();
}


bool ApplicationModel::isNfcEnabled() const
{
	return getFirstPlugInInfo(ReaderManagerPlugInType::NFC).isEnabled();
}


bool ApplicationModel::isBluetoothAvailable() const
{
	return getFirstPlugInInfo(ReaderManagerPlugInType::BLUETOOTH).isAvailable();
}


bool ApplicationModel::isBluetoothEnabled() const
{
	return getFirstPlugInInfo(ReaderManagerPlugInType::BLUETOOTH).isEnabled();
}


QString ApplicationModel::getCurrentWorkflow() const
{
	if (mContext.objectCast<ChangePinContext>())
	{
		return QStringLiteral("changepin");
	}
	if (mContext.objectCast<SelfAuthenticationContext>())
	{
		return QStringLiteral("selfauthentication");
	}
	if (mContext.objectCast<AuthContext>())
	{
		return QStringLiteral("authentication");
	}
	return QString();
}


void ApplicationModel::setBluetoothEnabled(bool pEnabled)
{
#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	if (pEnabled)
	{
		QBluetoothLocalDevice localDevice;
		localDevice.powerOn();
		qDebug() << "Bluetooth" << (pEnabled ? "Enabled" : "Disabled");
	}
#else
	qWarning() << (pEnabled ? "Enabling" : "Disabling") << "Bluetooth not supported on this platform";
#endif
}


bool ApplicationModel::locationPermissionRequired() const
{
#ifdef Q_OS_ANDROID
	if (QtAndroid::androidSdkVersion() < 23)
	{
		return false;
	}

	QAndroidJniEnvironment env;
	const int PERMISSION_GRANTED = 0;
	const auto& permission = QAndroidJniObject::fromString(QStringLiteral("android.permission.ACCESS_COARSE_LOCATION"));

	const auto result = QAndroidJniObject::callStaticMethod<jint>(
			"org/qtproject/qt5/android/QtNative",
			"checkSelfPermission",
			"(Ljava/lang/String;)I",
			permission.object<jstring>());

	if (env->ExceptionCheck())
	{
		env->ExceptionClear();
		qWarning() << "Exception occurred while checking location permission";
		return false;
	}

	return (result != PERMISSION_GRANTED) && ReaderManager::getInstance().getReaderInfos(ReaderManagerPlugInType::BLUETOOTH).isEmpty();

#else
	return false;

#endif
}
