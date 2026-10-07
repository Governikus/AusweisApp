/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

#include "UiPluginQml.h"

#include "AppSettings.h"
#include "AppUpdateDataModel.h"
#include "ApplicationModel.h"
#include "AuthModel.h"
#include "CertificateDescriptionModel.h"
#include "ChangePinModel.h"
#include "ChatModel.h"
#include "Env.h"
#include "Initializer.h"
#include "LogHandler.h"
#include "NumberModel.h"
#include "PlatformTools.h"
#include "RemoteServiceModel.h"
#include "SelfAuthModel.h"
#include "Service.h"
#include "SettingsModel.h"
#include "VersionNumber.h"
#include "VolatileSettings.h"
#include "WorkflowRequest.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"

#if defined(Q_OS_ANDROID)
	#include "UiLoader.h"

	#include <QFont>
	#include <QJniEnvironment>
	#include <QJniObject>
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
#include <QStyleHints>
#include <QSvgRenderer>
#include <QtPlugin>
#include <QtQml>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	#include <QAccessibilityHints>
#endif

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
	, mFontWeightAdjustment(getSystemFontWeightAdjustment())
	, mA11yButtonShapeActive(getA11yButtonShapeActive())
	, mA11yOnOffSwitchLabelActive(getA11yOnOffSwitchLabelActive())
#ifdef Q_OS_IOS
	, mPrivate(new Private())
#endif
{
	QSvgRenderer::setDefaultOptions(QtSvg::AssumeTrustedSource);

	Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);

#ifndef Q_OS_IOS
	QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/AusweisApp_Roboto_Regular.ttf"));
	QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/AusweisApp_Roboto_Medium.ttf"));
	QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/AusweisApp_Roboto_Bold.ttf"));
#endif

	onUseSystemFontChanged();

#ifndef Q_OS_ANDROID
	QGuiApplication::setWindowIcon(mTrayIcon.getIcon());
#endif
	QGuiApplication::setDesktopFileName(QStringLiteral("com.governikus.ausweisapp2"));
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	connect(QGuiApplication::styleHints()->accessibility(), &QAccessibilityHints::contrastPreferenceChanged, this, &UiPluginQml::fireHighContrastEnabledChanged);
#endif

	connect(&mTrayIcon, &TrayIcon::fireShow, this, &UiPluginQml::show);
	connect(&mTrayIcon, &TrayIcon::fireMessageClicked, this, [this](){
				const auto applicationModel = Env::getSingleton<ApplicationModel>();
				bool isWorkflowActive = applicationModel->getCurrentWorkflow() != ApplicationModel::Workflow::NONE;
				if (isWorkflowActive || !showUpdateInformationIfPending())
				{
					onShowUi(UiModule::CURRENT);
				}
			});
	connect(&mTrayIcon, &TrayIcon::fireQuit, this, [this] {
				Q_EMIT fireQuitApplicationRequest();
			});
	connect(this, &UiPluginQml::fireAppConfigChanged, this, &UiPluginQml::onAppConfigChanged);
	onAppConfigChanged();

	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireStatusChanged, this, &UiPluginQml::onReaderStatusChanged);

	QCoreApplication::instance()->installEventFilter(this);
}


int UiPluginQml::getFontWeightAdjustment() const
{
	return mFontWeightAdjustment;
}


void UiPluginQml::setFontWeightAdjustment(int pFontWeightAdjustment)
{
	if (pFontWeightAdjustment != mFontWeightAdjustment)
	{
		qCDebug(qml) << "System font weight adjustment has changed";
		mFontWeightAdjustment = pFontWeightAdjustment;
		Q_EMIT fireFontWeightAdjustmentChanged();
	}
}


void UiPluginQml::init()
{
	if (isChromeOS())
	{
		qputenv("QT_QUICK_CONTROLS_HOVER_ENABLED", "1");
	}
#ifdef Q_OS_ANDROID
	qputenv("QT_ANDROID_ENABLE_WORKAROUND_TO_DISABLE_PREDICTIVE_TEXT", "1");
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

	mEngine->addImportPath(QStringLiteral("qrc:///qml/"));
#ifndef QT_NO_DEBUG
	adjustQmlImportPath(mEngine.data());
#endif
	mEngine->loadFromModule(QAnyStringView("Governikus.Init"), QAnyStringView("App"));

	QQuickWindow* rootWindow = getRootWindow();
	if (rootWindow != nullptr)
	{
		connect(rootWindow, &QQuickWindow::sceneGraphError, this, &UiPluginQml::onSceneGraphError);
		qCDebug(qml) << "Using renderer interface:" << rootWindow->rendererInterface()->graphicsApi();

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
		rootWindow->resize(getInitialWindowSize());
#endif
		setOsDarkMode(QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
	}

	onUserDarkModeChanged();

#ifdef Q_OS_WIN
	QMetaObject::invokeMethod(this, [] {
				Env::getSingleton<Service>()->runUpdateIfNeeded(true);
			}, Qt::QueuedConnection);
#endif

	QGuiApplication::setAttribute(Qt::AA_QtQuickUseDefaultSizePolicy);
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
		erase_if(importPath, [primaryPrefix](const auto& pEntry){
					return pEntry == primaryPrefix;
				});
		primaryPrefix = QStringLiteral("qrc:/%1").arg(platform);
		importPath << primaryPrefix;
		pEngine->setImportPathList(importPath);
	}

	return primaryPrefix;
}


Q_INVOKABLE void UiPluginQml::emitFireShowRequest(UiModule pModule)
{
	Q_EMIT fireShowRequest(pModule);
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

		if (!context.objectCast<SelfAuthContext>() && !context->hasNextWorkflowPending())
		{
			const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			const bool skipUpdate = authContext->autoFinishBeforeQuit() && !settings.isTrayIconEnabled();
			const bool showUpdate = skipUpdate ? false : showUpdateInformationIfPending();
			if (!showUpdate && settings.isAutoCloseWindowAfterAuthentication())
			{
				onHideUi();
			}
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

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	if (!context->hasNextWorkflowPending())
	{
		Env::getSingleton<Service>()->runUpdateIfNeeded();
	}
#endif
}


void UiPluginQml::onApplicationInitialized()
{
	connect(Env::getSingleton<ChangePinModel>(), &ChangePinModel::fireStartWorkflow, this, &UiPlugin::fireWorkflowRequested);
	connect(Env::getSingleton<SelfAuthModel>(), &SelfAuthModel::fireStartWorkflow, this, &UiPlugin::fireWorkflowRequested);
	connect(Env::getSingleton<RemoteServiceModel>(), &RemoteServiceModel::fireStartWorkflow, this, &UiPlugin::fireWorkflowRequested);
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

	if (pModule == UiModule::UPDATEINFORMATION)
	{
#ifdef Q_OS_WIN
		if (!Env::getSingleton<AppUpdateDataModel>()->isUpdateAvailable())
#endif
		{
			pModule = UiModule::CURRENT;
		}
	}

	Q_EMIT fireShowRequest(pModule);

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	if (Env::getSingleton<ApplicationModel>()->getCurrentWorkflow() == ApplicationModel::Workflow::NONE)
#endif
	{
		Env::getSingleton<Service>()->runUpdateIfNeeded();
	}
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
	onShowUi(UiModule::CURRENT);
}


bool UiPluginQml::eventFilter(QObject* pObj, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::ThemeChange)
	{
		setOsDarkMode(QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
		return true;
	}

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


QQmlApplicationEngine* UiPluginQml::UiPluginQml::getEngine() const
{
	return mEngine.data();
}


QQuickWindow* UiPluginQml::getRootWindow() const
{
	if (!mEngine || mEngine->rootObjects().isEmpty())
	{
		return nullptr;
	}

	return qobject_cast<QQuickWindow*>(mEngine->rootObjects().constFirst());
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
		const auto& warningString = warning.toString();
		if (!warningString.contains(QStringLiteral("RangeError: Maximum call stack size exceeded.")))
		{
			Env::getSingleton<ApplicationModel>()->showFeedback(QStringLiteral("Got QML warning: %1").arg(warningString));
		}
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


bool UiPluginQml::isHighContrastEnabled() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	return QGuiApplication::styleHints()->accessibility()->contrastPreference() == Qt::ContrastPreference::HighContrast;

#else
	return false;

#endif
}


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


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
void UiPluginQml::onUserDarkModeChanged() const
{
}


#endif


void UiPluginQml::onUseSystemFontChanged()
{
	const auto guard = qScopeGuard([this] {
				qCDebug(qml) << "Request font family" << mFontFamily;
				const auto& family = QFontInfo(QFont(mFontFamily)).family();
				qCDebug(qml) << "Using font family" << family << "with styles" << QFontDatabase::styles(family);
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
		mFontFamily = font.family();
		Q_EMIT fireFontFamilyChanged();
		return;
	}

	const auto& knownFamilies = QFontDatabase::families();
	const QStringList families = {QStringLiteral("AusweisApp_Roboto"), QStringLiteral("Roboto")};
	for (const auto& family : families)
	{
		if (knownFamilies.contains(family))
		{
			mFontFamily = family;
			Q_EMIT fireFontFamilyChanged();
			return;
		}
	}

	mFontFamily = QFontInfo(QGuiApplication::font()).family();
	Q_EMIT fireFontFamilyChanged();
	qCCritical(qml) << "Roboto was not found in the FontDatabase. Staying on system default.";
}


void UiPluginQml::onTrayIconEnabledChanged()
{
	mTrayIcon.setVisible(Env::getSingleton<AppSettings>()->getGeneralSettings().isTrayIconEnabled());
}


void UiPluginQml::onAppConfigChanged()
{
	setFontScaleFactor(getSystemFontScaleFactor());
	setFontWeightAdjustment(getSystemFontWeightAdjustment());
	setA11yButtonShapeActive(getA11yButtonShapeActive());
	setA11yOnOffSwitchLabelActive(getA11yOnOffSwitchLabelActive());
	onUserDarkModeChanged();
}


void UiPluginQml::onReaderStatusChanged(const ReaderManagerPluginInfo& pInfo) const
{
#if defined(Q_OS_IOS)
	if (pInfo.getPluginType() != ReaderManagerPluginType::NFC)
	{
		return;
	}

	QQuickWindow* rootWindow = getRootWindow();
	if (!rootWindow)
	{
		return;
	}

	if (!pInfo.isScanRunning())
	{
		rootWindow->reportContentOrientationChange(Qt::PrimaryOrientation);
		return;
	}

	if (QScreen* screen = rootWindow->screen(); screen)
	{
		rootWindow->reportContentOrientationChange(screen->orientation());
	}
#else
	Q_UNUSED(pInfo)
#endif
}


void UiPluginQml::setA11yButtonShapeActive(bool pActive)
{
	if (pActive != mA11yButtonShapeActive)
	{
		qCDebug(qml) << "A11y button shape changed to" << pActive;
		mA11yButtonShapeActive = pActive;
		Q_EMIT fireA11yButtonShapeActiveChanged();
	}
}


bool UiPluginQml::isA11yButtonShapeActive() const
{
	return mA11yButtonShapeActive;
}


void UiPluginQml::setA11yOnOffSwitchLabelActive(bool pActive)
{
	if (pActive != mA11yOnOffSwitchLabelActive)
	{
		qCDebug(qml) << "A11y on off switch label changed to" << pActive;
		mA11yOnOffSwitchLabelActive = pActive;
		Q_EMIT fireA11yOnOffSwitchLabelActiveChanged();
	}
}


bool UiPluginQml::isA11yOnOffSwitchLabelActive() const
{
	return mA11yOnOffSwitchLabelActive;
}


QString UiPluginQml::getFontFamily() const
{
	return mFontFamily;
}
