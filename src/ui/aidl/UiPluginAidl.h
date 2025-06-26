/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UiPlugin.h"
#include "UiPluginJson.h"

#include <QAtomicPointer>
#include <QMutex>

namespace governikus
{

class UiPluginAidl
	: public UiPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)

	private:
		QPointer<UiPluginJson> mJson;
		QSharedPointer<WorkflowContext> mContext;
		QMutex mWorkflowIsActive;

		static QAtomicPointer<UiPluginAidl> instance;

	public:
		UiPluginAidl();
		~UiPluginAidl() override = default;

		static UiPluginAidl* getInstance(bool pBlock = true);
		[[nodiscard]] bool initialize() override;
		[[nodiscard]] bool isSuccessfullyInitialized() const;
		Q_INVOKABLE void onReceived(const QByteArray& pMessage);
		bool waitForWorkflowToFinish();

	public Q_SLOTS:
		void reset();
		void startReaderManagerScans() const;

	private Q_SLOTS:
		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;

		void onToSend(const QByteArray& pMessage) const;
};

} // namespace governikus
