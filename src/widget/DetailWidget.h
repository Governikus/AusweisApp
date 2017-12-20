/*!
 * \brief Widget for cvc description.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class DetailWidget;
}

namespace governikus
{

class DetailWidget
	: public QWidget
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::DetailWidget> mUi;

		virtual void paintEvent(QPaintEvent*) override;

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		DetailWidget(QWidget* pParent = nullptr);
		virtual ~DetailWidget() override;

		void setDetails(const QString& pDetails);

};

} /* namespace governikus */
