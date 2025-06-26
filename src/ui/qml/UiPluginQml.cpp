/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "AuthModel.h"
#include "CertificateDescriptionModel.h"
#include "ChangePinModel.h"
#include "ChatModel.h"
#include "Env.h"
#include "Initializer.h"
#include "LogHandler.h"
#include "NumberModel.h"
#include "PersonalizationModel.h"
#include "PlatformTools.h"
#include "RemoteServiceModel.h"
#include "SelfAuthModel.h"
#include "Service.h"
#include "SettingsModel.h"
#include "SmartModel.h"
#include "VersionNumber.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"

#ifdef USE_CUSTOM_REGISTRATION
	#include "DiagnosisModel.h"
	#include "SectionModel.h"
#endif


#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

#if defined(Q_OS_ANDROID)
	#include "UiLoader.h"

	#include <QFont>
	#include <QJniEnvironment>
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	#include <QAbstractButton>
	#include <QMessageBox>
	#include <QStyle>
#endif

#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QFontInfo>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QOpenGLContext>
#include <QQmlContext>
#include <QQuickStyle>
#include <QScopeGuard>
#include <QScreen>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
	#include <QStyleHints>
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(6, 8, 0))
	#include <QSvgRenderer>
#endif
#include <QtPlugin>
#include <QtQml>

#ifdef Q_OS_WIN
	#include <dwmapi.h>
	#include <sdkddkver.h>
	#include <windows.h>
#endif

Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QList<QQmlError>>("QList<QQmlError>");
		})


UiPluginQml::UiPluginQml()
	: UiPluginModel()
	, mEngine()
#ifdef Q_OS_MACOS
	, mMenuBar()
#endif
	, mQmlEngineWarningCount(0)
	, mTrayIcon()
	, mHighContrastEnabled(false)
	, mDarkMode(false)
	, mShowFocusIndicator(false)
	, mScaleFactor(1.0)
	, mFontScaleFactor(getSystemFontScaleFactor())
#ifdef Q_OS_IOS
	, mPrivate(new Private())
#endif
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 8, 0))
	QSvgRenderer::setDefaultOptions(QtSvg::AssumeTrustedSource);
#endif

	Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);

	QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/AusweisApp_Roboto_Regular.ttf"));
	QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/AusweisApp_Roboto_Medium.ttf"));
	QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/AusweisApp_Roboto_Bold.ttf"));
	onUseSystemFontChanged();

#ifndef Q_OS_ANDROID
	QGuiApplication::setWindowIcon(mTrayIcon.getIcon());
#endif
	QGuiApplication::setDesktopFileName(QStringLiteral("com.governikus.ausweisapp2"));

	connect(&mTrayIcon, &TrayIcon::fireShow, this, &UiPluginQml::show);
	connect(&mTrayIcon, &TrayIcon::fireQuit, this, [this] {
				Q_EMIT fireQuitApplicationRequest();
			});
	connect(this, &UiPluginQml::fireAppConfigChanged, this, &UiPluginQml::onAppConfigChanged);
	onAppConfigChanged();

	qApp->installEventFilter(this);
}


void UiPluginQml::registerQmlTypes()
{
#ifdef USE_CUSTOM_REGISTRATION
	constexpr const char* cModuleName = "Governikus.Type";
	qmlRegisterModule(cModuleName, 1, 0);

	qmlRegisterUncreatableMetaObject(EnumUiModule::staticMetaObject, cModuleName, 1, 0, "UiModule", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableMetaObject(EnumReaderManagerPluginType::staticMetaObject, cModuleName, 1, 0, "ReaderManagerPluginType", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableMetaObject(EnumCardReturnCode::staticMetaObject, cModuleName, 1, 0, "CardReturnCode", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableMetaObject(EnumGAnimation::staticMetaObject, cModuleName, 1, 0, "GAnimation", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableMetaObject(EnumGlobalStatusCode::staticMetaObject, cModuleName, 1, 0, "GlobalStatusCode", QStringLiteral("Not creatable as it is an enum type"));

	#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	qmlRegisterType<DiagnosisModel>(cModuleName, 1, 0, "DiagnosisModel");
	qmlRegisterType<SectionModel>(cModuleName, 1, 0, "SectionModel");
	#endif
#endif
}


void UiPluginQml::init()
{
	if (isChromeOS())
	{
		qputenv("QT_QUICK_CONTROLS_HOVER_ENABLED", "1");
	}

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	const QStringList cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	if (!cachePaths.isEmpty() && !cachePaths.first().isEmpty())
	{
		QString cacheBasePath = cachePaths.first();
		cacheBasePath.replace(QStringLiteral("AusweisApp"), QStringLiteral("AusweisApp2"));
		cacheBasePath.append(QStringLiteral("/qmlcache"));
		qputenv("QML_DISK_CACHE_PATH", cacheBasePath.toUtf8());
	}
#endif

	const auto basicStyle = QStringLiteral("Basic");
	if (QQuickStyle::name() != basicStyle)
	{
		QQuickStyle::setStyle(basicStyle);
	}

#ifdef Q_OS_MACOS
	mMenuBar.reset(new QMenuBar());
#endif
	mEngine.reset(new QQmlApplicationEngine());

	connect(mEngine.data(), &QQmlApplicationEngine::warnings, this, &UiPluginQml::onQmlWarnings, Qt::QueuedConnection);
	connect(mEngine.data(), &QQmlApplicationEngine::objectCreated, this, &UiPluginQml::onQmlObjectCreated, Qt::QueuedConnection);

	UiPluginQml::registerQmlTypes();

	mEngine->addImportPath(QStringLiteral("qrc:///qml/"));
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
	mEngine->addImportPath(QStringLiteral("qrc:///qt/qml/"));
	mEngine->load(QStringLiteral("qrc:///qt/qml/Governikus/Init/App.qml"));
#else
	#ifndef QT_NO_DEBUG
	adjustQmlImportPath(mEngine.data());
	#endif
	mEngine->loadFromModule(QAnyStringView("Governikus.Init"), QAnyStringView("App"));
#endif

	QQuickWindow* rootWindow = getRootWindow();
	if (rootWindow != nullptr)
	{
		connect(rootWindow, &QQuickWindow::sceneGraphInitialized, this, &UiPluginQml::fireSafeAreaMarginsChanged, Qt::QueuedConnection);
		connect(rootWindow->screen(), &QScreen::orientationChanged, this, &UiPluginQml::fireSafeAreaMarginsChanged, Qt::QueuedConnection);
		connect(rootWindow, &QQuickWindow::sceneGraphError, this, &UiPluginQml::onSceneGraphError);
		qCDebug(qml) << "Using renderer interface:" << rootWindow->rendererInterface()->graphicsApi();

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
		rootWindow->resize(getInitialWindowSize());
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
		setOsDarkMode(qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark);
#endif
	}

	onWindowPaletteChanged();
	onUserDarkModeChanged();

#ifdef Q_OS_WIN
	QMetaObject::invokeMethod(this, [] {
				Env::getSingleton<Service>()->runUpdateIfNeeded(true);
			}, Qt::QueuedConnection);
#endif
}


void UiPluginQml::hideFromTaskbar() const
{
	PlatformTools::hideFromTaskbar();
}


#ifndef QT_NO_DEBUG
QString UiPluginQml::adjustQmlImportPath(QQmlEngine* pEngine)
{
	auto primaryPrefix = QStringLiteral("qrc:/qt/qml");

	const auto& platform = getOverridePlatform();
	if (!platform.isEmpty() && pEngine)
	{
		qCDebug(qml) << "Override platform:" << platform;
		auto importPath = pEngine->importPathList();
		QMutableListIterator iter(importPath);
		while (iter.hasNext())
		{
			if (iter.next() == primaryPrefix)
			{
				iter.remove();
			}
		}
		primaryPrefix = QStringLiteral("qrc:/%1").arg(platform);
		importPath << primaryPrefix;
		pEngine->setImportPathList(importPath);
	}

	return primaryPrefix;
}


#endif


QString UiPluginQml::getOverridePlatform()
{
	QString platform;

	const char* const overrideSelector = "OVERRIDE_PLATFORM";
	if (!qEnvironmentVariableIsEmpty(overrideSelector))
	{
		platform = qEnvironmentVariable(overrideSelector);
	}

	return platform;
}


void UiPluginQml::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (isDominated())
	{
		return;
	}

	const auto& context = pRequest->getContext();
	context->claim(this);
	Env::getSingleton<ApplicationModel>()->keepScreenOn(true);
	Env::getSingleton<ApplicationModel>()->resetContext(context);
	Env::getSingleton<NumberModel>()->resetContext(context);
	Env::getSingleton<VolatileSettings>()->setDelay(1000);

	if (auto changePinContext = context.objectCast<ChangePinContext>())
	{
		if (changePinContext->isActivateUi())
		{
			onShowUi(UiModule::PINMANAGEMENT);
		}
		Env::getSingleton<ChangePinModel>()->resetChangePinContext(changePinContext);
	}

#if __has_include("context/PersonalizationContext.h")
	if (auto smartContext = context.objectCast<PersonalizationContext>())
	{
		onShowUi(UiModule::SMART_EID);
		Env::getSingleton<PersonalizationModel>()->resetPersonalizationContext(smartContext);
		Env::getSingleton<CertificateDescriptionModel>()->resetContext(smartContext);
		Env::getSingleton<ChatModel>()->resetContext(smartContext);
	}
	else
#endif
	{
		if (auto authContext = context.objectCast<AuthContext>())
		{
			if (authContext->isActivateUi())
			{
				onShowUi(UiModule::IDENTIFY);
			}
			Env::getSingleton<AuthModel>()->resetAuthContext(authContext);
			Env::getSingleton<CertificateDescriptionModel>()->resetContext(authContext);
			Env::getSingleton<ChatModel>()->resetContext(authContext);
		}
	}

	if (auto authContext = context.objectCast<SelfAuthContext>())
	{
		Env::getSingleton<SelfAuthModel>()->resetContext(authContext);
	}

	if (auto remoteServiceContext = context.objectCast<IfdServiceContext>())
	{
		Env::getSingleton<RemoteServiceModel>()->resetRemoteServiceContext(remoteServiceContext);
		Env::getSingleton<ChatModel>()->resetContext(remoteServiceContext);
		Env::getSingleton<CertificateDescriptionModel>()->resetContext(remoteServiceContext);
	}
}


void UiPluginQml::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	const auto& context = pRequest->getContext();
	Env::getSingleton<ApplicationModel>()->keepScreenOn(false);
	Env::getSingleton<ApplicationModel>()->resetContext();
	Env::getSingleton<NumberModel>()->resetContext();
	Env::getSingleton<VolatileSettings>()->setDelay();

	if (context.objectCast<ChangePinContext>())
	{
		Env::getSingleton<ChangePinModel>()->resetChangePinContext();
	}

	if (const auto& authContext = context.objectCast<AuthContext>())
	{
		Env::getSingleton<AuthModel>()->resetAuthContext();
		Env::getSingleton<CertificateDescriptionModel>()->resetContext();
		Env::getSingleton<ChatModel>()->resetContext();

		const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();

		// Only hide the UI if we don't need to show the update information view. This behaviour ensures that
		// the user is (aggressively) notified about a pending update if the AA is only shown for authentication
		// workflows and never manually brought to foreground in between.
		if (!context.objectCast<SelfAuthContext>()
#if __has_include("context/PersonalizationContext.h")
				&& !context.objectCast<PersonalizationContext>()
#endif
				&& !context->hasNextWorkflowPending()
				&& generalSettings.isAutoCloseWindowAfterAuthentication()
				&& !showUpdateInformationIfPending()
				&& !authContext->changeTransportPin())
		{
			onHideUi();
		}
	}

	if (context.objectCast<SelfAuthContext>())
	{
		Env::getSingleton<SelfAuthModel>()->resetContext();
	}

	if (context.objectCast<IfdServiceContext>())
	{
		Env::getSingleton<RemoteServiceModel>()->resetRemoteServiceContext();
		Env::getSingleton<ChatModel>()->resetContext();
		Env::getSingleton<CertificateDescriptionModel>()->resetContext();
	}

#if __has_include("context/PersonalizationContext.h")
	if (context.objectCast<PersonalizationContext>())
	{
		Env::getSingleton<PersonalizationModel>()->resetPersonalizationContext();
	}
#endif

	Env::getSingleton<SmartModel>()->workflowFinished(context);
}


void UiPluginQml::onApplicationInitialized()
{
	connect(Env::getSingleton<AuthModel>(), &AuthModel::fireShowUiRequest, this, &UiPlugin::fireShowUiRequested);
	connect(Env::getSingleton<ChangePinModel>(), &ChangePinModel::fireStartWorkflow, this, &UiPlugin::fireWorkflowRequested);
	connect(Env::getSingleton<SelfAuthModel>(), &SelfAuthModel::fireStartWorkflow, this, &UiPlugin::fireWorkflowRequested);
	connect(Env::getSingleton<RemoteServiceModel>(), &RemoteServiceModel::fireStartWorkflow, this, &UiPlugin::fireWorkflowRequested);
	connect(Env::getSingleton<PersonalizationModel>(), &PersonalizationModel::fireStartWorkflow, this, &UiPlugin::fireWorkflowRequested);
	connect(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireRawLog, this, &UiPluginQml::onRawLog, Qt::QueuedConnection);
	connect(Env::getSingleton<SettingsModel>(), &SettingsModel::fireTrayIconEnabledChanged, this, &UiPluginQml::onTrayIconEnabledChanged);
	connect(Env::getSingleton<SettingsModel>(), &SettingsModel::fireUseSystemFontChanged, this, &UiPluginQml::onUseSystemFontChanged);
	connect(Env::getSingleton<SettingsModel>(), &SettingsModel::fireDarkModeChanged, this, &UiPluginQml::onUserDarkModeChanged);
	connect(Env::getSingleton<SettingsModel>(), &SettingsModel::fireDarkModeChanged, this, &UiPluginQml::fireDarkModeEnabledChanged);

	const auto* service = Env::getSingleton<Service>();
	connect(service, &Service::fireAppcastFinished, this, &UiPluginModel::setUpdatePending);
	connect(service, &Service::fireUpdateScheduled, this, &UiPluginQml::onUpdateScheduled);

	init();
}


void UiPluginQml::onApplicationStarted()
{
	mTrayIcon.create();
	connect(this, &UiPluginQml::fireTranslationChanged, &mTrayIcon, &TrayIcon::onTranslationChanged);
	connect(this, &UiPluginQml::fireSystemSettingsChanged, Env::getSingleton<ApplicationModel>(), &ApplicationModel::fireScreenReaderRunningChanged);

#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	const bool showOnboarding = Env::getSingleton<SettingsModel>()->getStartupModule() == UiModule::ONBOARDING;
	const bool developerMode = generalSettings.isDeveloperMode();
	const bool missingTrayIcon = !QSystemTrayIcon::isSystemTrayAvailable() || !Env::getSingleton<AppSettings>()->getGeneralSettings().isTrayIconEnabled();
	if (missingTrayIcon || showOnboarding || developerMode)
#endif
	{
		QMetaObject::invokeMethod(this, &UiPluginQml::show, Qt::QueuedConnection);
	}

#if defined(Q_OS_ANDROID)
	QNativeInterface::QAndroidApplication::hideSplashScreen(250);
#endif

#ifdef Q_OS_WIN
	Env::getSingleton<AppSettings>()->getGeneralSettings().migrateSettings();
#endif
}


void UiPluginQml::onShowUi(UiModule pModule)
{
	PlatformTools::restoreToTaskbar();
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	const auto& startupModule = Env::getSingleton<SettingsModel>()->getStartupModule();
	if (pModule == UiModule::CURRENT && startupModule == UiModule::ONBOARDING)
	{
		pModule = UiModule::ONBOARDING;
	}
#endif
	if (isDominated())
	{
		pModule = UiModule::CURRENT;
	}
	Q_EMIT fireShowRequest(pModule);

	Env::getSingleton<Service>()->runUpdateIfNeeded();
}


void UiPluginQml::onHideUi()
{
	Q_EMIT fireHideRequest();
}


void UiPluginQml::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	ProxyCredentials proxyCredentials(pProxy, pAuthenticator);

	Q_EMIT fireProxyAuthenticationRequired(&proxyCredentials);
	proxyCredentials.waitForConfirmation();
}


void UiPluginQml::onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted)
{
	if (pUi == this)
	{
		return;
	}

	if (pAccepted)
	{
		mDominator = pInformation.isEmpty() ? QLatin1String("") : pInformation;
		Q_EMIT fireDominatorChanged();
	}
}


void UiPluginQml::onUiDominationReleased()
{
	if (!mDominator.isNull())
	{
		mDominator.clear();
		Q_EMIT fireDominatorChanged();
	}
}


void UiPluginQml::onShowUserInformation(const QString& pMessage)
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	Env::getSingleton<ApplicationModel>()->showFeedback(pMessage);
#else
	QMessageBox msgBox;
	msgBox.setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Information"));
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText(pMessage);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.button(QMessageBox::Ok)->setFocus();
	msgBox.exec();
#endif
}


void UiPluginQml::onUpdateScheduled() const
{
	if (!isHidden())
	{
		Env::getSingleton<Service>()->runUpdateIfNeeded();
	}
}


void UiPluginQml::show()
{
	if (!showUpdateInformationIfPending())
	{
		onShowUi(UiModule::CURRENT);
	}
}


bool UiPluginQml::eventFilter(QObject* pObj, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::ApplicationPaletteChange)
	{
		onWindowPaletteChanged();
		return true;
	}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
	if (pEvent->type() == QEvent::ThemeChange)
	{
		setOsDarkMode(qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark);
		return true;
	}
#endif

	if (pEvent->type() == QEvent::FocusIn)
	{
		const auto focusEvent = static_cast<QFocusEvent*>(pEvent);
		const auto focusReason = focusEvent->reason();

		mShowFocusIndicator = focusReason == Qt::TabFocusReason ||
				focusReason == Qt::BacktabFocusReason ||
				focusReason == Qt::OtherFocusReason;
		Q_EMIT fireShowFocusIndicator();
	}

	return QObject::eventFilter(pObj, pEvent);
}


void UiPluginQml::onTranslationChanged()
{
	if (mEngine)
	{
		mEngine->retranslate();
	}
	Q_EMIT fireTranslationChanged();
}


void UiPluginQml::doShutdown()
{
	mTrayIcon.shutdown();
}


QQuickWindow* UiPluginQml::getRootWindow() const
{
	if (!mEngine || mEngine->rootObjects().isEmpty())
	{
		return nullptr;
	}

	return qobject_cast<QQuickWindow*>(mEngine->rootObjects().first());
}


bool UiPluginQml::isHidden() const
{
	const QQuickWindow* rootWindow = getRootWindow();
	return !rootWindow || rootWindow->visibility() == QWindow::Hidden;
}


void UiPluginQml::onQmlWarnings(const QList<QQmlError>& pWarnings)
{
	mQmlEngineWarningCount += pWarnings.size();

#ifndef QT_NO_DEBUG
	for (const auto& warning : pWarnings)
	{
		Env::getSingleton<ApplicationModel>()->showFeedback(QStringLiteral("Got QML warning: %1").arg(warning.toString()));
	}
#endif
}


void UiPluginQml::onQmlObjectCreated(const QObject* pObject)
{
	const bool fatalErrors = pObject == nullptr;
	const QString result = fatalErrors ? QStringLiteral("fatal errors.") : QStringLiteral("%1 warnings.").arg(mQmlEngineWarningCount);
	qCDebug(qml).noquote() << "QML engine initialization finished with" << result;
#ifndef QT_NO_DEBUG
	if (fatalErrors)
	{
		Q_EMIT fireQuitApplicationRequest(EXIT_FAILURE);
	}
#endif
}


void UiPluginQml::onSceneGraphError(QQuickWindow::SceneGraphError pError, const QString& pMessage)
{
	qCDebug(qml) << "Cannot initialize UI rendering:" << pError << '|' << pMessage;

	const char* envKey = "QT_QUICK_BACKEND";
	const auto envValue = QByteArrayLiteral("software");
	if (qgetenv(envKey) != envValue)
	{
		qputenv(envKey, envValue);
		qCDebug(qml) << "Restart application with" << envKey << envValue;
		Q_EMIT fireRestartApplicationRequested();
	}
}


void UiPluginQml::onRawLog(const QString& pMessage, const QString& pCategoryName)
{
	if (pCategoryName == QLatin1String("developermode") || pCategoryName == QLatin1String("feedback"))
	{
#ifdef Q_OS_MACOS
		PlatformTools::postNotification(QCoreApplication::applicationName(), pMessage);
#else
		mTrayIcon.showMessage(QCoreApplication::applicationName(), pMessage);
#endif
	}
}


void UiPluginQml::doRefresh()
{
	qCDebug(qml) << "Reload qml files";
	QMetaObject::invokeMethod(this, &UiPluginQml::init, Qt::QueuedConnection);
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	QMetaObject::invokeMethod(this, [this]{
				Q_EMIT fireShowUiRequested(UiModule::CURRENT);
			}, Qt::QueuedConnection);
#endif
}


QString UiPluginQml::getQtVersion() const
{
	return QString::fromLatin1(qVersion());
}


bool UiPluginQml::isDebugBuild() const
{
#ifndef QT_NO_DEBUG
	return true;

#else
	return false;

#endif
}


bool UiPluginQml::isDeveloperVersion() const
{
	return VersionNumber::getApplicationVersion().isDeveloperVersion();
}


QString UiPluginQml::getDominator() const
{
	return mDominator;
}


bool UiPluginQml::isDominated() const
{
	return !mDominator.isNull();
}


#ifndef Q_OS_IOS
QVariantMap UiPluginQml::getSafeAreaMargins() const
{
	QVariantMap marginMap;

	marginMap[QStringLiteral("top")] = 0;
	marginMap[QStringLiteral("right")] = 0;
	marginMap[QStringLiteral("bottom")] = 0;
	marginMap[QStringLiteral("left")] = 0;

	#ifdef Q_OS_ANDROID
	if (QJniObject activity = QNativeInterface::QAndroidApplication::context(); activity.isValid())
	{
		auto screen = QGuiApplication::primaryScreen();
		auto windowInsets = activity.callObjectMethod("getWindowInsets", "()Landroid/view/ViewGroup$MarginLayoutParams;");

		marginMap[QStringLiteral("top")] = windowInsets.getField<jint>("topMargin") / screen->devicePixelRatio();
		marginMap[QStringLiteral("left")] = windowInsets.getField<jint>("leftMargin") / screen->devicePixelRatio();
		marginMap[QStringLiteral("right")] = windowInsets.getField<jint>("rightMargin") / screen->devicePixelRatio();
		marginMap[QStringLiteral("bottom")] = windowInsets.getField<jint>("bottomMargin") / screen->devicePixelRatio();
	}
	#endif

	return marginMap;
}


#endif

#ifdef Q_OS_ANDROID
extern "C"
{

JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_notifySafeAreaMarginsChanged(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)

	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
				UiPluginQml* const uiPlugin = Env::getSingleton<UiLoader>()->getLoaded<UiPluginQml>();
				if (uiPlugin)
				{
					Q_EMIT uiPlugin->fireSafeAreaMarginsChanged();
				}
			}, Qt::QueuedConnection);
}


}

#endif

#ifndef Q_OS_MACOS
bool UiPluginQml::isHighContrastEnabled() const
{
	#ifdef Q_OS_WIN
	HIGHCONTRAST hc;
	hc.cbSize = sizeof(hc);
	return SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, FALSE) && (hc.dwFlags & HCF_HIGHCONTRASTON);

	#else
	return false;

	#endif
}


#endif

bool UiPluginQml::isOsDarkModeEnabled() const
{
	return mDarkMode;
}


void UiPluginQml::setOsDarkMode(bool pState)
{
	if (mDarkMode != pState)
	{
		qCDebug(qml) << "Dark mode setting has changed";
		mDarkMode = pState;

		onUserDarkModeChanged();

		Q_EMIT fireDarkModeEnabledChanged();
	}
}


bool UiPluginQml::isOsDarkModeSupported() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
	return true;

#else
	return false;

#endif
}


bool UiPluginQml::isDarkModeEnabled() const
{
	const auto userDarkMode = Env::getSingleton<SettingsModel>()->getDarkMode();
	switch (userDarkMode)
	{
		case SettingsModel::ModeOption::ON:
			return true;

		case SettingsModel::ModeOption::OFF:
			return false;

		case SettingsModel::ModeOption::AUTO:
			return isOsDarkModeEnabled();
	}

	Q_UNREACHABLE();
}


QString UiPluginQml::getFixedFontFamily() const
{
	return QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
}


QSize UiPluginQml::getInitialWindowSize() const
{
	const auto& platform = getOverridePlatform();
	if (platform == QAnyStringView("android") || platform == QAnyStringView("ios"))
	{
		return QSize(432, 768);
	}
	return QSize(960, 720);
}


bool UiPluginQml::getShowFocusIndicator() const
{
	return mShowFocusIndicator;
}


qreal UiPluginQml::getScaleFactor() const
{

	return mScaleFactor;
}


void UiPluginQml::setScaleFactor(qreal pScaleFactor)
{
	if (qAbs(pScaleFactor - mScaleFactor) > 0)
	{
		mScaleFactor = pScaleFactor;
		Q_EMIT fireScaleFactorChanged();
	}

}


qreal UiPluginQml::getFontScaleFactor() const
{
	return mFontScaleFactor;
}


bool UiPluginQml::isChromeOS() const
{
#ifdef Q_OS_ANDROID
	if (QJniObject activity = QNativeInterface::QAndroidApplication::context(); activity.isValid())
	{
		return activity.callMethod<jboolean>("isChromeOS");
	}
#endif

	return false;
}


void UiPluginQml::setFontScaleFactor(qreal pFactor)
{
	if (pFactor != mFontScaleFactor)
	{
		qCDebug(qml) << "System font scale factor has changed";
		mFontScaleFactor = pFactor;
		Q_EMIT fireFontScaleFactorChanged();
	}
}


void UiPluginQml::onWindowPaletteChanged()
{
	const bool highContrast = isHighContrastEnabled();
	if (mHighContrastEnabled != highContrast)
	{
		mHighContrastEnabled = highContrast;
		Q_EMIT fireHighContrastEnabledChanged();
	}
}


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
void UiPluginQml::onUserDarkModeChanged() const
{
}


#endif

void UiPluginQml::onUseSystemFontChanged() const
{
	const auto guard = qScopeGuard([] {
				const auto& family = QGuiApplication::font().family();
				qDebug() << "Using" << family << "with styles" << QFontDatabase::styles(family);
			});

	if (Env::getSingleton<AppSettings>()->getGeneralSettings().isUseSystemFont())
	{
		auto font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
		if (QFontMetrics(font.family()).horizontalAdvance(QLatin1Char('m')) > 15)
		{
			// Fonts like "DejaVu Sans" (used on some Linux distributions) are unusually wide when compared to Windows' and macOS' default font. This will break some layouts where this wasn't taken into account.
			const auto oldFamily = QFontInfo(font).family();
			font.setFamily(QStringLiteral("Arial")); // will usually resolve to "Liberation Sans" or "Arimo"
			qCDebug(qml) << "Changing font family from" << oldFamily << "to" << QFontInfo(font).family();
		}
		QGuiApplication::setFont(font);
		return;
	}

	const auto& knownFamilies = QFontDatabase::families();
	const QStringList families = {QStringLiteral("AusweisApp_Roboto"), QStringLiteral("Roboto")};
	for (const auto& family : families)
	{
		if (knownFamilies.contains(family))
		{
			QGuiApplication::setFont(QFont(family));
			return;
		}
	}

	qCCritical(qml) << "Roboto was not found in the FontDatabase. Staying on system default.";
}


void UiPluginQml::onTrayIconEnabledChanged()
{
	mTrayIcon.setVisible(Env::getSingleton<AppSettings>()->getGeneralSettings().isTrayIconEnabled());
}


void UiPluginQml::onAppConfigChanged()
{
	setFontScaleFactor(getSystemFontScaleFactor());
}
