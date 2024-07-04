/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UiPlugin implementation of full automatic authentication.
 */

#pragma once

#include "UiPlugin.h"

class test_UiPluginAutomatic;

namespace governikus
{

class UiPluginAutomatic
	: public UiPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)
	friend class ::test_UiPluginAutomatic;

	private:
		QSharedPointer<WorkflowContext> mContext;
		QString mDominator;
		bool mPrevUsedAsSDK;
		bool mPrevUsedDeveloperMode;

		[[nodiscard]] bool isDominated() const;
		void handleInsertCard();
		void handleInsertCardScanFinished();
		void handlePassword();

	private Q_SLOTS:
		void onApplicationStarted() override;
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onStateChanged(const QString& pState);

	public:
		UiPluginAutomatic();
		~UiPluginAutomatic() override = default;
};

} // namespace governikus
