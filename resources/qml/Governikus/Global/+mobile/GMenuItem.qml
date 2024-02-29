/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
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
	color: Style.color.transparent
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
				textStyle: Style.text.subline
				visible: text !== ""
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
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
		}
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
	Item {
		states: [
			State {
				name: "pressed"
				when: mouseArea.pressed

				PropertyChanges {
					baseItem.color: Style.color.pane_pressed
					descriptionText.color: Style.color.text_pressed
					iconItem.tintColor: Style.color.text_pressed
					titleText.color: Style.color.text_subline_pressed
				}
			}
		]
		transitions: [
			EaseInPressedTransition {
				targets: [baseItem, descriptionText, iconItem, titleText]
			}
		]
	}
}
