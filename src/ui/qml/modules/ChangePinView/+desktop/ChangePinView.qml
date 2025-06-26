/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.TitleBar
import Governikus.MultiInfoView
import Governikus.View
import Governikus.Type
import Governikus.Global

SectionPage {
	id: root

	property bool activateUI: true
	property Component cardNotActivatedDelegate: null
	property Component errorViewDelegate: null
	property int navigationActionType: NavigationAction.Cancel
	property bool onlyCheckPin: false
	property bool skipInfoView: true
	property Component successViewDelegate: null
	property bool usedInOnboarding: false

	signal abortWithUnknownPin
	signal workflowFinished(bool pSuccess)

	//: LABEL DESKTOP
	title: qsTr("Change PIN")

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back

		onNavigationActionClicked: root.pop()
	}

	ProgressTracker {
		id: progressTracker

		baseProgressTracker: root.progress
	}
	ChangePinViewContent {
		anchors.fill: parent

		onChangePin: {
			if (root.skipInfoView) {
				root.push(changePinController, {
					changeTransportPin: false
				});
			} else {
				root.push(changePinInfoView);
			}
		}
		onChangePinInfoRequested: root.push(multiInfoView, {
			passwordType: MultiInfoData.CHANGE_PIN,
			progress: progressTracker
		})
		onChangeTransportPin: {
			if (root.skipInfoView) {
				root.push(changePinController, {
					changeTransportPin: true
				});
			} else {
				root.push(changeTransportPinInfoView);
			}
		}
		onNoPinAvailable: root.push(multiInfoView, {
			passwordType: MultiInfoData.NO_PIN,
			progress: noPinProgress,
			continueButtonText: root.usedInOnboarding ? qsTr("Abort setup") : ""
		})

		ProgressTracker {
			id: noPinProgress

			baseProgressTracker: progressTracker
			relativeProgress: 1
		}
	}
	Component {
		id: changePinController

		ChangePinController {
			required property bool changeTransportPin

			cardNotActivatedDelegate: root.cardNotActivatedDelegate
			errorViewDelegate: root.errorViewDelegate
			navigationActionType: root.navigationActionType
			successViewDelegate: root.successViewDelegate
			title: root.title

			baseProgressTracker: ProgressTracker {
				baseProgressTracker: progressTracker
				from: 0.3
			}

			Component.onCompleted: ChangePinModel.startWorkflow(changeTransportPin, root.activateUI, root.onlyCheckPin && !changeTransportPin)
			onWorkflowFinished: pSuccess => {
				pop(root);
				root.workflowFinished(pSuccess);
				root.leaveView();
			}
		}
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			id: infoView

			required property int passwordType

			infoContent: MultiInfoData {
				contentType: infoView.passwordType
			}
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: root.titleBarSettings.startEnabled

				onNavigationActionClicked: root.pop()
			}

			onContinueClicked: {
				if (root.usedInOnboarding) {
					root.abortWithUnknownPin();
				}
			}
		}
	}
	ProgressTracker {
		id: pinInfoProgress

		baseProgressTracker: progressTracker
		relativeProgress: 0.2
	}
	Component {
		id: changePinInfoView

		ChangePinInfoView {
			progress: pinInfoProgress
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: root.titleBarSettings.startEnabled

				onNavigationActionClicked: root.pop()
			}

			onContinueClicked: root.push(changePinController, {
				changeTransportPin: false
			})
			onLeaveView: root.pop()
		}
	}
	Component {
		id: changeTransportPinInfoView

		ChangeTransportPinInfoView {
			progress: pinInfoProgress
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: root.titleBarSettings.startEnabled

				onNavigationActionClicked: root.pop()
			}

			onContinueClicked: root.push(changePinController, {
				changeTransportPin: true
			})
			onLeaveView: root.pop()
		}
	}
}
