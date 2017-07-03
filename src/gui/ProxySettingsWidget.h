/*!
 * \brief Widget for the proxy settings.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "ProxySettings.h"
#include "generic/Page.h"

#include <QAbstractButton>
#include <QScopedPointer>

namespace Ui
{
class ProxySettingsWidget;
}

namespace governikus
{

class ProxySettingsWidget
	: public Page
{
	Q_OBJECT

	public:
		ProxySettingsWidget(QWidget* pParent = nullptr);
		virtual ~ProxySettingsWidget();

		//QNetworkProxy::ProxyType getProxyType() const;
		//void setProxyType(QNetworkProxy::ProxyType pType);

	Q_SIGNALS:
		void settingsChanged();

	private Q_SLOTS:
		void onProxyTypeChanged(bool pChecked);
		void onProxySettingsChanged();

	private:
		QScopedPointer<Ui::ProxySettingsWidget> mUi;

		virtual void paintEvent(QPaintEvent*) override;
};

} /* namespace governikus */
