/*!
 * \brief Class to store configuration data about available card readers.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "Env.h"
#include "ReaderConfigurationInfo.h"
#include "UpdatableFile.h"
#include "UsbId.h"

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVector>


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
		QVector<ReaderConfigurationInfo> mReaderConfigurationInfos;

		ReaderConfiguration();
		~ReaderConfiguration() override = default;
		bool parseReaderConfiguration(const QString& pPath);

	private Q_SLOTS:
		void onFileUpdated();

	public:
		static QString getNoReaderFoundIconPath();
		static QString getMultipleReaderIconPath();

		void update();
		[[nodiscard]] const QVector<ReaderConfigurationInfo>& getReaderConfigurationInfos() const;
		[[nodiscard]] QVector<ReaderConfigurationInfo> getSupportedReaderConfigurationInfos() const;
		[[nodiscard]] QVector<ReaderConfigurationInfo> getVirtualReaderConfigurationInfos() const;

		[[nodiscard]] ReaderConfigurationInfo getReaderConfigurationInfoById(const UsbId& pId) const;

	Q_SIGNALS:
		void fireUpdated();
		void fireNoUpdateAvailable();
};

} // namespace governikus
