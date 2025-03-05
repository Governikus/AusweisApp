/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderManagerPluginInfo.h"

#include <QQuickItem>
#include <QtQml/qqmlregistration.h>

class test_ReaderScanEnabler;

namespace governikus
{

class ReaderScanEnabler
	: public QQuickItem
{
	Q_OBJECT
	QML_ELEMENT

	friend class ::test_ReaderScanEnabler;

	Q_PROPERTY(governikus::EnumReaderManagerPluginType::ReaderManagerPluginType pluginType READ getPluginType WRITE setPluginType NOTIFY firePluginTypeChanged)
	Q_PROPERTY(bool scanRunning READ isScanRunning NOTIFY fireScanRunningChanged)

	private:
		ReaderManagerPluginType mPluginType;
		bool mObligedToStopScan;

		void enableScan(const bool pEnable);
		void enableScanIfVisible();
		[[nodiscard]] bool isScanRunning() const;

	private Q_SLOTS:
		void onStatusChanged(const ReaderManagerPluginInfo& pInfo);

	public:
		explicit ReaderScanEnabler(QQuickItem* pParent = nullptr);
		~ReaderScanEnabler() override;

		[[nodiscard]] ReaderManagerPluginType getPluginType() const;
		void setPluginType(ReaderManagerPluginType pPluginType);
		void itemChange(QQuickItem::ItemChange pChange, const QQuickItem::ItemChangeData& pValue) override;
		Q_INVOKABLE void restartScan();

	Q_SIGNALS:
		void firePluginTypeChanged();
		void fireScanRunningChanged();
};


} // namespace governikus
