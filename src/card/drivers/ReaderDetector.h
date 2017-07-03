/*!
 * \brief Interface specifying classes that can detect the attached card reader
 * devices on a specific platform.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "DeviceDescriptor.h"
#include "DriverSettings.h"
#include "ReaderInfo.h"

#ifdef Q_OS_OSX
#include <IOKit/IOKitLib.h>
#endif

#ifdef Q_OS_LINUX
#include <QThread>
class DeviceListener;
#endif

#ifdef Q_OS_WIN
#include <QAbstractNativeEventFilter>
#endif

#include <QSharedPointer>
#include <QVector>

namespace governikus
{
class ReaderDetector
	: public QObject
	#ifdef Q_OS_WIN
	, QAbstractNativeEventFilter
	#endif
{
	Q_OBJECT

	private:
		const QVector<QSharedPointer<DeviceDescriptor> > mSupportedDevices;

	#ifdef Q_OS_OSX
		io_iterator_t mIteratorPublish;

		io_iterator_t mIteratorTerminated;
	#endif

	#ifdef Q_OS_LINUX
		DeviceListener* mDeviceListener;
	#endif

		bool initNativeEvents();

		bool terminateNativeEvents();

	protected:
		static QVector<QSharedPointer<DeviceDescriptor> > convertSettings(const QSharedPointer<DriverSettings>& pSettings);

		QSharedPointer<DeviceDescriptor> lookupDevice(uint pVendorId, uint pProductId) const;

		virtual QVector<QPair<uint, uint> > attachedDevIds() const;

	public:
		ReaderDetector(const QSharedPointer<DriverSettings>& pDriverSettings);

		virtual ~ReaderDetector();

	#ifdef Q_OS_WIN
		bool nativeEventFilter(const QByteArray& pEventType, void* pMessage, long* pResult) override;
	#endif

		/*!
		 * \brief getAttachedDevices produce a list of supported devices that are
		 *        attached to the system
		 */
		QVector<QSharedPointer<DeviceDescriptor> > getAttachedDevices(const QVector<ReaderInfo>& pReaderInfos) const;

	Q_SIGNALS:
		void fireReaderChangeDetected();

};

} /* namespace governikus */
