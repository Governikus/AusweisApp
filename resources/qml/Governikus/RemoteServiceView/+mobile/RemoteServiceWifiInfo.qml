import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

RowLayout {
	spacing: Constants.text_spacing

	TintableIcon {
		source: "qrc:/images/info.svg"
		sourceSize.width: Style.dimens.medium_icon_size
		tintColor: infoText.color
	}
	GText {
		id: infoText

		color: Style.color.text_subline

		//: INFO ANDROID IOS The remote service is active. Hint that both devices need to be connected to the same network.
		text: qsTr("Both devices have to be connected to the same WiFi.")
	}
	GSpacer {
		Layout.fillWidth: true
	}
}
