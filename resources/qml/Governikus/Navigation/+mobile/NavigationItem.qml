/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

AbstractButton {
	id: root

	property bool flowHorizontally: true
	property bool selected: false
	property alias source: tabImage.source

	Accessible.name: text
	Accessible.role: Accessible.Button
	Layout.minimumWidth: tabImage.implicitWidth + leftPadding + rightPadding
	padding: Constants.text_spacing / 2

	background: Rectangle {
		id: pane

		border.color: Style.color.pane_border
		color: Style.color.pane
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
				tintColor: tabText.color
			}
			GText {
				id: tabText

				Accessible.ignored: true
				Layout.alignment: root.flowHorizontally ? Qt.AlignLeft : Qt.AlignCenter
				Layout.preferredWidth: Math.min(Math.ceil(implicitWidth), root.contentItem.width)
				color: Style.color.text
				elide: Text.ElideRight
				horizontalAlignment: Text.AlignHCenter
				maximumLineCount: 1
				text: root.text
				textStyle: Style.text.navigation
			}
		}
	}

	Item {
		id: d

		states: [
			State {
				name: "pressed"
				when: root.pressed

				PropertyChanges {
					pane.border.color: Style.color.pane_border_pressed
					pane.color: Style.color.pane_pressed
					tabText.color: Style.color.text_pressed
				}
			},
			State {
				name: "hover"
				when: root.hovered

				PropertyChanges {
					pane.border.color: Style.color.pane_border_hovered
					pane.color: Style.color.pane_hovered
					tabText.color: Style.color.text_hovered
				}
			},
			State {
				name: "selected"
				when: root.selected

				PropertyChanges {
					pane.border.color: Style.color.pane_border_active
					pane.color: Style.color.pane_active
					tabText.color: Style.color.text_active
				}
			}
		]
	}
}
