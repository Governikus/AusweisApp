/*!
 * ProviderWidget.h
 *
 * \brief The provider page in gui.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QElapsedTimer>
#include <QMap>
#include <QScopedPointer>

#include "generic/ListCheckItemWidget.h"
#include "generic/Page.h"

class QTableWidgetItem;

namespace Ui
{
class ProviderWidget;
}

namespace governikus
{

class ProviderWidget
	: public Page
{
	Q_OBJECT

	public:
		ProviderWidget(QWidget* pParent = nullptr);
		virtual ~ProviderWidget();

	public Q_SLOTS:
		void searchProvider();
		void onProviderChanged();

	private:
		QScopedPointer<Ui::ProviderWidget> mUi;

		virtual void paintEvent(QPaintEvent*) override;
		void fill();
};

} /* namespace governikus */
