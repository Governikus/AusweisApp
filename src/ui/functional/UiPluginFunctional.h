/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UiPlugin implementation of the integrated SDK (C-API).
 */

#pragma once

#include "UiPlugin.h"
#include "UiPluginJson.h"

namespace governikus
{

class UiPluginFunctional
	: public UiPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)

	private:
		QPointer<UiPluginJson> mJson;
		QSharedPointer<WorkflowContext> mContext;

	private Q_SLOTS:
		void onApplicationStarted() override;
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onJsonMessage(const QByteArray& pMessage);

	public Q_SLOTS:
		void doMessageProcessing(const QByteArray& pMsg);
		void doQuitApplicationRequest();

	public:
		UiPluginFunctional();
		~UiPluginFunctional() override = default;

		[[nodiscard]] bool initialize() override;
};

} // namespace governikus
