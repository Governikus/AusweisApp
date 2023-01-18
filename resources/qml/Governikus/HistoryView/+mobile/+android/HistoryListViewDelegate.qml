/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.HistoryModel 1.0

HistoryListItem {
	id: listItem
	contentMarginRight: deleteButton.width + deleteButton.anchors.leftMargin + deleteButton.anchors.rightMargin
	width: parent.width

	TintableIcon {
		id: deleteButton
		anchors.margins: Constants.groupbox_spacing
		anchors.right: parent.right
		anchors.rightMargin: Style.dimens.titlebar_padding
		anchors.verticalCenter: parent.verticalCenter
		source: "qrc:///images/material_delete.svg"
		sourceSize.width: Style.dimens.small_icon_size
		tintColor: Style.color.accent
	}
	MouseArea {

		//: LABEL ANDROID
		Accessible.name: qsTr("Delete entry")
		Accessible.role: Accessible.Button
		anchors.left: deleteButton.left
		anchors.leftMargin: -Constants.groupbox_spacing
		anchors.right: parent.right
		height: parent.height

		Accessible.onPressAction: clicked(null)
		onClicked: HistoryModel.removeRows(index, 1)
	}
}
