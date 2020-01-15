/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


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
	//: LABEL DESKTOP_QML
	placeholderText: qsTr("Search") + SettingsModel.translationTrigger

	Image {
		id: glassIcon

		sourceSize.height: parent.height - 2 * anchors.leftMargin
		sourceSize.width: height
		anchors.left: parent.left
		anchors.leftMargin: parent.height / 8
		anchors.verticalCenter: parent.verticalCenter

		source: "qrc:///images/search_icon.svg"
	}

	Image {
		id: iconItem

		visible: parent.displayText !== ""
		height: parent.height - 2 * anchors.rightMargin
		width: height
		anchors.right: parent.right
		anchors.rightMargin: parent.height / 8
		anchors.verticalCenter: parent.verticalCenter

		activeFocusOnTab: true
		Accessible.role: Accessible.Button
		Accessible.name: qsTr("Clear") + SettingsModel.translationTrigger

		source: "qrc:///images/search_cancel.svg"
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
