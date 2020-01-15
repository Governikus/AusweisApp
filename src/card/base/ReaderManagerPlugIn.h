/*!
 * \brief PlugIn to control different kinds of reader managers that will be used in \ref ReaderManager.
 * If you implement a class of this PlugIn you need to register it in \ref ReaderManager, otherwise it won't be used.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"
#include "ReaderManagerPlugInInfo.h"

#include <QObject>
#include <QThread>

namespace governikus
{

class Reader;

class ReaderManagerPlugIn
	: public QObject
{
	Q_OBJECT
	ReaderManagerPlugInInfo mInfo;

	private:
		bool mScanRunning;

	protected:
		void setPlugInEnabled(bool pEnabled)
		{
			if (mInfo.isEnabled() != pEnabled)
			{
				mInfo.setEnabled(pEnabled);
				Q_EMIT fireStatusChanged(mInfo);
			}
		}


		void setPlugInAvailable(bool pAvailable)
		{
			mInfo.setAvailable(pAvailable);
		}


		void setPlugInResponding(bool pResponding)
		{
			if (mInfo.isResponding() != pResponding)
			{
				mInfo.setResponding(pResponding);
				Q_EMIT fireStatusChanged(mInfo);
			}
		}


		void setPlugInValue(ReaderManagerPlugInInfo::Key pKey, const QVariant& pValue)
		{
			mInfo.setValue(pKey, pValue);
		}


	public:
		ReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType,
				bool pAvailable = false,
				bool pPlugInEnabled = false);
		virtual ~ReaderManagerPlugIn() = default;

		const ReaderManagerPlugInInfo& getInfo() const
		{
			return mInfo;
		}


		bool isScanRunning() const
		{
			return mScanRunning;
		}


		virtual QList<Reader*> getReaders() const = 0;


		virtual void init()
		{
			Q_ASSERT(QObject::thread() == QThread::currentThread());
		}


		virtual void shutdown()
		{
		}


		virtual void startScan(bool pAutoConnect);
		virtual void stopScan(const QString& pError = QString());

	Q_SIGNALS:
		void fireStatusChanged(const ReaderManagerPlugInInfo& pInfo);
		void fireReaderAdded(const QString& pReaderName);
		void fireReaderRemoved(const QString& pReaderName);
		void fireReaderDeviceError(GlobalStatus::Code pError);
		void fireCardInserted(const QString& pReaderName);
		void fireCardRemoved(const QString& pReaderName);
		void fireCardRetryCounterChanged(const QString& pReaderName);
		void fireReaderPropertiesUpdated(const QString& pReaderName);
};

} // namespace governikus

Q_DECLARE_INTERFACE(governikus::ReaderManagerPlugIn, "governikus.ReaderManagerPlugIn")
