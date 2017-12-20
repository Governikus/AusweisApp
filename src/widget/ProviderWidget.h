/*!
 * \brief The provider page in gui.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QElapsedTimer>
#include <QMap>
#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class ProviderWidget;
}

namespace governikus
{

class ProviderWidget
	: public QWidget
{
	Q_OBJECT

	public:
		ProviderWidget(QWidget* pParent = nullptr);
		virtual ~ProviderWidget() override;

	public Q_SLOTS:
		void searchProvider();
		void onProviderChanged();

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::ProviderWidget> mUi;

		virtual void paintEvent(QPaintEvent*) override;
		void fill();
};

} /* namespace governikus */
