/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Animations
import Governikus.Style

Item {
	id: root

	opacity: 0.5

	QtObject {
		id: d

		readonly property double a: halfWidth - (gap * 2)
		readonly property double b: halfHeight - (gap * 2)
		readonly property double c: Math.sqrt(a * a + b * b)
		readonly property int gap: 20
		readonly property double halfHeight: root.height / 2
		readonly property double halfWidth: root.width / 2
		readonly property double rotate: Math.atan(root.height / root.width) * 180 / Math.PI
	}
	Line {
		x: d.halfWidth + d.gap
		y: d.halfHeight + d.gap

		transform: Rotation {
			angle: d.rotate
		}
	}
	Line {
		x: d.halfWidth + d.gap
		y: d.halfHeight - d.gap

		transform: Rotation {
			angle: -d.rotate
		}
	}
	Line {
		x: d.halfWidth - d.gap
		y: d.halfHeight - d.gap

		transform: Rotation {
			angle: d.rotate - 180
		}
	}
	Line {
		width: d.c - warning.width
		x: d.halfWidth - d.gap
		y: d.halfHeight + d.gap

		transform: Rotation {
			angle: -d.rotate + 180
		}
	}
	StatusAnimation {
		id: warning

		sourceSize.height: Style.dimens.huge_icon_size - 10
		symbol.type: Symbol.Type.WARNING

		anchors {
			bottom: parent.bottom
			bottomMargin: d.gap
			left: parent.left
			leftMargin: d.gap
		}
	}

	component Line: Rectangle {
		antialiasing: true
		color: Style.color.warning
		height: Style.dimens.separator_size_large
		width: d.c
	}
}
