/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UiPlugin.h"
#include "context/WorkflowContext.h"

#include <QSharedPointer>

namespace governikus
{

class UiPluginLocalIfd
	: public UiPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UiPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::UiPlugin)

	private:
		QSharedPointer<WorkflowContext> mContext;

	private Q_SLOTS:
		void onStateChanged(const QString& pNewState);
		void onConnectedChanged(bool pConnected);
		void onSocketError(QAbstractSocket::SocketError pSocketError);

	public:
		UiPluginLocalIfd();

		void doShutdown() override;
		void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) override;
		void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) override;

	public Q_SLOTS:
		bool onStartWorkflowRequested(const QString& pPsk);
		void onAbortWorkflowRequested();
};

} // namespace governikus
