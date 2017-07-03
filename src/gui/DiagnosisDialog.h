/*!
 * DiagnosisDialog.h
 *
 * \brief Dialog for display the diagnosis information.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "DiagnosisWidget.h"
#include "context/DiagnosisContext.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
class DiagnosisDialog;
}

namespace governikus
{

class DiagnosisDialog
	: public QDialog
{
	Q_OBJECT

	public:
		DiagnosisDialog(DiagnosisContext* pContext, QWidget* pParent = nullptr);
		virtual ~DiagnosisDialog();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::DiagnosisDialog> mUi;
		DiagnosisWidget* mDiagnosisWidget;

	private Q_SLOTS:
		void onSaveButtonClicked();
};

} /* namespace governikus */
