/*!
 * \brief Information about reader drivers containing general information such
 *        as issue date, and detailed information (vendor id, product id, driver
 *        URL, etc) about each known card reader device.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "DriverSettings.h"
#include "SmartCardDefinitions.h"

#include <QDateTime>
#include <QSharedPointer>
#include <QVector>

namespace governikus
{
/*!
 * \brief Class that stores detailed information (vendor id, product id, driver
 *        URL, etc) about a single card reader device.
 */
class DeviceDescriptor
{
	private:
		const QSharedPointer<const Driver> mDriver;

		const ReaderType mReaderType;

		bool mHasDriver;

		Q_DISABLE_COPY(DeviceDescriptor)

	public:
		DeviceDescriptor(const QSharedPointer<const Driver>& pDriver);

		~DeviceDescriptor();

		void checkDriver(const QVector<ReaderType>& pReaderTypes);

		ReaderType getReaderType() const;

		uint getVendorId() const;

		uint getProductId() const;

		QString getName() const;

		QString getDriverUrl() const;

		bool hasDriver() const;
};


} /* namespace governikus */
