/*!
 * \brief UIPlugIn implementation of the Json API.
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "MessageDispatcher.h"
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

		inline void callFireMessage(const QByteArray& pMsg);

	public:
		UIPlugInJsonApi();
		virtual ~UIPlugInJsonApi() override;

	private Q_SLOTS:
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onReaderEvent(const QString& pName);
		void onStateChanged(const QString& pNewState);

	public Q_SLOTS:
		void doMessageProcessing(const QByteArray& pMsg);

	Q_SIGNALS:
		void fireMessage(const QByteArray& pMsg);
};

} /* namespace governikus */
