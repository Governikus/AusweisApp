/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include "ApplicationModel.h"
#include "AppSettings.h"
#include "AuthModel.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthContext.h"
#include "ChangePinModel.h"
#include "Env.h"
#include "FileDestination.h"
#include "Initializer.h"
#include "LogHandler.h"
#include "LogModel.h"
#include "NotificationModel.h"
#include "PlatformTools.h"
#include "ProviderCategoryFilterModel.h"
#include "Random.h"
#include "ReaderScanEnabler.h"
#include "RemoteServiceModel.h"
#include "SelfAuthModel.h"
#include "SelfDiagnosisModel.h"
#include "Service.h"
#include "SingletonHelper.h"
#include "SurveyModel.h"
#include "VersionNumber.h"

#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#include "ReaderDriverModel.h"
#endif

#if defined(Q_OS_ANDROID)
	#include <QFont>
	#include <QtAndroidExtras/QAndroidJniEnvironment>
	#include <QtAndroidExtras/QtAndroid>
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
#include <QAbstractButton>
#include <QMessageBox>
#endif

#include <QFile>
#include <QFontDatabase>
#include <QFontInfo>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QOpenGLContext>
#include <QOperatingSystemVersion>
#include <QQmlContext>
#include <QScreen>
#include <QtPlugin>
#include <QtQml>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


static Initializer::Entry X([] {
			qRegisterMetaType<QList<QQmlError> >("QList<QQmlError>");
		});


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
	Q_UNUSED(pScriptEngine)

	const auto model = Env::getSingleton<T>();
	pEngine->setObjectOwnership(model, QQmlEngine::CppOwnership);
	return model;
}


template<typename T>
static void registerQmlSingletonType(QObject* (*pTypeProvider)(QQmlEngine*, QJSEngine*))
{
	QByteArray qmlName(T::staticMetaObject.className());
	qmlName.replace(QByteArrayLiteral("governikus::"), QByteArray());

	const QByteArray url = QByteArrayLiteral("Governikus.Type.") + qmlName;

	qmlRegisterSingletonType<T>(url.constData(), 1, 0, qmlName.constData(), pTypeProvider);
}


template<typename T>
static void registerQmlType()
{
	QByteArray qmlName(T::staticMetaObject.className());
	qmlName.replace(QByteArrayLiteral("governikus::"), QByteArray());

	const QByteArray url = QByteArrayLiteral("Governikus.Type.") + qmlName;

	qmlRegisterType<T>(url.constData(), 1, 0, qmlName.constData());
}


UIPlugInQml::UIPlugInQml()
	: mEngine()
	, mQmlEngineWarningCount(0)
	, mVersionInformationModel()
	, mCertificateDescriptionModel()
	, mChatModel()
	, mExplicitPlatformStyle(getPlatformSelectors())
	, mConnectivityManager()
	, mUpdateInformationPending(false)
	, mTrayIcon()
	, mHighContrastEnabled(false)
#if defined(Q_OS_MACOS)
	, mMenuBar()
#endif
{
#if defined(Q_OS_ANDROID)
	QGuiApplication::setFont(QFont(QStringLiteral("Roboto")));
#elif defined(Q_OS_LINUX) && QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
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

	QGuiApplication::setWindowIcon(mTrayIcon.getIcon());

	connect(&mTrayIcon, &TrayIcon::fireShow, this, &UIPlugInQml::show);
	connect(&mTrayIcon, &TrayIcon::fireQuit, this, &UIPlugInQml::fireQuitApplicationRequest);

	connect(Env::getSingleton<ChangePinModel>(), &ChangePinModel::fireStartWorkflow, this, &UIPlugIn::fireChangePinRequest);
	connect(Env::getSingleton<SelfAuthModel>(), &SelfAuthModel::fireStartWorkflow, this, &UIPlugIn::fireSelfAuthenticationRequested);
	connect(Env::getSingleton<RemoteServiceModel>(), &RemoteServiceModel::fireStartWorkflow, this, &UIPlugIn::fireRemoteServiceRequested);
	connect(Env::getSingleton<LogHandler>(), &LogHandler::fireRawLog, this, &UIPlugInQml::onRawLog, Qt::QueuedConnection);
	connect(this, &UIPlugIn::fireShowUserInformation, this, &UIPlugInQml::onShowUserInformation);
	connect(qGuiApp, &QGuiApplication::paletteChanged, this, &UIPlugInQml::onWindowPaletteChanged);

	auto* service = Env::getSingleton<Service>();
	connect(service, &Service::fireAppUpdateFinished, this, &UIPlugInQml::onUpdateAvailable);
	connect(service, &Service::fireUpdateScheduled, this, &UIPlugInQml::onUpdateScheduled);
	service->runUpdateIfNeeded();

	init();
}


void UIPlugInQml::registerQmlTypes()
{
	qmlRegisterUncreatableType<EnumUiModule>("Governikus.Type.UiModule", 1, 0, "UiModule", QStringLiteral("Not creatable as it is an enum type"));
	qmlRegisterUncreatableType<EnumReaderManagerPlugInType>("Governikus.Type.ReaderPlugIn", 1, 0, "ReaderPlugIn", QStringLiteral("Not creatable as it is an enum type"));

	registerQmlType<ReaderScanEnabler>();

	registerQmlSingletonType<ProviderCategoryFilterModel>(&provideQmlType<ProviderCategoryFilterModel>);
	registerQmlSingletonType<HistoryModel>(&provideQmlType<HistoryModel>);
#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	registerQmlSingletonType<ReaderDriverModel>(&provideQmlType<ReaderDriverModel>);
#endif

	registerQmlSingletonType<ApplicationModel>(&provideSingletonQmlType<ApplicationModel>);
	registerQmlSingletonType<SettingsModel>(&provideSingletonQmlType<SettingsModel>);
	registerQmlSingletonType<AuthModel>(&provideSingletonQmlType<AuthModel>);
	registerQmlSingletonType<SelfAuthModel>(&provideSingletonQmlType<SelfAuthModel>);
	registerQmlSingletonType<SelfDiagnosisModel>(&provideSingletonQmlType<SelfDiagnosisModel>);
	registerQmlSingletonType<ChangePinModel>(&provideSingletonQmlType<ChangePinModel>);
	registerQmlSingletonType<NumberModel>(&provideSingletonQmlType<NumberModel>);
	registerQmlSingletonType<RemoteServiceModel>(&provideSingletonQmlType<RemoteServiceModel>);
	registerQmlSingletonType<LogModel>(&provideSingletonQmlType<LogModel>);
	registerQmlSingletonType<NotificationModel>(&provideSingletonQmlType<NotificationModel>);
	registerQmlSingletonType<SurveyModel>(&provideSingletonQmlType<SurveyModel>);
	registerQmlSingletonType<Service>(&provideSingletonQmlType<Service>);
	registerQmlSingletonType<Random>(&provideSingletonQmlType<Random>);
}


void UIPlugInQml::init()
{
#if !defined(QT_NO_DEBUG) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WINRT)
	qputenv("QML_DISABLE_DISK_CACHE", "true");
#endif

	// Activate logging of Qt scenegraph information on startup, e.g. GL_RENDERER, GL_VERSION, ...
	qputenv("QSG_INFO", "1");

	mEngine.reset(new QQmlApplicationEngine());

	connect(mEngine.data(), &QQmlApplicationEngine::warnings, this, &UIPlugInQml::onQmlWarnings, Qt::QueuedConnection);
	connect(mEngine.data(), &QQmlApplicationEngine::objectCreated, this, &UIPlugInQml::onQmlObjectCreated, Qt::QueuedConnection);

	mEngine->rootContext()->setContextProperty(QStringLiteral("plugin"), this);
	QQmlFileSelector::get(mEngine.data())->setExtraSelectors(mExplicitPlatformStyle.split(QLatin1Char(',')));

	mEngine->rootContext()->setContextProperty(QStringLiteral("versionInformationModel"), &mVersionInformationModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("certificateDescriptionModel"), &mCertificateDescriptionModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("chatModel"), &mChatModel);
	mEngine->rootContext()->setContextProperty(QStringLiteral("connectivityManager"), &mConnectivityManager);

	UIPlugInQml::registerQmlTypes();

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	mEngine->addImportPath(getPath(QStringLiteral("qml/"), false).toString());
#endif
	mEngine->addImportPath(getPath(QStringLiteral("qml/")).toString());
	mEngine->load(getPath(QStringLiteral("qml/main.qml")));

	if (!mEngine->rootObjects().isEmpty())
	{
		QQuickWindow* rootWindow = qobject_cast<QQuickWindow*>(mEngine->rootObjects().first());
		if (rootWindow != nullptr)
		{
#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(5, 14, 1)
			// we need to call create() explicitly because Windows needs a handle to fire WM_ENDSESSION!
			// Since we start as a systemtray only we don't have a correct handle until we call show()
			rootWindow->create();
#endif
			connect(rootWindow, &QQuickWindow::sceneGraphError, this, &UIPlugInQml::onSceneGraphError);
			qCDebug(qml) << "Using renderer interface:" << rootWindow->rendererInterface()->graphicsApi();
		}
	}

	Env::getSingleton<Service>()->updateConfigurations();

	onWindowPaletteChanged();
}


void UIPlugInQml::hide()
{
	PlatformTools::hideFromTaskbar();
}


void UIPlugInQml::switchUi()
{
	auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	generalSettings.setSelectedUi(QStringLiteral("widgets"));
	generalSettings.save();

	Q_EMIT fireRestartApplicationRequested();
}


QString UIPlugInQml::getPlatformSelectors() const
{
#ifndef QT_NO_DEBUG
	const char* const overrideSelector = "OVERRIDE_PLATFORM_SELECTOR";
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

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	const QString platform = QStringLiteral("mobile,");
	const QString tablet = (isTablet() ? QStringLiteral("tablet,") : QStringLiteral("phone,"));
	const QString brand = QGuiApplication::platformName();
#else
	const QString platform = QStringLiteral("desktop,");
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
	Env::getSingleton<ApplicationModel>()->keepScreenOn(true);
	Env::getSingleton<ApplicationModel>()->resetContext(pContext);
	Env::getSingleton<NumberModel>()->resetContext(pContext);

	if (auto changePinContext = pContext.objectCast<ChangePinContext>())
	{
		onShowUi(UiModule::PINMANAGEMENT);
		Env::getSingleton<ChangePinModel>()->resetContext(changePinContext);
	}

	if (auto authContext = pContext.objectCast<AuthContext>())
	{
		onShowUi(UiModule::IDENTIFY);
		mConnectivityManager.startWatching();
		Env::getSingleton<AuthModel>()->resetContext(authContext);
		mCertificateDescriptionModel.resetContext(authContext);
		mChatModel.resetContext(authContext);
	}

	if (auto authContext = pContext.objectCast<SelfAuthContext>())
	{
		onShowUi(UiModule::IDENTIFY);
		Env::getSingleton<SelfAuthModel>()->resetContext(authContext);
	}

	if (auto remoteServiceContext = pContext.objectCast<RemoteServiceContext>())
	{
		Env::getSingleton<RemoteServiceModel>()->resetContext(remoteServiceContext);
	}
}


void UIPlugInQml::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Env::getSingleton<ApplicationModel>()->keepScreenOn(false);
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

		const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();

		// Only hide the UI if we don't need to show the update information view. This behaviour ensures that
		// the user is (aggressively) notified about a pending update if the AA2 is only shown for authentication
		// workflows and never manually brought to foreground in between.
		if (!pContext.objectCast<SelfAuthContext>() && !pContext->hasNextWorkflowPending() && generalSettings.isAutoCloseWindowAfterAuthentication() && !showUpdateInformationIfPending())
		{
			Q_EMIT fireHideRequest();
		}
	}

	if (pContext.objectCast<SelfAuthContext>())
	{
		Env::getSingleton<SelfAuthModel>()->resetContext();
	}

	if (pContext.objectCast<RemoteServiceContext>())
	{
		Env::getSingleton<RemoteServiceModel>()->resetContext();
	}
}


void UIPlugInQml::onApplicationStarted()
{
	mTrayIcon.create();

#if defined(Q_OS_WIN) || (defined(Q_OS_BSD4) && !defined(Q_OS_IOS)) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	if (!QSystemTrayIcon::isSystemTrayAvailable()
			|| Env::getSingleton<AppSettings>()->getGeneralSettings().isShowSetupAssistant()
			|| Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
#endif
	{
		QMetaObject::invokeMethod(this, &UIPlugInQml::show, Qt::QueuedConnection);
	}

	Q_EMIT fireSafeAreaMarginsChanged();
}


void UIPlugInQml::onShowUi(UiModule pModule)
{
	PlatformTools::restoreToTaskbar();
	Q_EMIT fireShowRequest(pModule);
}


void UIPlugInQml::onHideUi()
{
	Q_EMIT fireHideRequest();
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
	Env::getSingleton<Service>()->runUpdateIfNeeded();
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


#ifndef Q_OS_IOS
bool UIPlugInQml::isTablet() const
{
#ifdef Q_OS_ANDROID
	const jboolean result = QtAndroid::androidActivity().callMethod<jboolean>("isTablet", "()Z");
	return result != JNI_FALSE;

#else
	return false;

#endif
}


#endif


void UIPlugInQml::onQmlWarnings(const QList<QQmlError>& pWarnings)
{
	mQmlEngineWarningCount += pWarnings.size();
}


void UIPlugInQml::onQmlObjectCreated(QObject* pObject)
{
	const bool fatalErrors = pObject == nullptr;
	const QString result = fatalErrors ? QStringLiteral("fatal errors.") : QStringLiteral("%1 warnings.").arg(mQmlEngineWarningCount);
	qCDebug(qml).noquote() << "QML engine initialization finished with" << result;
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

#ifdef Q_OS_ANDROID

	auto screen = QGuiApplication::primaryScreen();
	auto windowInsets = QtAndroid::androidActivity().callObjectMethod("getWindowInsets", "()Landroid/view/ViewGroup$MarginLayoutParams;");

	marginMap[QStringLiteral("top")] = windowInsets.getField<jint>("topMargin") / screen->devicePixelRatio();
	marginMap[QStringLiteral("left")] = windowInsets.getField<jint>("leftMargin") / screen->devicePixelRatio();
	marginMap[QStringLiteral("right")] = windowInsets.getField<jint>("rightMargin") / screen->devicePixelRatio();
	marginMap[QStringLiteral("bottom")] = windowInsets.getField<jint>("bottomMargin") / screen->devicePixelRatio();
#else
	marginMap[QStringLiteral("top")] = 0;
	marginMap[QStringLiteral("right")] = 0;
	marginMap[QStringLiteral("bottom")] = 0;
	marginMap[QStringLiteral("left")] = 0;
#endif

	return marginMap;
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


void UIPlugInQml::onWindowPaletteChanged()
{
	const bool highContrast = isHighContrastEnabled();
	if (mHighContrastEnabled != highContrast)
	{
		mHighContrastEnabled = highContrast;
		Q_EMIT fireHighContrastEnabledChanged();
	}
}


void UIPlugInQml::applyPlatformStyle(const QString& pPlatformStyle)
{
	if (mExplicitPlatformStyle != pPlatformStyle)
	{
		mExplicitPlatformStyle = pPlatformStyle;
		doRefresh();
	}
}
