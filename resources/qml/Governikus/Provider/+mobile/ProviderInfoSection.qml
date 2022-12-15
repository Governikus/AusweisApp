/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	property string imageSource
	property string name
	property string title

	clip: true
	height: Math.max(image.height, providerTitle.height)
	width: parent.width

	Image {
		id: image
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		fillMode: Image.PreserveAspectFit
		height: Style.dimens.icon_size
		source: imageSource
		width: Style.dimens.icon_size
	}
	LabeledText {
		id: providerTitle
		Accessible.ignored: true
		anchors.left: image.right
		anchors.leftMargin: Constants.groupbox_spacing
		anchors.right: parent.right
		anchors.verticalCenter: image.verticalCenter
		label: title
		text: name
	}
}
