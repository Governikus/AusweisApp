/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Row {
	id: root

	property string sectionName
	property string iconPath

	spacing: Constants.groupbox_spacing

	Image {
		id: sectionIcon

		visible: source !== ""
		sourceSize.height: Style.dimens.medium_icon_size
		sourceSize.width: Style.dimens.medium_icon_size

		source: iconPath
		fillMode: Image.PreserveAspectFit
	}

	GText {
		width: parent.width - sectionIcon.width
		anchors.verticalCenter: parent.verticalCenter

		text: sectionName
		textStyle: Style.text.header
		maximumLineCount: 1
		elide: Text.ElideRight
	}
}
