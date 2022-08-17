/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {

	property string imageSource
	property string title
	property string name

	width: parent.width
	height: Math.max(image.height, providerTitle.height)

	clip: true

	Image {
		id: image
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter

		height: Style.dimens.icon_size
		width: Style.dimens.icon_size

		source: imageSource
		fillMode: Image.PreserveAspectFit
	}

	LabeledText {
		id: providerTitle

		Accessible.ignored: true

		anchors.verticalCenter: image.verticalCenter
		anchors.left: image.right
		anchors.leftMargin: Constants.groupbox_spacing
		anchors.right: parent.right

		label: title
		text: name
	}
}
