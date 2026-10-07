/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "NativeTextField.h"

class NativeTextFieldPrivate
{
};


NativeTextField::NativeTextField(QQuickItem* pParent)
	: QQuickItem(pParent)
	, d(new NativeTextFieldPrivate())
{
}


NativeTextField::~NativeTextField() = default;


QFont NativeTextField::font() const
{
	return QFont();
}


void NativeTextField::setFont(const QFont& pFont) //NOSONAR
{
	Q_UNUSED(pFont)
}


QColor NativeTextField::textColor() const
{
	return QColor();
}


void NativeTextField::setTextColor(QColor pColor) //NOSONAR
{
	Q_UNUSED(pColor)
}


QColor NativeTextField::backgroundColor() const
{
	return QColor();
}


void NativeTextField::setBackgroundColor(QColor pColor) //NOSONAR
{
	Q_UNUSED(pColor)
}


QColor NativeTextField::borderColor() const
{
	return QColor();
}


void NativeTextField::setBorderColor(QColor pColor) //NOSONAR
{
	Q_UNUSED(pColor)
}


qreal NativeTextField::borderWidth() const
{
	return 0;
}


void NativeTextField::setBorderWidth(qreal pWidth) //NOSONAR
{
	Q_UNUSED(pWidth)
}


qreal NativeTextField::borderRadius() const
{
	return 0;
}


void NativeTextField::setBorderRadius(qreal pRadius) //NOSONAR
{
	Q_UNUSED(pRadius)
}


int NativeTextField::maximumLength() const
{
	return 0;
}


void NativeTextField::setMaximumLength(int pLength) //NOSONAR
{
	Q_UNUSED(pLength)
}


QString NativeTextField::text() const
{
	return QString();
}


void NativeTextField::setText(const QString& pText) //NOSONAR
{
	Q_UNUSED(pText)
}


QWindow* NativeTextField::nativeWindow() const
{
	return nullptr;
}


void NativeTextField::onParentChanged() //NOSONAR
{
}


void NativeTextField::updateEffectiveOpacity() //NOSONAR
{
}
