/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"
#include "UIPlugIn.h"

class test_UIPlugInScheme;

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

	friend class ::test_UIPlugInScheme;

	using RedirectHandler = std::function<void (const QUrl&, const GlobalStatus&)>;

	private:
		void handleTcTokenUrl(const QUrl& pActivationUrl, const QString& pReferrer);

	private Q_SLOTS:
		void onCustomUrl(const QUrl& pUrl);
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest) override;

	public:
		UIPlugInScheme();
		~UIPlugInScheme() override;
};

} // namespace governikus
