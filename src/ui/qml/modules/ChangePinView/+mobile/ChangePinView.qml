/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.EnterPasswordView
import Governikus.Global
import Governikus.AuthView
import Governikus.Style
import Governikus.TitleBar
import Governikus.PasswordInfoView
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.View
import Governikus.Workflow
import Governikus.Type

SectionPage {
	id: baseItem

	property bool autoInsertCard: false
	property bool hidePinTypeSelection: false
	property bool hideTechnologySwitch: false
	property var initialPlugin: null
	readonly property bool isSmartWorkflow: ChangePinModel.readerPluginType === ReaderManagerPluginType.SMART

	signal close
	signal workflowFinished

	contentIsScrolled: !changePinViewContent.atYBeginning
	smartEidUsed: isSmartWorkflow
	//: LABEL ANDROID IOS
	title: qsTr("Change PIN")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: baseItem.close()
	}

	FadeInAnimation {
		target: baseItem
	}
	Connections {
		function onActivate() {
			changePinViewContent.highlightScrollbar();
		}
	}
	Connections {
		function onFireWorkflowStarted() {
			changePinController.createObject(baseItem);
			setLockedAndHidden(true);
			ChangePinModel.setInitialPluginType();
		}

		enabled: visible
		target: ChangePinModel
	}
	Component {
		id: changePinController

		ChangePinController {
			autoInsertCard: baseItem.autoInsertCard
			hideTechnologySwitch: baseItem.hideTechnologySwitch
			initialPlugin: baseItem.initialPlugin
			smartEidUsed: baseItem.smartEidUsed
			stackView: baseItem.stackView
			title: baseItem.title

			onWorkflowFinished: {
				baseItem.workflowFinished();
				this.destroy();
			}
		}
	}
	ChangePinViewContent {
		id: changePinViewContent

		anchors.fill: parent
		visible: !baseItem.hidePinTypeSelection

		onChangePin: ChangePinModel.startWorkflow(false)
		onChangePinInfoRequested: push(changePinInfoView)
		onChangeTransportPin: ChangePinModel.startWorkflow(true)
		onNoPinAvailable: {
			setLockedAndHidden();
			push(pinUnknownView);
		}
	}
	Component {
		id: changePinInfoView

		PasswordInfoView {
			infoContent: changePinViewContent.pinInfo

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: pop()
			}

			onAbortCurrentWorkflow: ChangePinModel.cancelWorkflow()
		}
	}
	Component {
		id: pinUnknownView

		PasswordInfoView {
			infoContent: PasswordInfoData {
				contentType: PasswordInfoData.Type.NO_PIN
			}
			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: {
					pop();
					setLockedAndHidden(false);
				}
			}
		}
	}
}
