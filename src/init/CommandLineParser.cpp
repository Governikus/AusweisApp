/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CommandLineParser.h"

#include "controller/AppController.h"
#include "DatagramHandlerImpl.h"
#include "Env.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "PortFile.h"
#include "SingletonHelper.h"
#include "UILoader.h"

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
#include "HttpServer.h"
#endif

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
	, mOptionUi(QStringLiteral("ui"), QStringLiteral("Use given UI plugin."), UILoader::getDefault().join(QLatin1Char(',')))
	, mOptionPort(QStringLiteral("port"), QStringLiteral("Use listening port."), QString::number(PortFile::cDefaultPort))
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
}


void CommandLineParser::parse(QCoreApplication* pApp)
{
	if (pApp == nullptr)
	{
		return;
	}

	mParser.process(*pApp);
	parseUiPlugin();

	const auto& logHandler = Env::getSingleton<LogHandler>();
	if (mParser.isSet(mOptionKeepLog))
	{
		logHandler->setAutoRemove(false);
	}

	if (mParser.isSet(mOptionNoLogFile))
	{
		logHandler->setLogfile(false);
	}

	if (mParser.isSet(mOptionNoLogHandler))
	{
		logHandler->setUseHandler(false);
	}

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	if (mParser.isSet(mOptionShowWindow))
	{
		AppController::cShowUi = true;
	}
#endif

	if (mParser.isSet(mOptionProxy))
	{
		NetworkManager::lockProxy(true);
	}

	if (mParser.isSet(mOptionPort))
	{
		bool converted = false;
		const uint port = mParser.value(mOptionPort).toUInt(&converted);
		if (converted && port <= std::numeric_limits<quint16>::max())
		{
			DatagramHandlerImpl::cPort = static_cast<quint16>(port);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
			HttpServer::cPort = DatagramHandlerImpl::cPort;
#endif
		}
	}
}


void CommandLineParser::parseUiPlugin()
{
	if (mParser.isSet(mOptionUi))
	{
		UILoader::setDefault(mParser.values(mOptionUi));
	}
}
