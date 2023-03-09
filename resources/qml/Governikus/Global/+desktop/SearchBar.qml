/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

GTextField {
	id: searchField
	function clear() {
		searchField.text = "";
		searchField.forceActiveFocus(Qt.MouseFocusReason);
	}

	Accessible.name: placeholderText
	isOnLightBackground: false
	leftPadding: glassIcon.width + Constants.groupbox_spacing
	//: LABEL DESKTOP
	placeholderText: qsTr("Search")
	rightPadding: iconItem.width + Constants.groupbox_spacing
	textStyle: Style.text.normal
	width: 400 * ApplicationModel.scaleFactor

	TintableIcon {
		id: glassIcon
		anchors.left: parent.left
		anchors.leftMargin: parent.height / 8
		anchors.verticalCenter: parent.verticalCenter
		source: "qrc:///images/material_search.svg"
		sourceSize.height: parent.height - 2 * anchors.leftMargin
		sourceSize.width: height
		tintColor: Style.color.primary_text
	}
	TintableIcon {
		id: iconItem
		Accessible.name: qsTr("Clear")
		Accessible.role: Accessible.Button
		activeFocusOnTab: true
		anchors.right: parent.right
		anchors.rightMargin: parent.height / 8
		anchors.verticalCenter: parent.verticalCenter
		source: "qrc:///images/material_close.svg"
		sourceSize.height: parent.height - 2 * anchors.rightMargin
		sourceSize.width: height
		tintColor: Style.color.accent
		visible: parent.displayText !== ""

		Keys.onSpacePressed: searchField.clear()

		MouseArea {
			anchors.fill: parent
			cursorShape: Qt.PointingHandCursor

			onClicked: searchField.clear()
		}
		FocusFrame {
			marginFactor: 0.1
		}
	}
}
