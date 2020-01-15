/*!
 * \brief Widget for starting the self information workflow.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class SelfInformationWidget;
} // namespace Ui

namespace governikus
{

class SelfInformationWidget
	: public QWidget
{
	Q_OBJECT

	public:
		SelfInformationWidget(QWidget* pParent = nullptr);
		virtual ~SelfInformationWidget() override;

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	Q_SIGNALS:
		void selfAuthenticationRequested();

	private:
		QScopedPointer<Ui::SelfInformationWidget> mUi;
		QScopedPointer<QPixmap> mPixDescLogoLabel;

		virtual void paintEvent(QPaintEvent*) override;

	private Q_SLOTS:
		void onLanguageChanged();
		void onDeveloperOptionsChanged();
};

} // namespace governikus
