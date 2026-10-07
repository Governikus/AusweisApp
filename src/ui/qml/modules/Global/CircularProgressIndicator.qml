/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Shapes

import Governikus.Style

Shape {
	id: root

	readonly property double radius: Style.dimens.medium_icon_size / 2
	property bool reverse: false
	property string text
	required property double value

	Accessible.name: text !== "" ? text : value
	Accessible.role: Accessible.ProgressBar
	height: Style.dimens.medium_icon_size
	width: height

	CircleIndicator {
		strokeColor: Style.color.control.border.disabled_checked
	}
	CircleIndicator {
		startAngle: root.reverse ? 270 - 360 * root.value : -90
		strokeColor: Style.color.control.border.basic_unchecked
		sweepAngle: 360 * root.value
	}
	GText {
		Accessible.ignored: true
		anchors.centerIn: parent
		horizontalAlignment: Qt.AlignHCenter
		maximumLineCount: 1
		text: root.text
		width: root.radius * Math.SQRT2
	}

	component CircleIndicator: ShapePath {
		id: shapePath

		property double startAngle: -90
		property double sweepAngle: 360

		fillColor: Style.color.transparent
		strokeWidth: Style.dimens.circular_indicator_border_width

		PathAngleArc {
			centerX: root.width / 2
			centerY: root.height / 2
			radiusX: root.radius
			radiusY: radiusX
			startAngle: shapePath.startAngle
			sweepAngle: shapePath.sweepAngle
		}
	}
}
