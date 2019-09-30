/*!
 * \brief Dialog for display the diagnosis information.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DiagnosisTreeModel.h"

#include <QDialog>
#include <QScopedPointer>
#include <QTreeView>

namespace Ui
{
class DiagnosisDialog;
} // namespace Ui

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
		QScopedPointer<DiagnosisTreeModel> mDiagnosisModel;
		QTreeView* mTreeView;

	private Q_SLOTS:
		void onSaveButtonClicked();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		explicit DiagnosisDialog(const QSharedPointer<DiagnosisContext>& pContext, QWidget* pParent = nullptr);
		virtual ~DiagnosisDialog() override;
};

} // namespace governikus
