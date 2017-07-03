/*!
 * TabButtonGroup.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "TabButtonGroup.h"

#include <QAccessibleInterface>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QWidget>

using namespace governikus;

static QAccessibleInterface* createTabButtonInterface(const QString& pKey, QObject* pObject)
{
	if (pObject->isWidgetType() && pKey == QLatin1String("governikus::TabButton"))
	{
		return new AccessibleTabButton(static_cast<QWidget*>(pObject));
	}

	if (pObject->isWidgetType() && pKey == QLatin1String("governikus::TabButtonGroup"))
	{
		return new AccessibleTabButtonGroup(static_cast<QWidget*>(pObject));
	}

	return nullptr;
}


static void registerInterfaceFactory()
{
	static bool registered = (QAccessible::installFactory(&createTabButtonInterface), true);
	(void) registered;
}


TabButton::TabButton(QWidget* pParent)
	: QToolButton(pParent)
{
	registerInterfaceFactory();
}


TabButton::~TabButton()
{
}


void TabButton::focusInEvent(QFocusEvent* /*pEvent*/)
{
	setChecked(true);
}


void TabButton::nextCheckState()
{
	setChecked(true);
}


TabButtonGroup::TabButtonGroup(QWidget* pParent)
	: QWidget(pParent)
	, mButtonGroup(new ExclusiveButtonGroup(this))
	, mWorkflowActive(false)
{
	registerInterfaceFactory();

	setFocusPolicy(Qt::TabFocus);

	connect(mButtonGroup, &ExclusiveButtonGroup::buttonToggled, this, &TabButtonGroup::onButtonToggled);
}


TabButtonGroup::~TabButtonGroup()
{
}


void TabButtonGroup::addButton(QAbstractButton* pButton)
{
	mButtonGroup->addButton(pButton);

	updateFocusPolicies();

	if (pButton->parent() != this)
	{
		pButton->setParent(this);
	}

	pButton->installEventFilter(this);
}


void TabButtonGroup::setWorkflowActive(bool pWorkflowActiv)
{
	mWorkflowActive = pWorkflowActiv;
}


bool TabButtonGroup::eventFilter(QObject* pWatched, QEvent* pEvent)
{
	if (pEvent->type() != QEvent::KeyPress)
	{
		return false;
	}

	QAbstractButton* button = dynamic_cast<QAbstractButton*>(pWatched);
	if (button == nullptr)
	{
		return false;
	}

	// Handle cursor key navigation manually. It doesn't work, since only one
	// button is focussable at all.
	bool navigate = false;
	bool cycle = false;
	bool next = true;

	QKeyEvent* keyEvent = static_cast<QKeyEvent*>(pEvent);
	switch (keyEvent->key())
	{
		case Qt::Key_Up:
			next = false;

		// fall through
		case Qt::Key_Left:
		case Qt::Key_Right:
		case Qt::Key_Down:
		{
			bool reverse = layoutDirection() == Qt::RightToLeft;
			if ((keyEvent->key() == Qt::Key_Left && !reverse)
					|| (keyEvent->key() == Qt::Key_Right && reverse))
			{
				next = false;
			}

			navigate = true;
			break;
		}

		case Qt::Key_Tab:
		case Qt::Key_Backtab:
		{
			Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
			if ((modifiers& Qt::CTRL) != 0 && (modifiers & ~(Qt::CTRL | Qt::SHIFT)) == 0)
			{
				navigate = true;
				cycle = true;
				next = keyEvent->key() == Qt::Key_Tab;
			}
			break;
		}
	}

	if (!navigate)
	{
		return false;
	}

	pEvent->accept();

	if (QAbstractButton* nextButton = getNextPrevFocussableButton(button, next, cycle))
	{
		if (!mWorkflowActive)
		{
			nextButton->setChecked(true);
		}
	}

	return true;
}


void TabButtonGroup::onButtonToggled(QAbstractButton* pButton, bool pChecked)
{
	updateFocusPolicies();

	if (pButton->isChecked())
	{
		pButton->setFocus();
	}

	Q_EMIT buttonToggled(pButton, pChecked);
}


QAbstractButton* TabButtonGroup::getNextPrevFocussableButton(QAbstractButton* pCurrentButton, bool pNext, bool pCycle) const
{
	const QVector<QAbstractButton*>& buttons = mButtonGroup->getButtons();
	int count = buttons.count();
	if (count == 0)
	{
		return nullptr;
	}

	int index;
	if (pCurrentButton != nullptr)
	{
		index = buttons.indexOf(pCurrentButton);
	}
	else
	{
		index = pNext ? count - 1 : 0;
	}

	if (index < 0)
	{
		return nullptr;
	}

	int direction = pNext ? 1 : -1;
	for (;; )
	{
		index += direction;
		if (index < 0 || index >= count)
		{
			if (!pCycle)
			{
				return nullptr;
			}

			pCycle = false;
			index = (index + count) % count;
		}

		QAbstractButton* button = buttons.at(index);
		if (button->isVisibleTo(this))
		{
			return button;
		}
	}
}


void TabButtonGroup::updateFocusPolicies()
{
	bool anyButtonFocussable = false;
	const QVector<QAbstractButton*>& buttons = mButtonGroup->getButtons();
	for (QAbstractButton* button : buttons)
	{
		bool buttonFocussable = button->isChecked() && button->isVisibleTo(button->parentWidget());
		anyButtonFocussable |= buttonFocussable;
		button->setFocusPolicy(buttonFocussable ? Qt::TabFocus : Qt::NoFocus);
	}

	if (anyButtonFocussable)
	{
		setFocusPolicy(Qt::NoFocus);
	}
	else
	{
		setFocusPolicy(Qt::TabFocus);
		for (QAbstractButton* button : buttons)
		{
			if (button->isVisibleTo(button->parentWidget()))
			{
				button->setFocusPolicy(Qt::TabFocus);
				break;
			}
		}
	}
}


void TabButtonGroup::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


AccessibleTabButton::AccessibleTabButton(QWidget* pWidget)
	: QAccessibleWidget(pWidget, QAccessible::PageTab)
{
}


TabButton* AccessibleTabButton::getTabButton() const
{
	return static_cast<TabButton*>(widget());
}


QString AccessibleTabButton::text(QAccessible::Text /*pText*/) const
{
	QString result = getTabButton()->text();
	return result.replace(QLatin1Char('&'), QString());
}


QStringList AccessibleTabButton::actionNames() const
{
	return QStringList(pressAction());
}


void AccessibleTabButton::doAction(const QString& pActionName)
{
	if (pActionName == pressAction())
	{
		getTabButton()->setChecked(true);
	}
}


AccessibleTabButtonGroup::AccessibleTabButtonGroup(QWidget* pWidget)
	: QAccessibleWidget(pWidget, QAccessible::PageTabList)
{
}


AccessibleTabButtonGroup::~AccessibleTabButtonGroup()
{
}


TabButtonGroup* AccessibleTabButtonGroup::getTabButtonGroup() const
{
	return static_cast<TabButtonGroup*>(widget());
}
