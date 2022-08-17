/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

ResultView {
	id: baseItem

	readonly property bool hasErrorDetails: errorCode !== "" || errorDescription !== ""
	property alias errorDescription: textErrorDescription.text
	property alias mailButtonText: mailButton.text
	property bool errorDetailsShown: false
	property string errorCode
	signal mailClicked()

	onCancelClicked: continueClicked()
	onVisibleChanged: errorDetailsShown = false

	GButton {
		visible: baseItem.hasErrorDetails

		anchors.horizontalCenter: parent.horizontalCenter

		Accessible.name: qsTr("Show error details")

		buttonColor: Style.color.transparent
		textStyle: Style.text.normal_accent
		text: qsTr("Details") + (baseItem.errorDetailsShown ? "▲" : "▼")
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
		visible: baseItem.errorDetailsShown

		anchors {
			left: parent.left
			right: parent.right
		}

		text: qsTr("Error code: %1").arg(errorCode)
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

		visible: text !== ""
		anchors.horizontalCenter: parent.horizontalCenter

		icon.source: "qrc:///images/material_mail.svg"
		tintIcon: true

		onClicked: mailClicked()
	}
}
