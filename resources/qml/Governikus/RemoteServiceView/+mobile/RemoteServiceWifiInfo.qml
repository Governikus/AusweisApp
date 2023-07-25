import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

RowLayout {
	spacing: Constants.text_spacing

	TintableIcon {
		source: "qrc:/images/info.svg"
		sourceSize.width: Style.dimens.small_icon_size
		tintColor: Style.text.normal_secondary.textColor
	}
	GText {
		Layout.fillWidth: true

		//: INFO ANDROID IOS The remote service is active. Hint that both devices need to be connected to the same network.
		text: qsTr("Both devices have to be connected to the same WiFi.")
		textStyle: Style.text.normal_secondary
	}
}
