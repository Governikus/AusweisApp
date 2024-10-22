/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar

ResultView {
	id: root

	property string errorCode
	property alias errorDescription: textErrorDescription.text
	property bool errorDetailsShown: false
	readonly property bool hasErrorDetails: errorCode !== "" || errorDescription !== ""
	property alias mailButtonText: mailButton.text

	signal mailClicked

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel
		enabled: false
	}

	onCancelClicked: continueClicked()
	onVisibleChanged: errorDetailsShown = false

	GCollapsible {
		Layout.fillWidth: true
		Layout.leftMargin: -Constants.pane_padding * 2
		Layout.rightMargin: -Constants.pane_padding * 2
		arrowToLeft: true
		horizontalMargin: Constants.pane_padding * 2
		//: LABEL ANDROID IOS
		title: qsTr("Show Details")
		visible: hasErrorDetails

		GText {
			font.bold: true
			//: LABEL ANDROID IOS
			text: "%1 %2".arg(qsTr("Error code:")).arg(errorCode)
		}
		GText {
			id: textErrorDescription

		}
	}
	GButton {
		id: mailButton

		Layout.alignment: Qt.AlignHCenter
		icon.source: "qrc:///images/email_icon.svg"
		tintIcon: true
		visible: text !== ""

		onClicked: mailClicked()
		onFocusChanged: if (focus)
			root.positionViewAtItem(this)
	}
}
