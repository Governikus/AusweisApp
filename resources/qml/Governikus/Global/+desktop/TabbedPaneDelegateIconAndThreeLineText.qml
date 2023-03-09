/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

RowLayout {
	id: root

	property string footerText
	property string headerText
	property string iconPath
	property string sectionName

	spacing: Constants.groupbox_spacing
	width: parent.width

	Image {
		id: sectionIcon
		fillMode: Image.PreserveAspectFit
		source: iconPath
		sourceSize.height: Style.dimens.medium_icon_size
		sourceSize.width: Style.dimens.medium_icon_size
		visible: source !== ""
	}
	Column {
		Layout.fillWidth: true
		spacing: Constants.text_spacing

		GText {
			maximumLineCount: 1
			text: headerText
			textStyle: isCurrentItem ? Style.text.normal : Style.text.normal_inverse
			width: parent.width
		}
		GText {
			elide: Text.ElideRight
			maximumLineCount: 1
			text: sectionName
			textStyle: isCurrentItem ? Style.text.header : Style.text.header_inverse
			width: parent.width
		}
		GText {
			elide: Text.ElideRight
			maximumLineCount: 1
			text: footerText
			textStyle: isCurrentItem ? Style.text.normal : Style.text.normal_inverse
			width: parent.width
		}
	}
}
