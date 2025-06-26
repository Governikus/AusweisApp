/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
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
	//: LABEL ALL_PLATFORMS
	readonly property string hasCodeSubHeaderText: qsTr("Activate ID card with PIN reset letter")
	//: LABEL ALL_PLATFORMS
	readonly property string hasNoCodeSubHeaderText: qsTr("Request eID function activation")

	signal decisionHasCodeClicked
	signal decisionHasNoCodeClicked

	function pushSubView(hasCode, dict) {
		root.push(hasCode ? hasCodeView : hasNoCodeView, dict);
	}

	animationSymbol: Symbol.Type.BLOCK
	animationType: AnimationLoader.CARD_RESULT
	buttonText: ""
	//: LABEL ALL_PLATFORMS
	header: qsTr("eID function is not activated")
	//: LABEL ALL_PLATFORMS
	subheader: PinResetInformationModel.hasPinResetService ? qsTr("Did you recently order a PIN reset letter with an activation code?") : hasNoCodeSubHeaderText

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
				//: LABEL ALL_PLATFORMS
				description: qsTr("Activate your ID card using the activation code")
				isPane: true
				//: LABEL ALL_PLATFORMS
				text: qsTr("Yes, I already have an activation code")

				onClicked: root.decisionHasCodeClicked()
			}
			GInformativeButton {
				Layout.fillWidth: true
				//: LABEL ALL_PLATFORMS
				description: qsTr("Request the activation of the eID function")
				isPane: true
				//: LABEL ALL_PLATFORMS
				text: qsTr("No, I don't have an activation code")

				onClicked: root.decisionHasNoCodeClicked()
			}
			GContinueButton {
				//: LABEL ALL_PLATFORMS
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
				//: LABEL ALL_PLATFORMS
				text: qsTr("Enter your activation code of your present PIN reset letter into the following website.")
			}
			GButton {
				Layout.alignment: Qt.AlignHCenter
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ALL_PLATFORMS
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
				//: LABEL ALL_PLATFORMS
				buttonText: qsTr("Request action code")
				//: LABEL ALL_PLATFORMS
				text: qsTr("You can request a PIN reset letter with an activation code on the following website.")
				//: LABEL ALL_PLATFORMS
				title: qsTr("Online via PIN reset service")
				visible: PinResetInformationModel.hasPinResetService

				onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
			}
			Hint {
				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				//: LABEL ALL_PLATFORMS
				buttonText: qsTr("Find competent authority")
				//: LABEL ALL_PLATFORMS
				text: qsTr("You can activate the eID function directly at your competent authority.")
				//: LABEL ALL_PLATFORMS
				title: qsTr("At your competent authority")

				onClicked: Qt.openUrlExternally(PinResetInformationModel.administrativeSearchUrl)
			}
			GContinueButton {
				//: LABEL ALL_PLATFORMS
				text: qsTr("Abort setup")
				visible: root.continueButtonVisible && !PinResetInformationModel.hasPinResetService

				onClicked: root.continueClicked()
			}
		}
	}
}
