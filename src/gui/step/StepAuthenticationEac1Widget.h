/*!
 * StepAuthenticationEac1Widget.h
 *
 * \brief Widget for the desktop StepAuthenticationEac1Gui.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "StepGui.h"
#include "context/AuthContext.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QProgressBar;

namespace Ui
{
class StepAuthenticationEac1Widget;
}

class QAbstractButton;

namespace governikus
{

class PasswordEdit;

class StepAuthenticationEac1Widget
	: public QWidget
{
	Q_OBJECT

	public:
		StepAuthenticationEac1Widget(QWidget* pParent = nullptr);
		virtual ~StepAuthenticationEac1Widget();

		void setContext(QSharedPointer<AuthContext> pContext);

		void setState(StepDidAuthenticateEac1Ui::State pState);
		void forwardStep();

		void updateButtonsAndPinWidget();

	Q_SIGNALS:
		void setForwardButtonState(ButtonState pState, const QString& pText = QString());
		void setCancelButtonState(ButtonState pState);

		void firePinUpdated(const QString& pPin);
		void fireCanUpdated(const QString& pCan);

	private Q_SLOTS:
		void focusWidget();
		void onDetailsButtonClicked();
		void checkBoxChanged(int pCheckState);
		void canTextEdited(const QString& pText);
		void pinTextEdited(const QString& pText);
		void onRandomButtonClicked();

	private:
		void prepareChatsForGui();

		void addChatRightToGui(AccessRight pRight, bool pOptional, int pListSize);

		void clearPinWidgetLayout();

		void createBasicReaderWidget();

		void validateAndSetText(const QString& pPassword, QLineEdit* pLineEdit);

	private:
		QScopedPointer<Ui::StepAuthenticationEac1Widget> mUi;
		QSharedPointer<AuthContext> mContext;
		QMap<QCheckBox*, AccessRight> mMap;

		PasswordEdit* mCANField;
		PasswordEdit* mPINField;

		StepDidAuthenticateEac1Ui::State mState;
		QProgressBar* mProgressBar;
		QLabel* mProgressBarLabel;

		bool mCloseWindowWhenFinished;
};

} /* namespace governikus */
