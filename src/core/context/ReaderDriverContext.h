/*!
 * \brief Context for retrieving and presenting information about
 * reader devices and their drivers.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "DriverSettings.h"

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	#include "DeviceDescriptor.h"
	#include "ReaderDetector.h"
#endif

#include <QObject>


namespace governikus
{

class ReaderDriverContext
	: public QObject
{
	Q_OBJECT

	public:
		ReaderDriverContext();
		virtual ~ReaderDriverContext();

	#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
		QVector<QSharedPointer<DeviceDescriptor> > attachedDevices();
	#endif

	private:
	#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
		const QSharedPointer<ReaderDetector> mReaderDetector;
	#endif

		QSharedPointer<DriverSettings> getDriverSettings() const;

	Q_SIGNALS:
		void fireReaderChangeDetected();
};

} /* namespace governikus */
