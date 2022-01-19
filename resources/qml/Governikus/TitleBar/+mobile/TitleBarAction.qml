/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property alias icon: imageItem.source
	property alias text: textItem.text
	readonly property real minimumWidth: d.isEmpty ? 0 : Math.max(implicitHeight, d.textWidth)
	signal clicked

	height: implicitHeight
	width: implicitWidth
	implicitHeight: Style.dimens.titlebar_height
	implicitWidth: d.isEmpty ? 0 : Math.max(implicitHeight, d.imageWidth + d.textWidth)

	Accessible.role: Accessible.Button
	Accessible.name: text
	Accessible.ignored: d.isEmpty
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked()

	QtObject {
		id: d

		readonly property int imageWidth: imageItem.visible ? imageItem.x + imageItem.width : 0
		readonly property int textWidth: textItem.anchors.leftMargin + textItem.implicitWidth
		readonly property bool isEmpty: imageItem.source == "" && textItem.text === ""
	}

	TintableIcon {
		id: imageItem

		visible: imageItem.source != ""
		width: Style.dimens.small_icon_size
		height: width
		sourceSize.width: width
		anchors {
			horizontalCenter: baseItem.left
			horizontalCenterOffset: baseItem.implicitHeight / 2
			verticalCenter: parent.verticalCenter
		}

		tintColor: Style.color.primary_text_inverse
		playAnimation: true
	}

	GText {
		id: textItem

		visible: !imageItem.visible || baseItem.width >= baseItem.implicitWidth
		anchors {
			left: imageItem.visible ? imageItem.right : parent.left
			leftMargin: imageItem.visible || text === "" ? 0 : Style.dimens.titlebar_padding
			verticalCenter: parent.verticalCenter
		}

		Accessible.ignored: true

		verticalAlignment: Text.AlignVCenter
		maximumLineCount: 1
		elide: Text.ElideRight
		wrapMode: Text.NoWrap
		textStyle: Style.text.normal_inverse

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
