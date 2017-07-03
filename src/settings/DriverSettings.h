/*!
 * DriverSettings.h
 *
 * \brief Class to retrieve and update information about available card reader drivers.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once


#include "AbstractSettings.h"


#include <QDateTime>
#include <QString>
#include <QVector>


namespace governikus
{
class Driver
{
	private:
		const QString mReaderType;

		const uint mVendorId;

		const uint mProductId;

		const QString mName;

		const QString mUrl;

	public:
		Driver(const QString& pReaderType, uint pVendorId, uint pProductId,
				const QString& pName, const QString& pUrl);

		virtual ~Driver();

		const QString& getReaderType() const;

		uint getVendorId() const;

		uint getProductId() const;

		const QString& getName() const;

		const QString& getUrl() const;
};

inline bool operator==(const Driver& pLeft, const Driver& pRight)
{
	return &pLeft == &pRight || (
		pLeft.getReaderType() == pRight.getReaderType() &&
		pLeft.getVendorId() == pRight.getVendorId() &&
		pLeft.getProductId() == pRight.getProductId() &&
		pLeft.getName() == pRight.getName() &&
		pLeft.getUrl() == pRight.getUrl());
}


class DriverSettings
	: public AbstractSettings
{
	Q_OBJECT

	private:
		QDateTime mIssueDate;

		QVector<QSharedPointer<const Driver> > mDrivers;

	public:
		DriverSettings();

		DriverSettings(const QDateTime& pIssueDate, const QVector<QSharedPointer<const Driver> >& pDrivers);

		virtual ~DriverSettings();

		virtual void load() override;

		virtual bool isUnsaved() const override;

		virtual void save() override;

		virtual void update(const AbstractSettings& pOther) override;

		const QDateTime& getIssueDate() const;

		const QVector<QSharedPointer<const Driver> >& getDrivers() const;

};

inline bool operator==(const DriverSettings& pLeft, const DriverSettings& pRight)
{
	return &pLeft == &pRight || (
		pLeft.getIssueDate() == pRight.getIssueDate() &&
		pLeft.getDrivers() == pRight.getDrivers());
}


inline bool operator!=(const DriverSettings& pLeft, const DriverSettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
