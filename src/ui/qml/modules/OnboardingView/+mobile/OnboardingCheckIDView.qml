/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.CheckIDCardView
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

BaseOnboardingView {
	id: root

	readonly property bool connectedDeviceNamesEmpty: RemoteServiceModel.connectedServerDeviceNames === ""
	required property int pluginType

	signal pinDeactivated

	spacing: Style.dimens.pane_spacing

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.leaveView()
	}

	Component.onCompleted: checkIDCardModel.startScan(root.pluginType)

	CheckIDCardModel {
		id: checkIDCardModel

		onFireScanCompleted: {
			checkConnection.enabled = false;
			timerHelper.start();
		}
	}
	Timer {
		id: timerHelper

		interval: 1

		onTriggered: root.push(checkIDCardResultView, {
			pluginType: root.pluginType,
			result: checkIDCardModel.result
		})
	}
	Connections {
		id: checkConnection

		function onFireConnectedServerDisconnected() {
			if (root.connectedDeviceNamesEmpty) {
				root.leaveView();
			}
		}

		target: RemoteServiceModel
	}
	Component {
		id: checkIDCardResultView

		CheckIDCardResultView {
			id: resultView

			//: MOBILE
			header: qsTr("Read ID card")

			//: MOBILE
			subheader: qsTr("Test result")
			title: root.title
			usedInOnboarding: true

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.leaveView()
			}
			progress: ProgressTracker {
				baseProgressTracker: root.progress
				currentStage: 1
				currentStep: 5
				steps: 6
			}

			onContinueClicked: {
				switch (result) {
				case CheckIDCardModel.Result.PIN_DEACTIVATED:
					root.push(cardNotActivatedView);
					break;
				case CheckIDCardModel.Result.SUCCESS:
					root.pop();
					root.continueOnboarding();
					break;
				default:
					root.push(checkIDCardSuggestionView, {
						result: resultView.result
					});
				}
			}
		}
	}
	Component {
		id: cardNotActivatedView

		CardNotActivatedView {
			continueButtonVisible: true
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}
			progress: ProgressTracker {
				baseProgressTracker: root.progress
				currentStage: 1
				currentStep: 5
				steps: 6
			}

			onContinueClicked: root.pinDeactivated()
		}
	}
	Component {
		id: checkIDCardSuggestionView

		CheckIDCardSuggestionView {
			usedInOnboarding: true

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}
			progress: ProgressTracker {
				baseProgressTracker: root.progress
				currentStage: 1
				currentStep: 5
				steps: 6
			}

			onCheckSuccess: root.continueOnboarding()
			onRestartCheck: {
				root.pop(root);
				checkConnection.enabled = true;
				checkIDCardModel.startScan(root.pluginType);
			}
		}
	}
	Heading {
		//: MOBILE
		text: qsTr("Read ID card")
	}
	AnimationLoader {
		Layout.alignment: Qt.AlignHCenter
		animated: false
		symbol: Symbol.Type.QUESTION
		type: AnimationLoader.Type.WAIT_FOR_CARD_SAC
	}
	GText {
		Layout.alignment: Text.AlignHCenter
		//: MOBILE
		text: root.connectedDeviceNamesEmpty ? "" : qsTr("Connected to %1. Please follow the instructions on your smartphone.").arg(RemoteServiceModel.connectedServerDeviceNames)
	}
	GSpacer {
		Layout.fillHeight: true
	}
	RowLayout {
		Layout.alignment: Qt.AlignHCenter
		spacing: Style.dimens.text_spacing
		visible: checkIDCardModel.result <= CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED

		GText {
			//: MOBILE
			text: root.connectedDeviceNamesEmpty ? qsTr("Looking for device") : qsTr("Looking for ID card")
			textStyle: Style.text.link
		}
		SpinningLoader {
		}
	}
}
