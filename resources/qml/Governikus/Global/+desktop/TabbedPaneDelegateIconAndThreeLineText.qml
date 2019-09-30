/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.2

import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

RowLayout {
	id: root

	property string mainText
	property string headerText
	property string footerText
	property string iconPath

	Accessible.role: Accessible.PageTab
	Accessible.name: headerText + " " + mainText + " " + footerText

	width: parent.width

	spacing: Constants.groupbox_spacing

	Image {
		id: sectionIcon

		visible: source !== ""
		sourceSize.height: ApplicationModel.scaleFactor * 60
		sourceSize.width: ApplicationModel.scaleFactor * 60

		source: iconPath
		fillMode: Image.PreserveAspectFit
	}

	Column {
		Layout.fillWidth: true

		spacing: Constants.text_spacing

		GText {
			width: parent.width

			text: headerText
			textStyle: Style.text.normal_inverse
			maximumLineCount: 1
		}

		GText {
			width: parent.width

			text: mainText
			textStyle: Style.text.header_inverse
			maximumLineCount: 1
			elide: Text.ElideRight
		}

		GText {
			width: parent.width

			text: footerText
			textStyle: Style.text.normal_inverse
			maximumLineCount: 1
			elide: Text.ElideRight
		}
	}
}
