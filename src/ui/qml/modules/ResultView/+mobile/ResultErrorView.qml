/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.TitleBar
import Governikus.Style

ResultView {
	id: root

	property string errorCode
	property alias errorDescription: textErrorDescription.text
	property bool errorDetailsShown: false
	readonly property bool hasErrorDetails: errorCode !== "" || errorDescription !== ""

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel
		enabled: false
	}

	onCancelClicked: continueClicked()
	onVisibleChanged: errorDetailsShown = false

	GCollapsible {
		Layout.fillWidth: true
		Layout.leftMargin: -Style.dimens.pane_padding * 2
		Layout.rightMargin: -Style.dimens.pane_padding * 2
		horizontalMargin: Style.dimens.pane_padding * 2
		//: MOBILE
		title: qsTr("Show Details")
		visible: root.hasErrorDetails

		GText {
			font.weight: Style.font.bold
			text: root.errorCode
		}
		GText {
			id: textErrorDescription

		}
	}
}
