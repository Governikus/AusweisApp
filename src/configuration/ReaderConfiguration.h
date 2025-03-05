/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "Env.h"
#include "ReaderConfigurationInfo.h"
#include "UpdatableFile.h"
#include "UsbId.h"

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QString>


namespace governikus
{
class MockReaderConfiguration;

class ReaderConfiguration
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class MockReaderConfiguration;

	private:
		const QSharedPointer<UpdatableFile> mUpdatableFile;
		QList<ReaderConfigurationInfo> mReaderConfigurationInfos;

		ReaderConfiguration();
		~ReaderConfiguration() override = default;
		bool parseReaderConfiguration(const QString& pPath);

	private Q_SLOTS:
		void onFileUpdated();

	public:
		void update();
		[[nodiscard]] const QList<ReaderConfigurationInfo>& getReaderConfigurationInfos() const;
		[[nodiscard]] QList<ReaderConfigurationInfo> getSupportedReaderConfigurationInfos() const;
		[[nodiscard]] QList<ReaderConfigurationInfo> getVirtualReaderConfigurationInfos() const;

		[[nodiscard]] ReaderConfigurationInfo getReaderConfigurationInfoById(const UsbId& pId) const;

	Q_SIGNALS:
		void fireUpdated();
		void fireNoUpdateAvailable();
};

} // namespace governikus
