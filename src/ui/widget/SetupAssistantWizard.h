/*!
 * \brief Setup assistant wizard before application startup.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

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

class ReaderDeviceWidget;


class CardReaderPage
	: public QWizardPage
{
	Q_OBJECT

	private:
		ReaderDeviceWidget* const mWidget;

	public:
		CardReaderPage(const QString& pTitle, const QString& pAccessibleName);
		virtual ~CardReaderPage() = default;
};


class SetupAssistantWizard
	: public QWizard
{
	Q_OBJECT

	int mPageCount;
	QPointer<QCheckBox> mSaveHistoryCheckBox;
	QPointer<QPushButton> mChangeTransportPinButton;

	public:
		SetupAssistantWizard(QWidget* pParent = nullptr);
		virtual ~SetupAssistantWizard() override;

		virtual void accept() override;
		bool isRemindWizardAgain();

	Q_SIGNALS:
		void fireChangePinButtonClicked();

	private:
		QString createAccessibleName(const QString& pName, const QString& pText = QString()) const;
		QString createTitle(const QString& pName) const;
		QString createDescription(const QString& pTitle, const QString& pSummary) const;
		QWizardPage* createWizardInitialPinPage();
		QWizardPage* createWizardCardReaderPage();
		QWizardPage* createConclusionPage();

	private Q_SLOTS:
		void onChangeTransportPinButtonPressed();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

};

} // namespace governikus
