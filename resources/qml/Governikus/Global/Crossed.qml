/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style

Item {
	opacity: 0.5

	QtObject {
		id: d

		readonly property double a: halfWidth - (gap * 2)
		readonly property double b: halfHeight - (gap * 2)
		readonly property double c: Math.sqrt(a * a + b * b)
		readonly property int gap: 20
		readonly property double halfHeight: parent.height / 2
		readonly property double halfWidth: parent.width / 2
		readonly property double rotate: Math.atan(parent.height / parent.width) * 180 / Math.PI
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
	TintableIcon {
		id: warning

		anchors.bottom: parent.bottom
		anchors.bottomMargin: d.gap
		anchors.left: parent.left
		anchors.leftMargin: d.gap
		source: "qrc:///images/desktop/warning.svg"
		sourceSize.height: Style.dimens.huge_icon_size - 10
		tintColor: Style.color.warning
	}

	component Line: Rectangle {
		antialiasing: true
		color: Style.color.warning
		height: Style.dimens.separator_size_large
		width: d.c
	}
}
