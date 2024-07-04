/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import QtQuick.Layouts

GPaneBackground {
	id: root

	property alias contentSpacing: paneContent.spacing
	property alias horizontalTitleAlignment: titleText.horizontalAlignment
	property int padding: Constants.pane_padding
	default property alias paneData: paneContent.data
	property int spacing: Constants.pane_spacing
	property alias textStyle: titleText.textStyle
	property alias title: titleText.text

	implicitHeight: content.implicitHeight + 2 * padding
	implicitWidth: content.implicitWidth + 2 * padding

	ColumnLayout {
		id: content

		spacing: root.spacing

		anchors {
			fill: parent
			margins: root.padding
		}
		PaneTitle {
			id: titleText

			Layout.fillWidth: true
		}
		Column {
			id: paneContent

			Layout.fillWidth: true
			spacing: root.spacing
		}
	}
}
