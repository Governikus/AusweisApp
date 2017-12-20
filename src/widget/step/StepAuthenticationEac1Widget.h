/*!
 * \brief Widget for the desktop StepAuthenticationEac1Gui.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "generic/ButtonState.h"

#include <QCheckBox>

#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#endif

class QLabel;
class QProgressBar;

namespace Ui
{
class StepAuthenticationEac1Widget;
}


namespace governikus
{

class PasswordEdit;

class StepAuthenticationEac1Widget
	: public QWidget
{
	Q_OBJECT

	public:
		enum class State
		{
			INITIAL,
			EDIT_CHAT,
			ENTER_PIN,
			AUTHENTICATING_ESERVICE,
			AUTHENTICATING_CARD,
			READING_CARD_DATA,
			REDIRECTING_BROWSER,
			FINISHED,
		};
		Q_ENUM(State);

		StepAuthenticationEac1Widget(QWidget* pParent = nullptr);
		virtual ~StepAuthenticationEac1Widget() override;

		void setContext(const QSharedPointer<AuthContext>& pContext);

		void setState(State pState);
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
		void onResultChanged();

	protected:
		virtual void hideEvent(QHideEvent* pEvent) override;
		virtual void showEvent(QShowEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

	private:
		void setToolTip();
		void updateWidget();
		void setupChatView();
		void prepareChatsForGui();
		void updateProgressPanel(int pProgressValue = 0, const QString& pProgressText = QString());
		void addChatRightToGui(AccessRight pRight, bool pOptional, int pListSize);
		void clearPinWidgetLayout();
		void createBasicReaderWidget();

	private:
		QScopedPointer<Ui::StepAuthenticationEac1Widget> mUi;
		QSharedPointer<AuthContext> mContext;
		QMap<QCheckBox*, AccessRight> mMap;

		PasswordEdit* mCANField;
		PasswordEdit* mPINField;

		State mState;
		QProgressBar* mProgressBar;
		QLabel* mProgressBarLabel;

		bool mCloseWindowWhenFinished;

		#ifdef Q_OS_WIN
		QWinTaskbarButton* mTaskbarButton;
		#endif
};


defineEnumOperators(StepAuthenticationEac1Widget::State)

} /* namespace governikus */
