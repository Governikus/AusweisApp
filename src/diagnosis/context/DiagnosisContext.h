/*!
 * \brief Contains information collected by the diagnosis functionality.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderInfo.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QObject>
#include <QStringList>
#include <QVector>

namespace governikus
{

class DiagnosisContext
	: public QObject
{
	Q_OBJECT

	public:
		class ComponentInfo;

	private:
		QString mPcscVersion;
		QVector<ComponentInfo> mPcscComponents;
		QVector<ComponentInfo> mPcscDrivers;
		QVector<ReaderInfo> mReaderInfos;
		QVector<ReaderConfigurationInfo> mReaderInfosNoDriver;
		QDateTime mTimestamp;
		QList<QNetworkInterface> mNetworkInterfaces;

	public:
		DiagnosisContext();

		[[nodiscard]] const QString& getPcscVersion() const
		{
			return mPcscVersion;
		}


		[[nodiscard]] const QVector<ComponentInfo>& getPcscComponents() const
		{
			return mPcscComponents;
		}


		[[nodiscard]] const QVector<ComponentInfo>& getPcscDrivers() const
		{
			return mPcscDrivers;
		}


		void setPcscInfo(const QString& pVersion, const QVector<ComponentInfo>& pComponents, const QVector<ComponentInfo>& pDrivers);

		[[nodiscard]] const QVector<ReaderInfo>& getReaderInfos() const
		{
			return mReaderInfos;
		}


		void setReaderInfos(const QVector<ReaderInfo>& mInfos);


		[[nodiscard]] const QVector<ReaderConfigurationInfo>& getReaderInfosNoDriver() const
		{
			return mReaderInfosNoDriver;
		}


		void setReaderInfosNoDriver(const QVector<ReaderConfigurationInfo>& pInfos);

		[[nodiscard]] const QDateTime& getTimestamp() const
		{
			return mTimestamp;
		}


		void setTimestamp(const QDateTime& pTimestamp);

		void setNetworkInterfaces(const QList<QNetworkInterface>& pNetworkInterface);
		[[nodiscard]] const QList<QNetworkInterface>& getNetworkInterfaces() const;

	Q_SIGNALS:
		void pcscVersionChanged();
		void readerInfosChanged();
		void timestampChanged();
		void pcscInfoChanged();
		void modelChanged();
		void fireDataChanged();
		void fireNetworkInfoChanged();
};


class DiagnosisContext::ComponentInfo
{
	QString mPath;
	QString mDescription;
	QString mVersion;
	QString mManufacturer;

	public:
		ComponentInfo(const QString& pPath = QString(), const QString& pDescription = QString(),
				const QString& pVersion = QString(), const QString& pManufacturer = QString())
			: mPath(pPath)
			, mDescription(pDescription)
			, mVersion(pVersion)
			, mManufacturer(pManufacturer)
		{
		}


		[[nodiscard]] const QString& getPath() const
		{
			return mPath;
		}


		[[nodiscard]] const QString& getDescription() const
		{
			return mDescription;
		}


		[[nodiscard]] const QString& getVersion() const
		{
			return mVersion;
		}


		[[nodiscard]] const QString& getManufacturer() const
		{
			return mManufacturer;
		}


};

} // namespace governikus
