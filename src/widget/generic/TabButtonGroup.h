/*!
 * \brief Contains the accessibility friendly TabButtonGroup and TabButton classes.
 *
 * The other classes defined in this header are implementation private.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAccessibleWidget>
#include <QToolButton>

namespace governikus
{

class ExclusiveButtonGroup;


/*!
 * \brief A push button that is presented to accessibility clients as a page tab.
 */
class TabButton
	: public QToolButton
{
	Q_OBJECT

	public:
		TabButton(QWidget* pParent = nullptr);
		virtual ~TabButton() override;

	protected:
		virtual void focusInEvent(QFocusEvent* pEvent) override;
		virtual void nextCheckState() override;
};

/*!
 * \brief A widget that is presented to accessibility clients as a page tab list,
 * but uses TabButtons as tabs.
 *
 * The class is a regular widget. Buttons (that must be checkable) added via
 * addButton() are added to an exclusive button group. Only one button can be
 * checked at a time. The focus handling and cursor key navigation is overridden
 * so that it works like in the tab row of a regular QTabWidget.
 */
class TabButtonGroup
	: public QWidget
{
	Q_OBJECT

	public:
		TabButtonGroup(QWidget* pParent = nullptr);
		virtual ~TabButtonGroup() override;

		void addButton(QAbstractButton* pButton);

		void setWorkflowActive(bool pWorkflowActiv);

		virtual bool eventFilter(QObject* pWatched, QEvent* pEvent) override;

	private Q_SLOTS:
		void onButtonToggled(QAbstractButton* pButton, bool pChecked);

	private:
		QAbstractButton* getNextPrevFocussableButton(QAbstractButton* pCurrentButton, bool pNext, bool pCycle) const;
		void updateFocusPolicies();

	private:
		ExclusiveButtonGroup* mButtonGroup;
		bool mWorkflowActive;
		void paintEvent(QPaintEvent*) override;

	Q_SIGNALS:
		void buttonToggled(QAbstractButton* pButton, bool pChecked);
};

/*!
 * \brief Implementation private class providing the accessibility functionality
 * for TabButton.
 */
class AccessibleTabButton
	: public QAccessibleWidget
{
	public:
		AccessibleTabButton(QWidget* pWidget);

		TabButton* getTabButton() const;

		virtual QString text(QAccessible::Text pText) const override;

		virtual QStringList actionNames() const override;
		virtual void doAction(const QString& pActionName) override;

};

/*!
 * \brief Implementation private class providing the accessibility functionality
 * for TabButtonGroup.
 */
class AccessibleTabButtonGroup
	: public QAccessibleWidget
{
	public:
		AccessibleTabButtonGroup(QWidget* pWidget);
		virtual ~AccessibleTabButtonGroup();

		TabButtonGroup* getTabButtonGroup() const;
};

} /* namespace governikus */
