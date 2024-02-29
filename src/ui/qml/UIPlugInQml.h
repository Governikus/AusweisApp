/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UIPlugIn implementation of QML.
 */

#pragma once

#include "GlobalStatus.h"
#include "ProxyCredentials.h"
#include "TrayIcon.h"
#include "UIPlugIn.h"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QScopedPointer>
#if defined (Q_OS_MACOS)
	#include <QMenuBar>
#endif

#ifdef Q_OS_IOS
Q_FORWARD_DECLARE_OBJC_CLASS(FontChangeTracker);
#endif


class test_UIPlugInQml;


namespace governikus
{

class UIPlugInQml
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)
	Q_PROPERTY(QString platformStyle READ getPlatformStyle CONSTANT FINAL)
	Q_PROPERTY(bool debugBuild READ isDebugBuild CONSTANT)
	Q_PROPERTY(bool developerVersion READ isDeveloperVersion CONSTANT)
	Q_PROPERTY(QString dominator READ getDominator NOTIFY fireDominatorChanged)
	Q_PROPERTY(bool dominated READ isDominated NOTIFY fireDominatorChanged)
	Q_PROPERTY(QVariantMap safeAreaMargins READ getSafeAreaMargins NOTIFY fireSafeAreaMarginsChanged)
	Q_PROPERTY(bool highContrastEnabled READ isHighContrastEnabled NOTIFY fireHighContrastEnabledChanged)
	Q_PROPERTY(bool osDarkModeEnabled READ isOsDarkModeEnabled NOTIFY fireOsDarkModeChanged)
	Q_PROPERTY(bool osDarkModeSupported READ isOsDarkModeSupported)
	Q_PROPERTY(QString fixedFontFamily READ getFixedFontFamily CONSTANT)
	Q_PROPERTY(QSize initialWindowSize READ getInitialWindowSize CONSTANT)
	Q_PROPERTY(bool showFocusIndicator READ getShowFocusIndicator NOTIFY fireShowFocusIndicator)
	Q_PROPERTY(qreal scaleFactor READ getScaleFactor WRITE setScaleFactor NOTIFY fireScaleFactorChanged)
	Q_PROPERTY(qreal fontScaleFactor READ getFontScaleFactor NOTIFY fireFontScaleFactorChanged)
	friend class ::test_UIPlugInQml;

	private:
		QScopedPointer<QQmlApplicationEngine> mEngine;
		qsizetype mQmlEngineWarningCount;
		QString mExplicitPlatformStyle;
		bool mUpdateInformationPending;
		TrayIcon mTrayIcon;
		QString mDominator;
		bool mHighContrastEnabled;
		bool mDarkMode;
#if defined(Q_OS_MACOS)
		QMenuBar mMenuBar;
#endif
		bool mShowFocusIndicator;
		constexpr static qreal DEFAULT_SCALE_FACTOR = 0.6;
		qreal mScaleFactor;
		qreal mFontScaleFactor;

		[[nodiscard]] QString getPlatformSelectors() const;
		[[nodiscard]] static QUrl getPath(const QString& pRelativePath, bool pQrc = true);
		[[nodiscard]] QQuickWindow* getRootWindow() const;
		[[nodiscard]] bool isHidden() const;
		[[nodiscard]] bool isTablet() const;
		[[nodiscard]] bool showUpdateInformationIfPending();
		[[nodiscard]] qreal getSystemFontScaleFactor() const;
		void setFontScaleFactor(qreal pFactor);
		void setOsDarkMode(bool pState);

#ifdef Q_OS_IOS
		struct Private
		{
			Private();
			~Private();
			FontChangeTracker* const mFontChangeTracker;
		};
		const QScopedPointer<Private> mPrivate;
#endif

	protected:
		[[nodiscard]] bool eventFilter(QObject* pObj, QEvent* pEvent) override;

	public:
		UIPlugInQml();
		~UIPlugInQml() override = default;

		static void registerQmlTypes();

		[[nodiscard]] QString getPlatformStyle() const;
		[[nodiscard]] bool isDebugBuild() const;
		[[nodiscard]] bool isDeveloperVersion() const;
		[[nodiscard]] QString getDominator() const;
		[[nodiscard]] bool isDominated() const;
		[[nodiscard]] QVariantMap getSafeAreaMargins() const;
		[[nodiscard]] bool isHighContrastEnabled() const;
		[[nodiscard]] bool isOsDarkModeEnabled() const;
		[[nodiscard]] bool isOsDarkModeSupported() const;
		[[nodiscard]] QString getFixedFontFamily() const;
		[[nodiscard]] QSize getInitialWindowSize() const;
		[[nodiscard]] bool getShowFocusIndicator() const;
		[[nodiscard]] qreal getScaleFactor() const;
		void setScaleFactor(qreal pScaleFactor);
		[[nodiscard]] qreal getFontScaleFactor() const;


		Q_INVOKABLE void applyPlatformStyle(const QString& pPlatformStyle);
		Q_INVOKABLE void init();
		Q_INVOKABLE void hideFromTaskbar() const;

	Q_SIGNALS:
		void fireShowRequest(UiModule pModule);
		void fireHideRequest();
		void fireDominatorChanged();
		void fireSafeAreaMarginsChanged();
		void fireHighContrastEnabledChanged();
		void fireOsDarkModeChanged();
		void fireProxyAuthenticationRequired(ProxyCredentials* pProxyCredentials);
		void fireTranslationChanged();
		void fireShowFocusIndicator();
		void fireScaleFactorChanged();
		void fireFontScaleFactorChanged();
		void fireAppConfigChanged();

	private Q_SLOTS:
		void show();
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onApplicationInitialized() override;
		void onApplicationStarted() override;
		void onShowUi(UiModule pModule) override;
		void onHideUi() override;
		void onTranslationChanged() override;
		void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator) override;
		void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onShowUserInformation(const QString& pMessage) override;
		void onUpdateScheduled() const;
		void onUpdateAvailable(bool pUpdateAvailable, const GlobalStatus& pStatus);

		void onQmlWarnings(const QList<QQmlError>& pWarnings);
		void onQmlObjectCreated(QObject* pObject);
		void onSceneGraphError(QQuickWindow::SceneGraphError pError, const QString& pMessage);

		void onRawLog(const QString& pMessage, const QString& pCategoryName);

		void onWindowPaletteChanged();
		void onUseSystemFontChanged() const;
		void onAutoStartChanged();
		void onAppConfigChanged();

	public Q_SLOTS:
		void doRefresh();
};

} // namespace governikus
