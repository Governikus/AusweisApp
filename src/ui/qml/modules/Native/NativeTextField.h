/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#pragma once

#include <QFont>
#include <QQuickItem>
#include <QQuickWindow>

class NativeTextFieldPrivate;

class NativeTextField
	: public QQuickItem
{
	Q_OBJECT
	QML_ELEMENT

	Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
	Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
	Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
	Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
	Q_PROPERTY(qreal borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
	Q_PROPERTY(qreal borderRadius READ borderRadius WRITE setBorderRadius NOTIFY borderRadiusChanged)
	Q_PROPERTY(int maximumLength READ maximumLength WRITE setMaximumLength NOTIFY maximumLengthChanged)
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(QWindow * nativeWindow READ nativeWindow NOTIFY nativeWindowChanged)

	private:
		QScopedPointer<NativeTextFieldPrivate> const d;

	public:
		explicit NativeTextField(QQuickItem* pParent = nullptr);
		~NativeTextField() override;

		[[nodiscard]] QFont font() const;
		void setFont(const QFont& pFont);

		[[nodiscard]] QColor textColor() const;
		void setTextColor(QColor pColor);

		[[nodiscard]] QColor backgroundColor() const;
		void setBackgroundColor(QColor pColor);

		[[nodiscard]] QColor borderColor() const;
		void setBorderColor(QColor pColor);

		[[nodiscard]] qreal borderWidth() const;
		void setBorderWidth(qreal pWidth);

		[[nodiscard]] qreal borderRadius() const;
		void setBorderRadius(qreal pRadius);

		[[nodiscard]] int maximumLength() const;
		void setMaximumLength(int pLength);

		[[nodiscard]] QString text() const;
		void setText(const QString& pText);

		[[nodiscard]] QWindow* nativeWindow() const;

		void sinkFocus() const;

	Q_SIGNALS:
		void accepted();
		void fontChanged();
		void textChanged();
		void textEdited();
		void textColorChanged();
		void backgroundColorChanged();
		void borderColorChanged();
		void borderRadiusChanged();
		void borderWidthChanged();
		void nativeWindowChanged();
		void maximumLengthChanged();

	public Q_SLOTS:
		void onParentChanged();
		void updateEffectiveOpacity();

};
