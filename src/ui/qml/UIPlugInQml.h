/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UIPlugIn implementation of QML.
 */

#pragma once

#include "GlobalStatus.h"
#include "HistoryModel.h"
#include "NumberModel.h"
#include "ProxyCredentials.h"
#include "SettingsModel.h"
#include "TrayIcon.h"
#include "UIPlugIn.h"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QScopedPointer>
#if defined (Q_OS_MACOS)
	#include <QMenuBar>
#endif

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
	Q_PROPERTY(QString fixedFontFamily READ getFixedFontFamily CONSTANT)
	Q_PROPERTY(bool tablet READ isTablet CONSTANT)
	Q_PROPERTY(bool isTabletLayout READ isTabletLayout CONSTANT)
	Q_PROPERTY(QSize initialWindowSize READ getInitialWindowSize CONSTANT)
	Q_PROPERTY(bool showFocusIndicator READ getShowFocusIndicator NOTIFY fireShowFocusIndicator)

	private:
		QScopedPointer<QQmlApplicationEngine> mEngine;
		int mQmlEngineWarningCount;
		QString mExplicitPlatformStyle;
		bool mUpdateInformationPending;
		TrayIcon mTrayIcon;
		QString mDominator;
		bool mHighContrastEnabled;
#if defined(Q_OS_MACOS)
		QMenuBar mMenuBar;
#endif
		bool mShowFocusIndicator;

		[[nodiscard]] QString getPlatformSelectors() const;
		[[nodiscard]] static QUrl getPath(const QString& pRelativePath, bool pQrc = true);
		[[nodiscard]] QQuickWindow* getRootWindow() const;
		[[nodiscard]] bool isHidden() const;
		[[nodiscard]] bool isTablet() const;
		[[nodiscard]] bool isTabletLayout() const;
		[[nodiscard]] bool showUpdateInformationIfPending();

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
		[[nodiscard]] QString getFixedFontFamily() const;
		[[nodiscard]] QSize getInitialWindowSize() const;
		[[nodiscard]] bool getShowFocusIndicator() const;

		Q_INVOKABLE void applyPlatformStyle(const QString& pPlatformStyle);
		Q_INVOKABLE void init();
		Q_INVOKABLE void hideFromTaskbar();

	Q_SIGNALS:
		void fireShowRequest(UiModule pModule);
		void fireHideRequest();
		void fireDominatorChanged();
		void fireSafeAreaMarginsChanged();
		void fireHighContrastEnabledChanged();
		void fireProxyAuthenticationRequired(ProxyCredentials* pProxyCredentials);
		void fireTranslationChanged();
		void fireShowFocusIndicator();

	private Q_SLOTS:
		void show();
		void doShutdown() override;
		void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onApplicationInitialized() override;
		void onApplicationStarted() override;
		void onShowUi(UiModule pModule) override;
		void onHideUi() override;
		void onTranslationChanged() override;
		void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator) override;
		void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onShowUserInformation(const QString& pMessage) override;
		void onUpdateScheduled();
		void onUpdateAvailable(bool pUpdateAvailable, const GlobalStatus& pStatus);

		void onQmlWarnings(const QList<QQmlError>& pWarnings);
		void onQmlObjectCreated(QObject* pObject);
		void onSceneGraphError(QQuickWindow::SceneGraphError pError, const QString& pMessage);

		void onRawLog(const QString& pMessage, const QString& pCategoryName);

		void onWindowPaletteChanged();
		void onAutoStartChanged();

	public Q_SLOTS:
		void doRefresh();
};

} // namespace governikus
