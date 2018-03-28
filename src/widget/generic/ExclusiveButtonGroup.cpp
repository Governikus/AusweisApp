/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ExclusiveButtonGroup.h"

#include <QAbstractButton>
#include <QKeyEvent>

using namespace governikus;

ExclusiveButtonGroup::ExclusiveButtonGroup(QObject* pParent)
	: QObject(pParent)
	, mButtons()
{
}


ExclusiveButtonGroup::~ExclusiveButtonGroup()
{
}


void ExclusiveButtonGroup::addButton(QAbstractButton* pButton)
{
	mButtons += pButton;

	pButton->installEventFilter(this);

	connect(pButton, &QAbstractButton::clicked, this, &ExclusiveButtonGroup::onButtonClicked);
	connect(pButton, &QAbstractButton::pressed, this, &ExclusiveButtonGroup::onButtonPressed);
	connect(pButton, &QAbstractButton::released, this, &ExclusiveButtonGroup::onButtonReleased);
	connect(pButton, &QAbstractButton::toggled, this, &ExclusiveButtonGroup::onButtonToggled);
}


void ExclusiveButtonGroup::removeButton(QAbstractButton* pButton)
{
	if (mButtons.removeAll(pButton) == 0)
	{
		return;
	}

	pButton->removeEventFilter(this);

	disconnect(pButton, &QAbstractButton::clicked, this, &ExclusiveButtonGroup::onButtonClicked);
	disconnect(pButton, &QAbstractButton::pressed, this, &ExclusiveButtonGroup::onButtonPressed);
	disconnect(pButton, &QAbstractButton::released, this, &ExclusiveButtonGroup::onButtonReleased);
	disconnect(pButton, &QAbstractButton::toggled, this, &ExclusiveButtonGroup::onButtonToggled);
}


bool ExclusiveButtonGroup::eventFilter(QObject* pWatched, QEvent* pEvent)
{
	if (QAbstractButton* button = qobject_cast<QAbstractButton*>(pWatched))
	{
		if (pEvent->type() == QEvent::MouseButtonPress && button->isChecked())
		{
			return true;
		}

		if (pEvent->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(pEvent);
			if (keyEvent->key() == Qt::Key_Select || keyEvent->key() == Qt::Key_Space)
			{
				return true;
			}
		}
	}

	return false;
}


void ExclusiveButtonGroup::onButtonClicked(bool /*pChecked*/)
{
	if (QAbstractButton* button = qobject_cast<QAbstractButton*>(sender()))
	{
		Q_EMIT buttonClicked(button);
	}
}


void ExclusiveButtonGroup::onButtonPressed()
{
	if (QAbstractButton* button = qobject_cast<QAbstractButton*>(sender()))
	{
		Q_EMIT buttonPressed(button);
	}
}


void ExclusiveButtonGroup::onButtonReleased()
{
	if (QAbstractButton* button = qobject_cast<QAbstractButton*>(sender()))
	{
		Q_EMIT buttonReleased(button);
	}
}


void ExclusiveButtonGroup::onButtonToggled(bool pChecked)
{
	if (QAbstractButton* button = qobject_cast<QAbstractButton*>(sender()))
	{
		if (pChecked)
		{
			for (auto otherButton : qAsConst(mButtons))
			{
				if (otherButton != button && otherButton->isChecked())
				{
					otherButton->setChecked(false);
				}
			}
		}

		Q_EMIT buttonToggled(button, pChecked);
	}
}
