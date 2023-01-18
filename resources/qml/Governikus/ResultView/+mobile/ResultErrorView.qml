/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

ResultView {
	id: baseItem

	property string errorCode
	property alias errorDescription: textErrorDescription.text
	property bool errorDetailsShown: false
	readonly property bool hasErrorDetails: errorCode !== "" || errorDescription !== ""
	property alias mailButtonText: mailButton.text

	signal mailClicked

	onCancelClicked: continueClicked()
	onVisibleChanged: errorDetailsShown = false

	GButton {
		Accessible.name: qsTr("Show error details")
		anchors.horizontalCenter: parent.horizontalCenter
		buttonColor: Style.color.transparent
		text: qsTr("Details") + (baseItem.errorDetailsShown ? "▲" : "▼")
		textStyle: Style.text.normal_accent
		visible: baseItem.hasErrorDetails

		onClicked: baseItem.errorDetailsShown = !baseItem.errorDetailsShown
	}
	GSeparator {
		visible: baseItem.errorDetailsShown

		anchors {
			left: parent.left
			right: parent.right
		}
	}
	GText {
		text: qsTr("Error code: %1").arg(errorCode)
		visible: baseItem.errorDetailsShown

		anchors {
			left: parent.left
			right: parent.right
		}
	}
	GText {
		id: textErrorDescription
		visible: baseItem.errorDetailsShown

		anchors {
			left: parent.left
			right: parent.right
			topMargin: Constants.pane_spacing
		}
	}
	GButton {
		id: mailButton
		anchors.horizontalCenter: parent.horizontalCenter
		icon.source: "qrc:///images/material_mail.svg"
		tintIcon: true
		visible: text !== ""

		onClicked: mailClicked()
	}
}
