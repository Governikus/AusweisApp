/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UIPlugIn implementation of the UIPlugInWebService.
 */

#pragma once

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "HttpServer.h"
#include "UIPlugIn.h"

class test_UIPlugInWebService;

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

	private:
		QSharedPointer<HttpServer> mServer;

		[[nodiscard]] bool listening();
		[[nodiscard]] bool initialize() override;

		void handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest) override;
		void handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest) override;

	private Q_SLOTS:
		void doShutdown() override;
		void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onNewRequest(const QSharedPointer<HttpRequest>& pRequest);

	public:
		UIPlugInWebService();
		~UIPlugInWebService() override = default;
};

} // namespace governikus
