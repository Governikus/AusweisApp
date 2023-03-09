/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Column {
	id: root

	property alias chat: repeater.model
	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias titleStyle: dataTitle.textStyle

	signal scrollPageDown
	signal scrollPageUp

	spacing: 0
	visible: count > 0

	PaneTitle {
		id: dataTitle
		height: implicitHeight * 1.5
		verticalAlignment: Text.AlignTop
		width: parent.width
	}
	Repeater {
		id: repeater
		Item {
			Accessible.checkable: optional
			Accessible.checked: checkBox.checked
			Accessible.name: name
			Accessible.role: Accessible.ListItem
			height: 40
			width: parent.width

			Accessible.onPressAction: if (optional)
				selected = !selected
			Accessible.onScrollDownAction: baseItem.scrollPageDown()
			Accessible.onScrollUpAction: baseItem.scrollPageUp()

			GText {
				id: dataGroup
				Accessible.ignored: true
				anchors.left: parent.left
				anchors.right: checkBox.left
				anchors.verticalCenter: parent.verticalCenter
				text: name
				textStyle: writeRight ? Style.text.normal_warning : Style.text.normal_secondary
				width: parent.width
			}
			GSeparator {
				anchors.left: dataGroup.left
				anchors.right: dataGroup.right
				anchors.top: parent.bottom
				anchors.topMargin: -height
				visible: index < repeater.count - 1
			}
			GCheckBox {
				id: checkBox
				Accessible.ignored: true
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				checked: selected
				visible: optional
			}
			MouseArea {
				anchors.fill: parent
				enabled: optional

				onClicked: selected = !selected

				Rectangle {
					anchors.fill: parent
					color: Constants.grey
					opacity: parent.pressed ? 0.5 : 0

					Behavior on opacity  {
						NumberAnimation {
							duration: 100
						}
					}
				}
			}
		}
	}
}
