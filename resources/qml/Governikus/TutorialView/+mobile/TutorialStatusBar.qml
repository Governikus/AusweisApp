/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0


Item {
	property alias shaderSource: effectSource.sourceItem

	anchors.top: parent.top
	height: plugin.safeAreaMargins.top
	width: parent.width

	ShaderEffectSource {
		id: effectSource
		anchors.fill: parent
		sourceRect: Qt.rect(parent.x, parent.y, parent.width, parent.height)
	}
	FastBlur {
		anchors.fill: effectSource
		source: effectSource
		radius: 32
	}
	ColorOverlay {
		anchors.fill: parent
		color: footer.color
		Behavior on color { ColorAnimation { duration: Constants.animation_duration } }
		opacity: 0.7
	}
}
