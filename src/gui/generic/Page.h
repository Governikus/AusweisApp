/*!
 * Page.h
 *
 * \brief A page in a mobile GUI.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QWidget>

class QAbstractButton;

namespace governikus
{

class Page
	: public QWidget
{
	Q_OBJECT

	public:
		Page(QWidget* pParent = nullptr);
		Page(const QString& pTitle, QWidget* pParent = nullptr);
		virtual ~Page();

		const QString& getTitle() const
		{
			return mTitle;
		}


		void setTitle(const QString& pTitle)
		{
			mTitle = pTitle;
		}


		bool isSideNavigationPage() const
		{
			return mIsSideNavigationPage;
		}


		void setSideNavigationPage(bool pIsSideNavigationPage)
		{
			mIsSideNavigationPage = pIsSideNavigationPage;
		}


		bool isBackNavigationEnabled() const
		{
			return mBackNavigationEnabled;
		}


		void setBackNavigationEnabled(bool pEnabled);

		virtual void pageAboutToBeShown();
		virtual void pageShowFinished();

		virtual void pageAboutToBeHidden();
		virtual void pageHideFinished();

		/*!
		 * \brief Create buttons shown in the top navigation bar left to the page title. The method is invoked
		 * right before the page is shown. The buttons will be destroyed automatically right after the page
		 * has been hidden.
		 * \return The list of buttons to be shown. If empty (the default) the default back button will be
		 * shown, if there is a parent page.
		 */
		virtual QVector<QAbstractButton*> createLeftNavigationButtons();

		/*!
		 * \brief Create buttons shown in the top navigation bar right to the page title. The method is
		 * invoked right before the page is shown. The buttons will be destroyed automatically right after the
		 * page has been hidden.
		 * \return The list of buttons to be shown.
		 */
		virtual QVector<QAbstractButton*> createRightNavigationButtons();

	Q_SIGNALS:
		void activatePageRequested();
		void leftNavigationButtonsChanged();
		void rightNavigationButtonsChanged();
		void backNavigationEnabledChanged(bool pEnabled);

	protected:
		void paintEvent(QPaintEvent*);

	private:
		QString mTitle;
		bool mIsSideNavigationPage;
		bool mBackNavigationEnabled;
};

} /* namespace governikus */
