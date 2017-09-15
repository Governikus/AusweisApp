/*!
 * \brief Main class for the top level main widget
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QScopedPointer>
#include <QSettings>
#include <QTableWidgetItem>

#include "context/ChangePinContext.h"
#include "generic/GuiModule.h"
#include "LogFilesDialog.h"
#include "workflow/WorkflowWidgetParent.h"

namespace Ui
{
class AppQtMainWidget;
}

namespace governikus
{

class WorkflowQtWidget;

class AppQtMainWidget
	: public QMainWindow
{
	Q_OBJECT

	public:
		AppQtMainWidget();
		virtual ~AppQtMainWidget();

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

	protected:
		virtual void closeEvent(QCloseEvent* pEvent);
		virtual void keyPressEvent(QKeyEvent* keyEvent);
		virtual void changeEvent(QEvent* event);

	private:
		static void updateGeometryRecursively(QWidget* pWidget, QSet<QObject*>& pVisitedObjects);
		static void updateGeometryRecursively(QLayout* pLayout, QSet<QObject*>& pVisitedObjects);

	private Q_SLOTS:
		void onSettingsDone();

		void onOpenLoggingFileButtonClicked();
		void onSaveLoggingFileButtonClicked();
		void onTabButtonToggled(QAbstractButton* pButton, bool pChecked);
		void onTabActionTriggered();
		void onAboutActionClicked();
		void onSendErrorActionClicked();
		void onEvaluateActionClicked();
		void onQuestionActionClicked();
		void onContentActionClicked();

	public Q_SLOTS:
		void onSettingsButtonClicked();
		void onChangePinButtonClicked();

	Q_SIGNALS:
		void fireSetupAssistantWizardRequest();
		void fireChangePinRequested();
		void fireDiagnosisRequested();
		void fireReaderDriverRequested();
		void fireCloseWindowRequested(bool* pDoClose);
		void fireSelfAuthenticationRequested();
		void fireQuitApplicationRequested();
		void fireChangeHighContrast(bool* pHighContrastOn);

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
};

} /* namespace governikus */
