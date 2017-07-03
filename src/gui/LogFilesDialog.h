/*!
 * LogFilesDialog.h
 *
 * \brief Dialog for display the old log files.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
class LogFilesDialog;
}

namespace governikus
{

class LogFilesDialog
	: public QDialog
{
	Q_OBJECT

	public:
		LogFilesDialog(QWidget* pParent = nullptr);
		virtual ~LogFilesDialog();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::LogFilesDialog> mUi;

		void init();
		void appendLoggingDump(const QString& pLog);

	private Q_SLOTS:
		void doLogMsg(const QString& pMsg);
		void onSaveButtonClicked();
		void onCurrentIndexChanged(int pIndex);
		void onDeleteButtonClicked();
};

} /* namespace governikus */
