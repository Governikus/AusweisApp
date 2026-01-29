/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.ResultView
import Governikus.Type

ResultView {
	id: root

	property bool continueButtonVisible: false
	//: ALL_PLATFORMS
	readonly property string hasCodeSubHeaderText: qsTr("Activate ID card with PIN Reset Letter")
	//: ALL_PLATFORMS
	readonly property string hasNoCodeSubHeaderText: qsTr("How do I activate the eID function?")

	signal decisionHasCodeClicked
	signal decisionHasNoCodeClicked

	function pushSubView(hasCode, dict) {
		root.push(hasCode ? hasCodeView : hasNoCodeView, dict);
	}

	animationSymbol: Symbol.Type.BLOCK
	animationType: AnimationLoader.Type.CARD_RESULT
	buttonText: ""
	//: ALL_PLATFORMS
	header: qsTr("eID function is not activated")
	//: ALL_PLATFORMS
	subheader: PinResetInformationModel.hasPinResetService ? qsTr("Did you recently order a PIN Reset Letter with an activation code?") : hasNoCodeSubHeaderText

	Loader {
		Layout.fillWidth: true
		sourceComponent: PinResetInformationModel.hasPinResetService ? decisionViewContent : hasNoCodeContent
	}
	Component {
		id: hasCodeView

		ResultView {
			animationSymbol: root.animationSymbol
			animationType: root.animationType
			buttonText: ""
			header: root.header
			progress: root.progress
			subheader: root.hasCodeSubHeaderText
			title: root.title

			Loader {
				Layout.fillWidth: true
				sourceComponent: hasCodeContent
			}
		}
	}
	Component {
		id: hasNoCodeView

		ResultView {
			animationSymbol: root.animationSymbol
			animationType: root.animationType
			buttonText: ""
			header: root.header
			progress: root.progress
			subheader: root.hasNoCodeSubHeaderText
			title: root.title

			Loader {
				Layout.fillWidth: true
				sourceComponent: hasNoCodeContent
			}
		}
	}
	Component {
		id: decisionViewContent

		ColumnLayout {
			spacing: root.spacing

			GInformativeButton {
				Layout.fillWidth: true
				//: ALL_PLATFORMS
				description: qsTr("Activate your ID card using the activation code")
				isPane: true
				//: ALL_PLATFORMS
				text: qsTr("Yes, I already have an activation code")

				onClicked: root.decisionHasCodeClicked()
			}
			GInformativeButton {
				Layout.fillWidth: true
				//: ALL_PLATFORMS
				description: qsTr("Request the activation of the eID function")
				isPane: true
				//: ALL_PLATFORMS
				text: qsTr("No, I don't have an activation code")

				onClicked: root.decisionHasNoCodeClicked()
			}
			GContinueButton {
				//: ALL_PLATFORMS
				text: qsTr("Abort setup")
				visible: root.continueButtonVisible

				onClicked: root.continueClicked()
			}
		}
	}
	Component {
		id: hasCodeContent

		ColumnLayout {
			spacing: root.spacing

			GText {
				//: ALL_PLATFORMS
				text: qsTr("Enter your activation code of your present PIN Reset Letter into the following website.")
			}
			GButton {
				Accessible.description: Utils.platformAgnosticLinkOpenText(PinResetInformationModel.pinResetActivationUrl, Accessible.name)
				Accessible.role: Accessible.Link
				Layout.alignment: Qt.AlignHCenter
				icon.source: "qrc:///images/open_website.svg"
				//: ALL_PLATFORMS
				text: qsTr("Enter activation code")
				tintIcon: true

				onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetActivationUrl)
			}
		}
	}
	Component {
		id: hasNoCodeContent

		ColumnLayout {
			spacing: root.spacing

			Hint {
				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				buttonText: PinResetInformationModel.resetPinWithPRSActionText
				linkToOpen: PinResetInformationModel.pinResetUrl
				text: PinResetInformationModel.activateOnlineFunctionForPRSHint
				title: PinResetInformationModel.resetPinWithPRSHintTitle
				visible: PinResetInformationModel.hasPinResetService
			}
			Hint {
				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				buttonText: PinResetInformationModel.resetPinAtAuthorityActionText
				linkToOpen: PinResetInformationModel.administrativeSearchUrl
				text: PinResetInformationModel.activateOnlineFunctionAtAuthorityHint
				title: PinResetInformationModel.resetPinAtAuthorityHintTitle
			}
			GContinueButton {
				//: ALL_PLATFORMS
				text: qsTr("Abort setup")
				visible: root.continueButtonVisible && !PinResetInformationModel.hasPinResetService

				onClicked: root.continueClicked()
			}
		}
	}
}
