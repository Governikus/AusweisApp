/*!
 * \brief UIPlugIn implementation of CLI.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "ConsoleReader.h"
#include "UIPlugIn.h"

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

	using MemberFunc = void (UIPlugInCli::*)();

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
		virtual ~UIPlugInCli() override;

	public Q_SLOTS:
		virtual void onApplicationStarted() override;

		virtual void doShutdown() override;

	private Q_SLOTS:
		void doInput(const QString& pData);
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		void onStateChanged(const QString& pState);

		void handleOldPinEntered(const QString& pLine);
		void handleNewPinEntered(const QString& pLine);
		void handleNewPinEnteredAgain(const QString& pLine);
};

} // namespace governikus
