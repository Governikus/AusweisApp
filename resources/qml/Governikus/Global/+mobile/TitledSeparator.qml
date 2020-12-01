/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

RowLayout {
	id: baseItem

	property alias title: titleText.text
	property real contentMarginTop: Constants.component_spacing * 1.5
	property real contentMarginBottom: Constants.component_spacing / 2
	property real contentMarginLeft: Constants.component_spacing
	property real contentMarginRight: Constants.component_spacing


	spacing: Constants.component_spacing

	GText {
		id: titleText

		Layout.topMargin: baseItem.contentMarginTop
		Layout.bottomMargin: baseItem.contentMarginBottom
		Layout.leftMargin: baseItem.contentMarginLeft
		Layout.preferredWidth: titleText.implicitWidth

		textStyle: Style.text.title
	}

	GSeparator {
		Layout.fillWidth: true
		Layout.topMargin: baseItem.contentMarginTop
		Layout.bottomMargin: baseItem.contentMarginBottom
		Layout.rightMargin: baseItem.contentMarginRight
	}
}
