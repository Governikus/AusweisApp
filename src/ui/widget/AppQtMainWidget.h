/*!
 * \brief Main class for the top level main widget
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "generic/GuiModule.h"
#include "LogFilesDialog.h"
#include "workflow/WorkflowWidgetParent.h"

#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QScopedPointer>
#include <QSettings>
#include <QTableWidgetItem>

namespace Ui
{
class AppQtMainWidget;
} // namespace Ui

namespace governikus
{

class WorkflowQtWidget;

class AppQtMainWidget
	: public QMainWindow
{
	Q_OBJECT

	public:
		AppQtMainWidget();
		virtual ~AppQtMainWidget() override;

		bool remoteScanRunning() const;
		void workflowActivated(WorkflowWidgetParent pParent, const QString& pName);
		void workflowDeactivated();

		void switchToGuiModule(GuiModule pModule);

		void switchToPinSettingsAfterWorkflow();

		bool isHideWindowAfterWorkflow() const
		{
			return mHideWindowAfterWorkflow;
		}


		void setHideWindowAfterWorkflow(bool pHide)
		{
			mHideWindowAfterWorkflow = pHide;
		}


		void activateMenuBarItems(bool pEnable);

		WorkflowQtWidget* getAuthenticationWorkflowWidget() const
		{
			return mAuthenticationWorkflowWidget;
		}


		bool isRemindUserToClose();

		void setSelectedTab(QAbstractButton* pSelectedPushButton);

		void activateWindow();

		void hideWithoutConfirmation();

	protected:
		virtual void showEvent(QShowEvent* pEvent) override;
		virtual void closeEvent(QCloseEvent* pEvent) override;
		virtual void keyPressEvent(QKeyEvent* keyEvent) override;
		virtual void changeEvent(QEvent* event) override;

	private Q_SLOTS:
		void onSettingsDone();

		void onOpenLoggingFileButtonClicked();
		void onSaveLoggingFileButtonClicked();
		void onTabButtonToggled(QAbstractButton* pButton, bool pChecked);
		void onTabActionTriggered();
		void onAboutActionClicked();
		void onEvaluateActionClicked();
		void onQuestionActionClicked();
		void onContentActionClicked();
		void onAskUserToConfirmClosing();

	public Q_SLOTS:
		void onSettingsButtonClicked();
		void onChangePinButtonClicked();
		void onCloseWindowRequested();
		void onSendErrorActionClicked();

	Q_SIGNALS:
		void fireSetupAssistantWizardRequest();
		void fireChangePinRequested();
		void fireDiagnosisRequested();
		void fireCloseWindowRequested(bool* pDoClose);
		void fireSelfAuthenticationRequested();
		void fireQuitApplicationRequested();
		void fireChangeHighContrast(bool* pHighContrastOn);
		void fireAskUserToConfirmClosing();
		void fireCloseActiveDialogs();

	private:
		QScopedPointer<Ui::AppQtMainWidget> mUi;
		QMap<QAbstractButton*, QWidget*> mTabButton2Page;
		QMap<QAction*, QAbstractButton*> mTabAction2Button;
		WorkflowQtWidget* mAuthenticationWorkflowWidget;
		QAbstractButton* mSelectedPushButton;
		QAbstractButton* mSelectedPushButtonBeforeWorkflow;
		QVector<QWidget*> mSelectedPagesBeforeWorkflow;
		bool mHideWindowAfterWorkflow;
		QPointer<LogFilesDialog> mLogFilesDialog;
		QString mStyleSheet;
		void refreshLanguageButton();
		void setLanguage(QLocale::Language pLocale);
		bool mClosingDialogsPending;
		bool mCloseWithoutConfirmation;
};

} // namespace governikus
