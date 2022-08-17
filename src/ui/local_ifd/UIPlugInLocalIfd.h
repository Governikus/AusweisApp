/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UIPlugIn.h"
#include "context/WorkflowContext.h"

#include <QSharedPointer>

namespace governikus
{

class UIPlugInLocalIfd
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private:
		QSharedPointer<WorkflowContext> mContext;

	private Q_SLOTS:
		void onStateChanged(const QString& pNewState);

	public:
		UIPlugInLocalIfd();

		void doShutdown() override;
		void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;

	public Q_SLOTS:
		bool onStartWorkflowRequested(const QString& pPsk);
		void onAbortWorkflowRequested();
};

} // namespace governikus
