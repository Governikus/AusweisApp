/*!
 * \brief UIPlugIn implementation of the integrated SDK (C-API).
 *
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UIPlugIn.h"
#include "UIPlugInJson.h"

namespace governikus
{

class UIPlugInFunctional
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private:
		QPointer<UIPlugInJson> mJson;
		QSharedPointer<WorkflowContext> mContext;

	private Q_SLOTS:
		void onApplicationStarted() override;
		void doShutdown() override;
		void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onJsonMessage(const QByteArray& pMessage);

	public Q_SLOTS:
		void doMessageProcessing(const QByteArray& pMsg);
		void doQuitApplicationRequest();

	public:
		UIPlugInFunctional();
		~UIPlugInFunctional() override = default;

		[[nodiscard]] bool initialize() override;
};

} // namespace governikus
