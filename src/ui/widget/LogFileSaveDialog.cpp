/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
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


QString LogFileSaveDialog::generateLogFilename(const QString& pSource)
{
	const QDateTime creationDateTime = pSource.isEmpty() ? Env::getSingleton<LogHandler>()->getCurrentLogfileDate() : LogHandler::getFileDate(pSource);
	const QString filename = QStringLiteral("%1.%2.log").arg(QCoreApplication::applicationName(), creationDateTime.toString(QStringLiteral("yyyy-MM-dd_HH-mm")));

	return filename;
}


QString LogFileSaveDialog::getSaveFileName(QWidget* pParent, const QString& pSource)
{
	Q_ASSERT(mFileDialog.isNull());

	const QString caption(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Save"));
	const QString filename = generateLogFilename(pSource);
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

	if (!filename.isEmpty())
	{
		saveLogFileToFilename(pParent, filename, pSource);
	}
}


bool LogFileSaveDialog::saveLogFileToFilename(QWidget* pParent, const QString& pFilename, const QString& pSource)
{
	if (pFilename.isEmpty())
	{
		return false;
	}

	QString filename(pFilename);

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
		box.setText(tr("An error occurred while saving the file:") + QStringLiteral("\n\"%1\"").arg(filename));
		box.setStandardButtons(QMessageBox::Ok);
		box.button(QMessageBox::Ok)->setFocus();
		box.exec();
	}

	return copied;
}


void LogFileSaveDialog::closeActiveDialogs()
{
	if (mFileDialog)
	{
		mFileDialog->reject();
	}
}
