/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QFileDialog>
#include <QObject>
#include <QPointer>

namespace governikus
{

class LogFileSaveDialog
	: public QObject
{
	Q_OBJECT

	private:
		QPointer<QFileDialog> mFileDialog;

		QString getSaveFileName(QWidget* pParent, const QString& pSource);

	public:
		void saveLogFile(QWidget* pParent, const QString& pSource = QString());
		bool saveLogFileToFilename(QWidget* pParent, const QString& pFilename, const QString& pSource = QString());
		static QString generateLogFilename(const QString& pSource = QString());

		void closeActiveDialogs();
};

} // namespace governikus
