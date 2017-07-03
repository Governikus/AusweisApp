/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "StepGui.h"
#include "context/AuthContext.h"

#include <QMessageBox>
#include <QPointer>

namespace governikus
{

class AuthenticateStepsWidget;
class StepAuthenticationEac1Widget;

class StepAuthenticationEac1Gui
	: public StepGui
{
	Q_OBJECT

	private:
		QSharedPointer<AuthContext> mContext;
		AuthenticateStepsWidget* mStepsWidget;
		StepAuthenticationEac1Widget* mWidget;
		StepDidAuthenticateEac1Ui::State mState;
		QPointer<QMessageBox> mPayAttentionToReaderMsgBox;
		QString mPin;
		QString mCan;

	public:
		StepAuthenticationEac1Gui(QSharedPointer<AuthContext> pContext, AuthenticateStepsWidget* pStepsWidget);
		virtual ~StepAuthenticationEac1Gui();

		virtual void activate() override;
		virtual void deactivate() override;

		virtual void setState(StepDidAuthenticateEac1Ui::State pState);
		virtual void incorrectPinError();

		virtual void forwardStep() override;

		virtual void hidePayAttentionToReader();

	public Q_SLOTS:
		virtual void onShowPayAttentionToReader();
		void onPinUpdated(const QString& pPin);
		void onCanUpdated(const QString& pCan);
		void onUiFinished();
};

} /* namespace governikus */
