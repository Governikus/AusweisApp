/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "AuthModel.h"
#include "CardPosition.h"
#include "CardPositionModel.h"
#include "CertificateDescriptionModel.h"
#include "ChangePinModel.h"
#include "ChatModel.h"
#include "CheckIDCardModel.h"
#include "ConnectivityManager.h"
#include "Env.h"
#include "FileDestination.h"
#include "FormattedTextModel.h"
#include "Initializer.h"
#include "LogFilterModel.h"
#include "LogHandler.h"
#include "LogModel.h"
#include "NotificationModel.h"
#include "NumberModel.h"
#include "PersonalizationModel.h"
#include "PinResetInformationModel.h"
#include "PlatformTools.h"
#include "ProviderCategoryFilterModel.h"
#include "ReaderScanEnabler.h"
#include "ReleaseInformationModel.h"
#include "RemoteServiceModel.h"
#include "SelfAuthModel.h"
#include "Service.h"
#include "SmartModel.h"
#include "SurveyModel.h"
#include "UILoader.h"
#include "VersionInformationModel.h"
#include "VersionNumber.h"
#include "VolatileSettings.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	#include "SelfDiagnosisModel.h"
#endif

#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	#include "ReaderModel.h"
#endif

#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

#if defined(Q_OS_ANDROID)
	#include "DeviceInfo.h"
	#include "UILoader.h"

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
#include <QOperatingSystemVersion>
#include <QQmlContext>
#include <QQuickStyle>
#include <QScreen>
#include <QtPlugin>
#include <QtQml>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QList<QQmlError>>("QList<QQmlError>");
		})

namespace
{
template<typename T>
[[nodiscard]] QObject* provideQmlType(const QQmlEngine*, const QJSEngine*)
{
	return new T();
}


template<typename T>
[[nodiscard]] QObject* provideSingletonQmlType(const QQmlEngine*, const QJSEngine*)
{
	const auto model = Env::getSingleton<T>();
	QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);

	const auto& metaObject = model->metaObject();
	const auto& onTranslationChangedSlot = metaObject->method(metaObject->indexOfSlot("onTranslationChanged()"));
	if (onTranslationChangedSlot.isValid())
	{
		const auto& qmlPlugin = Env::getSingleton<UILoader>()->getLoaded<UIPlugInQml>();
		if (qmlPlugin)
		{
			QObject::connect(qmlPlugin, QMetaMethod::fromSignal(&UIPlugInQml::fireTranslationChanged), model, onTranslationChangedSlot);
		}
	}

	return model;
}


template<typename T>
void registerQmlSingletonType(const std::function<QObject* (const QQmlEngine*, const QJSEngine*)>& pTypeProvider)
{
	QByteArray qmlName(T::staticMetaObject.className());
	qmlName.replace(QByteArrayLiteral("governikus::"), QByteArray());

	const QByteArray url = QByteArrayLiteral("Governikus.Type.") + qmlName;

	qmlRegisterSingletonType<T>(url.constData(), 1, 0, qmlName.constData(), pTypeProvider);
}


template<typename T>
void registerQmlType()
{
	QByteArray qmlName(T::staticMetaObject.className());
	qmlName.replace(QByteArrayLiteral("governikus::"), QByteArray());

	const QByteArray url = QByteArrayLiteral("Governikus.Type.") + qmlName;

	qmlRegisterType<T>(url.constData(), 1, 0, qmlName.constData());
}


} // namespace

UIPlugInQml::UIPlugInQml()
	: UIPlugIn()
	, mEngine()
	, mQmlEngineWarningCount(0)
	, mExplicitPlatformStyle(getPlatformSelectors())
	, mUpdateInformationPending(false)
	, mTrayIcon()
	, mHighContrastEnabled(false)
#if defined(Q_OS_MACOS)
	, mMenuBar()
#endif
	, mShowFocusIndicator(false)
{
	Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);

#if defined(Q_OS_ANDROID)
	// see QTBUG-69494
	if (DeviceInfo::getFingerprint().contains(QLatin1String("OnePlus")))
	{
		const QDir dir(QStringLiteral("/system/fonts"));
		const auto entries = dir.entryInfoList({QStringLiteral("Roboto-*.ttf")}, QDir::Files);
		for (const auto& file : entries)
		{
			if (file.fileName().contains(QLatin1String("_subset")))
			{
				qCDebug(qml) << "Ignore font" << file;
				continue;
			}
			qCDebug(qml) << "Add font" << file;
			QFontDatabase::addApplicationFont(file.absoluteFilePath());
		}
	}

	QGuiApplication::setFont(QFont(QStringLiteral("Roboto")));
#elif defined(Q_OS_LINUX)
	if (auto font = QGuiApplication::font(); QFontMetrics(font.family()).horizontalAdvance(QLatin1Char('m')) > 15)
	{
		// Fonts like "DejaVu Sans" (used on some Linux distributions) are unusually wide when compared to Windows' and macOS' default font. This will break some layouts where this wasn't taken into account.
		const auto oldFamily = QFontInfo(font).family();
		font.setFamily(QStringLiteral("Arial")); // will usually resolve to "Liberation Sans" or "Arimo"
		qCDebug(qml) << "Changing font family from" << oldFamily << "to" << QFontInfo(font).family();
		QGuiApplication::setFont(font);
	}
#endif

#ifdef Q_OS_WIN
	QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
#endif

#ifndef Q_OS_ANDROID
	QGuiApplication::setWindowIcon(mTrayIcon.getIcon());
#endif

	connect(&mTrayIcon, &TrayIcon::fireShow, this, &UIPlugInQml::show);
	connect(&mTrayIcon, &TrayIcon::fireQuit, this, [this] {
			Q_EMIT fireQuitApplicationRequest();
		});

	qApp->installEventFilter(this);
}


void UIPlugInQml::registerQmlTypes()
{
	qmlRegisterUncreatableMetaObject(EnumUiModule::staticMetaObject, "Governikus.Type.UiModule", 1, 0, "UiModule", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableMetaObject(GlobalStatus::staticMetaObject, "Governikus.Type.GlobalStatus", 1, 0, "GlobalStatus", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableMetaObject(EnumReaderManagerPlugInType::staticMetaObject, "Governikus.Type.ReaderPlugIn", 1, 0, "ReaderPlugIn", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableMetaObject(EnumPasswordType::staticMetaObject, "Governikus.Type.PasswordType", 1, 0, "PasswordType", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableMetaObject(FormattedTextModel::staticMetaObject, "Governikus.Type.FormattedTextModel", 1, 0, "LineType", QStringLiteral("Not creatable as it is an enum type"));

	registerQmlType<ReaderScanEnabler>();
	registerQmlType<CardPosition>();
	registerQmlType<CardPositionModel>();
	registerQmlType<CheckIDCardModel>();
	registerQmlType<ReleaseInformationModel>();
	registerQmlType<LogFilterModel>();

	registerQmlSingletonType<ProviderCategoryFilterModel>(&provideQmlType<ProviderCategoryFilterModel>);
	registerQmlSingletonType<HistoryModel>(&provideQmlType<HistoryModel>);
#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	registerQmlSingletonType<ReaderModel>(&provideQmlType<ReaderModel>);
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	registerQmlSingletonType<SelfDiagnosisModel>(&provideSingletonQmlType<SelfDiagnosisModel>);
#endif

	registerQmlSingletonType<ApplicationModel>(&provideSingletonQmlType<ApplicationModel>);
	registerQmlSingletonType<SettingsModel>(&provideSingletonQmlType<SettingsModel>);
	registerQmlSingletonType<WorkflowModel>(&provideSingletonQmlType<WorkflowModel>);
	registerQmlSingletonType<AuthModel>(&provideSingletonQmlType<AuthModel>);
	registerQmlSingletonType<SelfAuthModel>(&provideSingletonQmlType<SelfAuthModel>);
	registerQmlSingletonType<PersonalizationModel>(&provideSingletonQmlType<PersonalizationModel>);
	registerQmlSingletonType<ChangePinModel>(&provideSingletonQmlType<ChangePinModel>);
	registerQmlSingletonType<NumberModel>(&provideSingletonQmlType<NumberModel>);
	registerQmlSingletonType<RemoteServiceModel>(&provideSingletonQmlType<RemoteServiceModel>);
	registerQmlSingletonType<LogModel>(&provideSingletonQmlType<LogModel>);
	registerQmlSingletonType<NotificationModel>(&provideSingletonQmlType<NotificationModel>);
	registerQmlSingletonType<SurveyModel>(&provideSingletonQmlType<SurveyModel>);
	registerQmlSingletonType<Service>(&provideSingletonQmlType<Service>);
	registerQmlSingletonType<CertificateDescriptionModel>(&provideSingletonQmlType<CertificateDescriptionModel>);
	registerQmlSingletonType<ChatModel>(&provideSingletonQmlType<ChatModel>);
	registerQmlSingletonType<VersionInformationModel>(&provideSingletonQmlType<VersionInformationModel>);
	registerQmlSingletonType<ConnectivityManager>(&provideSingletonQmlType<ConnectivityManager>);
	registerQmlSingletonType<SmartModel>(&provideSingletonQmlType<SmartModel>);
	registerQmlSingletonType<PinResetInformationModel>(&provideSingletonQmlType<PinResetInformationModel>);
}


void UIPlugInQml::init()
{
	// Activate logging of Qt scenegraph information on startup, e.g. GL_RENDERER, GL_VERSION, ...
	qputenv("QSG_INFO", "1");

#ifdef Q_OS_ANDROID
	const auto orientation = isTabletLayout()
		? "SCREEN_ORIENTATION_SENSOR_LANDSCAPE"
		: "SCREEN_ORIENTATION_PORTRAIT";
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	context.callMethod<void>("setRequestedOrientation", "(I)V", QJniObject::getStaticField<jint>("android/content/pm/ActivityInfo", orientation));
#endif

	// https://bugreports.qt.io/browse/QTBUG-98098
	QQuickStyle::setStyle(QStringLiteral("Basic"));

	mEngine.reset(new QQmlApplicationEngine());

	connect(mEngine.data(), &QQmlApplicationEngine::warnings, this, &UIPlugInQml::onQmlWarnings, Qt::QueuedConnection);
	connect(mEngine.data(), &QQmlApplicationEngine::objectCreated, this, &UIPlugInQml::onQmlObjectCreated, Qt::QueuedConnection);

	mEngine->rootContext()->setContextProperty(QStringLiteral("plugin"), this);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	mEngine->setExtraFileSelectors(mExplicitPlatformStyle.split(QLatin1Char(',')));
#else
	(new QQmlFileSelector(mEngine.data()))->setExtraSelectors(mExplicitPlatformStyle.split(QLatin1Char(',')));
#endif

	UIPlugInQml::registerQmlTypes();

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	mEngine->addImportPath(getPath(QStringLiteral("qml/"), false).toString());
#endif
	mEngine->addImportPath(getPath(QStringLiteral("qml/")).toString());
	mEngine->load(getPath(QStringLiteral("qml/main.qml")));

	QQuickWindow* rootWindow = getRootWindow();
	if (rootWindow != nullptr)
	{
		connect(rootWindow, &QQuickWindow::sceneGraphInitialized, this, &UIPlugInQml::fireSafeAreaMarginsChanged);
		connect(rootWindow->screen(), &QScreen::orientationChanged, this, &UIPlugInQml::fireSafeAreaMarginsChanged);
		connect(rootWindow, &QQuickWindow::sceneGraphError, this, &UIPlugInQml::onSceneGraphError);
		qCDebug(qml) << "Using renderer interface:" << rootWindow->rendererInterface()->graphicsApi();

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
		rootWindow->resize(getInitialWindowSize());
#endif
	}

	onWindowPaletteChanged();
}


void UIPlugInQml::hideFromTaskbar()
{
	PlatformTools::hideFromTaskbar();
}


QString UIPlugInQml::getPlatformSelectors() const
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QString qtVersion = QStringLiteral("qt5,");
#else
	QString qtVersion = QStringLiteral("qt6,");
#endif

#ifndef QT_NO_DEBUG
	const char* const overrideSelector = "OVERRIDE_PLATFORM_SELECTOR";
	if (!qEnvironmentVariableIsEmpty(overrideSelector))
	{
		const auto& platform = qEnvironmentVariable(overrideSelector);
		qCDebug(qml) << "Override platform selector:" << platform;
		if (platform.startsWith(QLatin1Char('+')) || platform.contains(QLatin1String(",+")))
		{
			Q_ASSERT(false && "Please supply selectors without a '+'.");
		}
		return qtVersion + platform;
	}
#endif

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	const QString platform = QStringLiteral("mobile,");
	const QString tablet = (isTabletLayout() ? QStringLiteral("tablet,") : QStringLiteral("phone,"));
	const QString brand = QGuiApplication::platformName();
#else
	const QString platform = QStringLiteral("desktop,");
	const QString tablet;
	#if defined(Q_OS_WIN)
	const QString brand = QStringLiteral("win");
	#else
	const QString brand = QStringLiteral("nowin");
	#endif
#endif

	return qtVersion + platform + tablet + brand;
}


void UIPlugInQml::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	if (isDominated())
	{
		return;
	}

	pContext->claim(this);
	Env::getSingleton<ApplicationModel>()->keepScreenOn(true);
	Env::getSingleton<ApplicationModel>()->resetContext(pContext);
	Env::getSingleton<NumberModel>()->resetContext(pContext);
	Env::getSingleton<VolatileSettings>()->setDelay(1000);

	if (auto changePinContext = pContext.objectCast<ChangePinContext>())
	{
		onShowUi(UiModule::PINMANAGEMENT);
		Env::getSingleton<ChangePinModel>()->resetChangePinContext(changePinContext);
	}

#if __has_include("context/PersonalizationContext.h")
	if (auto smartContext = pContext.objectCast<PersonalizationContext>())
	{
		onShowUi(UiModule::SMART);
		Env::getSingleton<ConnectivityManager>()->startWatching();
		Env::getSingleton<PersonalizationModel>()->resetPersonalizationContext(smartContext);
		Env::getSingleton<CertificateDescriptionModel>()->resetContext(smartContext);
		Env::getSingleton<ChatModel>()->resetContext(smartContext);
	}
	else
#endif

	if (auto authContext = pContext.objectCast<AuthContext>())
	{
		onShowUi(UiModule::IDENTIFY);
		Env::getSingleton<ConnectivityManager>()->startWatching();
		Env::getSingleton<AuthModel>()->resetAuthContext(authContext);
		Env::getSingleton<CertificateDescriptionModel>()->resetContext(authContext);
		Env::getSingleton<ChatModel>()->resetContext(authContext);
	}

	if (auto authContext = pContext.objectCast<SelfAuthContext>())
	{
		onShowUi(UiModule::IDENTIFY);
		Env::getSingleton<SelfAuthModel>()->resetContext(authContext);
	}

	if (auto remoteServiceContext = pContext.objectCast<IfdServiceContext>())
	{
		Env::getSingleton<RemoteServiceModel>()->resetRemoteServiceContext(remoteServiceContext);
		Env::getSingleton<ChatModel>()->resetContext(remoteServiceContext);
		Env::getSingleton<CertificateDescriptionModel>()->resetContext(remoteServiceContext);
	}
}


void UIPlugInQml::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Env::getSingleton<ApplicationModel>()->keepScreenOn(false);
	Env::getSingleton<ApplicationModel>()->resetContext();
	Env::getSingleton<NumberModel>()->resetContext();
	Env::getSingleton<VolatileSettings>()->setDelay();

	if (pContext.objectCast<ChangePinContext>())
	{
		Env::getSingleton<ChangePinModel>()->resetChangePinContext();
	}

	if (const auto& context = pContext.objectCast<AuthContext>())
	{
		Env::getSingleton<ConnectivityManager>()->stopWatching();
		Env::getSingleton<AuthModel>()->resetAuthContext();
		Env::getSingleton<CertificateDescriptionModel>()->resetContext();
		Env::getSingleton<ChatModel>()->resetContext();

		const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();

		// Only hide the UI if we don't need to show the update information view. This behaviour ensures that
		// the user is (aggressively) notified about a pending update if the AA2 is only shown for authentication
		// workflows and never manually brought to foreground in between.
		if (!pContext.objectCast<SelfAuthContext>()
#if __has_include("context/PersonalizationContext.h")
				&& !pContext.objectCast<PersonalizationContext>()
#endif
				&& !pContext->hasNextWorkflowPending()
				&& generalSettings.isAutoCloseWindowAfterAuthentication()
				&& !showUpdateInformationIfPending()
				&& !context->showChangePinView())
		{
			onHideUi();
		}
	}

	if (pContext.objectCast<SelfAuthContext>())
	{
		Env::getSingleton<SelfAuthModel>()->resetContext();
	}

	if (pContext.objectCast<IfdServiceContext>())
	{
		Env::getSingleton<RemoteServiceModel>()->resetRemoteServiceContext();
		Env::getSingleton<ChatModel>()->resetContext();
		Env::getSingleton<CertificateDescriptionModel>()->resetContext();
	}

#if __has_include("context/PersonalizationContext.h")
	if (pContext.objectCast<PersonalizationContext>())
	{
		Env::getSingleton<PersonalizationModel>()->resetPersonalizationContext();
	}
#endif

	Env::getSingleton<SmartModel>()->workflowFinished(pContext);
}


void UIPlugInQml::onApplicationInitialized()
{
	connect(Env::getSingleton<ChangePinModel>(), &ChangePinModel::fireStartWorkflow, this, &UIPlugIn::fireWorkflowRequested);
	connect(Env::getSingleton<SelfAuthModel>(), &SelfAuthModel::fireStartWorkflow, this, &UIPlugIn::fireWorkflowRequested);
	connect(Env::getSingleton<RemoteServiceModel>(), &RemoteServiceModel::fireStartWorkflow, this, &UIPlugIn::fireWorkflowRequested);
	connect(Env::getSingleton<PersonalizationModel>(), &PersonalizationModel::fireStartWorkflow, this, &UIPlugIn::fireWorkflowRequested);
	connect(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireRawLog, this, &UIPlugInQml::onRawLog, Qt::QueuedConnection);
	connect(Env::getSingleton<SettingsModel>(), &SettingsModel::fireAutoStartChanged, this, &UIPlugInQml::onAutoStartChanged);

	const auto* service = Env::getSingleton<Service>();
	connect(service, &Service::fireAppcastFinished, this, &UIPlugInQml::onUpdateAvailable);
	connect(service, &Service::fireUpdateScheduled, this, &UIPlugInQml::onUpdateScheduled);

	init();
}


void UIPlugInQml::onApplicationStarted()
{
	mTrayIcon.create();
	connect(this, &UIPlugInQml::fireTranslationChanged, &mTrayIcon, &TrayIcon::onTranslationChanged);

#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	const bool showSetupAssistant = Enum<UiModule>::fromString(generalSettings.getStartupModule(), UiModule::TUTORIAL) == UiModule::TUTORIAL;
	const bool developerMode = generalSettings.isDeveloperMode();
	bool missingTrayIcon = !QSystemTrayIcon::isSystemTrayAvailable();
	#ifdef Q_OS_MACOS
	missingTrayIcon |= !Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoStart();
	#endif
	if (missingTrayIcon || showSetupAssistant || developerMode)
#endif
	{
		QMetaObject::invokeMethod(this, &UIPlugInQml::show, Qt::QueuedConnection);
	}

#if defined(Q_OS_ANDROID)
	QNativeInterface::QAndroidApplication::hideSplashScreen(250);
#endif
}


void UIPlugInQml::onShowUi(UiModule pModule)
{
	PlatformTools::restoreToTaskbar();
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	const auto& startupModule = Enum<UiModule>::fromString(Env::getSingleton<AppSettings>()->getGeneralSettings().getStartupModule(), UiModule::TUTORIAL);
	if (pModule == UiModule::CURRENT && startupModule == UiModule::TUTORIAL)
	{
		pModule = UiModule::TUTORIAL;
	}
#endif
	if (isDominated())
	{
		pModule = UiModule::CURRENT;
	}
	Q_EMIT fireShowRequest(pModule);

	Env::getSingleton<Service>()->runUpdateIfNeeded();
}


void UIPlugInQml::onHideUi()
{
	Q_EMIT fireHideRequest();
}


void UIPlugInQml::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	ProxyCredentials proxyCredentials(pProxy, pAuthenticator);

	Q_EMIT fireProxyAuthenticationRequired(&proxyCredentials);
	proxyCredentials.waitForConfirmation();
}


void UIPlugInQml::onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted)
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


void UIPlugInQml::onUiDominationReleased()
{
	if (!mDominator.isNull())
	{
		mDominator.clear();
		Q_EMIT fireDominatorChanged();
	}
}


void UIPlugInQml::onShowUserInformation(const QString& pMessage)
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


void UIPlugInQml::onUpdateScheduled()
{
	if (!isHidden())
	{
		Env::getSingleton<Service>()->runUpdateIfNeeded();
	}
}


void UIPlugInQml::onUpdateAvailable(bool pUpdateAvailable, const GlobalStatus& pStatus)
{
	Q_UNUSED(pStatus)
	mUpdateInformationPending = pUpdateAvailable;
}


void UIPlugInQml::show()
{
	if (!showUpdateInformationIfPending())
	{
		onShowUi(UiModule::CURRENT);
	}
}


bool UIPlugInQml::isTabletLayout() const
{
	const auto screenOrientationSetting = Env::getSingleton<AppSettings>()->getGeneralSettings().getScreenOrientation();
	if (screenOrientationSetting == QLatin1String("landscape"))
	{
		return true;
	}
	if (screenOrientationSetting == QLatin1String("portrait"))
	{
		return false;
	}
	return isTablet();
}


bool UIPlugInQml::showUpdateInformationIfPending()
{
	if (!mUpdateInformationPending)
	{
		return false;
	}

	mUpdateInformationPending = false;
	onShowUi(UiModule::UPDATEINFORMATION);

	return true;
}


bool UIPlugInQml::eventFilter(QObject* pObj, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::ApplicationPaletteChange)
	{
		onWindowPaletteChanged();
		return true;
	}

	if (pEvent->type() == QEvent::Quit)
	{
		Q_EMIT fireQuitApplicationRequest();
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


void UIPlugInQml::onTranslationChanged()
{
	if (mEngine)
	{
		mEngine->retranslate();
	}
	Q_EMIT fireTranslationChanged();
}


void UIPlugInQml::doShutdown()
{
	mTrayIcon.shutdown();
}


QUrl UIPlugInQml::getPath(const QString& pRelativePath, bool pQrc)
{
	if (pQrc)
	{
		return QStringLiteral("qrc:///") + pRelativePath;
	}

	return QUrl::fromLocalFile(FileDestination::getPath(pRelativePath, QStandardPaths::LocateDirectory));
}


QQuickWindow* UIPlugInQml::getRootWindow() const
{
	if (mEngine->rootObjects().isEmpty())
	{
		return nullptr;
	}

	return qobject_cast<QQuickWindow*>(mEngine->rootObjects().first());
}


bool UIPlugInQml::isHidden() const
{
	const QQuickWindow* rootWindow = getRootWindow();
	return !rootWindow || rootWindow->visibility() == QWindow::Hidden;
}


#ifndef Q_OS_IOS
bool UIPlugInQml::isTablet() const
{
	#ifdef Q_OS_ANDROID
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	const jboolean result = context.callMethod<jboolean>("isTablet", "()Z");
	return result != JNI_FALSE;

	#else
	return false;

	#endif
}


#endif


void UIPlugInQml::onQmlWarnings(const QList<QQmlError>& pWarnings)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
	for (const auto& entry : pWarnings)
	{
		if (!entry.description().contains(QLatin1String("QML Connections: Implicitly defined onFoo properties in Connections are deprecated. Use this syntax instead:")))
		{
			++mQmlEngineWarningCount;
		}
	}
#else
	mQmlEngineWarningCount += pWarnings.size();
#endif

#ifndef QT_NO_DEBUG
	for (const auto& warning : pWarnings)
	{
		Env::getSingleton<ApplicationModel>()->showFeedback(QStringLiteral("Got QML warning: %1").arg(warning.toString()), false);
	}
#endif
}


void UIPlugInQml::onQmlObjectCreated(QObject* pObject)
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


void UIPlugInQml::onSceneGraphError(QQuickWindow::SceneGraphError pError, const QString& pMessage)
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


void UIPlugInQml::onRawLog(const QString& pMessage, const QString& pCategoryName)
{
	if (pCategoryName == QLatin1String("developermode") || pCategoryName == QLatin1String("feedback"))
	{
#ifdef Q_OS_MACOS
		if (QOperatingSystemVersion::current() >= QOperatingSystemVersion::MacOSMojave)
		{
			PlatformTools::postNotification(QCoreApplication::applicationName(), pMessage);
			return;
		}

#endif
		mTrayIcon.showMessage(QCoreApplication::applicationName(), pMessage);
	}
}


void UIPlugInQml::doRefresh()
{
	qCDebug(qml) << "Reload qml files";
	QMetaObject::invokeMethod(this, &UIPlugInQml::init, Qt::QueuedConnection);
}


QString UIPlugInQml::getPlatformStyle() const
{
	return mExplicitPlatformStyle;
}


bool UIPlugInQml::isDebugBuild() const
{
#ifndef QT_NO_DEBUG
	return true;

#else
	return false;

#endif
}


bool UIPlugInQml::isDeveloperVersion() const
{
	return VersionNumber::getApplicationVersion().isDeveloperVersion();
}


QString UIPlugInQml::getDominator() const
{
	return mDominator;
}


bool UIPlugInQml::isDominated() const
{
	return !mDominator.isNull();
}


#ifndef Q_OS_IOS
QVariantMap UIPlugInQml::getSafeAreaMargins() const
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
			UIPlugInQml* const uiPlugIn = Env::getSingleton<UILoader>()->getLoaded<UIPlugInQml>();
			if (uiPlugIn)
			{
				Q_EMIT uiPlugIn->fireSafeAreaMarginsChanged();
			}
		}, Qt::QueuedConnection);
}


}

#endif

#ifndef Q_OS_MACOS
bool UIPlugInQml::isHighContrastEnabled() const
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


QString UIPlugInQml::getFixedFontFamily() const
{
	return QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
}


QSize UIPlugInQml::getInitialWindowSize() const
{
	const QString platform = getPlatformSelectors();
	bool isTablet = platform.contains(QLatin1String("tablet"));
	bool isPhone = platform.contains(QLatin1String("phone"));

	if (isTablet || isPhone)
	{
		// Use 4:3 in landscape for tablets and 16:9 in portrait for phones.
		return QSize(isTablet ? 1024 : 432, 768);
	}

	return QSize(960, 720);
}


bool UIPlugInQml::getShowFocusIndicator() const
{
	return mShowFocusIndicator;
}


void UIPlugInQml::onWindowPaletteChanged()
{
	const bool highContrast = isHighContrastEnabled();
	if (mHighContrastEnabled != highContrast)
	{
		mHighContrastEnabled = highContrast;
		Q_EMIT fireHighContrastEnabledChanged();
	}
}


void UIPlugInQml::onAutoStartChanged()
{
#ifdef Q_OS_MACOS
	mTrayIcon.setVisible(Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoStart());
#endif
}


void UIPlugInQml::applyPlatformStyle(const QString& pPlatformStyle)
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QString platformStyle = QStringLiteral("qt5,%1").arg(pPlatformStyle);
#else
	QString platformStyle = QStringLiteral("qt6,%1").arg(pPlatformStyle);
#endif

	if (mExplicitPlatformStyle != platformStyle)
	{
		mExplicitPlatformStyle = platformStyle;
		Env::getSingleton<AppSettings>()->getGeneralSettings().setScreenOrientation(pPlatformStyle.indexOf(QLatin1String("tablet")) == -1 ? QStringLiteral("portrait") : QStringLiteral("landscape"));
		doRefresh();
	}
}
