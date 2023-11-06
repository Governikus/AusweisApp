/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UIPlugIn implementation of the UIPlugInProxy.
 */

#pragma once

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "HttpServer.h"
#include "UIPlugIn.h"

class test_UIPlugInProxy;

namespace governikus
{
class UIPlugInProxy
	: public UIPlugIn
	, private HttpHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)
	friend class ::test_UIPlugInProxy;

	private:
		QSharedPointer<HttpServer> mServer;

		[[nodiscard]] bool listen();

		void handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest) override;
		void handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest) override;

	private Q_SLOTS:
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onNewRequest(const QSharedPointer<HttpRequest>& pRequest);
		void onNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest);

	public:
		UIPlugInProxy();
		~UIPlugInProxy() override = default;

		[[nodiscard]] bool initialize() override;
};

} // namespace governikus
