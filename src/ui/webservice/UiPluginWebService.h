/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UiPlugin implementation of the UiPluginWebService.
 */

#pragma once

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "HttpServer.h"
#include "UiPlugin.h"

#include "context/AuthContext.h"

class test_UiPluginWebService;
class test_UiPluginWebServiceBrowserHandler;

namespace governikus
{

/*!
 * This an API through a local web service as specified by TR-03124-1.
 */
class UiPluginWebService
	: public UiPlugin
	, private HttpHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)
	friend class ::test_UiPluginWebService;
	friend class ::test_UiPluginWebServiceBrowserHandler;

	private:
		enum class ExistingAppResult : int
		{
			REBIND_SUCCEED,
			REBIND_FAILED,
			SHOWUI_SUCCEED,
			SHOWUI_TIMEOUT
		};

		QSharedPointer<HttpServer> mServer;

		[[nodiscard]] bool listening();
		[[nodiscard]] bool initialize() override;
		[[nodiscard]] ExistingAppResult handleExistingApp(quint16 pPort, const QHostAddress& pHost) const;

		void setCommonHeaders(HttpResponse& pResponse) const;
		QString sendErrorPage(const QSharedPointer<HttpRequest>& pRequest, http_status pStatusCode, const GlobalStatus& pStatus) const;
		QString sendRedirect(const QSharedPointer<HttpRequest>& pRequest, const QUrl& pRedirectAddress) const;
		QString sendRedirect(const QSharedPointer<HttpRequest>& pRequest, const QSharedPointer<AuthContext>& pContext) const;

		void sendWorkflowAlreadyActive(const QSharedPointer<HttpRequest>& pRequest) const;
		void handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest) override;
		void handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest) override;

	private Q_SLOTS:
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onNewRequest(const QSharedPointer<HttpRequest>& pRequest);

	public:
		UiPluginWebService();
		~UiPluginWebService() override = default;
};

} // namespace governikus
