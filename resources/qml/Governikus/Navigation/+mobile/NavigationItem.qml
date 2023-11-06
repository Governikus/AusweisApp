/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

AbstractButton {
	id: root

	property bool flowHorizontally: true
	readonly property color itemColor: selected ? Style.color.text_navigation : Style.color.text_navigation_unchecked
	property bool selected: false
	property alias source: tabImage.source

	Accessible.name: text
	Accessible.role: Accessible.Button
	Layout.minimumWidth: tabImage.implicitWidth + leftPadding + rightPadding
	padding: Constants.text_spacing / 2

	background: Rectangle {
		color: selected ? Style.color.control : Style.color.transparent
		radius: Style.dimens.control_radius
	}
	contentItem: Item {
		implicitHeight: grid.implicitHeight
		implicitWidth: Math.ceil(tabImage.implicitWidth + grid.columnSpacing + tabText.implicitWidth)

		GridLayout {
			id: grid

			Accessible.ignored: true
			anchors.centerIn: parent
			columnSpacing: Constants.text_spacing
			columns: 2
			flow: root.flowHorizontally ? GridLayout.LeftToRight : GridLayout.TopToBottom
			rowSpacing: Style.dimens.navigation_bar_text_padding
			rows: 2

			Accessible.onPressAction: root.clicked()

			TintableIcon {
				id: tabImage

				Accessible.ignored: true
				Layout.alignment: root.flowHorizontally ? Qt.AlignRight : Qt.AlignCenter
				Layout.maximumWidth: implicitWidth
				sourceSize.height: Style.dimens.navigation_bar_icon_size
				tintColor: root.itemColor
			}
			GText {
				id: tabText

				Accessible.ignored: true
				Layout.alignment: root.flowHorizontally ? Qt.AlignLeft : Qt.AlignCenter
				Layout.preferredWidth: Math.min(Math.ceil(implicitWidth), root.contentItem.width)
				color: root.itemColor
				elide: Text.ElideRight
				horizontalAlignment: Text.AlignHCenter
				maximumLineCount: 1
				text: root.text
				textStyle: Style.text.navigation
			}
		}
	}
}
