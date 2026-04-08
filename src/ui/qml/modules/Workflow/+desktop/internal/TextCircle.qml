/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.Type

Rectangle {
	id: root

	readonly property double maximumHeight: enabledFontMetrics.height * 2.5
	property alias text: number.text

	border.color: Style.color.control.border.basic_unchecked
	border.width: Style.dimens.border_width
	color: Style.color.transparent
	enabled: false
	height: maximumHeight * d.scaleFactor
	radius: width * 0.5
	width: height

	QtObject {
		id: d

		property double scaleFactor: root.enabled ? 1.0 : 0.7

		Behavior on scaleFactor {
			enabled: SettingsModel.useAnimations

			PropertyAnimation {
				duration: 2 * Style.animation_duration
				easing.type: Easing.InOutCubic
			}
		}
	}
	GText {
		id: number

		Accessible.ignored: true
		anchors.centerIn: parent
		font.pixelSize: Style.dimens.textHeadline * d.scaleFactor
		textStyle: Style.text.headline
	}
	FontMetrics {
		id: enabledFontMetrics

		font.pixelSize: Style.dimens.textHeadline
	}
}
