/*!
 * \brief UIPlugIn implementation of CLI.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "ConsoleReader.h"
#include "context/WorkflowContext.h"
#include "view/UIPlugIn.h"

#include <QMap>
#include <QString>

#include <functional>

namespace governikus
{

class UIPlugInCli
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	typedef void (UIPlugInCli::* MemberFunc)();

	private:
		QString mOldPin;
		QString mNewPin;
		ConsoleReader mReader;
		QMap<QString, std::function<void()> > mAvailableCommands;
		QSharedPointer<WorkflowContext> mContext;
		QStringList mCurrentCommandArgs;

		void addCommand(const QString& pCmd, MemberFunc pFunc)
		{
			mAvailableCommands.insert(pCmd, std::bind(pFunc, this));
		}


		void handleCancelWorkflow();
		void handleChangePin();
		void handleEnterPin();
		void handleHelp();
		void handlePing();
		void handlePort();
		void handleQuit();

	public:
		UIPlugInCli();
		virtual ~UIPlugInCli();

	public Q_SLOTS:
		virtual void onApplicationStarted() override;

		virtual void doShutdown() override;

	private Q_SLOTS:
		void doInput(const QString& pData);
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onCurrentStateChanged(const QString& pState);

		void handleOldPinEntered(const QString& pLine);
		void handleNewPinEntered(const QString& pLine);
		void handleNewPinEnteredAgain(const QString& pLine);
};

} /* namespace governikus */
