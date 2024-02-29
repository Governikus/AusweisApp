/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief PlugIn to control different kinds of reader managers that will be used in \ref ReaderManager.
 * If you implement a class of this PlugIn you need to register it in \ref ReaderManager, otherwise it won't be used.
 */

#pragma once

#include "Reader.h"
#include "ReaderInfo.h"
#include "ReaderManagerPlugInInfo.h"

#include <QObject>
#include <QThread>

namespace governikus
{

class ReaderManagerPlugIn
	: public QObject
{
	Q_OBJECT
	friend class MockReaderManagerPlugIn;

	private:
		ReaderManagerPlugInInfo mInfo;

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
			Q_EMIT fireStatusChanged(mInfo);
		}


		void setPlugInValue(ReaderManagerPlugInInfo::Key pKey, const QVariant& pValue)
		{
			mInfo.setValue(pKey, pValue);
		}

	public:
		ReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType,
				bool pAvailable = false,
				bool pPlugInEnabled = false);
		~ReaderManagerPlugIn() override = default;

		[[nodiscard]] const ReaderManagerPlugInInfo& getInfo() const
		{
			return mInfo;
		}


		[[nodiscard]] virtual QList<Reader*> getReaders() const = 0;


		virtual void init()
		{
			Q_ASSERT(QObject::thread() == QThread::currentThread());
		}


		void reset()
		{
			shutdown();
			init();
		}


		virtual void shutdown()
		{
		}


		virtual void insert(const QString& pReaderName, const QVariant& pData)
		{
			Q_UNUSED(pReaderName)
			Q_UNUSED(pData)
		}


		void shelve() const;


		virtual void startScan(bool pAutoConnect);
		virtual void stopScan(const QString& pError = QString());

	Q_SIGNALS:
		void fireStatusChanged(const ReaderManagerPlugInInfo& pInfo);
		void fireReaderAdded(const ReaderInfo& pInfo);
		void fireReaderRemoved(const ReaderInfo& pInfo);
		void fireCardInserted(const ReaderInfo& pInfo);
		void fireCardRemoved(const ReaderInfo& pInfo);
		void fireCardInfoChanged(const ReaderInfo& pInfo);
		void fireReaderPropertiesUpdated(const ReaderInfo& pInfo);
};

} // namespace governikus

Q_DECLARE_INTERFACE(governikus::ReaderManagerPlugIn, "governikus.ReaderManagerPlugIn")
