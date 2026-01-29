/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
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
	animationType: AnimationLoader.Type.NFC_RESULT
	//: MOBILE
	buttonText: qsTr("Retry")
	//: MOBILE
	header: qsTr("Weak NFC signal")
	//: MOBILE
	subheader: qsTr("Connection between ID card and smartphone cannot be established")

	ColumnLayout {
		spacing: Style.dimens.text_spacing

		GText {
			//: MOBILE The NFC signal is weak, by repositioning the card the signal might improve.
			text: qsTr("This is what you can do:")
		}
		Repeater {
			model: [
				//: MOBILE
				qsTr("keep the ID card close to the device's backside"),
				//: MOBILE
				qsTr("change the position of the ID card until the connection is established"),
				//: MOBILE
				qsTr("remove any present mobile phone cases"),
				//: MOBILE
				qsTr("connect your smartphone to a charger and turn off battery saver mode if necessary")]

			BulletPointText {
				required property string modelData

				text: modelData
			}
		}
	}
}
