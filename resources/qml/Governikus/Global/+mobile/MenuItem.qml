/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

RoundedRectangle {
	id: baseItem

	property alias description: descriptionText.text
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	property var icon: "qrc:///images/material_arrow_right.svg"
	property alias tintIcon: iconItem.tintEnabled
	property alias title: titleText.text

	signal clicked

	Accessible.name: title + ". " + description
	Accessible.role: Accessible.Button
	bottomLeftCorner: drawBottomCorners
	bottomRightCorner: drawBottomCorners
	color: mouseArea.pressed ? Style.color.pane_active : Style.color.transparent
	implicitHeight: layout.implicitHeight + layout.anchors.topMargin + layout.anchors.bottomMargin
	implicitWidth: layout.implicitWidth + layout.anchors.leftMargin + layout.anchors.rightMargin
	topLeftCorner: drawTopCorners
	topRightCorner: drawTopCorners

	Accessible.onPressAction: clicked()

	RowLayout {
		id: layout

		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		spacing: 0

		ColumnLayout {
			GText {
				id: titleText

				Accessible.ignored: true
				color: mouseArea.pressed ? Style.color.text_subline_pressed : textStyle.textColor
				textStyle: Style.text.subline
				visible: text !== ""
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
				color: mouseArea.pressed ? Style.color.text_pressed : textStyle.textColor
				visible: text !== ""
			}
		}
		TintableIcon {
			id: iconItem

			Accessible.ignored: true
			Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
			Layout.preferredWidth: Style.dimens.icon_size
			source: baseItem.icon
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: Style.color.text
		}
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
}
