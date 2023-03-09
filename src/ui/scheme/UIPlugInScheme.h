/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UIPlugIn.h"

namespace governikus
{

/*!
 * This ActivationHandler implements an API by opening custom URLs with scheme 'eid',
 * as specified by TR-03124-1.
 * Custom schemes can be registered for iOS apps.
 */
class UIPlugInScheme
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private Q_SLOTS:
		void onCustomUrl(const QUrl& pUrl);
		void doShutdown() override;
		void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;

	public:
		UIPlugInScheme();
		~UIPlugInScheme() override;
};

} // namespace governikus
