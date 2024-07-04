/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UiPlugin implementation of the UiPluginProxy.
 */

#pragma once

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "HttpServer.h"
#include "UiPlugin.h"

class test_UiPluginProxy;

namespace governikus
{
class UiPluginProxy
	: public UiPlugin
	, private HttpHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)
	friend class ::test_UiPluginProxy;

	private:
		QSharedPointer<HttpServer> mServer;

		[[nodiscard]] bool listen();

		void handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest) override;
		void handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest) override;

	private Q_SLOTS:
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onNewRequest(const QSharedPointer<HttpRequest>& pRequest);
		void onNewWebSocketRequest(const QSharedPointer<HttpRequest>& pRequest);

	public:
		UiPluginProxy();
		~UiPluginProxy() override = default;

		[[nodiscard]] bool initialize() override;
};

} // namespace governikus
