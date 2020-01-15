/*!
 * \brief UIPlugIn implementation of the AIDL UI.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UIPlugIn.h"
#include "UIPlugInJson.h"

#include <QAtomicPointer>
#include <QMutex>

namespace governikus
{

class UIPlugInAidl
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private:
		UIPlugInJson* mJson;
		QSharedPointer<WorkflowContext> mContext;
		QMutex mWorkflowIsActive;

		static QAtomicPointer<UIPlugInAidl> instance;
		bool mInitializationSuccessfull;

	public:
		UIPlugInAidl();
		virtual ~UIPlugInAidl() override;

		static UIPlugInAidl* getInstance(bool pBlock = true);
		bool isSuccessfullInitialized();
		Q_INVOKABLE void onReceived(const QByteArray& pMessage);
		bool waitForWorkflowToFinish();

	public Q_SLOTS:
		void reset();

	private Q_SLOTS:
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;

		void onToSend(const QByteArray& pMessage);
};

} // namespace governikus
