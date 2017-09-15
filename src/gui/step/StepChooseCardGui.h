/*!
 * \brief GUI to select reader/card.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "DiagnosisGui.h"
#include "ReaderDriverGui.h"
#include "ReaderManager.h"
#include "StepGui.h"

#include <QMessageBox>

class QLabel;

namespace governikus
{

class AuthenticateStepsWidget;

class StepChooseCardGui
	: public StepGui
{
	Q_OBJECT

	private:
		QPointer<QMessageBox> mInformationMessageBox;
		QPointer<DiagnosisGui> mDiagnosisGui;
		QPointer<ReaderDriverGui> mReaderDriverGui;
		QPushButton* mCancelButton, * mReaderDriverButton, * mDiagnosisButton;

		QSharedPointer<WorkflowContext> mContext;

		QString getCurrentReaderImage(const QVector<ReaderInfo>& pReaderInfos);
		void updateErrorMessage(const QString& pTitle, const QString& pMessage, bool closeErrorMessage = false);

	public Q_SLOTS:
		void onReaderManagerSignal();

	public:
		StepChooseCardGui(const QSharedPointer<WorkflowContext>& pContext, AuthenticateStepsWidget* pStepsWidget);
		virtual ~StepChooseCardGui();

		virtual void activate() override;
		virtual void deactivate() override;
};

} /* namespace governikus */
