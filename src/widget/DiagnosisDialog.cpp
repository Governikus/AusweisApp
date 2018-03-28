/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisDialog.h"

#include "context/DiagnosisContext.h"
#include "DiagnosisWidget.h"
#include "generic/HelpAction.h"
#include "ui_DiagnosisDialog.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>

using namespace governikus;


DiagnosisDialog::DiagnosisDialog(DiagnosisContext* pContext, QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::DiagnosisDialog)
	, mDiagnosisWidget(new DiagnosisWidget(pContext, pParent))
{
	mUi->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
	setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Diagnosis"));

	installEventFilter(this);

	mUi->diagnosisLayout->addWidget(mDiagnosisWidget);

	connect(mUi->saveButton, &QAbstractButton::clicked, this, &DiagnosisDialog::onSaveButtonClicked);
	connect(mUi->closeButton, &QAbstractButton::clicked, this, &DiagnosisDialog::close);
}


DiagnosisDialog::~DiagnosisDialog()
{
}


void DiagnosisDialog::onSaveButtonClicked()
{
	const auto& creationTime = mDiagnosisWidget->getCreationTime();

	QString filename = QStringLiteral("AusweisApp2.Diagnosis.%1.txt").arg(creationTime.toString(QStringLiteral("yyyy-MM-dd_HH-mm")));
	filename = QFileDialog::getSaveFileName(this,
			QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Save"),
			QDir::homePath() + QLatin1Char('/') + filename,
#ifndef Q_OS_MACOS
			tr("Text files") + QStringLiteral(" (*.txt)"));
#else
			tr("Text files") + QStringLiteral(" (*.txt)"), nullptr, QFileDialog::DontUseNativeDialog);
#endif

	if (filename.isEmpty())
	{
		return;
	}
	if (!filename.endsWith(QLatin1String(".txt"), Qt::CaseSensitivity::CaseInsensitive))
	{
		filename += QStringLiteral(".txt");
	}

	QString text = mDiagnosisWidget->getInfoTextEdit();
#ifdef Q_OS_WIN
	text.replace(QLatin1Char('\n'), QStringLiteral("\r\n"));
#endif

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QFile::Truncate) || file.write(text.toUtf8()) < 0)
	{
		QMessageBox box(this);
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


bool DiagnosisDialog::eventFilter(QObject* pObject, QEvent* pEvent)
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


void DiagnosisDialog::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
