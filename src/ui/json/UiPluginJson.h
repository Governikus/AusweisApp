/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief UiPlugin implementation of the Json API.
 */

#pragma once

#include "MessageDispatcher.h"
#include "UiPlugin.h"


class test_UiPluginJson;
class test_MsgHandlerAuth;
class test_MsgHandlerPersonalization;


namespace governikus
{

class UiPluginJson
	: public UiPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)
	friend class ::test_UiPluginJson;
	friend class ::test_MsgHandlerAuth;
	friend class ::test_MsgHandlerPersonalization;

	private:
		MessageDispatcher mMessageDispatcher;
		bool mEnabled;

		inline void callFireMessage(const QByteArray& pMsg, bool pLogging = true);

	public:
		UiPluginJson();
		~UiPluginJson() override = default;

		void setEnabled(bool pEnable = true);
		[[nodiscard]] bool isEnabled() const;

	private Q_SLOTS:
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onCardInfoChanged(const ReaderInfo& pInfo);
		void onReaderEvent(const ReaderInfo& pInfo);
		void onCardInserted(const ReaderInfo& pInfo);
		void onStateChanged(const QString& pNewState);
		void onProgressChanged();

	public Q_SLOTS:
		void doMessageProcessing(const QByteArray& pMsg);

	Q_SIGNALS:
		void fireMessage(const QByteArray& pMsg);
};

} // namespace governikus
