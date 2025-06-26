/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.TitleBar
import Governikus.MultiInfoView
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	property bool activateUI: true
	property bool autoInsertCard: false
	property Component cardNotActivatedDelegate: null
	property Component errorViewDelegate: null
	property bool hidePinTypeSelection: false
	property bool hideTechnologySwitch: false
	property var initialPlugin: null
	readonly property bool isSmartWorkflow: ChangePinModel.readerPluginType === ReaderManagerPluginType.SMART
	property int navigationActionType: NavigationAction.Action.Cancel
	property bool onlyCheckPin: false
	property bool skipInfoView: true
	property bool skipProgressView: false
	property Component successViewDelegate: null
	property bool usedInOnboarding: false

	signal abortWithUnknownPin
	signal close
	signal workflowFinished(bool pSuccess)

	function startPinChange(pChangeTransportPin) {
		workflowStartHandler.enabled = true;
		ChangePinModel.startWorkflow(pChangeTransportPin, root.activateUI, root.onlyCheckPin && !pChangeTransportPin);
	}

	contentIsScrolled: !changePinViewContent.atYBeginning
	smartEidUsed: isSmartWorkflow
	//: LABEL ANDROID IOS
	title: qsTr("Change PIN")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.close()
	}

	QtObject {
		id: d

		property bool oldLockedAndHiddenStatus
	}
	FadeInAnimation {
		target: root
	}
	Connections {
		function onActivate() {
			changePinViewContent.highlightScrollbar();
		}
	}
	Connections {
		id: workflowStartHandler

		function onFireWorkflowStarted() {
			changePinController.createObject(root);
			d.oldLockedAndHiddenStatus = root.getLockedAndHidden();
			root.setLockedAndHidden(true);
			ChangePinModel.setInitialPluginType();
			enabled = false;
		}

		enabled: false
		target: ChangePinModel
	}
	Component {
		id: changePinController

		ChangePinController {
			autoInsertCard: root.autoInsertCard
			cardNotActivatedDelegate: root.cardNotActivatedDelegate
			errorViewDelegate: root.errorViewDelegate
			hideTechnologySwitch: root.hideTechnologySwitch
			initialPlugin: root.initialPlugin
			navigationActionType: root.navigationActionType
			skipProgressView: root.skipProgressView
			smartEidUsed: root.smartEidUsed
			stackView: root.stackView
			successViewDelegate: root.successViewDelegate
			title: root.title

			progress: ProgressTracker {
				baseProgressTracker: root.progress
				from: 0.3
			}

			onWorkflowFinished: pSuccess => {
				root.pop(root);
				root.setLockedAndHidden(d.oldLockedAndHiddenStatus);
				root.workflowFinished(pSuccess);
				this.destroy();
			}
		}
	}
	ChangePinViewContent {
		id: changePinViewContent

		anchors.fill: parent
		visible: !root.hidePinTypeSelection

		onChangePin: {
			if (root.skipInfoView) {
				root.startPinChange(false);
			} else {
				root.push(changePinInfoView);
			}
		}
		onChangePinInfoRequested: root.push(pinTypeInfoView)
		onChangeTransportPin: {
			if (root.skipInfoView) {
				root.startPinChange(true);
			} else {
				root.push(changeTransportPinInfoView);
			}
		}
		onNoPinAvailable: {
			d.oldLockedAndHiddenStatus = root.getLockedAndHidden();
			root.setLockedAndHidden();
			root.push(pinUnknownView);
		}
	}
	ProgressTracker {
		id: pinInfoProgress

		baseProgressTracker: root.progress
		relativeProgress: 0.1
	}
	Component {
		id: changePinInfoView

		ChangePinInfoView {
			progress: pinInfoProgress
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onContinueClicked: root.startPinChange(false)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: changeTransportPinInfoView

		ChangeTransportPinInfoView {
			progress: pinInfoProgress
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onContinueClicked: root.startPinChange(true)
			onLeaveView: root.pop()
		}
	}
	Component {
		id: pinTypeInfoView

		MultiInfoView {
			infoContent: changePinViewContent.pinInfo
			progress: root.progress

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onAbortCurrentWorkflow: ChangePinModel.cancelWorkflow()
		}
	}
	Component {
		id: pinUnknownView

		MultiInfoView {
			continueButtonText: root.usedInOnboarding ? qsTr("Abort setup") : ""

			infoContent: MultiInfoData {
				contentType: MultiInfoData.Type.NO_PIN
			}
			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: {
					root.pop();
					root.setLockedAndHidden(d.oldLockedAndHiddenStatus);
				}
			}
			progress: ProgressTracker {
				baseProgressTracker: pinInfoProgress
				relativeProgress: 1
			}

			onContinueClicked: {
				if (root.usedInOnboarding) {
					root.abortWithUnknownPin();
				}
			}
		}
	}
}
