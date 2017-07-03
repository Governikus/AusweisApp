/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "UIPlugInCli.h"

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
#include "WebserviceActivationHandler.h"
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
		connect(mContext.data(), &WorkflowContext::fireCurrentStateChanged,
				this, &UIPlugInCli::onCurrentStateChanged);
		mContext->setReaderType(ReaderManagerPlugInType::PCSC);
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


void UIPlugInCli::onCurrentStateChanged(const QString& pState)
{
	Q_UNUSED(pState);

	bool userInteractionRequired = false;

	if (pState == QLatin1String("StateEstablishPacePin"))
	{
		userInteractionRequired = true;
		qCInfo(cli) << "enter-pin";
	}

	mContext->setStateApproved(!userInteractionRequired);
}


void UIPlugInCli::doInput(const QString& pData)
{
	qCInfo(stdinput) << pData;

	const QStringList chunks = pData.split(QChar(' '));
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
	qCDebug(cli) << "Change PIN requested";

	const QRegularExpression regexOldPin(QStringLiteral("^[0-9]{5,6}$"));
	QString oldPin;
	while (!regexOldPin.match(oldPin).hasMatch() && mReader.isInputOpen())
	{
		oldPin = mReader.requestText(QStringLiteral("Please enter old PIN"));
	}

	const QRegularExpression regexNewPin(QStringLiteral("^[0-9]{6}$"));
	QString newPin;
	for (QString newPin2; (newPin != newPin2 || newPin.isEmpty()) && mReader.isInputOpen(); )
	{
		while (!regexNewPin.match(newPin).hasMatch() && mReader.isInputOpen())
		{
			newPin = mReader.requestText(QStringLiteral("Please enter new PIN"));
		}

		if (mReader.isInputOpen())
		{
			newPin2 = mReader.requestText(QStringLiteral("Please enter new PIN again"));
			if (newPin != newPin2)
			{
				qCInfo(cli) << "PINs were not equal";
			}
		}
	}

	if (mReader.isInputOpen())
	{
		qCDebug(cli) << "Start";
		Q_EMIT fireChangePinRequest();
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
	qCInfo(cli) << "Available commands:" << QStringList(mAvailableCommands.keys()).join(QStringLiteral(" | "));
}


void UIPlugInCli::handlePing()
{
	qCInfo(cli) << "Pong!";
}


void UIPlugInCli::handlePort()
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	auto handler = ActivationHandler::getInstance<WebserviceActivationHandler>();
	if (handler)
	{
		qCInfo(cli) << "Port:" << handler->getServerPort();
	}
#else
	qCInfo(cli) << "Port is undefined";
#endif
}


void UIPlugInCli::handleQuit()
{
	qCInfo(cli) << "Shutdown application...";
	Q_EMIT fireQuitApplicationRequest();
}
