/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Contains information collected by the diagnosis functionality.
 */

#pragma once

#include "ReaderInfo.h"

#include <QDateTime>
#include <QList>
#include <QNetworkInterface>
#include <QObject>
#include <QStringList>

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
		QList<ComponentInfo> mPcscComponents;
		QList<ComponentInfo> mPcscDrivers;
		QList<ReaderInfo> mReaderInfos;
		QList<ReaderConfigurationInfo> mReaderInfosNoDriver;
		QDateTime mTimestamp;
		QList<QNetworkInterface> mNetworkInterfaces;

	public:
		DiagnosisContext();

		[[nodiscard]] const QString& getPcscVersion() const
		{
			return mPcscVersion;
		}


		[[nodiscard]] const QList<ComponentInfo>& getPcscComponents() const
		{
			return mPcscComponents;
		}


		[[nodiscard]] const QList<ComponentInfo>& getPcscDrivers() const
		{
			return mPcscDrivers;
		}


		void setPcscInfo(const QString& pVersion, const QList<ComponentInfo>& pComponents, const QList<ComponentInfo>& pDrivers);

		[[nodiscard]] const QList<ReaderInfo>& getReaderInfos() const
		{
			return mReaderInfos;
		}


		void setReaderInfos(const QList<ReaderInfo>& mInfos);


		[[nodiscard]] const QList<ReaderConfigurationInfo>& getReaderInfosNoDriver() const
		{
			return mReaderInfosNoDriver;
		}


		void setReaderInfosNoDriver(const QList<ReaderConfigurationInfo>& pInfos);

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
