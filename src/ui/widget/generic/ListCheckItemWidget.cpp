/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ListCheckItemWidget.h"
#include "ui_ListCheckItemWidget.h"

#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QStyleOption>

using namespace governikus;


ListCheckItemWidget::ListCheckItemWidget(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::ListCheckItemWidget())
{
	mUi->setupUi(this);
	mUi->listIcon->hide();

	connect(mUi->listCheckBox, &QCheckBox::stateChanged, this, &ListCheckItemWidget::onCheckBoxChanged);

	installEventFilter(this);
}


ListCheckItemWidget::ListCheckItemWidget(QWidget* pParent, const QPixmap& pPixmap)
	: QWidget(pParent)
	, mUi(new Ui::ListCheckItemWidget())
{
	mUi->setupUi(this);
	mUi->listIcon->setScaledContents(true);
	mUi->listIcon->setPixmap(pPixmap);

	connect(mUi->listCheckBox, &QCheckBox::stateChanged, this, &ListCheckItemWidget::onCheckBoxChanged);

	installEventFilter(this);
}


ListCheckItemWidget::~ListCheckItemWidget()
{
}


void ListCheckItemWidget::itemWidgetReleased()
{
	mUi->listItemLayout->setObjectName(QStringLiteral("listItemLayout"));
	//qApp->setStyleSheet(qApp->styleSheet());
}


void ListCheckItemWidget::onCheckBoxChanged(int /*pChanged*/)
{
	Q_EMIT listItemWidgetChecked(this);
}


void ListCheckItemWidget::itemWidgetPressed()
{
	mUi->listItemLayout->setObjectName(QStringLiteral("listItemLayout_pressed"));
	//qApp->setStyleSheet(qApp->styleSheet());

	Q_EMIT listItemWidgetChecked(this);
}


bool ListCheckItemWidget::eventFilter(QObject* /*pWatched*/, QEvent* pEvent)
{
	switch (pEvent->type())
	{
		case QEvent::MouseButtonPress:
			itemWidgetPressed();
			return false;

		case QEvent::MouseButtonRelease:
			itemWidgetReleased();
			return false;

		default:
			return false;
	}
}


void ListCheckItemWidget::setHeading(const QString& pHeading)
{
	mUi->heading->setText(pHeading);
}


void ListCheckItemWidget::setSubHeading(const QString& pSubHeading)
{
	if (!pSubHeading.isNull() || !pSubHeading.isEmpty())
	{
		mUi->subHeading->setText(pSubHeading);
	}
	else
	{
		mUi->headingFrameLayout->layout()->removeWidget(mUi->subHeading);
		delete mUi->subHeading;
	}
}


void ListCheckItemWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


QCheckBox* ListCheckItemWidget::getListItemCheckBox()
{
	return mUi->listCheckBox;
}


void ListCheckItemWidget::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}

	QWidget::changeEvent(pEvent);
}
