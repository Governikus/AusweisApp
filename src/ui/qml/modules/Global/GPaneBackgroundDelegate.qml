/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

RoundedRectangle {
	property int count: 0
	property int idx: -1
	readonly property bool isFirst: idx === 0
	readonly property bool isLast: idx === (count - 1)
	readonly property bool isOnlyElement: count === 1

	bottomLeftCorner: isLast || isOnlyElement
	bottomRightCorner: isLast || isOnlyElement
	color: Style.color.pane.background.basic
	topLeftCorner: isFirst || isOnlyElement
	topRightCorner: isFirst || isOnlyElement
}
