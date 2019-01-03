/*!
 * \brief List item widget for list actions.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCheckBox>
#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class ListCheckItemWidget;
} // namespace Ui

namespace governikus
{

class ListCheckItemWidget
	: public QWidget
{
	Q_OBJECT

	public:
		ListCheckItemWidget(QWidget* pParent, const QPixmap& pPixmap);
		ListCheckItemWidget(QWidget* pParent);
		virtual ~ListCheckItemWidget() override;

		void setHeading(const QString& pHeading);
		void setSubHeading(const QString& pSubHeading);

		QCheckBox* getListItemCheckBox();

	protected:
		void changeEvent(QEvent* pEvent) override;

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

} // namespace governikus
