/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UIPlugIn implementation of the Json API.
 */

#pragma once

#include "MessageDispatcher.h"
#include "UIPlugIn.h"
#include "context/WorkflowContext.h"

class test_MsgHandlerAuth;
class test_MsgHandlerPersonalization;

namespace governikus
{

class UIPlugInJson
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)
	friend class ::test_MsgHandlerAuth;
	friend class ::test_MsgHandlerPersonalization;

	private:
		MessageDispatcher mMessageDispatcher;
		bool mEnabled;

		inline void callFireMessage(const QByteArray& pMsg, bool pLogging = true);

	public:
		UIPlugInJson();
		~UIPlugInJson() override = default;

		void setEnabled(bool pEnable = true);
		[[nodiscard]] bool isEnabled() const;

	private Q_SLOTS:
		void doShutdown() override;
		void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onCardInfoChanged(const ReaderInfo& pInfo);
		void onReaderEvent(const ReaderInfo& pInfo);
		void onStateChanged(const QString& pNewState);
		void onProgressChanged();

	public Q_SLOTS:
		void doMessageProcessing(const QByteArray& pMsg);

	Q_SIGNALS:
		void fireMessage(const QByteArray& pMsg);
};

} // namespace governikus
