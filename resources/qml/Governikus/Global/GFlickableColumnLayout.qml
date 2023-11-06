/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global

GFlickable {
	id: root

	default property alias data: contentLayout.data
	property real maximumContentWidth: -1
	property alias spacing: contentLayout.spacing

	bottomMargin: Constants.pane_padding
	contentHeight: contentLayout.height
	contentWidth: limitingLayout.width
	implicitHeight: contentLayout.implicitHeight + topMargin + bottomMargin
	implicitWidth: contentLayout.implicitWidth + leftMargin + rightMargin
	leftMargin: Constants.pane_padding
	rightMargin: Constants.pane_padding
	topMargin: Constants.pane_padding

	ColumnLayout {
		id: limitingLayout

		height: root.height - root.topMargin - root.bottomMargin
		width: root.width - root.leftMargin - root.rightMargin

		ColumnLayout {
			id: contentLayout

			Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
			Layout.maximumWidth: root.maximumContentWidth
		}
	}
}
