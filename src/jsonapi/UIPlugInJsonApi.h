/*!
 * \brief UIPlugIn implementation of the Json API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "MessageDispatcher.h"
#include "context/AuthContext.h"
#include "view/UIPlugIn.h"

namespace governikus
{

class UIPlugInJsonApi
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private:
		MessageDispatcher mMessageDispatcher;

	public:
		UIPlugInJsonApi();
		virtual ~UIPlugInJsonApi();

	private Q_SLOTS:
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onReaderEvent(const QString& pName);
		void onCurrentStateChanged(const QString& pNewState);

	public Q_SLOTS:
		void doMessageProcessing(const QByteArray& pMsg);

	Q_SIGNALS:
		void fireMessage(const QByteArray& pMsg);
};

} /* namespace governikus */
