/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	readonly property alias searchText: searchField.text

	id: baseItem
	height: Style.dimens.searchbar_height

	Accessible.role: Accessible.EditableText
	Accessible.description: qsTr("Enter search string") + SettingsModel.translationTrigger

	MouseArea {
		id: pageArea
		onClicked: pageArea.focus = true

		height: Style.dimens.searchbar_height
		width: parent.width
		anchors.bottom: parent.bottom

		Rectangle {
			anchors.left: parent.left
			anchors.right: cancelButton.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.leftMargin: 8
			anchors.rightMargin: 10
			anchors.topMargin: 6
			anchors.bottomMargin: 8
			radius: 6
			color: Constants.white

			Image {
				id: glassIcon
				height: parent.height - 2 * anchors.leftMargin
				width: height
				anchors.left: parent.left
				anchors.leftMargin: 4
				anchors.verticalCenter: parent.verticalCenter
				sourceSize.width: width
				source: "qrc:///images/iOS/search_icon.svg"
			}

			TextField {
				id: searchField

				anchors.margins: 8
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: glassIcon.right
				anchors.right: textEditX.left

				Accessible.role: Accessible.EditableText
				Accessible.name: displayText

				horizontalAlignment: Text.AlignLeft
				background: Item {}
				EnterKey.type: Qt.EnterKeySearch

				onVisibleChanged: {
					if (visible === false){
						Qt.inputMethod.hide()
					}
				}
			}

			Label {
				id: searchLabel

				visible: searchField.text.trim().length === 0
				anchors.left: searchField.left
				anchors.leftMargin: 8
				anchors.verticalCenter: searchField.verticalCenter

				Accessible.ignored: true

				//: LABEL IOS
				text: qsTr("Search providers") + SettingsModel.translationTrigger
				color: Constants.grey
				font.pixelSize: Style.dimens.normal_font_size
			}

			MouseArea {
				id: textEditX

				visible: searchField.text.length > 0
				height: parent.height
				width: height
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter

				Accessible.role: Accessible.Button
				Accessible.name: qsTr("Clear search string") + SettingsModel.translationTrigger
				Accessible.onPressAction: textEditX.clicked(null)
				Accessible.ignored: !visible

				onClicked: searchField.text = ""

				Image {
					anchors.margins: 4
					anchors.fill: parent
					source: "qrc:///images/iOS/search_cancel.svg"
				}
			}
		}

		GButton {
			id: cancelButton

			visible: false
			anchors.right: parent.right
			anchors.rightMargin: visible ? 8 : 0
			anchors.verticalCenter: parent.verticalCenter
			width: visible ? implicitWidth : 0

			buttonColor: Style.color.transparent
			//: LABEL IOS
			text: qsTr("Cancel") + SettingsModel.translationTrigger
			onClicked: {
				searchField.text = ""
				pageArea.clicked(null)
			}
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
