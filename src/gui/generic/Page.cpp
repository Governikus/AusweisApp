/*!
 * Page.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "Page.h"

#include <QPainter>
#include <QStyleOption>

using namespace governikus;

Page::Page(QWidget* pParent)
	: Page(QStringLiteral("untitled page"), pParent)
{
}


Page::Page(const QString& pTitle, QWidget* pParent)
	: QWidget(pParent)
	, mTitle(pTitle)
	, mIsSideNavigationPage(false)
	, mBackNavigationEnabled(true)
{
}


Page::~Page()
{
}


void Page::setBackNavigationEnabled(bool pEnabled)
{
	if (pEnabled != mBackNavigationEnabled)
	{
		mBackNavigationEnabled = pEnabled;
		Q_EMIT backNavigationEnabledChanged(mBackNavigationEnabled);
	}
}


void Page::pageAboutToBeShown()
{
	// can be overridden by derived classes
}


void Page::pageShowFinished()
{
	// can be overridden by derived classes
}


void Page::pageAboutToBeHidden()
{
	// can be overridden by derived classes
}


void Page::pageHideFinished()
{
	// can be overridden by derived classes
}


QVector<QAbstractButton*> Page::createLeftNavigationButtons()
{
	// can be overridden by derived classes
	return QVector<QAbstractButton*>();
}


QVector<QAbstractButton*> Page::createRightNavigationButtons()
{
	// can be overridden by derived classes
	return QVector<QAbstractButton*>();
}


void Page::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
