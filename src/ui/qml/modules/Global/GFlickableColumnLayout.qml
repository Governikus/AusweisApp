/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

GFlickable {
	id: root

	default property alias data: contentLayout.data
	property real maximumContentWidth: -1
	property alias spacing: contentLayout.spacing

	bottomMargin: Style.dimens.pane_padding
	contentHeight: contentLayout.height
	contentWidth: limitingLayout.width
	implicitHeight: contentLayout.implicitHeight + topMargin + bottomMargin
	implicitWidth: contentLayout.implicitWidth + leftMargin + rightMargin
	leftMargin: Style.dimens.pane_padding
	rightMargin: Style.dimens.pane_padding
	topMargin: Style.dimens.pane_padding

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
