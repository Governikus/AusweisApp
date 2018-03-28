/*
 * \brief Provides and parses command line options.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCommandLineParser>
#include <QCoreApplication>

namespace governikus
{

class CommandLineParser
{
	private:
		QCommandLineParser mParser;
		const QCommandLineOption mOptionKeepLog;
		const QCommandLineOption mOptionShowWindow;
		const QCommandLineOption mOptionProxy;
		const QCommandLineOption mOptionUi;
		const QCommandLineOption mOptionPort;
#ifndef QT_NO_DEBUG
		const QCommandLineOption mOptionPortWebSocket;
#endif

		Q_DISABLE_COPY(CommandLineParser)

		void addOptions();
		void parseUiPlugin();

	protected:
		CommandLineParser();
		~CommandLineParser();

	public:
		static CommandLineParser& getInstance();

		void parse(QCoreApplication* pApp = QCoreApplication::instance());

};

} /* namespace governikus */
