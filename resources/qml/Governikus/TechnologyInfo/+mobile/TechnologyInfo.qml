/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: baseItem
	property alias enableText: enableInfo.text
	property alias enableButtonText: enableButton.text
	property alias enableButtonVisible: enableButton.visible
	property alias titleText: title.text
	property alias subTitleText: subTitle.text

	signal enableClicked()

	GText {
		id: enableInfo

		visible: !!text
		anchors.bottom: enableButton.top
		anchors.margins: Constants.component_spacing
		anchors.left: parent.left
		anchors.right: parent.right

		Accessible.name: ApplicationModel.stripHtmlTags(text)

		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.normal_warning

		Behavior on text {
			SequentialAnimation {
				PropertyAction { target: enableInfo; property: "opacity"; value: 0.0 }
				PropertyAction { target: enableInfo; property: "text" }
				PropertyAnimation { target: enableInfo; property: "opacity"; to: 1.0; duration: 500}
			}
		}
	}

	GButton {
		id: enableButton
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter
		onClicked: parent.enableClicked()
	}

	GText {
		id: title

		visible: !enableInfo.visible && !enableButton.visible
		anchors.bottom: parent.verticalCenter
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		Accessible.name: ApplicationModel.stripHtmlTags(text)

		textStyle: Style.text.header_accent

		Behavior on text {
			SequentialAnimation {
				NumberAnimation { target: rotation; property: "angle"; to: 90; duration: 500 }
				PropertyAction { target: title; property: "text" }
				NumberAnimation { target: rotation; property: "angle"; to: 0; duration: 500 }
			}
		}

		transform: Rotation {
			id: rotation
			origin.x: 0
			origin.y: title.height/2
			axis.x: 1; axis.y: 0; axis.z: 0
			angle: 0
		}
	}

	GText {
		id: subTitle

		visible: !enableInfo.visible && !enableButton.visible
		anchors.left: parent.left
		anchors.top: title.bottom
		anchors.right: parent.right
		anchors.bottom: enableButton.top

		Accessible.name: ApplicationModel.stripHtmlTags(text)

		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignTop
		textStyle: Style.text.normal_secondary

		Behavior on text {
			SequentialAnimation {
				PropertyAnimation { target: subTitle; property: "opacity"; to: 0; duration: 500}
				PropertyAction { target: subTitle; property: "text" }
				PropertyAnimation { target: subTitle; property: "opacity"; to: 1.0; duration: 500 }
				PropertyAction { target: subTitle; property: "color"; value: NumberModel.hasError ? Style.color.warning_text : Style.color.secondary_text }
			}
		}
	}
}
