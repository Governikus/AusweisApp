/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "step/StepAuthenticationEac1Widget.h"
#include "StepGui.h"

#include <QMessageBox>
#include <QPointer>

namespace governikus
{

class AuthenticateStepsWidget;

class StepAuthenticationEac1Gui
	: public StepGui
{
	Q_OBJECT

	private:
		QSharedPointer<AuthContext> mContext;
		AuthenticateStepsWidget* mStepsWidget;
		StepAuthenticationEac1Widget* mWidget;
		StepAuthenticationEac1Widget::State mState;
		QPointer<QMessageBox> mPayAttentionToReaderMsgBox;
		QString mPin;
		QString mCan;
		bool mActive;

	public:
		StepAuthenticationEac1Gui(QSharedPointer<AuthContext> pContext, AuthenticateStepsWidget* pStepsWidget);
		virtual ~StepAuthenticationEac1Gui() override;

		virtual void activate() override;
		virtual void deactivate() override;
		bool isActive() const;

		virtual void setState(StepAuthenticationEac1Widget::State pState);
		virtual void incorrectPinError();

		virtual void forwardStep() override;

		virtual void hidePayAttentionToReader();

	public Q_SLOTS:
		virtual void onShowPayAttentionToReader();
		void onPinUpdated(const QString& pPin);
		void onCanUpdated(const QString& pCan);
		void onUiFinished();
};

} // namespace governikus
