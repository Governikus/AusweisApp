/*!
 * \brief Base class for workflow Qt GUI widgets.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "step/StepGui.h"

#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace governikus
{

class WorkflowQtWidget
	: public QWidget
{
	Q_OBJECT

	public:
		WorkflowQtWidget(QWidget* pParent = nullptr);
		virtual ~WorkflowQtWidget();

		QWidget* getStepWidgetArea() const
		{
			return mStepWidgetArea;
		}


		void addStepWidget(QWidget* widget);
		void removeStepWidget(QWidget* widget);

	Q_SIGNALS:
		/*!
		 * This signal is sent when the user presses the "Cancel" button.
		 */
		void fireUserCancelled();

		/*!
		 * This signal is sent when the user presses the "Continue" button.
		 */
		void forwardStep();

	public Q_SLOTS:
		void onCancelButtonClicked();
		void setForwardButtonState(ButtonState pState, const QString& pText = QString());
		void setCancelButtonState(ButtonState pState);

	private:
		void setButtonState(QAbstractButton* pButton, ButtonState pState, const QString& pText);
		void focusForwardButton();

	private:
		QVBoxLayout* mMainLayout;
		QWidget* mStepWidgetArea;
		QPushButton* mCancelButton;
		QPushButton* mForwardButton;
};

} // namespace governikus
