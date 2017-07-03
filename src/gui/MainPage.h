/*!
 * MainPage.h
 *
 * \brief Main page widget.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "generic/Page.h"

class QAbstractButton;

namespace governikus
{

class TabButton;

class MainPage
	: public Page
{
	Q_OBJECT

	public:
		MainPage(QWidget* pParent = nullptr);
		virtual ~MainPage();

		void setNavigationEnabled(bool pEnabled);

		QAbstractButton* getSelfInfoButton() const;
		QAbstractButton* getBookmarksButton() const;
		QAbstractButton* getHistoryButton() const;
		QAbstractButton* getSettingsButton() const;

	private:
		TabButton* mSelfInfoButton;
		TabButton* mBookmarksButton;
		TabButton* mHistoryButton;
		TabButton* mSettingsButton;

		void paintEvent(QPaintEvent*);
};

} /* namespace governikus */
