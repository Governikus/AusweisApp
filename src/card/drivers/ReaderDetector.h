/*!
 * \brief Interface specifying classes that can detect the attached card reader
 * devices on a specific platform.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "ReaderConfiguration.h"
#include "UsbId.h"

#ifdef Q_OS_LINUX
#include "DeviceListener.h"
#endif

#ifdef Q_OS_MACOS
#include <IOKit/IOKitLib.h>
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
	friend class Env;

	private:
	#ifdef Q_OS_MACOS
		io_iterator_t mIteratorPublish;

		io_iterator_t mIteratorTerminated;
	#endif

	#ifdef Q_OS_LINUX
		DeviceListener* mDeviceListener;
	#endif

		bool initNativeEvents();
		bool terminateNativeEvents();

	protected:
		ReaderDetector();
		~ReaderDetector() override;

	public:
		virtual QVector<UsbId> attachedDevIds() const;

	#ifdef Q_OS_WIN
		bool nativeEventFilter(const QByteArray& pEventType, void* pMessage, long* pResult) override;
	#endif

		/*!
		 * \brief getAttachedDevices produce a list of supported devices that are
		 *        attached to the system
		 */
		QVector<ReaderConfigurationInfo> getAttachedSupportedDevices() const;

		ReaderConfigurationInfo getReaderConfigurationInfo(const QString& pReaderName) const;

	Q_SIGNALS:
		void fireReaderChangeDetected();
};

} // namespace governikus
