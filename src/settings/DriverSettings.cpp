/*!
 * DriverSettings.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */


#include "DriverSettings.h"

#include <QDebug>

using namespace governikus;


namespace
{
const QLatin1String SETTINGS_GROUP_NAME_DRIVERS("drivers");

const QLatin1String SETTINGS_NAME_ISSUEDATE("issuedate");

const QLatin1String SETTINGS_NAME_DRIVERS("list");

const QLatin1String SETTINGS_NAME_DRIVERREADERTYPE("readertype");

const QLatin1String SETTINGS_NAME_DRIVERVENDORID("vendorid");

const QLatin1String SETTINGS_NAME_DRIVERPRODUCTID("productid");

const QLatin1String SETTINGS_NAME_DRIVERNAME("name");

const QLatin1String SETTINGS_NAME_DRIVERURL("url");
}


Driver::Driver(const QString& pReaderType, uint pVendorId, uint pProductId,
		const QString& pName, const QString& pUrl)
	: mReaderType(pReaderType)
	, mVendorId(pVendorId)
	, mProductId(pProductId)
	, mName(pName)
	, mUrl(pUrl)
{
}


Driver::~Driver()
{
}


const QString& Driver::getReaderType() const
{
	return mReaderType;
}


uint Driver::getVendorId() const
{
	return mVendorId;
}


uint Driver::getProductId() const
{
	return mProductId;
}


const QString& Driver::getName() const
{
	return mName;
}


const QString& Driver::getUrl() const
{
	return mUrl;
}


DriverSettings::DriverSettings()
	: AbstractSettings()
	, mIssueDate()
	, mDrivers()
{
}


DriverSettings::DriverSettings(const QDateTime& pIssueDate, const QVector<QSharedPointer<const Driver> >& pDrivers)
	: AbstractSettings()
	, mIssueDate(pIssueDate)
	, mDrivers(pDrivers)
{
}


DriverSettings::~DriverSettings()
{
}


void DriverSettings::load()
{
	auto settings = getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_DRIVERS);

	const int listSize = settings->beginReadArray(SETTINGS_NAME_DRIVERS);
	QVector<QSharedPointer<const Driver> > drivers;
	drivers.reserve(listSize);
	for (int i = 0; i < listSize; i++)
	{
		bool uintOk = false;
		settings->setArrayIndex(i);

		const QString readerType = settings->value(SETTINGS_NAME_DRIVERREADERTYPE).toString();
		const uint vendorId = settings->value(SETTINGS_NAME_DRIVERVENDORID).toUInt(&uintOk);
		if (!uintOk)
		{
			qWarning() << "invalid uint value in settings, skipping entry";

			continue;
		}

		const uint productId = settings->value(SETTINGS_NAME_DRIVERPRODUCTID).toUInt(&uintOk);
		if (!uintOk)
		{
			qWarning() << "invalid uint value in settings, skipping entry";

			continue;
		}

		const QString name = settings->value(SETTINGS_NAME_DRIVERNAME).toString();
		const QString url = settings->value(SETTINGS_NAME_DRIVERURL).toString();
		const QSharedPointer<const Driver> driver(new Driver(readerType, vendorId, productId, name, url));

		drivers += driver;
	}
	settings->endArray();

	mDrivers = drivers;

	mIssueDate = settings->value(SETTINGS_NAME_ISSUEDATE).toDateTime();

	settings->endGroup();
}


bool DriverSettings::isUnsaved() const
{
	DriverSettings oldSettings;
	oldSettings.load();
	return oldSettings != *this;
}


void DriverSettings::save()
{
	auto settings = getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_DRIVERS);
	settings->remove(QString()); // remove the whole group first

	settings->beginWriteArray(SETTINGS_NAME_DRIVERS);
	for (int i = 0; i < mDrivers.size(); i++)
	{
		settings->setArrayIndex(i);

		settings->setValue(SETTINGS_NAME_DRIVERREADERTYPE, mDrivers.at(i)->getReaderType());
		settings->setValue(SETTINGS_NAME_DRIVERVENDORID, mDrivers.at(i)->getVendorId());
		settings->setValue(SETTINGS_NAME_DRIVERPRODUCTID, mDrivers.at(i)->getProductId());
		settings->setValue(SETTINGS_NAME_DRIVERNAME, mDrivers.at(i)->getName());
		settings->setValue(SETTINGS_NAME_DRIVERURL, mDrivers.at(i)->getUrl());
	}
	settings->endArray();

	settings->setValue(SETTINGS_NAME_ISSUEDATE, mIssueDate);

	settings->endGroup();
	settings->sync();
}


void DriverSettings::update(const AbstractSettings& pOther)
{
	const DriverSettings* const other = qobject_cast<const DriverSettings* const>(&pOther);
	if (other != nullptr)
	{
		mIssueDate = other->getIssueDate();
		mDrivers = other->getDrivers();
	}
}


const QDateTime& DriverSettings::getIssueDate() const
{
	return mIssueDate;
}


const QVector<QSharedPointer<const Driver> >& DriverSettings::getDrivers() const
{
	return mDrivers;
}
