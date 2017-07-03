/*!
 * ListCheckItemWidget.h
 *
 * \brief List item widget for list actions.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QCheckBox>
#include <QScopedPointer>

#include "generic/Page.h"

namespace Ui
{
class ListCheckItemWidget;
}

namespace governikus
{

class ListCheckItemWidget
	: public QWidget
{
	Q_OBJECT

	public:
		ListCheckItemWidget(QWidget* pParent, const QPixmap& pPixmap);
		ListCheckItemWidget(QWidget* pParent);
		virtual ~ListCheckItemWidget();

		void setHeading(const QString& pHeading);
		void setSubHeading(const QString& pSubHeading);

		QCheckBox* getListItemCheckBox();

	Q_SIGNALS:
		void listItemWidgetChecked(ListCheckItemWidget* pListCheckItemWidget);

	private Q_SLOTS:
		void onCheckBoxChanged(int pChanged);

	private:
		QScopedPointer<Ui::ListCheckItemWidget> mUi;

		virtual bool eventFilter(QObject* pWatched, QEvent* pEvent) override;
		virtual void paintEvent(QPaintEvent*) override;

		void itemWidgetReleased();
		void itemWidgetPressed();
};

} /* namespace governikus */
