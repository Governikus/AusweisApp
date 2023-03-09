/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CommandLineParser.h"

#include "Env.h"
#include "HttpServer.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "PortFile.h"
#include "SingletonHelper.h"
#include "UILoader.h"
#include "controller/AppController.h"

#include <QCoreApplication>


using namespace governikus;


defineSingleton(CommandLineParser)


CommandLineParser::CommandLineParser()
	: mParser()
	, mOptionKeepLog(QStringLiteral("keep"), QStringLiteral("Keep logfile."))
	, mOptionNoLogFile(QStringLiteral("no-logfile"), QStringLiteral("Disable logfile."))
	, mOptionNoLogHandler(QStringLiteral("no-loghandler"), QStringLiteral("Disable default log handler."))
	, mOptionShowWindow(QStringLiteral("show"), QStringLiteral("Show window on startup."))
	, mOptionProxy(QStringLiteral("no-proxy"), QStringLiteral("Ignore proxy settings."))
	, mOptionUi(QStringLiteral("ui"), QStringLiteral("Use given UI plugin."), UILoader::getDefault())
	, mOptionPort(QStringLiteral("port"), QStringLiteral("Use listening port."), QString::number(PortFile::cDefaultPort))
	, mOptionAddresses(QStringLiteral("address"), QStringLiteral("Use address binding."), HttpServer::getDefault())
{
	addOptions();
}


void CommandLineParser::addOptions()
{
	mParser.addHelpOption();
	mParser.addVersionOption();

	mParser.addOption(mOptionKeepLog);
	mParser.addOption(mOptionNoLogFile);
	mParser.addOption(mOptionNoLogHandler);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	mParser.addOption(mOptionShowWindow);
#endif

	mParser.addOption(mOptionProxy);
	mParser.addOption(mOptionUi);
	mParser.addOption(mOptionPort);
	mParser.addOption(mOptionAddresses);
}


void CommandLineParser::parse(const QCoreApplication* pApp)
{
	if (pApp == nullptr)
	{
		return;
	}

	mParser.process(*pApp);
	parseUiPlugin();

	const auto& logHandler = Env::getSingleton<LogHandler>();

	logHandler->setAutoRemove(!mParser.isSet(mOptionKeepLog));
	logHandler->setLogFile(!mParser.isSet(mOptionNoLogFile));
	logHandler->setUseHandler(!mParser.isSet(mOptionNoLogHandler));

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	AppController::cShowUi = mParser.isSet(mOptionShowWindow);
#endif

	NetworkManager::lockProxy(mParser.isSet(mOptionProxy));

	if (mParser.isSet(mOptionPort))
	{
		bool converted = false;
		const uint port = mParser.value(mOptionPort).toUInt(&converted);
		if (converted && port <= std::numeric_limits<quint16>::max())
		{
			HttpServer::cPort = static_cast<quint16>(port);
		}
	}

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mParser.isSet(mOptionAddresses))
	{
		HttpServer::cAddresses.clear();

		const auto& addresses = mParser.values(mOptionAddresses);
		for (const auto& ip : addresses)
		{
			const QHostAddress entry(ip);
			if (!entry.isNull())
			{
				HttpServer::cAddresses << entry;
			}
		}
	}
#endif
}


void CommandLineParser::parseUiPlugin()
{
	if (mParser.isSet(mOptionUi))
	{
		UILoader::setUserRequest(mParser.values(mOptionUi));
	}
}
