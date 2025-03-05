/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

Rectangle {
	id: root

	property bool boldFont: false
	property real contentMarginLeft: Style.dimens.groupbox_spacing
	property real contentMarginRight: Style.dimens.groupbox_spacing
	property alias footerText: footerItem.text
	property alias headerText: headerItem.text
	property alias icon: imageItem.source
	property alias mouseAreaEnabled: mouseArea.enabled
	property bool showSeparator: true
	property alias text: textItem.text
	property alias tintIcon: imageItem.tintEnabled

	signal pressAndHold

	Accessible.name: headerText + ". " + text + ". " + footerText
	Accessible.role: Accessible.ListItem
	color: colors.paneBackground
	height: content.implicitHeight + Style.dimens.text_spacing
	width: parent ? parent.width : 0

	GSeparator {
		anchors.right: parent.right
		anchors.top: parent.bottom
		anchors.topMargin: -height
		visible: root.showSeparator
		width: Style.is_layout_ios ? (parent.width - textLayout.x - root.contentMarginLeft) : parent.width
	}
	RowLayout {
		id: content

		anchors.fill: parent
		anchors.leftMargin: root.contentMarginLeft
		anchors.rightMargin: root.contentMarginRight
		spacing: Style.dimens.groupbox_spacing

		TintableIcon {
			id: imageItem

			sourceSize.height: parent.height - 2 * Style.dimens.groupbox_spacing
			tintColor: colors.textNormal
			tintEnabled: false
			visible: root.icon !== ""
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
				activeFocusOnTab: false
				color: colors.textSubline
				elide: Text.ElideRight
				font.bold: root.boldFont
				maximumLineCount: 8
				visible: root.headerText !== ""
			}
			GText {
				id: textItem

				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
				activeFocusOnTab: false
				color: colors.textNormal
				elide: Text.ElideRight
				font.bold: root.boldFont
				maximumLineCount: 64
				visible: root.text !== ""
			}
			GText {
				id: footerItem

				Accessible.ignored: true
				Layout.alignment: Qt.AlignLeft
				activeFocusOnTab: false
				color: colors.textNormal
				elide: Text.ElideRight
				font.bold: root.boldFont
				maximumLineCount: 8
				visible: root.footerText !== ""
			}
		}
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent
		hoverEnabled: UiPluginModel.isChromeOS

		onPressAndHold: root.pressAndHold()
	}
	StatefulColors {
		id: colors

		checkedCondition: false
		hoveredCondition: mouseArea.containsMouse
		statefulControl: mouseArea
	}
}
