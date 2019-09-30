/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0


Rectangle {

	property string imageSource;
	property string title;
	property string name;

	width: parent.width
	height: Math.max(image.height, providerTitle.height)

	color: Constants.white
	clip: true

	Image {
		id: image
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter

		height: 40
		width: 40

		source: imageSource
		fillMode: Image.PreserveAspectFit
	}

	LabeledText {
		id: providerTitle
		anchors.verticalCenter: image.verticalCenter
		anchors.left: image.right
		anchors.leftMargin: 10
		anchors.right: parent.right

		label: title
		//: LABEL ANDROID IOS
		text: name.length > 0 ? name : qsTr("Touch for more details") + SettingsModel.translationTrigger
	}
}
