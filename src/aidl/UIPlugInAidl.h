/*!
 * \brief UIPlugIn implementation of the AIDL UI.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "UIPlugInJsonApi.h"
#include "view/UIPlugIn.h"

#include <QAtomicPointer>

namespace governikus
{

class UIPlugInAidl
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private:
		UIPlugInJsonApi* mJsonApi;
		QSharedPointer<WorkflowContext> mContext;

		static QAtomicPointer<UIPlugInAidl> instance;
		bool mInitializationSuccessfull;

	public:
		UIPlugInAidl();
		virtual ~UIPlugInAidl();

		static UIPlugInAidl* getInstance(bool pBlock = true);
		bool isSuccessfullInitialized();
		Q_INVOKABLE void onReceived(const QByteArray& pMessage);
		void reset();

	private Q_SLOTS:
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;

		void onToSend(const QByteArray& pMessage);
};

} /* namespace governikus */
