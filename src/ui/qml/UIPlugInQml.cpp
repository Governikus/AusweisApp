/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include "ApplicationModel.h"
#include "AppSettings.h"
#include "AuthModel.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"
#include "CardReturnCode.h"
#include "ChangePinModel.h"
#include "DpiCalculator.h"
#include "Env.h"
#include "FileDestination.h"
#include "LogModel.h"
#include "PlatformTools.h"
#include "ProviderCategoryFilterModel.h"
#include "RemoteServiceModel.h"
#include "Service.h"
#include "StatusBarUtil.h"

#if defined(Q_OS_ANDROID)
	#include <QFont>
	#include <QtAndroidExtras/QAndroidJniEnvironment>
	#include <QtAndroidExtras/QtAndroid>
#endif

#include <QFile>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QOperatingSystemVersion>
#include <QQmlContext>
#include <QScreen>
#include <QtPlugin>
#include <QtQml>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


template<typename T>
QObject* provideQmlType(QQmlEngine* pEngine, QJSEngine* pScriptEngine)
{
	Q_UNUSED(pEngine)
	Q_UNUSED(pScriptEngine)

	return new T();
}


template<typename T>
static QObject* provideSingletonQmlType(QQmlEngine* pEngine, QJSEngine* pScriptEngine)
{
	Q_UNUSED(pScriptEngine);

	const auto model = Env::getSingleton<T>();
	pEngine->setObjectOwnership(model, QQmlEngine::CppOwnership);
	return model;
}


template<typename T>
static void registerQmlType(QObject* (*pTypeProvider)(QQmlEngine*, QJSEngine*))
{
	QByteArray qmlName(T::staticMetaObject.className());
	qmlName.replace(QByteArrayLiteral("governikus::"), QByteArray());

	const QByteArray url = QByteArrayLiteral("Governikus.Type.") + qmlName;

	qmlRegisterSingletonType<T>(url.constData(), 1, 0, qmlName.constData(), pTypeProvider);
}


UIPlugInQml::UIPlugInQml()
	: mEngine()
	, mHistoryModel(&Env::getSingleton<AppSettings>()->getHistorySettings())
	, mVersionInformationModel()
	, mQmlExtension()
	, mSelfAuthModel()
	, mSettingsModel()
	, mCertificateDescriptionModel()
	, mChatModel()
	, mExplicitPlatformStyle(getPlatformSelectors())
	, mConnectivityManager()
	, mTrayIcon()
{
#if defined(Q_OS_ANDROID)
	QGuiApplication::setFont(QFont(QStringLiteral("Roboto")));
#endif

	connect(&mTrayIcon, &TrayIcon::fireShow, this, &UIPlugInQml::show);
	connect(&mTrayIcon, &TrayIcon::fireQuit, this, &UIPlugInQml::fireQuitApplicationRequest);

	connect(Env::getSingleton<ChangePinModel>(), &ChangePinModel::fireStartWorkflow, this, &UIPlugIn::fireChangePinRequest);
	connect(&mSelfAuthModel, &SelfAuthModel::fireStartWorkflow, this, &UIPlugIn::fireSelfAuthenticationRequested);
	connect(Env::getSingleton<RemoteServiceModel>(), &RemoteServiceModel::fireStartWorkflow, this, &UIPlugIn::fireRemoteServiceRequested);
	connect(this, &UIPlugIn::fireShowUserInformation, this, &UIPlugInQml::onShowUserInformation);
	init();
}


void UIPlugInQml::registerQmlTypes()
{
	qmlRegisterUncreatableType<EnumUiModule>("Governikus.Type.UiModule", 1, 0, "UiModule", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableType<EnumReaderManagerPlugInType>("Governikus.Type.ReaderPlugIn", 1, 0, "ReaderPlugIn", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableType<EnumCardReturnCode>("Governikus.Type.CardReturnCode", 1, 0, "CardReturnCode", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableType<EnumPacePasswordId>("Governikus.Type.PacePasswordId", 1, 0, "PacePasswordId", QStringLiteral("Not creatable as it is an enum type"));

	registerQmlType<ProviderCategoryFilterModel>(&provideQmlType<ProviderCategoryFilterModel> );
	registerQmlType<StatusBarUtil>(&provideQmlType<StatusBarUtil> );

	registerQmlType<ApplicationModel>(&provideSingletonQmlType<ApplicationModel> );
	registerQmlType<AuthModel>(&provideSingletonQmlType<AuthModel> );
	registerQmlType<ChangePinModel>(&provideSingletonQmlType<ChangePinModel> );
	registerQmlType<NumberModel>(&provideSingletonQmlType<NumberModel> );
	registerQmlType<RemoteServiceModel>(&provideSingletonQmlType<RemoteServiceModel> );
	registerQmlType<LogModel>(&provideSingletonQmlType<LogModel> );
}


void UIPlugInQml::init()
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	qputenv("QML_DISABLE_DISK_CACHE", "true");
#endif

	mEngine.reset(new QQmlApplicationEngine());

	mEngine->rootContext()->setContextProperty(QStringLiteral("plugin"), this);
	QQmlFileSelector::get(mEngine.data())->setExtraSelectors(mExplicitPlatformStyle.split(QLatin1Char(',')));

	mEngine->rootContext()->setContextProperty(QStringLiteral("screenDpiScale"), DpiCalculator::getDpiScale());
	mEngine->rootContext()->setContextProperty(QStringLiteral("historyModel"), &mHistoryModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("versionInformationModel"), &mVersionInformationModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("qmlExtension"), &mQmlExtension);
	mEngine->rootContext()->setContextProperty(QStringLiteral("selfAuthModel"), &mSelfAuthModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("settingsModel"), &mSettingsModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("certificateDescriptionModel"), &mCertificateDescriptionModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("chatModel"), &mChatModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("connectivityManager"), &mConnectivityManager);

	UIPlugInQml::registerQmlTypes();

	mEngine->addImportPath(getPath(QStringLiteral("qml/"), false).toString());
	mEngine->addImportPath(getPath(QStringLiteral("qml/")).toString());
	mEngine->load(getPath(QStringLiteral("qml/main.qml")));

	Env::getSingleton<Service>()->updateConfigurations();
}


void UIPlugInQml::hide()
{
	PlatformTools::hideFromTaskbar();
}


QString UIPlugInQml::getPlatformSelectors() const
{
#ifndef QT_NO_DEBUG
	const char* overrideSelector = "OVERRIDE_PLATFORM_SELECTOR";
	if (!qEnvironmentVariableIsEmpty(overrideSelector))
	{
		const auto& platform = QString::fromLocal8Bit(qgetenv(overrideSelector));
		qCDebug(qml) << "Override platform selector:" << platform;
		if (platform.startsWith(QLatin1Char('+')) || platform.contains(QLatin1String(",+")))
		{
			Q_ASSERT(false && "Please supply selectors without a '+'.");
		}
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
	const QString platform = QStringLiteral("mobile,");
	const QString tablet = (isTablet ? QStringLiteral("tablet,") : QStringLiteral("phone,"));
	const QString brand = QGuiApplication::platformName();
#else
	const QString platform = QStringLiteral("desktop,");
	Q_UNUSED(isTablet);
	const QString tablet;
#if defined(Q_OS_MAC)
	const QString brand = QStringLiteral("mac");
#else
	const QString brand = QStringLiteral("win");
#endif
#endif

	return platform + tablet + brand;
}


void UIPlugInQml::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	onShowUi(UiModule::IDENTIFY);
	mQmlExtension.keepScreenOn(true);

	Env::getSingleton<ApplicationModel>()->resetContext(pContext);
	Env::getSingleton<NumberModel>()->resetContext(pContext);

	if (auto changePinContext = pContext.objectCast<ChangePinContext>())
	{
		Env::getSingleton<ChangePinModel>()->resetContext(changePinContext);
	}

	if (auto authContext = pContext.objectCast<AuthContext>())
	{
		mConnectivityManager.startWatching();
		Env::getSingleton<AuthModel>()->resetContext(authContext);
		mCertificateDescriptionModel.resetContext(authContext);
		mChatModel.resetContext(authContext);
	}

	if (auto authContext = pContext.objectCast<SelfAuthContext>())
	{
		mSelfAuthModel.resetContext(authContext);
	}

	if (auto remoteServiceContext = pContext.objectCast<RemoteServiceContext>())
	{
		Env::getSingleton<RemoteServiceModel>()->resetContext(remoteServiceContext);
	}
}


void UIPlugInQml::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	mQmlExtension.keepScreenOn(false);

	Env::getSingleton<ApplicationModel>()->resetContext();
	Env::getSingleton<NumberModel>()->resetContext();

	if (pContext.objectCast<ChangePinContext>())
	{
		Env::getSingleton<ChangePinModel>()->resetContext();
	}

	if (pContext.objectCast<AuthContext>())
	{
		mConnectivityManager.stopWatching();
		Env::getSingleton<AuthModel>()->resetContext();
		mCertificateDescriptionModel.resetContext();
		mChatModel.resetContext();
	}

	if (pContext.objectCast<SelfAuthContext>())
	{
		mSelfAuthModel.resetContext();
	}

	if (pContext.objectCast<RemoteServiceContext>())
	{
		Env::getSingleton<RemoteServiceModel>()->resetContext();
	}
}


void UIPlugInQml::onApplicationStarted()
{
	mTrayIcon.create();
	show();
}


void UIPlugInQml::onShowUi(UiModule pModule)
{
	PlatformTools::restoreToTaskbar();
	Q_EMIT fireShowRequest(pModule);
}


void UIPlugInQml::onShowUserInformation(const QString& pMessage)
{
	mQmlExtension.showFeedback(pMessage);
}


void UIPlugInQml::show()
{
	onShowUi(UiModule::CURRENT);
}


void UIPlugInQml::doShutdown()
{
}


QUrl UIPlugInQml::getPath(const QString& pRelativePath, bool pQrc)
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	const QString ressourceFolderPath(QStringLiteral(RES_DIR) + QLatin1Char('/') + pRelativePath);
	if (QFile::exists(ressourceFolderPath))
	{
		return QUrl::fromLocalFile(ressourceFolderPath);
	}

#endif

	if (pQrc)
	{
		return QStringLiteral("qrc:///") + pRelativePath;
	}

	const QString path = FileDestination::getPath(pRelativePath);
	return QUrl::fromLocalFile(path);
}


void UIPlugInQml::doRefresh()
{
	qCDebug(qml) << "Reload qml files";
	QMetaObject::invokeMethod(this, &UIPlugInQml::init, Qt::QueuedConnection);
}


bool UIPlugInQml::useFlatStyleOnDesktop() const
{
	return QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::Windows8);
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
