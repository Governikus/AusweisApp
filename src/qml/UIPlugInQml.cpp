/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include "AppSettings.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"
#include "DpiCalculator.h"
#include "Env.h"
#include "FileDestination.h"
#include "Service.h"

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#endif

#include <QFile>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QScreen>
#include <QtPlugin>
#include <QtQml>

#if defined(Q_OS_ANDROID)
#include <QFont>
#endif

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

UIPlugInQml::UIPlugInQml()
	: mEngine()
	, mDpi(DpiCalculator::getDpi())
	, mProviderModel()
	, mHistoryModel(&AppSettings::getInstance().getHistorySettings())
	, mChangePinModel()
	, mAuthModel()
	, mVersionInformationModel()
	, mQmlExtension()
	, mSelfAuthenticationModel()
	, mSettingsModel()
	, mCertificateDescriptionModel()
	, mChatModel()
	, mNumberModel()
	, mApplicationModel()
	, mExplicitPlatformStyle(getPlatformSelectors())
	, mStatusBarUtil()
	, mConnectivityManager()
	, mRemoteServiceModel()
{
#if defined(Q_OS_ANDROID)
	QGuiApplication::setFont(QFont("Roboto"));
#endif

	connect(&mChangePinModel, &ChangePinModel::fireStartWorkflow, this, &UIPlugIn::fireChangePinRequest);
	connect(&mSelfAuthenticationModel, &SelfAuthenticationModel::fireStartWorkflow, this, &UIPlugIn::fireSelfAuthenticationRequested);
	connect(&mRemoteServiceModel, &RemoteServiceModel::fireStartWorkflow, this, &UIPlugIn::fireRemoteServiceRequested);
	connect(this, &UIPlugIn::fireShowUserInformation, this, &UIPlugInQml::onShowUserInformation);
	init();
}


UIPlugInQml::~UIPlugInQml()
{
}


void UIPlugInQml::init()
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	qputenv("QML_DISABLE_DISK_CACHE", "true");
#endif

	mEngine.reset(new QQmlApplicationEngine());

	mEngine->rootContext()->setContextProperty(QStringLiteral("plugin"), this);
	QQmlFileSelector::get(mEngine.data())->setExtraSelectors(mExplicitPlatformStyle.split(QLatin1Char(',')));

	mEngine->rootContext()->setContextProperty(QStringLiteral("screenDpi"), mDpi);
	mEngine->rootContext()->setContextProperty(QStringLiteral("providerModel"), &mProviderModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("historyModel"), &mHistoryModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("changePinModel"), &mChangePinModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("authModel"), &mAuthModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("versionInformationModel"), &mVersionInformationModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("qmlExtension"), &mQmlExtension);
	mEngine->rootContext()->setContextProperty(QStringLiteral("selfAuthenticationModel"), &mSelfAuthenticationModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("settingsModel"), &mSettingsModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("certificateDescriptionModel"), &mCertificateDescriptionModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("chatModel"), &mChatModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("numberModel"), &mNumberModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("applicationModel"), &mApplicationModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("statusBarUtil"), &mStatusBarUtil);
	mEngine->rootContext()->setContextProperty(QStringLiteral("connectivityManager"), &mConnectivityManager);
	mEngine->rootContext()->setContextProperty(QStringLiteral("remoteServiceModel"), &mRemoteServiceModel);

	mEngine->addImportPath(getPath(QStringLiteral("qml/")).toString());
	mEngine->load(getPath(QStringLiteral("qml/main.qml")));

	Env::getSingleton<Service>()->updateConfigurations();
}


QString UIPlugInQml::getPlatformSelectors() const
{
#ifndef Q_NO_DEBUG
	const char* overrideSelector = "OVERRIDE_PLATFORM_SELECTOR";
	if (!qEnvironmentVariableIsEmpty(overrideSelector))
	{
		const auto& platform = QString::fromLocal8Bit(qgetenv(overrideSelector));
		qCDebug(qml) << "Override platform selector:" << platform;
		return platform;
	}
#endif

#if defined(Q_OS_ANDROID)
	const jboolean result = QtAndroid::androidActivity().callMethod<jboolean>("isTablet", "()Z");
	const bool isTablet = result != JNI_FALSE;
#else
	// A device with a screen diagnonal above 6 inches is considered a tablet.
	static const double MAX_SMARTPHONE_DIAGONAL_IN = 6.0;
	static const double MAX_SMARTPHONE_DIAGONAL_MM = 25.4 * MAX_SMARTPHONE_DIAGONAL_IN;

	const QList<QScreen*> screens = QGuiApplication::screens();

	Q_ASSERT(!screens.isEmpty());

	QScreen* const mainScreen = screens.first();
	Q_ASSERT(mainScreen != nullptr);

	const QSizeF size = mainScreen->physicalSize();
	const double width = size.width();
	const double height = size.height();
	const double diagonal = sqrt(width * width + height * height);
	const bool isTablet = diagonal > MAX_SMARTPHONE_DIAGONAL_MM;
#endif

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	const QString platform = QGuiApplication::platformName();
#else
	const QString platform = QStringLiteral("android");
#endif

	return isTablet ? QStringLiteral("tablet,") + platform : platform;
}


void UIPlugInQml::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	mApplicationModel.resetContext(pContext);
	mNumberModel.resetContext(pContext);

	if (auto changePinContext = pContext.objectCast<ChangePinContext>())
	{
		mChangePinModel.resetContext(changePinContext);
	}

	if (auto authContext = pContext.objectCast<AuthContext>())
	{
		mConnectivityManager.startWatching();
		mAuthModel.resetContext(authContext);
		mCertificateDescriptionModel.resetContext(authContext);
		mChatModel.resetContext(authContext);
	}

	if (auto authContext = pContext.objectCast<SelfAuthContext>())
	{
		mSelfAuthenticationModel.resetContext(authContext);
	}

	if (auto remoteServiceContext = pContext.objectCast<RemoteServiceContext>())
	{
		mRemoteServiceModel.resetContext(remoteServiceContext);
	}
}


void UIPlugInQml::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	mApplicationModel.resetContext();
	mNumberModel.resetContext();

	if (pContext.objectCast<ChangePinContext>())
	{
		mChangePinModel.resetContext();
	}

	if (pContext.objectCast<AuthContext>())
	{
		mConnectivityManager.stopWatching();
		mAuthModel.resetContext();
		mCertificateDescriptionModel.resetContext();
		mChatModel.resetContext();
	}

	if (pContext.objectCast<SelfAuthContext>())
	{
		mSelfAuthenticationModel.resetContext();
	}

	if (pContext.objectCast<RemoteServiceContext>())
	{
		mRemoteServiceModel.resetContext();
	}
}


void UIPlugInQml::onShowUserInformation(const QString& pMessage)
{
	mQmlExtension.showFeedback(pMessage);
}


void UIPlugInQml::doShutdown()
{

}


QUrl UIPlugInQml::getPath(const QString& pRelativePath)
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	const QString ressourceFolderPath(QStringLiteral(RES_DIR) + QLatin1Char('/') + pRelativePath);
	if (QFile::exists(ressourceFolderPath))
	{
		return QUrl::fromLocalFile(ressourceFolderPath);
	}

#endif

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
	return QStringLiteral("qrc:///") + pRelativePath;

#else
	const QString path = FileDestination::getPath(pRelativePath);
	return QUrl::fromLocalFile(path);

#endif
}


void UIPlugInQml::doRefresh()
{
	qCDebug(qml) << "Reload qml files";
	QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}


QString UIPlugInQml::getPlatformStyle() const
{
	return mExplicitPlatformStyle;
}


void UIPlugInQml::applyPlatformStyle(const QString& pPlatformStyle)
{
	if (mExplicitPlatformStyle != pPlatformStyle)
	{
		mExplicitPlatformStyle = pPlatformStyle;
		doRefresh();
	}
}


bool UIPlugInQml::isDeveloperBuild() const
{
#ifndef QT_NO_DEBUG
	return true;

#else
	return false;

#endif
}
