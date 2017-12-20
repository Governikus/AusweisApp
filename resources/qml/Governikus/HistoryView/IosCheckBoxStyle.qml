import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Governikus.Global 1.0

CheckBoxStyle {
	id: style
	spacing: 0

	indicator: Component {
		Rectangle {
			id: rectangle
			implicitWidth: control.width
			implicitHeight: control.height
			color: Constants.background_color

			Rectangle {
				anchors.centerIn: parent
				height: Utils.dp(radius * 2 + border.width * 2)
				width: height
				radius: 8
				border.color: control.activeFocus ? Constants.blue : Constants.grey
				border.width: 1
				color: style.control.parent.color ? style.control.parent.color : "white"

				Rectangle {
					visible: control.checked
					color: Constants.blue
					border.color: "#333"
					radius: parent.radius
					anchors.fill: parent
				}
			}
		}
	}
}
