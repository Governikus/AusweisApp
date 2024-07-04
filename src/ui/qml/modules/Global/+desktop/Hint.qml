/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

GPane {
	id: root

	property alias buttonIconSource: hintButton.icon.source
	property alias buttonText: hintButton.text
	property alias buttonTooltip: hintButton.enabledTooltipText
	property alias text: hintText.text

	signal clicked

	color: Style.color.paneSublevel.background.basic
	drawShadow: false
	spacing: 0

	GText {
		id: hintText

		Layout.topMargin: Constants.text_spacing
		activeFocusOnTab: true
		visible: text !== ""

		FocusFrame {
		}
	}
	GButton {
		id: hintButton

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.component_spacing
		icon.source: "qrc:///images/open_website.svg"
		tintIcon: hintText.color
		visible: text !== ""

		onClicked: root.clicked()
	}
}
