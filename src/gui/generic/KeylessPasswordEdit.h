/*!
 * KeylessPasswordEdit.h
 *
 * \brief Widget for entering a PIN/CAN/PUK via pointer device only.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QWidget>

class QGridLayout;

namespace governikus
{

class KeylessPasswordEdit
	: public QWidget
{
	Q_OBJECT

	private:
		class RenderingHelper;
		class DigitButton;
		class DigitField;

	public:
		KeylessPasswordEdit(QWidget* pParent = nullptr);
		virtual ~KeylessPasswordEdit();

		int getMinLength() const
		{
			return mMinLength;
		}


		void setMinLength(int pLength);

		int getMaxLength() const
		{
			return mMaxLength;
		}


		void setMaxLength(int pLength);

		void clear();

		QString text() const;
		void setText(const QString& pText);

		void setDigitFieldsInvalid(bool pInvalid);

	Q_SIGNALS:
		void textEdited(const QString& pText);

	protected:
		virtual void paintEvent(QPaintEvent* pEvent) override;
		virtual void focusInEvent(QFocusEvent* pEvent) override;
		virtual void focusOutEvent(QFocusEvent* pEvent) override;
		virtual void keyPressEvent(QKeyEvent* pEvent) override;

	private:
		void onFieldActivationRequested();
		void onFieldDeactivationRequested();
		void onFieldValueEdited();

		void setActiveField(int pIndex);

	private:
		QGridLayout* mLayout;
		int mMinLength;
		int mMaxLength;
		QScopedPointer<RenderingHelper> mRenderingHelper;
		QVector<DigitField*> mDigitFields;
		int mActiveFieldIndex;
};

} /* namespace governikus */
