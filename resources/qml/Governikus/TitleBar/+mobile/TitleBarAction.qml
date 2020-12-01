/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property string icon
	property string text
	property bool iconOnly: false
	signal clicked
	signal textUpdated

	height: Style.dimens.titlebar_height
	implicitWidth: imageItem.implicitWidth + textItem.implicitWidth
	width: imageItem.implicitWidth + (iconOnly ? 0 : textItem.implicitWidth)

	Accessible.role: Accessible.Button
	Accessible.name: text
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked()

	TintableIcon {
		id: imageItem

		sourceSize.width: source ? Style.dimens.small_icon_size : 0
		anchors {
			left: parent.left
			verticalCenter: parent.verticalCenter
		}

		source: icon
		tintColor: Style.color.primary_text_inverse
		playAnimation: true
	}

	GText {
		id: textItem

		visible: !iconOnly
		anchors {
			left: imageItem.right
			verticalCenter: parent.verticalCenter
		}

		Accessible.ignored: true

		verticalAlignment: Text.AlignVCenter
		maximumLineCount: 1
		elide: Text.ElideRight
		wrapMode: Text.NoWrap
		textStyle: Style.text.normal_inverse
		text: baseItem.text

		onTextChanged: baseItem.textUpdated()

		onVisibleChanged: {
			if (visible) {
				fadeIn.start()
			} else {
				fadeOut.start()
			}
		}

		PropertyAnimation {
			id: fadeOut
			target: textItem
			property: "opacity"
			to: 0
			duration: Constants.animation_duration
			easing.type: Easing.InCubic
		}
		PropertyAnimation {
			id: fadeIn
			target: textItem
			property: "opacity"
			to: 1
			duration: Constants.animation_duration
			easing.type: Easing.OutCubic
		}

		Behavior on text {
			SequentialAnimation {
				PropertyAnimation {
					target: textItem
					property: "opacity"
					to: 0
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
				}
				PropertyAction {
					target: textItem
					property: "text"
				}
				PropertyAnimation {
					target: textItem
					property: "opacity"
					to: 1
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
				}
			}
		}
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		onClicked: baseItem.clicked()
	}
}
