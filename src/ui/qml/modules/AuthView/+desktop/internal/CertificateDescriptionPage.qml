/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.View
import Governikus.Type
import Governikus.Style

FlickableSectionPage {
	id: root

	fillWidth: true

	//: LABEL DESKTOP
	title: qsTr("Provider Information")

	ColumnLayout {
		Layout.maximumWidth: Number.POSITIVE_INFINITY
		spacing: Style.dimens.pane_spacing

		Repeater {
			id: listView

			model: CertificateDescriptionModel

			LabeledText {
				Layout.fillWidth: true
				Layout.leftMargin: Style.dimens.pane_padding
				Layout.maximumWidth: implicitWidth
				textFormat: Text.PlainText

				onFocusChanged: if (focus)
					root.positionViewAtItem(this)
			}
		}
	}
}
