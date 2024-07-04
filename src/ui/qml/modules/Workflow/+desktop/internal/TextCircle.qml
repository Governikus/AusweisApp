/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style

Rectangle {
	id: root

	readonly property double maximumHeight: enabledFontMetrics.height * 2.5
	property alias text: number.text

	border.color: Style.color.control.border.basic
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
			PropertyAnimation {
				duration: 500
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
