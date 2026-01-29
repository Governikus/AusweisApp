/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

ColumnLayout {
	id: root

	default property alias containerData: contentLayout.data
	property alias containerPadding: contentLayout.anchors.margins
	property alias containerSpacing: contentLayout.spacing
	property alias title: titleText.text

	spacing: Style.dimens.pane_spacing

	Heading {
		id: titleText

		Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
		horizontalAlignment: Text.AlignLeft
		visible: text !== ""
	}
	GPaneBackground {
		Layout.fillWidth: true
		implicitHeight: contentLayout.implicitHeight + contentLayout.anchors.bottomMargin + contentLayout.anchors.topMargin
		implicitWidth: contentLayout.implicitWidth + contentLayout.anchors.leftMargin + contentLayout.anchors.rightMargin

		ColumnLayout {
			id: contentLayout

			anchors.fill: parent
			spacing: 0
		}
	}
}
