/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import "Utils.js" as Utils

import Governikus.Style 1.0

ComboBox {
	id: control

	property var textStyle: Style.text.normal

	Accessible.role: Accessible.ComboBox
	Accessible.name: displayText

	spacing: Constants.groupbox_spacing
	font.pixelSize: textStyle.fontSize

	onPressedChanged: canvas.requestPaint()
	Component.onCompleted: canvas.requestPaint()

	delegate: ItemDelegate {
		width: control.width
		contentItem: GText {
			text: modelData
			textStyle: control.textStyle
			elide: Text.ElideRight
			verticalAlignment: Text.AlignVCenter
		}
		highlighted: control.highlightedIndex === index
	}

	indicator: Canvas {
		id: canvas
		x: control.width - width - control.rightPadding
		y: control.topPadding + (control.availableHeight - height) / 2
		width: 12
		height: 8
		contextType: "2d"

		onPaint: {
			context.reset();
			context.moveTo(0, 0);
			context.lineTo(width, 0);
			context.lineTo(width / 2, height);
			context.closePath();
			context.fillStyle = Constants.black
			context.fill();
		}
	}

	contentItem: GText {
		padding: control.spacing
		rightPadding: control.indicator.width + control.spacing

		text: control.displayText
		textStyle: control.textStyle
		verticalAlignment: Text.AlignVCenter
		elide: Text.ElideRight
	}

	background: Rectangle {
		border.color: Constants.black
		border.width: control.visualFocus ? 2 : 1
		radius: 2
	}

	popup: Popup {
		y: control.height - 1
		width: control.width
		implicitHeight: contentItem.implicitHeight
		padding: 1

		contentItem: ListView {
			clip: true
			implicitHeight: contentHeight
			model: control.popup.visible ? control.delegateModel : null
			currentIndex: control.highlightedIndex

			ScrollIndicator.vertical: ScrollIndicator { }
		}

		background: Rectangle {
			border.color: Constants.black
			radius: 2
		}
	}
}
