/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

RowLayout {
	id: baseItem

	property alias title: titleText.text
	property real contentMarginTop: Constants.component_spacing * 1.5
	property real contentMarginBottom: Constants.component_spacing / 2
	property real contentMarginLeft: Constants.component_spacing
	property real contentMarginRight: Constants.component_spacing
	property real minimumSeperatorWidth: Constants.component_spacing

	Accessible.role: Accessible.Heading
	Accessible.name: title
	Accessible.focusable: true

	spacing: Constants.component_spacing

	GText {
		id: titleText

		Layout.topMargin: baseItem.contentMarginTop
		Layout.bottomMargin: baseItem.contentMarginBottom
		Layout.leftMargin: baseItem.contentMarginLeft
		Layout.fillWidth: true
		Layout.maximumWidth: Math.ceil(titleText.implicitWidth)

		Accessible.ignored: true

		textStyle: Style.text.title
	}

	GSeparator {
		Layout.fillWidth: true
		Layout.minimumWidth: baseItem.minimumSeperatorWidth
		Layout.topMargin: baseItem.contentMarginTop
		Layout.bottomMargin: baseItem.contentMarginBottom
		Layout.rightMargin: baseItem.contentMarginRight
	}
}
