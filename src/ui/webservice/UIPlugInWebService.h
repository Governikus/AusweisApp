/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UIPlugIn implementation of the UIPlugInWebService.
 */

#pragma once

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "HttpServer.h"
#include "UIPlugIn.h"

#include "context/AuthContext.h"

class test_UIPlugInWebService;
class test_UIPlugInWebServiceBrowserHandler;

namespace governikus
{

/*!
 * This an API through a local web service as specified by TR-03124-1.
 */
class UIPlugInWebService
	: public UIPlugIn
	, private HttpHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)
	friend class ::test_UIPlugInWebService;
	friend class ::test_UIPlugInWebServiceBrowserHandler;

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
		QString sendRedirect(const QSharedPointer<HttpRequest>& pRequest, const QUrl& pRedirectAddress, const ECardApiResult& pResult) const;
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
		UIPlugInWebService();
		~UIPlugInWebService() override = default;
};

} // namespace governikus
