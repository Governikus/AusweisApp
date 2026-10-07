/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "UiPlugin.h"

namespace governikus
{

class UiPluginDev
	: public UiPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)

	private:
		QString getActiveSelector() const;
		QStringList getTestFiles() const;

	public:
		UiPluginDev();
		~UiPluginDev() override = default;

		[[nodiscard]] bool initialize() override;
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onApplicationStarted() override;
};

} // namespace governikus
