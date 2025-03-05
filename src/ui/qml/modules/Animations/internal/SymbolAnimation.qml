/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.Type

TintableIcon {
	id: root

	property bool animated: true
	property alias model: symbols.model
	property SymbolProperties symbol: SymbolProperties {
	}

	sourceSize.height: Style.dimens.header_icon_size
	tintColor: Style.color.image

	Repeater {
		id: symbols

		Symbol {
			id: innerIcon

			property real animatedSize
			readonly property real maxSize: root.symbol.maxSize
			readonly property real minSize: root.symbol.minSize
			readonly property real size: animation.running ? animatedSize : maxSize
			required property real sx
			required property real sy

			sourceSize.height: root.height * size * (type === Symbol.Type.BLOCK ? 1.5 : 1)
			tintColor: root.tintColor
			tintEnabled: root.symbol.tintEnabled
			type: root.symbol.type

			SequentialAnimation on animatedSize {
				id: animation

				loops: Animation.Infinite
				running: root.animated && SettingsModel.useAnimations

				NumberAnimation {
					duration: 1000
					easing.type: Easing.InOutQuad
					from: innerIcon.maxSize
					to: innerIcon.minSize
				}
				NumberAnimation {
					duration: 1000
					easing.type: Easing.InOutQuad
					from: innerIcon.minSize
					to: innerIcon.maxSize
				}
			}

			anchors {
				horizontalCenter: parent.left
				horizontalCenterOffset: root.width * sx
				verticalCenter: parent.top
				verticalCenterOffset: root.height * sy
			}
		}
	}

	component SymbolProperties: QtObject {
		property real maxSize: 0.202
		property real minSize: 0.141
		property bool tintEnabled: root.tintEnabled && (root.symbol === Symbol.Type.STAR || root.symbol === Symbol.Type.QUESTION)
		property int type: Symbol.Type.QUESTION
	}
}
