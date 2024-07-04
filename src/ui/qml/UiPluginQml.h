/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UiPlugin implementation of QML.
 */

#pragma once

#include "GlobalStatus.h"
#include "ProxyCredentials.h"
#include "TrayIcon.h"
#include "UiPluginModel.h"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QScopedPointer>
#if defined (Q_OS_MACOS)
	#include <QMenuBar>
#endif

#ifdef Q_OS_IOS
Q_FORWARD_DECLARE_OBJC_CLASS(FontChangeTracker);
#endif


class test_UiPluginQml;


namespace governikus
{

class UiPluginQml
	: public UiPluginModel
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)
	friend class ::test_UiPluginQml;

	private:
		QScopedPointer<QQmlApplicationEngine> mEngine;
#ifdef Q_OS_MACOS
		QScopedPointer<QMenuBar> mMenuBar;
#endif
		qsizetype mQmlEngineWarningCount;
		bool mUpdateInformationPending;
		TrayIcon mTrayIcon;
		QString mDominator;
		bool mHighContrastEnabled;
		bool mDarkMode;
		bool mShowFocusIndicator;
		constexpr static qreal DEFAULT_SCALE_FACTOR = 0.6;
		qreal mScaleFactor;
		qreal mFontScaleFactor;

		void init();
		[[nodiscard]] static QString getOverridePlatform();
		[[nodiscard]] QQuickWindow* getRootWindow() const;
		[[nodiscard]] bool isHidden() const;
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
		UiPluginQml();
		~UiPluginQml() override = default;

		static void registerQmlTypes();
#ifndef QT_NO_DEBUG
		static QString adjustQmlImportPath(QQmlEngine* pEngine);
#endif

		[[nodiscard]] bool isDebugBuild() const override;
		[[nodiscard]] bool isDeveloperVersion() const override;
		[[nodiscard]] QString getDominator() const override;
		[[nodiscard]] bool isDominated() const override;
		[[nodiscard]] QVariantMap getSafeAreaMargins() const override;
		[[nodiscard]] bool isHighContrastEnabled() const override;
		[[nodiscard]] bool isOsDarkModeEnabled() const override;
		[[nodiscard]] bool isOsDarkModeSupported() const override;
		[[nodiscard]] QString getFixedFontFamily() const override;
		[[nodiscard]] QSize getInitialWindowSize() const override;
		[[nodiscard]] bool getShowFocusIndicator() const override;
		[[nodiscard]] qreal getScaleFactor() const override;
		void setScaleFactor(qreal pScaleFactor) override;
		[[nodiscard]] qreal getFontScaleFactor() const override;
		[[nodiscard]] bool isChromeOS() const override;

		Q_INVOKABLE void hideFromTaskbar() const override;
		Q_INVOKABLE void doRefresh() override;

	Q_SIGNALS:
		void fireTranslationChanged();
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
		void onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onShowUserInformation(const QString& pMessage) override;
		void onUpdateScheduled() const;
		void onAppcastFinished(bool pUpdateAvailable);

		void onQmlWarnings(const QList<QQmlError>& pWarnings);
		void onQmlObjectCreated(QObject* pObject);
		void onSceneGraphError(QQuickWindow::SceneGraphError pError, const QString& pMessage);

		void onRawLog(const QString& pMessage, const QString& pCategoryName);

		void onWindowPaletteChanged();
		void onUseSystemFontChanged() const;
		void onAutoStartChanged();
		void onAppConfigChanged();
};

} // namespace governikus
