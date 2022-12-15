/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: roundedRectangle

	property color borderColor: Style.color.border
	property real borderWidth: 0
	property bool bottomLeftCorner: true
	property bool bottomRightCorner: true
	property color color: Style.color.background
	property real radius: Style.dimens.corner_radius
	property bool topLeftCorner: true
	property bool topRightCorner: true

	onBorderColorChanged: canvas.requestPaint()
	onBorderWidthChanged: canvas.requestPaint()
	onBottomLeftCornerChanged: canvas.requestPaint()
	onBottomRightCornerChanged: canvas.requestPaint()
	onColorChanged: canvas.requestPaint()
	onRadiusChanged: canvas.requestPaint()
	onTopLeftCornerChanged: canvas.requestPaint()
	onTopRightCornerChanged: canvas.requestPaint()
	onVisibleChanged: canvas.requestPaint()

	Canvas {
		id: canvas
		anchors.fill: parent

		onPaint: {
			let context = getContext("2d");
			if (context === null) {
				return;
			}
			context.save();
			context.reset();
			context.beginPath();
			context.moveTo(0, height / 2);
			if (topLeftCorner) {
				context.lineTo(0, radius);
				context.arcTo(0, 0, radius, 0, radius);
			} else {
				context.lineTo(0, 0);
			}
			if (topRightCorner) {
				context.lineTo(width - radius, 0);
				context.arcTo(width, 0, width, radius, radius);
			} else {
				context.lineTo(width, 0);
			}
			if (bottomRightCorner) {
				context.lineTo(width, height - radius);
				context.arcTo(width, height, width - radius, height, radius);
			} else {
				context.lineTo(width, height);
			}
			if (bottomLeftCorner) {
				context.lineTo(radius, height);
				context.arcTo(0, height, 0, height - radius, radius);
			} else {
				context.lineTo(0, height);
			}
			context.lineTo(height / 2);
			context.closePath();
			context.fillStyle = color;
			context.fill();
			if (borderWidth > 0) {
				context.clip();
				context.lineWidth = borderWidth * 2;
				context.strokeStyle = borderColor;
				context.stroke();
			}
			context.restore();
		}
	}
}
