/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0


GPane {
	id: root

	property alias text: hintText.text
	property alias buttonText: hintButton.text
	property alias buttonIconSource: hintButton.icon.source

	signal clicked()

	ColumnLayout {
		width: parent.width

		spacing: 0

		GText {
			Layout.fillWidth: true

			activeFocusOnTab: true
			textStyle: Style.text.normal
			text: qsTr("Hint")
			font.bold: true

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GText {
			id: hintText

			Layout.fillWidth: true
			Layout.topMargin: Constants.text_spacing

			activeFocusOnTab: true
			textStyle: Style.text.normal

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GButton {
			id: hintButton

			visible: text !== ""

			Layout.alignment: Qt.AlignRight
			Layout.topMargin: Constants.component_spacing

			icon.source: "qrc:///images/material_open_in_new.svg"
			onClicked: root.clicked()
		}
	}
}
