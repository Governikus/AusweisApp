/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	property string imageSource
	property string name
	property string title

	clip: true
	height: Math.max(image.height, providerTitle.height)
	width: parent.width

	TintableIcon {
		id: image

		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		fillMode: Image.PreserveAspectFit
		height: Style.dimens.small_icon_size
		source: imageSource
		tintColor: Style.color.text_subline
		width: Style.dimens.small_icon_size
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
