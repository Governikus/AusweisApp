/*!
 * PasswordEdit.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "generic/PasswordEdit.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QToolButton>
#include <QToolTip>

#include "KeylessPasswordEdit.h"
#include "PasswordLineEdit.h"

using namespace governikus;

PasswordEdit::PasswordEdit(QWidget* pParent)
	: QWidget(pParent)
	, mKeylessEdit(new KeylessPasswordEdit(this))
	, mLineEdit(new PasswordLineEdit(this))
	, mStackedLayout(new QStackedLayout)
{
	QHBoxLayout* myLayout = new QHBoxLayout(this);
	myLayout->setMargin(0);
	myLayout->addLayout(mStackedLayout);
	mStackedLayout->setMargin(0);
	mStackedLayout->addWidget(createWrapperWidget(mLineEdit));
	mStackedLayout->addWidget(createWrapperWidget(mKeylessEdit));

	connect(mLineEdit, &PasswordLineEdit::textEdited, this, &PasswordEdit::textEdited);
	connect(mLineEdit, &PasswordLineEdit::selectionChanged, this, &PasswordEdit::selectionChanged);

	connect(mKeylessEdit, &KeylessPasswordEdit::textEdited, this, &PasswordEdit::textEdited);

	setKeylessPasswordMode(false);
}


PasswordEdit::~PasswordEdit()
{
}


void PasswordEdit::setMaxLength(int pLength)
{
	mKeylessEdit->setMaxLength(pLength);
	mLineEdit->setMaxLength(pLength);
}


void PasswordEdit::setFieldWidth(int pWidth)
{
	mLineEdit->setFieldWidth(pWidth);
}


void PasswordEdit::configureValidation(const QRegularExpression& pExpression, const QString& pInvalidValueToolTip)
{
	mLineEdit->configureValidation(pExpression, pInvalidValueToolTip);
}


void PasswordEdit::clear()
{
	mLineEdit->clear();
	mKeylessEdit->clear();
}


QString PasswordEdit::text() const
{
	return isLineEditActive() ? mLineEdit->text() : mKeylessEdit->text();
}


void PasswordEdit::setCursorPosition(int pIndex)
{
	mLineEdit->setCursorPosition(pIndex);
}


bool PasswordEdit::isLineEditActive() const
{
	return mStackedLayout->currentIndex() == 0;
}


void PasswordEdit::setKeylessPasswordMode(bool pKeylessPassword)
{
	if (pKeylessPassword)
	{
		setFocusProxy(mKeylessEdit);
		mStackedLayout->setCurrentIndex(1);
	}
	else
	{
		setFocusProxy(mLineEdit);
		mStackedLayout->setCurrentIndex(0);
	}
}


void PasswordEdit::setDigitFieldInvalid(bool pMakeInvalid, const QString& pInvalidMessage)
{
	if (pMakeInvalid)
	{
		if (isLineEditActive())
		{
			mLineEdit->setStyleSheet(QStringLiteral("background-color: red;"));
			QToolTip::showText(mapToGlobal(QPoint(0, 20)), pInvalidMessage, mLineEdit, QRect(), 3000);
		}
		else
		{
			mKeylessEdit->setDigitFieldsInvalid(true);
		}
	}
	else
	{
		isLineEditActive() ? mLineEdit->setStyleSheet(QString()) : mKeylessEdit->setDigitFieldsInvalid(false);
	}
}


PasswordLineEdit* PasswordEdit::getLineEdit()
{
	return mLineEdit;
}


QWidget* PasswordEdit::createWrapperWidget(QWidget* pWidget)
{
	QWidget* wrapperWidget = new QWidget;
	QHBoxLayout* wrapperLayout = new QHBoxLayout(wrapperWidget);
	wrapperLayout->setMargin(0);
	wrapperLayout->addWidget(pWidget);
	return wrapperWidget;
}
