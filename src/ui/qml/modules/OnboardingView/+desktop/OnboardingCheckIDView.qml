/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
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

	readonly property bool isPcsc: pluginType === ReaderManagerPluginType.ReaderManagerPluginType.PCSC
	required property int pluginType

	signal pinDeactivated

	function stopScanAndLeaveView() {
		checkIDCardModel.stopScan();
		root.leaveView();
	}

	spacing: Style.dimens.pane_spacing

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back
		startEnabled: false

		onNavigationActionClicked: root.stopScanAndLeaveView()
	}

	Component.onCompleted: checkIDCardModel.startScan(root.pluginType)

	Connections {
		function onFireConnectedServerDeviceNamesChanged() {
			if (RemoteServiceModel.connectedServerDeviceNames === "") {
				root.stopScanAndLeaveView();
			}
		}

		enabled: checkIDCardModel.result <= CheckIDCardModel.Result.CARD_NOT_DETECTED
		target: RemoteServiceModel
	}
	CheckIDCardModel {
		id: checkIDCardModel

		onFireScanCompleted: timerHelper.start()
	}
	Timer {
		id: timerHelper

		interval: 1

		onTriggered: root.push(checkIDCardResultView, {
			pluginType: root.pluginType,
			result: checkIDCardModel.result
		})
	}
	Component {
		id: checkIDCardResultView

		CheckIDCardResultView {
			id: resultView

			//: LABEL DESKTOP
			header: qsTr("Read ID card")

			//: LABEL DESKTOP
			subheader: qsTr("Test result")
			title: root.title
			usedInOnboarding: true

			progress: ProgressTracker {
				baseProgressTracker: root.progress
				currentStep: 2
				steps: 3
			}
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: false

				onNavigationActionClicked: {
					root.pop();
					root.leaveView();
				}
			}

			onLeaveView: {
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

			progress: ProgressTracker {
				baseProgressTracker: root.progress
				currentStep: 2
				steps: 3
			}
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: false

				onNavigationActionClicked: root.pop()
			}

			onContinueClicked: root.pinDeactivated()
		}
	}
	Component {
		id: checkIDCardSuggestionView

		CheckIDCardSuggestionView {
			usedInOnboarding: true

			progress: ProgressTracker {
				baseProgressTracker: root.progress
				currentStep: 2
				steps: 3
			}
			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back
				startEnabled: false

				onNavigationActionClicked: root.pop()
			}

			onCheckSuccess: root.continueOnboarding()
			onRestartCheck: {
				root.pop(root);
				checkIDCardModel.startScan(root.pluginType);
			}
		}
	}
	GText {
		Layout.alignment: Text.AlignHCenter
		//: INFO DESKTOP
		text: qsTr("Read ID card")
		textStyle: Style.text.headline
	}
	AnimationLoader {
		Layout.alignment: Qt.AlignHCenter
		animated: false
		symbol: Symbol.Type.QUESTION
		type: root.isPcsc ? AnimationLoader.WAIT_FOR_CARD_USB : AnimationLoader.WAIT_FOR_CARD_SAC
	}
	GText {
		id: subText

		Layout.alignment: Text.AlignHCenter
		//: INFO DESKTOP
		text: root.isPcsc ? qsTr("Please ensure that the ID card is placed on the card reader.") :
		//: INFO DESKTOP
		qsTr("Please follow the instructions on your smartphone.")
	}
	GSpacer {
		Layout.fillHeight: true
	}
	RowLayout {
		Layout.alignment: Qt.AlignHCenter
		spacing: Style.dimens.text_spacing
		visible: root.isPcsc && checkIDCardModel.result <= CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED

		GText {
			//: INFO DESKTOP
			text: qsTr("Looking for ID card")
			textStyle: Style.text.link
		}
		SpinningLoader {
		}
	}
}
