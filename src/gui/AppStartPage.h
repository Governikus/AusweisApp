/*!
 * AppStartPage.h
 *
 * \brief Main page widget.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "generic/Page.h"

#include <QLabel>
#include <QScopedPointer>

namespace Ui
{
class AppStartPage;
}

class QAbstractButton;

namespace governikus
{

class TabButton;

class AppStartPage
	: public Page
{
	Q_OBJECT

	public:
		AppStartPage(QWidget* pParent = nullptr);
		virtual ~AppStartPage();

	Q_SIGNALS:
		void selfInfoPageRequested();
		void bookmarksPageRequested();
		void settingsPageRequested();

	private:
		QLabel* mApplicationLogoLabel;
		QScopedPointer<Ui::AppStartPage> mUi;

		void paintEvent(QPaintEvent*);
};

} /* namespace governikus */
