import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage
{
	enum SubViews {
		Connectivity = 1,
		Progress,
		AccessRights,
		Workflow,
		Password,
		Data,
		Result,
		ReturnToMain
	}

	id: identifyView

	KeyNavigation.tab: visibleChildren.length > 1 ? visibleChildren[1] : navSuccessor

	Accessible.role: Accessible.Grouping
	Accessible.name: qsTr("Identify view") + settingsModel.translationTrigger
	Accessible.description: qsTr("This is the identify view of the AusweisApp2.") + settingsModel.translationTrigger

	readonly property var tabTarget: visibleChildren.length > 1 ? visibleChildren[1] : identifyView

	property int activeView: 0

	titleBarAction: TitleBarAction {
		text: qsTr("Identify")
		showSettings: false
		showHelp: false

		onClicked: editRights.showProviderInformation(false)

		customSubAction: CancelAction {
			onClicked: {
				if (identifyResult.visible) {
					AuthModel.continueWorkflow()
					identifyView.nextView(SectionPage.Views.Main)
				} else {
					AuthModel.cancelWorkflow()
				}
			}
		}
	}
/*
	content: IdentifyViewInfo {
		id: identifyViewInfo
		width: identifyEditChatView.width
		height: identifyEditChatView.height
	}
*/
	IdentifyController {
		id: identifyController

		onNextView: {
			if (pName < 1) {
				console.warn("Unknown view requested: " + pName)
				return;
			}

			if (pName === IdentifyView.SubViews.ReturnToMain) {
				identifyView.nextView(SectionPage.Views.Main)
				return;
			}

			identifyView.activeView = pName
		}
	}

	EditRights {
		id: editRights
		visible: identifyView.activeView === IdentifyView.SubViews.AccessRights
	}

	SelfAuthenticationData {
		visible: identifyView.activeView === IdentifyView.SubViews.Data
		onVisibleChanged: ApplicationWindow.menuBar.updateActions()
	}
/*
	IdentifyWorkflow {
		id: identifyWorkflow
		visible: false
	}

	EnterPinView {
		id: enterPinView
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: AuthModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger }
		visible: false

		onPinEntered: {
			firePop()
			AuthModel.continueWorkflow()
		}
	}
*/

	ProgressView {
		id: identifyProgressView

		readonly property bool inProgress: identifyController.workflowState !== IdentifyController.WorkflowStates.Initial

		visible: identifyView.activeView === IdentifyView.SubViews.Progress
		text: (inProgress ? qsTr("Authentication in progress") : qsTr("Acquiring provider certificate")) + settingsModel.translationTrigger
		subText: {
			settingsModel.translationTrigger;
			if (!visible) {
				return ""
			}
			if (AuthModel.isBasicReader) {
				return qsTr("Please wait a moment...")
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError
			}
			if (NumberModel.pinDeactivated) {
				return qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.")
			}
			return qsTr("Please wait a moment...")
		}
		subTextColor: !AuthModel.isBasicReader && (NumberModel.inputError || NumberModel.pinDeactivated) ? "red" : Constants.secondary_text
		progressValue: identifyController.workflowProgressValue
		progressText: (progressValue === 0 ? "" :
						progressValue === 1 ? qsTr("Service provider is being verified") :
						progressValue === 2 ? qsTr("Card is being verified") :
						progressValue === 3 ? qsTr("Reading data") :
						progressValue === 4 ? qsTr("Sending data to service provider") :
						progressValue === 5 ? qsTr("Preparing results") :
						"") + settingsModel.translationTrigger
		progressBarVisible: identifyController.workflowProgressVisible
	}

	ProgressView {
		id: checkConnectivityView
		visible: identifyView.activeView === IdentifyView.SubViews.Connectivity
		text: qsTr("No network connectivity") + settingsModel.translationTrigger
		subText: qsTr("Please enable the network interface or cancel the workflow.") + settingsModel.translationTrigger
		subTextColor: Constants.red
	}

	ResultView {
		id: identifyResult
		navSuccessor: identifyView.navSuccessor

		isError: AuthModel.resultString
		text: AuthModel.resultString
		onNextView: {
			AuthModel.continueWorkflow()
			identifyView.nextView(pName)
		}
		visible: identifyView.activeView === IdentifyView.SubViews.Result
	}
}
