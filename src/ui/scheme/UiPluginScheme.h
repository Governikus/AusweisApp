/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UiPlugin.h"

#include <QString>
#include <QUrl>


class test_UiPluginScheme;


namespace governikus
{

/*!
 * This ActivationHandler implements an API by opening custom URLs with scheme 'eid',
 * as specified by TR-03124-1.
 * Custom schemes can be registered for iOS apps.
 */
class UiPluginScheme
	: public UiPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)

	friend class ::test_UiPluginScheme;

	private:
		class Referrer
			: public QString
		{
			public:
				void operator=(const QString& pOther)
				{
					QString::operator=(pOther);
				}


		};

		void sendRedirect(const Referrer& pReferrer, const QUrl& pRedirectAddress) const;

	private Q_SLOTS:
		void onCustomUrl(const QUrl& pUrl);
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest) override;

	public:
		UiPluginScheme();
		~UiPluginScheme() override;
};

} // namespace governikus
