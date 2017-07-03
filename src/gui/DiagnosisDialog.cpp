/*!
 * DiagnosisDialog.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "DiagnosisDialog.h"
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
	setWindowTitle(QCoreApplication::applicationName() + " - " + tr("Diagnosis"));

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

	QString fileName = QFileDialog::getSaveFileName(this, QCoreApplication::applicationName() + " - " + tr("Save diagnosis result"), QDir::homePath() + tr("/AusweisApp2-diagnosis.txt"),
			tr("Text files (*.txt)"));


	if (fileName.isEmpty())
	{
		return;
	}
	if (!fileName.endsWith(QStringLiteral(".txt"), Qt::CaseSensitivity::CaseInsensitive))
	{
		fileName += QStringLiteral(".txt");
	}

	QString text = mDiagnosisWidget->getInfoTextEdit();
#ifdef Q_OS_WIN
	text.replace(QLatin1Char('\n'), QStringLiteral("\r\n"));
#endif

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QFile::Truncate) || file.write(text.toUtf8()) < 0)
	{
		QMessageBox::warning(this, QCoreApplication::applicationName() + " - " + tr("File error"), tr("An error occurred while saving the file."));
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
