/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UIPlugIn implementation of full automatic authentication.
 */

#pragma once

#include "UIPlugIn.h"

class test_UIPlugInAutomatic;

namespace governikus
{

class UIPlugInAutomatic
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)
	friend class ::test_UIPlugInAutomatic;

	private:
		QSharedPointer<WorkflowContext> mContext;
		QString mDominator;
		bool mPrevUsedAsSDK;
		bool mPrevUsedDeveloperMode;

		[[nodiscard]] bool isDominated() const;
		void handleInsertCard();
		void handlePassword();

	private Q_SLOTS:
		void onApplicationStarted() override;
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onStateChanged(const QString& pState);

	public:
		UIPlugInAutomatic();
		~UIPlugInAutomatic() override = default;
};

} // namespace governikus
