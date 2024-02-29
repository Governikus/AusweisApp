/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Provides and parses command line options.
 */

#pragma once

#include <QCommandLineParser>
#include <QCoreApplication>

namespace governikus
{

class CommandLineParser
{
	Q_DISABLE_COPY(CommandLineParser)

	private:
		QCommandLineParser mParser;
		const QCommandLineOption mOptionKeepLog;
		const QCommandLineOption mOptionNoLogFile;
		const QCommandLineOption mOptionNoLogHandler;
		const QCommandLineOption mOptionShowWindow;
		const QCommandLineOption mOptionProxy;
		const QCommandLineOption mOptionUi;
		const QCommandLineOption mOptionPort;
		const QCommandLineOption mOptionAddresses;

		void addOptions();
		void parseUiPlugin();

	protected:
		CommandLineParser();
		~CommandLineParser() = default;

	public:
		static CommandLineParser& getInstance();

		void parse(const QCoreApplication* pApp = QCoreApplication::instance());

};

} // namespace governikus
