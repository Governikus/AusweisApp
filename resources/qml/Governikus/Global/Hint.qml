/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

GPane {
	id: root

	property alias buttonIconSource: hintButton.icon.source
	property alias buttonText: hintButton.text
	property alias text: hintText.text

	signal clicked

	title: qsTr("Hint")

	ColumnLayout {
		spacing: 0
		width: parent.width

		GText {
			id: hintText
			Layout.fillWidth: true
			Layout.topMargin: Constants.text_spacing
			activeFocusOnTab: true
			textStyle: Style.text.normal
			visible: text !== ""

			FocusFrame {
			}
		}
		GButton {
			id: hintButton
			Layout.alignment: Qt.AlignRight
			Layout.topMargin: Constants.component_spacing
			icon.source: "qrc:///images/material_open_in_new.svg"
			visible: text !== ""

			onClicked: root.clicked()
		}
	}
}
