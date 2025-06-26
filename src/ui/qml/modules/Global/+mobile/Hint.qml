/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

GPane {
	id: root

	property alias buttonIconSource: hintButton.icon.source
	property alias buttonText: hintButton.text
	property alias text: hintText.text

	signal clicked

	color: Style.color.paneSublevel.background.basic
	drawShadow: false
	textStyle: Style.text.subline

	ColumnLayout {
		spacing: 0
		width: parent.width

		GText {
			id: hintText

			visible: text !== ""
		}
		GButton {
			id: hintButton

			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Style.dimens.groupbox_spacing
			icon.source: "qrc:///images/open_website.svg"
			tintIcon: true
			visible: text !== ""

			onClicked: root.clicked()
		}
	}
}
