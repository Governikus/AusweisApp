/*!
 * \brief Class to store configuration data about available card readers.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
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

	private:
		friend class MockReaderConfiguration;

		const QSharedPointer<UpdatableFile> mUpdatableFile;
		QVector<ReaderConfigurationInfo> mReaderConfigurationInfos;

		bool parseReaderConfiguration();

	private Q_SLOTS:
		void onFileUpdated();

	protected:
		ReaderConfiguration();
		virtual ~ReaderConfiguration() = default;
		static ReaderConfiguration& getInstance();

	public:
		static QString getNoReaderFoundIconPath();
		static QString getMultipleReaderIconPath();

		void update();
		const ReaderConfigurationInfo getRemoteReaderConfigurationInfo() const;
		const QVector<ReaderConfigurationInfo>& getReaderConfigurationInfos() const;
		const QVector<ReaderConfigurationInfo> getSupportedReaderConfigurationInfos() const;

		ReaderConfigurationInfo getReaderConfigurationInfoById(const UsbId& pId) const;

	Q_SIGNALS:
		void fireUpdated();
};

} // namespace governikus
