/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

ColumnLayout {
	id: root

	signal infoLinkClicked

	spacing: Style.dimens.text_spacing

	Repeater {
		model: [
			//: LABEL ALL_PLATFORM Assistance text for pairing new devices. Step 1 of 5. %1 is a placeholder for the app name. %2 is a version placeholder. %3 and %4 are for bold formatting.
			qsTr("%3Install and open%4 the %3%1%4 (at least version %2) on your NFC-enabled smartphone that you want to use as a card reader.").arg(Qt.application.name).arg("2.1.0").arg("<b>").arg("</b>"),
			//: LABEL ALL_PLATFORM Assistance text for pairing new devices. Step 2 of 5. %1 and %2 are for bold formatting.
			qsTr("Connect both devices to the %1same network%2, e.g. via WLAN.").arg("<b>").arg("</b>"),
			//: LABEL ALL_PLATFORM Assistance text for pairing new devices. Step 3 of 5. %1 and %2 are for bold formatting.
			qsTr("On the NFC-enabled smartphone, select the %1Card reader%2 menu item in the lower menu bar. You may need to skip the initial setup first.").arg("<b>").arg("</b>"),
			//: LABEL ALL_PLATFORM Assistance text for pairing new devices. Step 4 of 5. %1 and %2 are for bold formatting.
			qsTr("Then select %1Pair (new) device%2. You may need to activate NFC first.").arg("<b>").arg("</b>"),
			//: LABEL ALL_PLATFORM Assistance text for pairing new devices. Step 5 of 5
			qsTr("As soon as a pairing code is displayed on the smartphone, you can use it for pairing here.")]

		RowLayout {
			id: delegateLayout

			required property int index
			required property string modelData

			GText {
				Accessible.ignored: true
				Layout.alignment: Qt.AlignTop
				text: (delegateLayout.index + 1) + ". "
			}
			GText {
				Accessible.name: (delegateLayout.index + 1) + ". " + ApplicationModel.stripHtmlTags(delegateLayout.modelData)
				Layout.alignment: Qt.AlignTop
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				text: delegateLayout.modelData
			}
		}
	}
	MoreInformationLink {
		//: LABEL ALL_PLATFORM
		text: qsTr("My smartphone does not show up")

		onClicked: root.infoLinkClicked()
	}
}
