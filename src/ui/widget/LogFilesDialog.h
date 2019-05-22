/*!
 * \brief Dialog for display the old log files.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "LogFileSaveDialog.h"

#include <QDialog>
#include <QFileDialog>
#include <QPointer>
#include <QScopedPointer>

namespace Ui
{
class LogFilesDialog;
} // namespace Ui

namespace governikus
{

class LogFilesDialog
	: public QDialog
{
	Q_OBJECT

	public:
		void saveLogFile(QWidget* pParent, const QString& pSource = QString());

		LogFilesDialog(QWidget* pParent = nullptr);
		virtual ~LogFilesDialog() override;
		void closeActiveDialogs();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::LogFilesDialog> mUi;
		LogFileSaveDialog mFileDialog;

		void init();
		void appendLoggingDump(const QString& pLog);

	private Q_SLOTS:
		void doLogMsg(const QString& pMsg);
		void onSaveButtonClicked();
		void onCurrentIndexChanged(int pIndex);
		void onDeleteButtonClicked();
};

} // namespace governikus
