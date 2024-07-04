/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

AbstractButton {
	id: root

	property bool flowHorizontally: true
	property alias source: tabImage.source

	Accessible.name: text
	Accessible.role: Accessible.Button
	Layout.minimumWidth: tabImage.implicitWidth + leftPadding + rightPadding
	padding: Constants.text_spacing / 2

	background: Rectangle {
		id: pane

		border.color: colors.paneBorder
		color: colors.paneBackground
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

			TintableIcon {
				id: tabImage

				Accessible.ignored: true
				Layout.alignment: root.flowHorizontally ? Qt.AlignRight : Qt.AlignCenter
				Layout.maximumWidth: implicitWidth
				sourceSize.height: Style.dimens.navigation_bar_icon_size
				tintColor: tabText.color
			}
			GText {
				id: tabText

				Accessible.ignored: true
				Layout.alignment: root.flowHorizontally ? Qt.AlignLeft : Qt.AlignCenter
				Layout.preferredWidth: Math.min(Math.ceil(implicitWidth), root.contentItem.width)
				color: colors.textNormal
				elide: Text.ElideRight
				horizontalAlignment: Text.AlignHCenter
				maximumLineCount: 1
				text: root.text
				textStyle: Style.text.navigation
			}
		}
	}

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
	FocusFrame {
	}
}
