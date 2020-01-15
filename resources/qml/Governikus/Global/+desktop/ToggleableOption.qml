/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: root

	property alias text: optionText.text
	property alias checked: optionState.checked

	property TextStyle textStyle: enabled ? Style.text.normal : Style.text.normal_secondary

	implicitHeight: layout.implicitHeight
	implicitWidth: layout.implicitWidth

	activeFocusOnTab: true

	Accessible.name: root.text
	Accessible.role: Accessible.CheckBox

	Keys.onSpacePressed: optionState.toggle()

	RowLayout {
		id: layout

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		spacing: Constants.groupbox_spacing

		GCheckBox {
			id: optionState

			activeFocusOnTab: false
		}

		GText {
			id: optionText

			Layout.fillWidth: true
			activeFocusOnTab: false

			textStyle: root.textStyle
		}
	}

	FocusFrame {
		border.color: optionText.color
	}

	MouseArea {
		anchors.fill: parent
		onClicked: optionState.toggle()
		cursorShape: Qt.PointingHandCursor
	}
}
