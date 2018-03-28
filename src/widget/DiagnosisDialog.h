/*!
 * \brief Dialog for display the diagnosis information.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
class DiagnosisDialog;
}

namespace governikus
{

class DiagnosisContext;
class DiagnosisWidget;

class DiagnosisDialog
	: public QDialog
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::DiagnosisDialog> mUi;
		DiagnosisWidget* mDiagnosisWidget;

	private Q_SLOTS:
		void onSaveButtonClicked();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		DiagnosisDialog(DiagnosisContext* pContext, QWidget* pParent = nullptr);
		virtual ~DiagnosisDialog() override;
};

} /* namespace governikus */
