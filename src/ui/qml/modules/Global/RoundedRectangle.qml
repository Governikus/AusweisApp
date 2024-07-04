/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	id: roundedRectangle

	property color borderColor: color
	property real borderWidth: Style.dimens.border_width
	property bool bottomLeftCorner: true
	property bool bottomRightCorner: true
	property color color: Style.color.background
	property color gradientColor: color
	property real radius: Style.dimens.pane_radius
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
			let oddHeight = height + (Constants.is_desktop || height % 2 ? 0 : 1);
			context.save();
			context.reset();
			context.beginPath();
			context.moveTo(0, oddHeight / 2);
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
				context.lineTo(width, oddHeight - radius);
				context.arcTo(width, oddHeight, width - radius, oddHeight, radius);
			} else {
				context.lineTo(width, oddHeight);
			}
			if (bottomLeftCorner) {
				context.lineTo(radius, oddHeight);
				context.arcTo(0, oddHeight, 0, oddHeight - radius, radius);
			} else {
				context.lineTo(0, oddHeight);
			}
			context.lineTo(oddHeight / 2);
			context.closePath();
			if (gradientColor != color) {
				let gradient = context.createLinearGradient(0, 0, 0, 100);
				gradient.addColorStop(0.0, color);
				gradient.addColorStop(1.0, gradientColor);
				context.fillStyle = gradient;
			} else {
				context.fillStyle = color;
			}
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
