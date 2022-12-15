/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

RowLayout {
	id: baseItem

	property real contentMarginBottom: Constants.component_spacing / 2
	property real contentMarginLeft: Constants.component_spacing
	property real contentMarginRight: Constants.component_spacing
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
		Layout.fillWidth: true
		Layout.leftMargin: baseItem.contentMarginLeft
		Layout.maximumWidth: Math.ceil(titleText.implicitWidth)
		Layout.topMargin: baseItem.contentMarginTop
		textStyle: Style.text.title
	}
	GSeparator {
		Layout.bottomMargin: baseItem.contentMarginBottom
		Layout.fillWidth: true
		Layout.minimumWidth: baseItem.minimumSeperatorWidth
		Layout.rightMargin: baseItem.contentMarginRight
		Layout.topMargin: baseItem.contentMarginTop
	}
}
