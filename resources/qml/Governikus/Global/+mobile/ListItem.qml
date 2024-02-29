/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

Rectangle {
	id: baseItem

	property real contentMarginLeft: Constants.groupbox_spacing
	property real contentMarginRight: Constants.groupbox_spacing
	property bool fixedHeight: true
	property alias footerText: footerItem.text
	property alias headerText: headerItem.text
	property alias icon: imageItem.source
	property alias mouseAreaEnabled: mouseArea.enabled
	property bool pressed: mouseArea.pressed
	property bool showSeparator: true
	property alias text: textItem.text
	property alias tintIcon: imageItem.tintEnabled

	signal clicked
	signal pressAndHold

	Accessible.name: headerText + ". " + text + ". " + footerText
	Accessible.role: Accessible.ListItem
	color: pressed ? Style.color.pane_active : Style.color.pane
	height: fixedHeight ? Style.dimens.list_item_height : (content.implicitHeight + Constants.text_spacing)
	width: parent ? parent.width : 0

	Accessible.onPressAction: mouseArea.clicked(null)

	GSeparator {
		anchors.right: parent.right
		anchors.top: parent.bottom
		anchors.topMargin: -height
		visible: showSeparator
		width: Constants.is_layout_ios ? (parent.width - textLayout.x - contentMarginLeft) : parent.width
	}
	RowLayout {
		id: content

		anchors.fill: parent
		anchors.leftMargin: contentMarginLeft
		anchors.rightMargin: contentMarginRight
		spacing: Constants.groupbox_spacing

		TintableIcon {
			id: imageItem

			sourceSize.height: parent.height - 2 * Constants.groupbox_spacing
			tintColor: Style.color.text
			tintEnabled: false
			visible: baseItem.icon !== ""
		}
		ColumnLayout {
			id: textLayout

			Layout.fillWidth: true
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			spacing: 0

			GText {
				id: headerItem

				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
				color: baseItem.pressed ? Style.color.text_pressed : Style.color.text_subline
				elide: Text.ElideRight
				maximumLineCount: fixedHeight ? 1 : 8
				visible: baseItem.headerText !== ""
			}
			GText {
				id: textItem

				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
				color: baseItem.pressed ? Style.color.text_pressed : Style.color.text
				elide: Text.ElideRight
				maximumLineCount: fixedHeight ? (headerText === "" ? 2 : 1) : 64
				visible: baseItem.text !== ""
			}
			GText {
				id: footerItem

				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
				color: baseItem.pressed ? Style.color.text_pressed : Style.color.text
				elide: Text.ElideRight
				maximumLineCount: fixedHeight ? (headerText === "" ? 2 : 1) : 8
				visible: baseItem.footerText !== ""
			}
		}
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.clicked()
		onPressAndHold: baseItem.pressAndHold()
	}
}
