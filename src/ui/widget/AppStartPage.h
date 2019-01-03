/*!
 * \brief Main page widget.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QLabel>
#include <QScopedPointer>

namespace Ui
{
class AppStartPage;
} // namespace Ui

namespace governikus
{

class AppStartPage
	: public QWidget
{
	Q_OBJECT

	public:
		AppStartPage(QWidget* pParent = nullptr);
		virtual ~AppStartPage() override;

	Q_SIGNALS:
		void selfInfoPageRequested();
		void bookmarksPageRequested();
		void settingsPageRequested();

	private:
		QLabel* mApplicationLogoLabel;
		QScopedPointer<Ui::AppStartPage> mUi;

		virtual void paintEvent(QPaintEvent*) override;

	protected:
		virtual void changeEvent(QEvent* pEvent) override;
};

} // namespace governikus
