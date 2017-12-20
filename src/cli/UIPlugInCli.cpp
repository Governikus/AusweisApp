/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInCli.h"

#include "states/StateEstablishPacePin.h"

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
#include "Env.h"
#include "HttpServer.h"
#endif

#include <QLoggingCategory>
#include <QRegularExpression>

Q_DECLARE_LOGGING_CATEGORY(stdinput)
Q_DECLARE_LOGGING_CATEGORY(cli)

using namespace governikus;

UIPlugInCli::UIPlugInCli()
	: mReader()
	, mAvailableCommands()
{
	addCommand(QStringLiteral("cancel"), &UIPlugInCli::handleCancelWorkflow);
	addCommand(QStringLiteral("changepin"), &UIPlugInCli::handleChangePin);
	addCommand(QStringLiteral("enter-pin"), &UIPlugInCli::handleEnterPin);
	addCommand(QStringLiteral("help"), &UIPlugInCli::handleHelp);
	addCommand(QStringLiteral("ping"), &UIPlugInCli::handlePing);
	addCommand(QStringLiteral("port"), &UIPlugInCli::handlePort);
	addCommand(QStringLiteral("quit"), &UIPlugInCli::handleQuit);

	connect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::doInput);
	mReader.init();
}


UIPlugInCli::~UIPlugInCli()
{
}


void UIPlugInCli::onApplicationStarted()
{
	qCInfo(cli) << "ready";
}


void UIPlugInCli::doShutdown()
{
	mReader.shutdown();
}


void UIPlugInCli::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	mContext = pContext;
	if (!mContext.isNull())
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged,
				this, &UIPlugInCli::onStateChanged);
		mContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC});
	}
}


void UIPlugInCli::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext);
	if (!mContext.isNull())
	{
		mContext->disconnect(this);
		mContext.reset();
	}
}


void UIPlugInCli::onStateChanged(const QString& pState)
{
	Q_UNUSED(pState);

	bool userInteractionRequired = false;

	if (AbstractState::isState<StateEstablishPacePin>(pState))
	{
		userInteractionRequired = true;
		qCInfo(cli) << "enter-pin";
	}

	mContext->setStateApproved(!userInteractionRequired);
}


void UIPlugInCli::doInput(const QString& pData)
{
	qCInfo(stdinput) << pData;

	const QStringList chunks = pData.split(QLatin1Char(' '));
	auto func = mAvailableCommands.value(chunks.at(0).toLower());
	if (func)
	{
		mCurrentCommandArgs = chunks.mid(1);
		func();
	}
	else
	{
		qCWarning(cli) << "Unknown command:" << pData;
	}
}


void UIPlugInCli::handleChangePin()
{
	mOldPin.clear();
	mNewPin.clear();

	qCDebug(cli) << "Change PIN requested";
	qCInfo(cli) << "Please enter old PIN";

	disconnect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::doInput);
	connect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::handleOldPinEntered);
}


void UIPlugInCli::handleOldPinEntered(const QString& pLine)
{
	const QRegularExpression regexOldPin(QStringLiteral("^[0-9]{5,6}$"));
	if (regexOldPin.match(pLine).hasMatch() && mReader.isInputOpen())
	{
		mOldPin = pLine;
		qCInfo(cli) << "Please enter new PIN";
		disconnect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::handleOldPinEntered);
		connect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::handleNewPinEntered);
	}
	else
	{
		qCInfo(cli) << "Please enter old PIN";
	}
}


void UIPlugInCli::handleNewPinEntered(const QString& pLine)
{
	const QRegularExpression regexNewPin(QStringLiteral("^[0-9]{6}$"));
	if (regexNewPin.match(pLine).hasMatch() && mReader.isInputOpen())
	{
		mNewPin = pLine;
		qCInfo(cli) << "Please enter new PIN again";
		disconnect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::handleNewPinEntered);
		connect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::handleNewPinEnteredAgain);
	}
	else
	{
		qCInfo(cli) << "Please enter new PIN";
	}
}


void UIPlugInCli::handleNewPinEnteredAgain(const QString& pLine)
{
	if (mNewPin != pLine)
	{
		qCInfo(cli) << "PINs were not equal";
		qCInfo(cli) << "Please enter new PIN";

		disconnect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::handleNewPinEnteredAgain);
		connect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::handleNewPinEntered);
	}
	else if (mReader.isInputOpen())
	{
		qCDebug(cli) << "Start";
		Q_EMIT fireChangePinRequest();
		connect(&mReader, &ConsoleReader::fireText, this, &UIPlugInCli::doInput);
	}
}


void UIPlugInCli::handleCancelWorkflow()
{
	if (mContext.isNull())
	{
		qCInfo(cli) << "error";
	}
	else
	{
		Q_EMIT mContext->fireCancelWorkflow();
		qCInfo(cli) << "ok";
	}
}


void UIPlugInCli::handleEnterPin()
{
	if (mCurrentCommandArgs.size() != 1 || mContext.isNull())
	{
		qCInfo(cli) << "error";
	}
	else
	{
		mContext->setPin(mCurrentCommandArgs.at(0));
		mContext->setStateApproved(true);
		qCInfo(cli) << "ok";
	}
}


void UIPlugInCli::handleHelp()
{
	qCInfo(cli) << "Available commands:" << mAvailableCommands.keys().join(QStringLiteral(" | "));
}


void UIPlugInCli::handlePing()
{
	qCInfo(cli) << "Pong!";
}


void UIPlugInCli::handlePort()
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	qCInfo(cli) << "Port:" << Env::getShared<HttpServer>()->getServerPort();
#else
	qCInfo(cli) << "Port is undefined";
#endif
}


void UIPlugInCli::handleQuit()
{
	qCInfo(cli) << "Shutdown application...";
	Q_EMIT fireQuitApplicationRequest();
}
