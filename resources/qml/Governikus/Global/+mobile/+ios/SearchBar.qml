/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	readonly property alias searchText: searchField.displayText

	function reset() {
		searchField.clear();
		if (searchField.visible) {
			Qt.inputMethod.hide();
		}
	}

	Accessible.description: qsTr("Enter search string")
	Accessible.role: Accessible.EditableText
	height: Style.dimens.searchbar_height

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
					Qt.inputMethod.hide();
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
				duration: 200
				easing.type: Easing.InOutQuad
			}
		}
	]

	Rectangle {
		border.color: searchField.valid ? Style.color.border : Constants.red
		border.width: Style.dimens.separator_size
		color: searchField.enabled ? Style.color.background_pane : Constants.grey
		radius: Style.dimens.button_radius

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: Constants.groupbox_spacing
			right: cancelButton.left
			top: parent.top
		}
		Image {
			id: glassIcon
			height: parent.height - 2 * anchors.leftMargin
			source: "qrc:///images/material_search.svg"
			sourceSize.width: width
			width: height

			anchors {
				left: parent.left
				leftMargin: Constants.groupbox_spacing / 2
				verticalCenter: parent.verticalCenter
			}
		}
		GTextField {
			id: searchField
			Accessible.name: displayText
			Accessible.role: Accessible.EditableText
			EnterKey.type: Qt.EnterKeySearch
			horizontalAlignment: Text.AlignLeft
			leftPadding: 0
			placeholderText: qsTr("Search providers")
			rightPadding: 0

			background: Item {
			}

			onVisibleChanged: {
				if (visible === false) {
					Qt.inputMethod.hide();
				}
			}

			anchors {
				left: glassIcon.right
				leftMargin: Constants.groupbox_spacing / 2
				right: textEditX.left
				verticalCenter: parent.verticalCenter
			}
		}
		MouseArea {
			id: textEditX
			Accessible.ignored: !visible
			Accessible.name: qsTr("Clear search string")
			Accessible.role: Accessible.Button
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			cursorShape: Qt.PointingHandCursor
			height: parent.height
			visible: searchField.length > 0
			width: height

			Accessible.onPressAction: textEditX.clicked(null)
			onClicked: searchField.clear()

			Image {
				anchors.fill: parent
				anchors.margins: Constants.groupbox_spacing / 2
				source: "qrc:///images/ios/material_cancel.svg"
			}
		}
	}
	GButton {
		id: cancelButton
		buttonColor: Style.color.transparent
		//: LABEL IOS
		text: qsTr("Cancel")
		visible: false
		width: visible ? implicitWidth : 0

		onClicked: {
			searchField.clear();
			Qt.inputMethod.hide();
		}

		anchors {
			right: parent.right
			rightMargin: visible ? Constants.groupbox_spacing : 0
			verticalCenter: parent.verticalCenter
		}
	}
}
