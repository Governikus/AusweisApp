/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

RowLayout {
	id: baseItem

	property real contentMarginBottom: Constants.component_spacing / 2
	property real contentMarginTop: Constants.component_spacing * 1.5
	property real minimumSeperatorWidth: Constants.component_spacing
	property alias title: titleText.text

	Accessible.focusable: true
	Accessible.name: title
	Accessible.role: Accessible.Heading
	spacing: Constants.component_spacing

	GText {
		id: titleText

		Accessible.ignored: true
		Layout.bottomMargin: baseItem.contentMarginBottom
		Layout.topMargin: baseItem.contentMarginTop
		textStyle: Style.text.headline
	}
	GSeparator {
		Layout.bottomMargin: baseItem.contentMarginBottom
		Layout.fillWidth: true
		Layout.minimumWidth: baseItem.minimumSeperatorWidth
		Layout.topMargin: baseItem.contentMarginTop
	}
}
