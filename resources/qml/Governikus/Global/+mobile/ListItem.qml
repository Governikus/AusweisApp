/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: baseItem

	property real contentMarginLeft: Constants.groupbox_spacing
	property real contentMarginRight: Constants.groupbox_spacing
	property bool fixedHeight: true
	property alias footerText: footerItem.text
	property alias headerText: headerItem.text
	property alias icon: imageItem.source
	property string linkIconSource: "qrc:///images/mobile/material_arrow_right.svg"
	property alias mouseAreaEnabled: mouseArea.enabled
	property bool pressed: mouseArea.pressed
	property bool showLinkIcon: Constants.is_layout_ios
	property bool showSeparator: true
	property alias text: textItem.text
	property alias tintIcon: imageItem.tintEnabled

	signal clicked
	signal pressAndHold

	Accessible.name: headerText + ". " + text + ". " + footerText
	Accessible.role: Accessible.ListItem
	color: pressed ? Style.color.background_pane_active : Style.color.background_pane
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
			tintColor: Style.color.secondary_text
			tintEnabled: false
			visible: baseItem.icon !== ""
		}
		ColumnLayout {
			id: textLayout
			Layout.fillWidth: true
			spacing: 0

			GText {
				id: headerItem
				Accessible.ignored: true
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: fixedHeight ? 1 : 8
				textStyle: Style.text.hint_accent
				visible: baseItem.headerText !== ""
			}
			GText {
				id: textItem
				Accessible.ignored: true
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: fixedHeight ? (headerText === "" ? 2 : 1) : 64
				visible: baseItem.text !== ""
			}
			GText {
				id: footerItem
				Accessible.ignored: true
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: fixedHeight ? (headerText === "" ? 2 : 1) : 8
				textStyle: Style.text.hint_secondary
				visible: baseItem.footerText !== ""
			}
		}
		TintableIcon {
			source: linkIconSource
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: Style.color.secondary_text
			visible: showLinkIcon
		}
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent

		onClicked: baseItem.clicked()
		onPressAndHold: baseItem.pressAndHold()
	}
}
