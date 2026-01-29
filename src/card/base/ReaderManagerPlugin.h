/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Reader.h"
#include "ReaderInfo.h"
#include "ReaderManagerPluginInfo.h"

#include <QObject>
#include <QPointer>
#include <QThread>


namespace governikus
{

class ReaderManagerPlugin
	: public QObject
{
	Q_OBJECT
	friend class MockReaderManagerPlugin;

	private:
		ReaderManagerPluginInfo mInfo;

	protected:
		void setPluginEnabled(bool pEnabled);
		void setPluginAvailable(bool pAvailable);
		void setPluginValue(ReaderManagerPluginInfo::Key pKey, const QVariant& pValue);
		static void shelve(const QPointer<Reader>& pReader);

	public:
		explicit ReaderManagerPlugin(ReaderManagerPluginType pPluginType,
				bool pAvailable = false,
				bool pPluginEnabled = false);
		~ReaderManagerPlugin() override = default;

		[[nodiscard]] const ReaderManagerPluginInfo& getInfo() const;
		[[nodiscard]] virtual QPointer<Reader> getReader(const QString& pReaderName) const = 0;

		virtual void init();
		virtual void shutdown();
		void reset();

		virtual void startScan(bool pAutoConnect);
		virtual void stopScan(const QString& pError = QString());
		void setInitialScanState(ReaderManagerPluginInfo::InitialScan pState);

		virtual void insert(const QString& pReaderName, const QVariant& pData);
		virtual void shelveAll() const = 0;

#ifndef QT_NO_DEBUG
		void setPluginInfo(const ReaderManagerPluginInfo& pInfo);
#endif

	Q_SIGNALS:
		void fireStatusChanged(const ReaderManagerPluginInfo& pInfo);
		void fireReaderAdded(const ReaderInfo& pInfo);
		void fireReaderRemoved(const ReaderInfo& pInfo);
		void fireCardInserted(const ReaderInfo& pInfo);
		void fireCardRemoved(const ReaderInfo& pInfo);
		void fireCardInfoChanged(const ReaderInfo& pInfo);
		void fireReaderPropertiesUpdated(const ReaderInfo& pInfo);
};

} // namespace governikus

Q_DECLARE_INTERFACE(governikus::ReaderManagerPlugin, "governikus.ReaderManagerPlugin")
