/*!
 * \brief Contains information collected by the diagnosis functionality.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDateTime>
#include <QObject>
#include <QStringList>
#include <QVector>

#include "ReaderInfo.h"

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
		QDateTime mTimestamp;

	public:
		DiagnosisContext();

		const QString& getPcscVersion() const
		{
			return mPcscVersion;
		}


		const QVector<ComponentInfo>& getPcscComponents() const
		{
			return mPcscComponents;
		}


		const QVector<ComponentInfo>& getPcscDrivers() const
		{
			return mPcscDrivers;
		}


		void setPcscInfo(const QString& pVersion, const QVector<ComponentInfo>& pComponents, const QVector<ComponentInfo>& pDrivers);

		const QVector<ReaderInfo>& getReaderInfos() const
		{
			return mReaderInfos;
		}


		void setReaderInfos(const QVector<ReaderInfo>& mInfos);

		const QDateTime& getTimestamp() const
		{
			return mTimestamp;
		}


		void setTimestamp(const QDateTime& pTimestamp);

	Q_SIGNALS:
		void pcscVersionChanged();
		void readerInfosChanged();
		void timestampChanged();
		void pcscInfoChanged();
		void modelChanged();
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


		const QString& getPath() const
		{
			return mPath;
		}


		const QString& getDescription() const
		{
			return mDescription;
		}


		const QString& getVersion() const
		{
			return mVersion;
		}


		const QString& getManufacturer() const
		{
			return mManufacturer;
		}


};

} /* namespace governikus */
