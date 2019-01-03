import QtQuick 2.10
import QtQuick.Controls 2.3

import "Utils.js" as Utils

RadioButton {
	id: control
	spacing: Constants.groupbox_spacing

	indicator: Rectangle {
		implicitWidth:  Utils.dp(26)
		implicitHeight:  implicitWidth
		x: control.leftPadding
		y: parent.height / 2 - height / 2
		radius: height / 2
		border.color: Qt.darker(Constants.blue, control.down ? 1.5 : 1)

		Rectangle {
			width: Utils.dp(14)
			height: width
			x: (parent.width - width) / 2
			y: x
			radius: width / 2
			color: Qt.darker(Constants.blue, control.down ? 1.5 : 1)
			visible: control.checked
		}
	}

	contentItem: Text {
		text: control.text
		font.pixelSize: Constants.label_font_size
		color: Constants.secondary_text
		verticalAlignment: Text.AlignVCenter
		leftPadding: control.indicator.width + control.spacing
	}
}
