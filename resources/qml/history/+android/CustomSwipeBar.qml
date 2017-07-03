import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as QtControls


import "../"
import "../global"

QtControls.TabBar {
	id: rootItem

	height: firstButton.height
	width: parent.width

	QtControls.TabButton {
		id: firstButton
		padding: Utils.dp(10)
		// TODO: Workaround, use contentItem when switching to Qt 5.7.1
		//       See https://bugreports.qt.io/browse/QTBUG-50992
		text: qsTr("CONTACT")

/*
		contentItem: Text {
			text: qsTr("Contact")
			font.pixelSize: Constants.normal_font_size
			font.capitalization: Font.AllUppercase
			elide: Text.ElideRight
			opacity: enabled ? 1 : 0.3
			color: !parent.checked ? "black" : parent.pressed ? "black" : Constants.blue
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
		}
*/
	}


	QtControls.TabButton {
		padding: Utils.dp(10)
		// TODO: Workaround, use contentItem when switching to Qt 5.7.1
		//       See https://bugreports.qt.io/browse/QTBUG-50992
		text: qsTr("HISTORY")

/*
		contentItem: Text {
			text: qsTr("History")
			font.capitalization: Font.AllUppercase
			font.pixelSize: Constants.normal_font_size
			elide: Text.ElideRight
			opacity: enabled ? 1 : 0.3
			color: !parent.checked ? "black" : parent.pressed ? "black" : Constants.blue
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
		}
*/
	}
}
