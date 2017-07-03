/*!
 * \brief UIPlugIn implementation of QML.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "view/UIPlugIn.h"

#include "ApplicationModel.h"
#include "AuthModel.h"
#include "CertificateDescriptionModel.h"
#include "ChangePinModel.h"
#include "ChatModel.h"
#include "ConnectivityManager.h"
#include "HistoryModel.h"
#include "NumberModel.h"
#include "ProviderCategoryFilterModel.h"
#include "QmlExtension.h"
#include "SelfAuthenticationModel.h"
#include "SettingsModel.h"
#include "StatusBarUtil.h"
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
		qreal mDpi;
		ProviderCategoryFilterModel mProviderModel;
		HistoryModel mHistoryModel;
		ChangePinModel mChangePinModel;
		AuthModel mAuthModel;
		VersionInformationModel mVersionInformationModel;
		QmlExtension mQmlExtension;
		SelfAuthenticationModel mSelfAuthenticationModel;
		SettingsModel mSettingsModel;
		CertificateDescriptionModel mCertificateDescriptionModel;
		ChatModel mChatModel;
		NumberModel mNumberModel;
		ApplicationModel mApplicationModel;
		QString mExplicitPlatformStyle;
		StatusBarUtil mStatusBarUtil;
		ConnectivityManager mConnectivityManager;

		QString getPlatformSelectors() const;

	public:
		UIPlugInQml();
		virtual ~UIPlugInQml();
		QString getPlatformStyle() const;
		Q_INVOKABLE void applyPlatformStyle(const QString& pPlatformStyle);
		Q_INVOKABLE bool isDeveloperBuild() const;
		Q_INVOKABLE void init();

		static QUrl getFile(const QString& pFile);

	private Q_SLOTS:
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;

	public Q_SLOTS:
		void doRefresh();
};

} /* namespace governikus */
