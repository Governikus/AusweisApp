/*!
 * \brief Rudimentary replacement for QButtonGroup to work around tab navigation issues.
 *
 * Bug in Qt 5.2.1: Buttons in a QButtonGroup cannot be navigated via the Tab key. This
 * class provides a work-around for simple cases.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QVector>

class QAbstractButton;

namespace governikus
{

class ExclusiveButtonGroup
	: public QObject
{
	Q_OBJECT

	public:
		ExclusiveButtonGroup(QObject* pParent = nullptr);
		virtual ~ExclusiveButtonGroup() override;

		const QVector<QAbstractButton*>& getButtons() const
		{
			return mButtons;
		}


		void addButton(QAbstractButton* pButton);
		void removeButton(QAbstractButton* pButton);

		virtual bool eventFilter(QObject* pWatched, QEvent* pEvent) override;

	Q_SIGNALS:
		void buttonClicked(QAbstractButton* pButton);
		void buttonPressed(QAbstractButton* pButton);
		void buttonReleased(QAbstractButton* pButton);
		void buttonToggled(QAbstractButton* pButton, bool pChecked);

	private Q_SLOTS:
		void onButtonClicked(bool pChecked);
		void onButtonPressed();
		void onButtonReleased();
		void onButtonToggled(bool pChecked);

	private:
		QVector<QAbstractButton*> mButtons;
};

} /* namespace governikus */
