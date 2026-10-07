/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include <QCoreApplication>
#include <QFile>
#include <QFileInfoList>
#include <QList>
#include <QString>

namespace governikus
{

class PortFile
{
	private:
		quint16 mDefaultPort;
		QFile mPortFile;

		[[nodiscard]] static quint16 readPortFile(const QString& pFile);

	public:
		static constexpr quint16 cDefaultPort = 24727;

		[[nodiscard]] static QFileInfoList getAllPortFiles();
		[[nodiscard]] static QString getPortFilename(const QString& pUsage = QString(),
				qint64 pPid = QCoreApplication::applicationPid(),
				const QString& pApp = QCoreApplication::applicationName());
		[[nodiscard]] static QList<quint16> readAllPortFiles();

		explicit PortFile(const QString& pUsage = QString(), quint16 pDefaultPort = cDefaultPort);
		~PortFile();

		void handlePort(quint16 pCurrentPort);
		void remove();
		[[nodiscard]] const QFile& getFile() const;
};

} // namespace governikus
