/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0


Item {
	id: baseItem

	readonly property alias searchText: searchField.displayText
	function reset() {
		searchField.clear()
		if (searchField.visible) {
			Qt.inputMethod.hide()
		}
	}

	height: Style.dimens.searchbar_height

	Accessible.role: Accessible.EditableText
	Accessible.description: qsTr("Enter search string")

	Rectangle {
		anchors {
			left: parent.left
			right: cancelButton.left
			top: parent.top
			bottom: parent.bottom
			margins: Constants.groupbox_spacing
		}

		radius: Style.dimens.button_radius
		color: searchField.enabled ? Style.color.background_pane : Constants.grey
		border.color: searchField.valid ? Style.color.border : Constants.red
		border.width: Style.dimens.separator_size

		Image {
			id: glassIcon

			height: parent.height - 2 * anchors.leftMargin
			width: height
			anchors {
				left: parent.left
				leftMargin: Constants.groupbox_spacing / 2
				verticalCenter: parent.verticalCenter
			}
			sourceSize.width: width
			source: "qrc:///images/material_search.svg"
		}

		GTextField {
			id: searchField

			anchors {
				verticalCenter: parent.verticalCenter
				left: glassIcon.right
				right: textEditX.left
				leftMargin: Constants.groupbox_spacing / 2
			}
			leftPadding: 0
			rightPadding: 0

			Accessible.role: Accessible.EditableText
			Accessible.name: displayText

			horizontalAlignment: Text.AlignLeft
			background: Item {}
			EnterKey.type: Qt.EnterKeySearch

			placeholderText: qsTr("Search providers")

			onVisibleChanged: {
				if (visible === false){
					Qt.inputMethod.hide()
				}
			}
		}

		MouseArea {
			id: textEditX

			visible: searchField.length > 0
			height: parent.height
			width: height
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter

			Accessible.role: Accessible.Button
			Accessible.name: qsTr("Clear search string")
			Accessible.onPressAction: textEditX.clicked(null)
			Accessible.ignored: !visible

			onClicked: searchField.clear()
			cursorShape: Qt.PointingHandCursor

			Image {
				anchors.margins: Constants.groupbox_spacing / 2
				anchors.fill: parent
				source: "qrc:///images/ios/material_cancel.svg"
			}
		}
	}

	GButton {
		id: cancelButton

		visible: false
		width: visible ? implicitWidth : 0
		anchors {
			right: parent.right
			rightMargin: visible ? Constants.groupbox_spacing : 0
			verticalCenter: parent.verticalCenter
		}

		buttonColor: Style.color.transparent
		//: LABEL IOS
		text: qsTr("Cancel")
		onClicked: {
			searchField.clear()
			Qt.inputMethod.hide()
		}
	}

	states: [
		State {
			name: "searchBarActive"
			when: searchField.activeFocus || searchField.text.trim().length !== 0

			PropertyChanges {
				target: cancelButton
				visible: true
			}
		},
		State {
			name: ""
			StateChangeScript {
				script: {
					Qt.inputMethod.hide()
				}
			}

			PropertyChanges {
				target: cancelButton
				visible: false
			}
		}
	]

	transitions: [
		Transition {
			from: "*"
			to: "*"
			AnchorAnimation {
				duration: 200;
				easing.type: Easing.InOutQuad
			}
		}
	]
}
