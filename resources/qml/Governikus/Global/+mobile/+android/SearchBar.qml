/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Row {
	id: root

	property int availableWidth: 0
	readonly property alias searchText: searchField.displayText
	readonly property int contentWidth: root.implicitWidth
	function reset() {
		searchField.clear()
		if (searchField.visible) {
			searchField.visible = false
			Qt.inputMethod.hide()
		}
	}

	anchors.top: parent ? parent.top : undefined
	anchors.topMargin: Style.dimens.titlebar_padding
	anchors.right: parent ? parent.right : undefined
	anchors.bottom: parent ? parent.bottom : undefined
	anchors.bottomMargin: Style.dimens.titlebar_padding
	spacing: Style.dimens.titlebar_padding

	GTextField {
		id: searchField
		height: parent.height
		width: root.availableWidth - parent.spacing - iconItem.width - Style.dimens.titlebar_padding

		visible: false

		enterKeyType: Qt.EnterKeySearch

		Accessible.role: Accessible.EditableText
		//: LABEL ANDROID
		Accessible.name: qsTr("Type provider to search for")

		onAccepted: {
			iconItem.forceActiveFocus(Qt.MouseFocusReason)
		}

		Behavior on visible {
			PropertyAnimation {
				duration: 150
			}
		}
	}

	TintableIcon {
		id: iconItem

		sourceSize.height: parent.height
		tintColor: Style.color.button_text
		source: searchField.visible ? "qrc:///images/material_close.svg" : "qrc:///images/material_search.svg"

		Accessible.role: Accessible.Button
		//: LABEL ANDROID
		Accessible.name: qsTr("Search")
		//: LABEL ANDROID
		Accessible.description: qsTr("Search provider list")

		MouseArea {
			id: button

			anchors.fill: parent

			Accessible.ignored: true

			onClicked: {
				// Storage of the new value is needed because the query
				// of searchField.visible still delivers the old value.
				var searchFieldVisible = !searchField.visible

				if (searchFieldVisible) {
					searchField.forceActiveFocus(Qt.MouseFocusReason)
					Qt.inputMethod.show()
				} else {
					iconItem.forceActiveFocus(Qt.MouseFocusReason)
					searchField.text = ""
					Qt.inputMethod.hide()
				}

				searchField.visible = searchFieldVisible
			}
		}
	}
}
