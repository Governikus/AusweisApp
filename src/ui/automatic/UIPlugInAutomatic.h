/*!
 * \brief UIPlugIn implementation of full automatic authentication.
 *
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
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

		[[nodiscard]] bool isDominated() const;
		void handleInsertCard();
		void handlePassword();

	private Q_SLOTS:
		void onApplicationStarted() override;
		void doShutdown() override;
		void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted) override;
		void onUiDominationReleased() override;
		void onStateChanged(const QString& pState);

	public:
		UIPlugInAutomatic();
		~UIPlugInAutomatic() override = default;
};

} // namespace governikus
