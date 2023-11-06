/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
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
	property alias text: hintText.text

	signal clicked

	bold: true
	color: Style.color.pane_sublevel
	drawShadow: false
	textStyle: Style.text.subline
	//: LABEL ANDROID IOS
	title: qsTr("Hint")

	ColumnLayout {
		spacing: 0
		width: parent.width

		GText {
			id: hintText

			activeFocusOnTab: true
			visible: text !== ""

			FocusFrame {
			}
		}
		GButton {
			id: hintButton

			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.groupbox_spacing
			icon.source: "qrc:///images/open_website.svg"
			iconSize: Style.dimens.navigation_bar_icon_size
			tintIcon: hintText.color
			visible: text !== ""

			onClicked: root.clicked()
		}
	}
}
