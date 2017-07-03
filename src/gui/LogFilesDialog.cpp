/*!
 * LogFilesDialog.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "LogFilesDialog.h"
#include "ui_LogFilesDialog.h"

#include "LogHandler.h"
#include "generic/HelpAction.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfoList>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTextCursor>


using namespace governikus;


LogFilesDialog::LogFilesDialog(QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::LogFilesDialog)
{
	mUi->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
	setWindowTitle(QCoreApplication::applicationName() + " - " + tr("Log"));

	connect(mUi->saveButton, &QAbstractButton::clicked, this, &LogFilesDialog::onSaveButtonClicked);
	connect(mUi->deleteButton, &QAbstractButton::clicked, this, &LogFilesDialog::onDeleteButtonClicked);
	connect(mUi->closeButton, &QAbstractButton::clicked, this, &LogFilesDialog::close);

	init();
}


LogFilesDialog::~LogFilesDialog()
{
}


void LogFilesDialog::init()
{
	mUi->logFilesComboBox->clear();

	auto model = new QStandardItemModel(this);
	mUi->logFilesComboBox->setModel(model);

	const auto& otherLogs = LogHandler::getInstance().getOtherLogfiles();
	QList<QStandardItem*> items;
	items.reserve(otherLogs.size() + 1);
	items << new QStandardItem(tr("Current log"));
	for (const auto& entry : otherLogs)
	{
		auto date = entry.created();
		auto item = new QStandardItem(date.toString(tr("MM/dd/yyyy hh:mm:ss")));
		item->setData(date, Qt::UserRole);
		item->setData(entry.absoluteFilePath());
		items << item;
	}

	mUi->logFilesComboBox->setEnabled(!otherLogs.isEmpty());
	mUi->deleteButton->setEnabled(!otherLogs.isEmpty());

	model->invisibleRootItem()->appendRows(items);
	model->setSortRole(Qt::UserRole);
	model->sort(0, Qt::DescendingOrder);

	connect(mUi->logFilesComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LogFilesDialog::onCurrentIndexChanged);

	mUi->plainTextEdit->clear();
	appendLoggingDump(QString::fromUtf8(LogHandler::getInstance().getBacklog()).toHtmlEscaped());
	mUi->plainTextEdit->moveCursor(QTextCursor::Start);
	connect(&LogHandler::getInstance(), &LogHandler::fireLog, this, &LogFilesDialog::doLogMsg);
}


void LogFilesDialog::appendLoggingDump(const QString& pLog)
{
	mUi->plainTextEdit->appendHtml(QStringLiteral("<pre>%1</pre>").arg(pLog));
}


void LogFilesDialog::doLogMsg(const QString& pMsg)
{
	appendLoggingDump(pMsg.toHtmlEscaped());
}


void LogFilesDialog::onCurrentIndexChanged(int pIndex)
{
	mUi->plainTextEdit->clear();
	if (pIndex == 0)
	{
		connect(&LogHandler::getInstance(), &LogHandler::fireLog, this, &LogFilesDialog::doLogMsg);

		appendLoggingDump(QString::fromUtf8(LogHandler::getInstance().getBacklog()).toHtmlEscaped());
		mUi->plainTextEdit->moveCursor(QTextCursor::Start);
	}
	else
	{
		disconnect(&LogHandler::getInstance(), &LogHandler::fireLog, this, &LogFilesDialog::doLogMsg);

		QFile file(mUi->logFilesComboBox->itemData(pIndex, Qt::UserRole + 1).toString());
		if (file.size() < 3145728)
		{
			if (file.open(QIODevice::ReadOnly))
			{
				appendLoggingDump(QString::fromUtf8(file.readAll()));
			}
			else
			{
				appendLoggingDump(tr("File could not be opened: ") + file.fileName());
			}
		}
		else
		{
			appendLoggingDump(tr("File is larger than 3 MB and can not be displayed: ") + file.fileName());
		}
	}
}


void LogFilesDialog::onDeleteButtonClicked()
{
	QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Delete log files"), tr("Do you really want to delete all old log files?"), QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes)
	{
		LogHandler::getInstance().removeOtherLogfiles();
		init();
	}
}


void LogFilesDialog::onSaveButtonClicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, QCoreApplication::applicationName() + " - " + tr("Save log file"), QDir::homePath() + tr("/AusweisApp2-logfile.log"),
			tr("Text files (*.txt)"));

	if (fileName.isEmpty())
	{
		return;
	}
	if (!fileName.endsWith(QLatin1String(".txt"), Qt::CaseSensitivity::CaseInsensitive))
	{
		fileName += QLatin1String(".txt");
	}

	QString text = mUi->plainTextEdit->toPlainText();
   #ifdef Q_OS_WIN
	text.replace(QLatin1Char('\n'), QStringLiteral("\r\n"));
   #endif

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QFile::Truncate) || file.write(text.toUtf8()) < 0)
	{
		QMessageBox::warning(this, QCoreApplication::applicationName() + " - " + tr("File error"), tr("An error occurred while saving the file."));
	}
}


bool LogFilesDialog::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp();
			return true;
		}
	}
	return QDialog::eventFilter(pObject, pEvent);
}
