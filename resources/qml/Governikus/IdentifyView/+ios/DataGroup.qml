import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import Governikus.Global 1.0


Pane {
	property alias chat: repeater.model

	id: pane
	spacing: 0
	visible: repeater.count > 0

	Repeater {
		id: repeater

		Rectangle {
			width: parent.width
			height: Utils.dp(40)
			radius: 3
			color: "white"
			Text {
				id: dataGroup
				color: Constants.secondary_text
				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: checkBox.left
				width: parent.width
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
				text: name
			}
			Rectangle {
				anchors.top: parent.bottom
				anchors.topMargin: -height
				height: 1
				anchors.left: dataGroup.left
				anchors.right: dataGroup.right
				color: Constants.grey
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
