/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import Governikus.Global
import Governikus.ProgressView
import Governikus.TitleBar
import Governikus.Type
import Governikus.View
import Governikus.Workflow

SectionPage {
	id: root

	readonly property bool cardNotFound: checkIDCardModel.result <= CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED
	required property int readerType
	property bool usedInOnboarding: false

	signal cancel
	signal checkSuccess
	signal pinDeactivated
	signal restartCheck
	signal showRemoteServiceSettings

	contentIsScrolled: nfcWorkflow.visible && !nfcWorkflow.atYBeginning || progressView.visible && progressView.contentIsScrolled

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancel()
	}

	Component.onCompleted: checkIDCardModel.startScan(readerType)

	CheckIDCardModel {
		id: checkIDCardModel

		onFireScanCompleted: timerHelper.start()
	}
	Timer {
		id: timerHelper

		interval: 1

		onTriggered: root.push(checkIDCardResultView, {
			pluginType: root.readerType,
			result: checkIDCardModel.result
		})
	}
	ProgressTracker {
		id: progressTracker

		baseProgressTracker: root.progress
		currentStage: 1
		currentStep: 5
		steps: 6
	}
	Component {
		id: checkIDCardResultView

		CheckIDCardResultView {
			progress: progressTracker
			title: root.title
			usedInOnboarding: root.usedInOnboarding

			navigationAction: NavigationAction {
				action: root.navigationAction.action

				onClicked: {
					root.pop(root);
					root.leaveView();
				}
			}

			onCancelClicked: root.cancel()
			onContinueClicked: {
				switch (checkIDCardModel.result) {
				case CheckIDCardModel.Result.PIN_DEACTIVATED:
					root.push(cardNotActivatedView);
					break;
				case CheckIDCardModel.Result.SUCCESS:
					root.checkSuccess();
					break;
				default:
					root.push(checkIDCardSuggestionView, {
						result: checkIDCardModel.result
					});
				}
			}
		}
	}
	Component {
		id: cardNotActivatedView

		CardNotActivatedView {
			continueButtonVisible: root.usedInOnboarding
			progress: progressTracker
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onContinueClicked: root.pinDeactivated()
		}
	}
	Component {
		id: checkIDCardSuggestionView

		CheckIDCardSuggestionView {
			progress: progressTracker
			usedInOnboarding: root.usedInOnboarding

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onCancelClicked: root.cancel()
			onCheckSuccess: root.checkSuccess()
			onRestartCheck: {
				root.pop(root);
				root.restartCheck();
			}
		}
	}
	Component {
		id: nfcConnectionInfoView

		NfcConnectionInfoView {
			progress: root.progress
		}
	}
	NfcWorkflow {
		id: nfcWorkflow

		anchors.fill: parent
		visible: root.readerType === ReaderManagerPluginType.NFC && root.cardNotFound

		onShowNfcInformation: root.push(nfcConnectionInfoView)
		onShowRemoteServiceSettings: root.showRemoteServiceSettings()
		onStartScanIfNecessary: checkIDCardModel.startScanIfNecessary()
	}
	RemoteWorkflow {
		id: remoteWorkflow

		anchors.fill: parent
		foundSelectedReader: RemoteServiceModel.connectedServerDeviceNames !== ""
		visible: root.readerType === ReaderManagerPluginType.REMOTE_IFD && root.cardNotFound

		onFoundSelectedReaderChanged: {
			if (visible && !foundSelectedReader) {
				root.cancel();
			}
		}
		onShowRemoteServiceSettings: root.showRemoteServiceSettings()
	}
	ProgressView {
		id: progressView

		anchors.fill: parent

		//: LABEL ANDROID IOS
		headline: qsTr("Checking ID card")
		//: LABEL ANDROID IOS
		text: qsTr("Please do not move the ID card.")
		title: root.title
		visible: checkIDCardModel.result > CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED
	}
}
