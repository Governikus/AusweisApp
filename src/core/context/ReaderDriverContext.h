/*!
 * \brief Context for retrieving and presenting information about
 * reader devices and their drivers.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "DriverSettings.h"

#include "ReaderDetector.h"

#include <QObject>


namespace governikus
{
class DeviceDescriptor;

class ReaderDriverContext
	: public QObject
{
	Q_OBJECT

	public:
		ReaderDriverContext();
		~ReaderDriverContext();

		QVector<QSharedPointer<DeviceDescriptor> > attachedDevices();

	private:
		const QSharedPointer<ReaderDetector> mReaderDetector;

		QSharedPointer<DriverSettings> getDriverSettings() const;

	Q_SIGNALS:
		void fireReaderChangeDetected();
};

} /* namespace governikus */
