/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.ResultView
import Governikus.Style

ResultView {
	id: root

	animationSymbol: Symbol.Type.ERROR
	animationType: AnimationLoader.NFC_RESULT
	//: LABEL ANDROID IOS
	buttonText: qsTr("Retry")
	//: LABEL ANDROID IOS
	header: qsTr("Weak NFC signal")
	//: LABEL ANDROID IOS
	subheader: qsTr("Connection between ID card and smartphone cannot be established")

	ColumnLayout {
		spacing: Style.dimens.text_spacing

		GText {
			//: INFO ANDROID IOS The NFC signal is weak, by repositioning the card the signal might improve.
			text: qsTr("This is what you can do:")
		}
		Repeater {
			model: [
				//: LABEL ANDROID IOS
				qsTr("keep the ID card close to the device's backside"),
				//: LABEL ANDROID IOS
				qsTr("change the position of the ID card until the connection is established"),
				//: LABEL ANDROID IOS
				qsTr("remove any present mobile phone cases"),
				//: LABEL ANDROID IOS
				qsTr("connect your smartphone to a charger and turn off battery saver mode if necessary")]

			BulletPointText {
				required property string modelData

				text: modelData
			}
		}
	}
}
