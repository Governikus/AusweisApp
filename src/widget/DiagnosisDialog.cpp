/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
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
	QString fileName = QFileDialog::getSaveFileName(this, QCoreApplication::applicationName()
			+ QStringLiteral(" - ")
			+ tr("Save diagnosis result")
			, QDir::homePath()
			+ QLatin1Char('/')
			+ tr("AusweisApp2-diagnosis.txt")
#ifndef Q_OS_MACOS
			, tr("Text files (*.txt)"));
#else
			, tr("Text files (*.txt)"), nullptr, QFileDialog::DontUseNativeDialog);
#endif

	if (fileName.isEmpty())
	{
		return;
	}
	if (!fileName.endsWith(QLatin1String(".txt"), Qt::CaseSensitivity::CaseInsensitive))
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
		QMessageBox::warning(this, QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("File error"), tr("An error occurred while saving the file."));
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
