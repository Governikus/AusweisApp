/*!
 * \brief UIPlugIn implementation of QML.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CertificateDescriptionModel.h"
#include "ChatModel.h"
#include "ConnectivityManager.h"
#include "HistoryModel.h"
#include "NumberModel.h"
#include "SettingsModel.h"
#include "TrayIcon.h"
#include "UIPlugIn.h"
#include "VersionInformationModel.h"

#include <QQmlApplicationEngine>
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
	Q_PROPERTY(QString platformStyle READ getPlatformStyle CONSTANT)
	Q_PROPERTY(bool developerBuild READ isDeveloperBuild CONSTANT)
	Q_PROPERTY(QString dominator READ getDominator NOTIFY fireDominatorChanged)
	Q_PROPERTY(bool dominated READ isDominated NOTIFY fireDominatorChanged)
	Q_PROPERTY(QVariantMap safeAreaMargins READ getSafeAreaMargins NOTIFY fireSafeAreaMarginsChanged)

	private:
		QScopedPointer<QQmlApplicationEngine> mEngine;
		int mQmlEngineWarningCount;
		VersionInformationModel mVersionInformationModel;
		CertificateDescriptionModel mCertificateDescriptionModel;
		ChatModel mChatModel;
		QString mExplicitPlatformStyle;
		ConnectivityManager mConnectivityManager;
		TrayIcon mTrayIcon;
		QString mDominator;
#if defined(Q_OS_MACOS)
		QMenuBar mMenuBar;
#endif

		void logRenderingEnvironment() const;
		QString getPlatformSelectors() const;
		static QUrl getPath(const QString& pRelativePath, bool pQrc = true);
		bool isTablet() const;

	public:
		UIPlugInQml();
		virtual ~UIPlugInQml() override = default;

		static void registerQmlTypes();

		QString getPlatformStyle() const;
		bool isDeveloperBuild() const;
		QString getDominator() const;
		bool isDominated() const;
		QVariantMap getSafeAreaMargins() const;

		Q_INVOKABLE void applyPlatformStyle(const QString& pPlatformStyle);
		Q_INVOKABLE void init();
		Q_INVOKABLE void hide();
		Q_INVOKABLE void switchUi();

	Q_SIGNALS:
		void fireShowRequest(UiModule pModule);
		void fireHideRequest();
		void fireDominatorChanged();
		void fireSafeAreaMarginsChanged();

	private Q_SLOTS:
		void show();
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onApplicationStarted() override;
		virtual void onShowUi(UiModule pModule) override;
		virtual void onHideUi() override;
		virtual void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted) override;
		virtual void onUiDominationReleased() override;
		void onShowUserInformation(const QString& pMessage);

		void onQmlWarnings(const QList<QQmlError>& pWarnings);
		void onQmlObjectCreated(QObject* pObject);

		void onRawLog(const QString& pMessage, const QString& pCategoryName);

	public Q_SLOTS:
		void doRefresh();
};

} // namespace governikus
