/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


GTextField {
	id: searchField

	function clear() {
		searchField.text = ""
		searchField.forceActiveFocus()
	}

	width: 400 * ApplicationModel.scaleFactor

	Accessible.name: placeholderText

	leftPadding: glassIcon.width + Constants.groupbox_spacing
	rightPadding: iconItem.width + Constants.groupbox_spacing

	textStyle: Style.text.normal
	//: LABEL DESKTOP
	placeholderText: qsTr("Search")

	TintableIcon {
		id: glassIcon

		sourceSize.height: parent.height - 2 * anchors.leftMargin
		sourceSize.width: height
		anchors.left: parent.left
		anchors.leftMargin: parent.height / 8
		anchors.verticalCenter: parent.verticalCenter

		tintColor: Style.color.primary_text
		source: "qrc:///images/material_search.svg"
	}

	TintableIcon {
		id: iconItem

		visible: parent.displayText !== ""
		sourceSize.height: parent.height - 2 * anchors.rightMargin
		sourceSize.width: height
		anchors.right: parent.right
		anchors.rightMargin: parent.height / 8
		anchors.verticalCenter: parent.verticalCenter

		activeFocusOnTab: true
		Accessible.role: Accessible.Button
		Accessible.name: qsTr("Clear")

		tintColor: Style.color.accent
		source: "qrc:///images/material_close.svg"
		Keys.onSpacePressed: searchField.clear()

		MouseArea {
			anchors.fill: parent

			cursorShape: Qt.PointingHandCursor

			onClicked: searchField.clear()
		}

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}
}
