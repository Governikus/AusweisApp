/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0


Pane {
	id: pane

	signal scrollPageUp()
	signal scrollPageDown()

	property alias chat: repeater.model

	visible: repeater.count > 0

	contentSpacing: 0

	Repeater {
		id: repeater

		Rectangle {
			width: parent.width
			height: 40

			Accessible.role: Accessible.ListItem
			Accessible.name: name
			Accessible.checkable: true
			Accessible.checked: checkBox.checked
			Accessible.onPressAction: if (optional) selected = !selected
			Accessible.onScrollDownAction: baseItem.scrollPageDown()
			Accessible.onScrollUpAction: baseItem.scrollPageUp()

			radius: 3
			color: Constants.white

			GText {
				id: dataGroup

				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: checkBox.left
				width: parent.width

				Accessible.ignored: true

				text: name
				textStyle: Style.text.normal_secondary
			}
			GSeparator {
				anchors.top: parent.bottom
				anchors.topMargin: -height
				anchors.left: dataGroup.left
				anchors.right: dataGroup.right
			}
			GCheckBox {
				id: checkBox
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				visible: optional
				checked: selected
			}

			MouseArea {
				anchors.fill: parent
				enabled: optional
				onClicked: selected = !selected
				Rectangle {
					anchors.fill: parent
					color: Constants.grey
					opacity: parent.pressed ? 0.5 : 0
					Behavior on opacity { NumberAnimation { duration: 100 } }
				}
			}
		}
	}
}
