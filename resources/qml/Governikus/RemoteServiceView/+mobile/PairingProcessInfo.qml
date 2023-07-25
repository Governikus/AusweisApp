/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

ColumnLayout {
	Repeater {
		model: [
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 1 of 4. %1 is a placeholder-tag for the app name.
			qsTr("Ensure that the %1 on your Smartphone as card reader has at least version %2.").arg(Qt.application.name).arg(Qt.application.version),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 2 of 4. %1 is a placeholder-tag for the app name.
			qsTr("Open %1 on your smartphone as card reader.").arg(Qt.application.name),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 3 of 4. %1 and %2 are surrounding tags for bold font.
			qsTr("On that device choose %1Card reader%2 and then %1Pair device%2 resp. %1Pair new device%2.").arg("<b>").arg("</b>"),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 4 of 4
			qsTr("Choose the smartphone in the list shown here to pair it.")]

		RowLayout {
			Layout.fillWidth: true
			Layout.topMargin: Constants.component_spacing
			spacing: Constants.text_spacing

			GText {
				Accessible.ignored: true
				Layout.alignment: Qt.AlignTop
				Layout.preferredWidth: Style.dimens.small_icon_size
				text: (index + 1) + "."
			}
			GText {
				Accessible.name: (index + 1) + ". " + ApplicationModel.stripHtmlTags(modelData)
				Layout.alignment: Qt.AlignTop
				Layout.fillWidth: true
				text: modelData
			}
		}
	}
}
