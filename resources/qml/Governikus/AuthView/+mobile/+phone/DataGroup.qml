/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0


Column {
	id: root

	signal scrollPageUp()
	signal scrollPageDown()

	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias titleStyle: dataTitle.textStyle
	property alias chat: repeater.model

	visible: count > 0
	spacing: 0

	PaneTitle {
		id: dataTitle

		width: parent.width
		height: implicitHeight * 1.5
		verticalAlignment: Text.AlignTop
	}

	Repeater {
		id: repeater

		Item {
			width: parent.width
			height: 40

			Accessible.role: Accessible.ListItem
			Accessible.name: name
			Accessible.checkable: optional
			Accessible.checked: checkBox.checked
			Accessible.onPressAction: if (optional) selected = !selected
			Accessible.onScrollDownAction: baseItem.scrollPageDown()
			Accessible.onScrollUpAction: baseItem.scrollPageUp()

			GText {
				id: dataGroup

				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: checkBox.left
				width: parent.width

				Accessible.ignored: true

				text: name
				textStyle: writeRight ? Style.text.normal_warning : Style.text.normal_secondary
			}
			GSeparator {
				anchors.top: parent.bottom
				anchors.topMargin: -height
				anchors.left: dataGroup.left
				anchors.right: dataGroup.right
				visible: index < repeater.count - 1
			}
			GCheckBox {
				id: checkBox

				visible: optional
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter

				Accessible.ignored: true

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
