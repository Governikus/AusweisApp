/*!
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderManagerPlugInInfo.h"

#include <QQuickItem>

class test_ReaderScanEnabler;

namespace governikus
{

class ReaderScanEnabler
	: public QQuickItem
{
	Q_OBJECT
	friend class ::test_ReaderScanEnabler;

	Q_PROPERTY(ReaderManagerPlugInType plugInType READ getPlugInType WRITE setPlugInType NOTIFY firePlugInTypeChanged)
	Q_PROPERTY(bool scanRunning READ isScanRunning NOTIFY fireScanRunningChanged)

	private:
		ReaderManagerPlugInType mPlugInType;
		bool mObligedToStopScan;

		void enableScan(const bool pEnable);
		void enableScanIfVisible();
		bool isScanRunning() const;

	private Q_SLOTS:
		void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);

	public:
		explicit ReaderScanEnabler(QQuickItem* pParent = nullptr);
		~ReaderScanEnabler() override;

		ReaderManagerPlugInType getPlugInType() const;
		void setPlugInType(ReaderManagerPlugInType pPlugInType);
		void itemChange(QQuickItem::ItemChange pChange, const QQuickItem::ItemChangeData& pValue) override;
		Q_INVOKABLE void restartScan();

	Q_SIGNALS:
		void firePlugInTypeChanged();
		void fireScanRunningChanged();
};


} // namespace governikus
