/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
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
		void parseUiPlugin() const;

	protected:
		CommandLineParser();
		~CommandLineParser() = default;

	public:
		static CommandLineParser& getInstance();

		void parse(const QCoreApplication* pApp = QCoreApplication::instance());

};

} // namespace governikus
