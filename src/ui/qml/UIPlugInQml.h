/*!
 * \brief UIPlugIn implementation of QML.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CertificateDescriptionModel.h"
#include "ChatModel.h"
#include "ConnectivityManager.h"
#include "HistoryModel.h"
#include "NumberModel.h"
#include "QmlExtension.h"
#include "SelfAuthModel.h"
#include "SettingsModel.h"
#include "TrayIcon.h"
#include "UIPlugIn.h"
#include "VersionInformationModel.h"

#include <QQmlApplicationEngine>
#include <QScopedPointer>

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

	private:
		QScopedPointer<QQmlApplicationEngine> mEngine;
		HistoryModel mHistoryModel;
		VersionInformationModel mVersionInformationModel;
		QmlExtension mQmlExtension;
		SelfAuthModel mSelfAuthModel;
		SettingsModel mSettingsModel;
		CertificateDescriptionModel mCertificateDescriptionModel;
		ChatModel mChatModel;
		QString mExplicitPlatformStyle;
		ConnectivityManager mConnectivityManager;
		TrayIcon mTrayIcon;

		QString getPlatformSelectors() const;
		static QUrl getPath(const QString& pRelativePath, bool pQrc = true);
		void createTrayIcon();

	public:
		UIPlugInQml();
		virtual ~UIPlugInQml() override = default;

		static void registerQmlTypes();

		Q_INVOKABLE bool useFlatStyleOnDesktop() const;
		QString getPlatformStyle() const;
		Q_INVOKABLE void applyPlatformStyle(const QString& pPlatformStyle);
		Q_INVOKABLE bool isDeveloperBuild() const;
		Q_INVOKABLE void init();
		Q_INVOKABLE void hide();

	Q_SIGNALS:
		void fireShowRequest(UiModule pModule);

	private Q_SLOTS:
		void show();
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onApplicationStarted() override;
		virtual void onShowUi(UiModule pModule) override;
		void onShowUserInformation(const QString& pMessage);

	public Q_SLOTS:
		void doRefresh();
};

} // namespace governikus
