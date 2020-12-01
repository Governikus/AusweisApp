/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0

Item {

	property string imageSource;
	property string title;
	property string name;

	width: parent.width
	height: Math.max(image.height, providerTitle.height)

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
		text: name.length > 0 ? name : qsTr("Touch for more details")
	}
}
