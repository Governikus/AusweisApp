/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel

ColumnLayout {
	Repeater {
		model: [
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 1 of 4. %1 is a placeholder-tag for the app name.
			qsTr("Ensure that the %1 on your Smartphone as card reader has at least version %2.").arg(Qt.application.name).arg("1.26.5"),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 2 of 4. %1 is a placeholder-tag for the app name.
			qsTr("Open %1 on your smartphone as card reader.").arg(Qt.application.name),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 3 of 4. %1 and %2 are surrounding tags for bold font.
			qsTr("On that device choose %1Card reader%2 and then %1Pair device%2 resp. %1Pair new device%2.").arg("<b>").arg("</b>"),
			//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 4 of 4
			qsTr("Choose the smartphone in the list shown here to pair it.")]

		GText {
			Accessible.name: ApplicationModel.stripHtmlTags(text)
			Layout.alignment: Qt.AlignTop
			text: (index + 1) + ". " + modelData
		}
	}
}
