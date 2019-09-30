/*!
 * \brief GUI to select reader/card.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "DiagnosisGui.h"
#include "ReaderDeviceGui.h"
#include "ReaderManager.h"
#include "step/StepAuthenticationEac1Widget.h"
#include "StepGui.h"

#include <QMessageBox>


namespace governikus
{

class AuthenticateStepsWidget;

class StepChooseCardGui
	: public StepGui
{
	Q_OBJECT

	private:
		const QSharedPointer<WorkflowContext> mContext;
		StepAuthenticationEac1Widget* mAuthWidget;
		QPointer<QMessageBox> mInformationMessageBox;
		QPointer<ReaderDeviceGui> mReaderDeviceGui;
		QPushButton* mCancelButton, * mDeviceButton, * mRetryButton;
		bool mSubDialogOpen;

		QString getCurrentReaderImage(const QVector<ReaderInfo>& pReaderInfos);
		static QString formatErrorMessages(const QString& pMessage1, const QString& pMessage2);
		void updateErrorMessage(const QString& pTitle, const QString& pMessage1, const QString& pMessage2 = QString(), bool pCloseErrorMessage = false, const QString& pIconPath = QString());
		const QString connectedRemoteReaderNames() const;

	private Q_SLOTS:
		void onSubDialogFinished();

	public Q_SLOTS:
		void onReaderManagerSignal();

	public:
		StepChooseCardGui(const QSharedPointer<WorkflowContext>& pContext, QWidget* pParent);
		virtual ~StepChooseCardGui() override;

		virtual void activate() override;
		virtual void deactivate() override;
};

} // namespace governikus
