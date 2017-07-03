/*!
 * SetupAssistantWizard.h
 *
 * \brief Setup assistant wizard before application startup.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "GeneralSettingsWidget.h"
#include "NoScriptFinder.h"
#include "SmartCardDefinitions.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QScopedPointer>
#include <QTableWidget>
#include <QWizard>


namespace governikus
{

class SetupAssistantWizard;
class ReaderDriverWidget;

class TabButton;

class CardReaderPage
	: public QWizardPage
{
	Q_OBJECT

	public:
		CardReaderPage(int pStepIndex, int pPageCount, SetupAssistantWizard* pWizard);

		virtual ~CardReaderPage();

		virtual void initializePage() override;

		ReaderDriverWidget* const mWidget;
};


class SetupAssistantWizard
	: public QWizard
{
	Q_OBJECT

	int mPageCount;
	NoScriptFinder mNoScriptFinder;
	QPointer<QCheckBox> mSaveHistoryCheckBox;
	QPointer<QPushButton> mChangeTransportPinButton;

	public:
		SetupAssistantWizard(QWidget* pParent = nullptr);
		virtual ~SetupAssistantWizard();

		virtual void accept() override;
		bool isRemindWizardAgain();

	Q_SIGNALS:
		void fireChangePinButtonClicked();

	private:
		QWizardPage* createWizardInitialPinPage();
		QWizardPage* createWizardCardReaderPage();
		QWizardPage* createWizardNoScriptExtensionPage();
		QWizardPage* createConclusionPage();

		void addReader(QTableWidget* pTableWidget, int pRow, ReaderType pReaderType);

	private Q_SLOTS:
		void onChangeTransportPinButtonPressed();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

};

} /* namespace governikus */
