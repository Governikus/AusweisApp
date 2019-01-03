/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "Env.h"
#include "LogFileSaveDialog.h"
#include "LogHandler.h"

#include <QAbstractButton>
#include <QApplication>
#include <QLoggingCategory>
#include <QMessageBox>

Q_DECLARE_LOGGING_CATEGORY(gui)

using namespace governikus;


QString LogFileSaveDialog::getSaveFileName(QWidget* pParent, const QString& pSource)
{
	Q_ASSERT(mFileDialog.isNull());

	const QString caption(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Save"));
	const QDateTime creationDateTime = pSource.isEmpty() ? Env::getSingleton<LogHandler>()->getCurrentLogfileDate() : LogHandler::getFileDate(pSource);
	const QString filename = QStringLiteral("AusweisApp2.%1.log").arg(creationDateTime.toString(QStringLiteral("yyyy-MM-dd_HH-mm")));
	const QString directory = QDir::homePath() + QLatin1Char('/') + filename;
	const QString filter = QStringLiteral("*.log");

	mFileDialog = QPointer<QFileDialog>(new QFileDialog(pParent, caption, directory, filter));
	mFileDialog->setAcceptMode(QFileDialog::AcceptSave);

	const QStringList fileNames = mFileDialog->exec() == QDialog::Accepted ? mFileDialog->selectedFiles() : QStringList();
	mFileDialog->deleteLater();

	return fileNames.isEmpty() ? QString() : fileNames.first();
}


void LogFileSaveDialog::saveLogFile(QWidget* pParent, const QString& pSource)
{
	Q_ASSERT(pParent != nullptr);

	QString filename = getSaveFileName(pParent, pSource);
	if (!filename.isEmpty()) // if user does not select "cancel"
	{
		if (!filename.endsWith(QLatin1String(".log"), Qt::CaseSensitivity::CaseInsensitive)
				&& !filename.endsWith(QLatin1String(".txt"), Qt::CaseSensitivity::CaseInsensitive))
		{
			filename += QStringLiteral(".log");
		}

		qCDebug(gui) << "File location:" << filename;

		if (QFile::exists(filename))
		{
			const bool deleted = QFile::remove(filename);
			qCDebug(gui) << "Delete file location:" << deleted;
		}

		const bool copied = pSource.isEmpty() ? Env::getSingleton<LogHandler>()->copy(filename) : QFile::copy(pSource, filename);
		qCDebug(gui) << "Copy log to file location:" << copied;
		if (!copied)
		{
			QMessageBox box(pParent);
			box.setWindowTitle(QApplication::applicationName() + QStringLiteral(" - ") + tr("File error"));
			box.setWindowModality(Qt::ApplicationModal);
			box.setIcon(QMessageBox::Warning);
			box.setWindowFlags(box.windowFlags() & ~Qt::WindowContextHelpButtonHint);
			box.setText(tr("An error occurred while saving the file."));
			box.setStandardButtons(QMessageBox::Ok);
			box.button(QMessageBox::Ok)->setFocus();
			box.exec();
		}
	}
}


void LogFileSaveDialog::closeActiveDialogs()
{
	if (mFileDialog)
	{
		mFileDialog->reject();
	}
}
