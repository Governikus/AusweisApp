/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Row {
	id: root

	property int availableWidth: 0
	readonly property int contentWidth: root.implicitWidth
	readonly property alias searchText: searchField.displayText

	function reset() {
		searchField.clear();
		if (searchField.visible) {
			searchField.visible = false;
			Qt.inputMethod.hide();
		}
	}

	anchors.bottom: parent ? parent.bottom : undefined
	anchors.bottomMargin: Style.dimens.titlebar_padding
	anchors.right: parent ? parent.right : undefined
	anchors.top: parent ? parent.top : undefined
	anchors.topMargin: Style.dimens.titlebar_padding
	spacing: Style.dimens.titlebar_padding

	GTextField {
		id: searchField
		//: LABEL ANDROID
		Accessible.name: qsTr("Type provider to search for")
		Accessible.role: Accessible.EditableText
		enterKeyType: Qt.EnterKeySearch
		height: parent.height
		visible: false
		width: root.availableWidth - parent.spacing - iconItem.width - Style.dimens.titlebar_padding

		Behavior on visible  {
			PropertyAnimation {
				duration: 150
			}
		}

		onAccepted: {
			iconItem.forceActiveFocus(Qt.MouseFocusReason);
		}
	}
	TintableIcon {
		id: iconItem
		//: LABEL ANDROID
		Accessible.description: qsTr("Search provider list")
		Accessible.name: searchField.visible ?
		//: LABEL ANDROID
		qsTr("Abort search") :
		//: LABEL ANDROID
		qsTr("Search")
		Accessible.role: Accessible.Button
		source: searchField.visible ? "qrc:///images/material_close.svg" : "qrc:///images/material_search.svg"
		sourceSize.height: parent.height
		tintColor: Style.color.button_text

		Accessible.onPressAction: button.clicked(null)

		MouseArea {
			id: button
			Accessible.ignored: true
			anchors.fill: parent

			onClicked: {
				// Storage of the new value is needed because the query
				// of searchField.visible still delivers the old value.
				var searchFieldVisible = !searchField.visible;
				if (searchFieldVisible) {
					searchField.forceActiveFocus(Qt.MouseFocusReason);
					Qt.inputMethod.show();
				} else {
					iconItem.forceActiveFocus(Qt.MouseFocusReason);
					searchField.text = "";
					Qt.inputMethod.hide();
				}
				searchField.visible = searchFieldVisible;
			}
		}
	}
}
