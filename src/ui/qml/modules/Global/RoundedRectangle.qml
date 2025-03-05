/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style

Item {
	id: root

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
			let oddHeight = height + (Style.is_layout_desktop || height % 2 ? 0 : 1);
			context.save();
			context.reset();
			context.beginPath();
			context.moveTo(0, oddHeight / 2);
			if (root.topLeftCorner) {
				context.lineTo(0, root.radius);
				context.arcTo(0, 0, root.radius, 0, root.radius);
			} else {
				context.lineTo(0, 0);
			}
			if (root.topRightCorner) {
				context.lineTo(width - root.radius, 0);
				context.arcTo(width, 0, width, root.radius, root.radius);
			} else {
				context.lineTo(width, 0);
			}
			if (root.bottomRightCorner) {
				context.lineTo(width, oddHeight - root.radius);
				context.arcTo(width, oddHeight, width - root.radius, oddHeight, root.radius);
			} else {
				context.lineTo(width, oddHeight);
			}
			if (root.bottomLeftCorner) {
				context.lineTo(root.radius, oddHeight);
				context.arcTo(0, oddHeight, 0, oddHeight - root.radius, root.radius);
			} else {
				context.lineTo(0, oddHeight);
			}
			context.lineTo(oddHeight / 2);
			context.closePath();
			if (root.gradientColor != root.color) {
				let gradient = context.createLinearGradient(0, 0, 0, 100);
				gradient.addColorStop(0.0, root.color);
				gradient.addColorStop(1.0, root.gradientColor);
				context.fillStyle = gradient;
			} else {
				context.fillStyle = root.color;
			}
			context.fill();
			if (root.borderWidth > 0) {
				context.clip();
				context.lineWidth = root.borderWidth * 2;
				context.strokeStyle = root.borderColor;
				context.stroke();
			}
			context.restore();
		}
	}
}
