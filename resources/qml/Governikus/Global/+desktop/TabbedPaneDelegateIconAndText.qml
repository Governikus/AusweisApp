/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Row {
	id: root

	property string iconPath
	property string sectionName

	spacing: Constants.groupbox_spacing

	Image {
		id: sectionIcon
		fillMode: Image.PreserveAspectFit
		source: iconPath
		sourceSize.height: Style.dimens.medium_icon_size
		sourceSize.width: Style.dimens.medium_icon_size
		visible: source !== ""
	}
	GText {
		anchors.verticalCenter: parent.verticalCenter
		elide: Text.ElideRight
		maximumLineCount: 1
		text: sectionName
		textStyle: isCurrentItem ? Style.text.header : Style.text.header_inverse
		width: parent.width - sectionIcon.width
	}
}
