/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0


GPane {
	id: root

	property alias text: hintText.text
	property alias buttonText: hintButton.text
	property alias buttonIconSource: hintButton.icon.source

	signal clicked()

	title: qsTr("Hint")

	ColumnLayout {
		width: parent.width

		spacing: 0

		GText {
			id: hintText

			visible: text !== ""

			Layout.fillWidth: true
			Layout.topMargin: Constants.text_spacing

			activeFocusOnTab: true
			textStyle: Style.text.normal

			FocusFrame {}
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
