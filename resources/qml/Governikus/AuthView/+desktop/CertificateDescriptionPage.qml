/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.CertificateDescriptionModel

FlickableSectionPage {
	fillWidth: true

	titleBarAction: TitleBarAction {
		rootEnabled: false
		//: LABEL DESKTOP
		text: qsTr("Provider Information")
	}

	ColumnLayout {
		Layout.maximumWidth: Number.POSITIVE_INFINITY
		spacing: Constants.pane_spacing

		Repeater {
			id: listView

			model: CertificateDescriptionModel

			LabeledText {
				Layout.fillWidth: true
				Layout.leftMargin: Constants.pane_padding
				Layout.maximumWidth: implicitWidth
				label: model.label
				text: model.text
				textFormat: Text.PlainText
			}
		}
	}
}
