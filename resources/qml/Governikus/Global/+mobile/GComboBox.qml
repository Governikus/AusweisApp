import QtQuick 2.10
import QtQuick.Controls 2.3

import "Utils.js" as Utils

ComboBox {
	id: control
	spacing: Constants.groupbox_spacing
	font.pixelSize: Constants.normal_font_size

	QtObject {
		id: d
		readonly property var mainColor: Constants.black
		readonly property var pressedColor: Constants.black
	}

	onPressedChanged: canvas.requestPaint()
	Component.onCompleted: canvas.requestPaint()

	delegate: ItemDelegate {
		width: control.width
		contentItem: Text {
			text: modelData
			color: d.mainColor
			font: control.font
			elide: Text.ElideRight
			verticalAlignment: Text.AlignVCenter
		}
		highlighted: control.highlightedIndex === index
	}

	indicator: Canvas {
		id: canvas
		x: control.width - width - control.rightPadding
		y: control.topPadding + (control.availableHeight - height) / 2
		width: Utils.dp(12)
		height: Utils.dp(8)
		contextType: "2d"

		onPaint: {
			context.reset();
			context.moveTo(0, 0);
			context.lineTo(width, 0);
			context.lineTo(width / 2, height);
			context.closePath();
			context.fillStyle = control.pressed ? d.pressedColor : d.mainColor;
			context.fill();
		}
	}

	contentItem: Text {
		padding: control.spacing
		rightPadding: control.indicator.width + control.spacing

		text: control.displayText
		font: control.font
		color: control.pressed ? d.pressedColor : d.mainColor
		verticalAlignment: Text.AlignVCenter
		elide: Text.ElideRight
	}

	background: Rectangle {
		border.color: control.pressed ? d.pressedColor : d.mainColor
		border.width: control.visualFocus ? Utils.dp(2) : Utils.dp(1)
		radius: Utils.dp(2)
	}

	popup: Popup {
		y: control.height - 1
		width: control.width
		implicitHeight: contentItem.implicitHeight
		padding: Utils.dp(1)

		contentItem: ListView {
			clip: true
			implicitHeight: contentHeight
			model: control.popup.visible ? control.delegateModel : null
			currentIndex: control.highlightedIndex

			ScrollIndicator.vertical: ScrollIndicator { }
		}

		background: Rectangle {
			border.color: d.mainColor
			radius: Utils.dp(2)
		}
	}
}
