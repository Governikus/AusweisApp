import QtQuick 2.5

import "."

Image {
	property bool checked

	source: checked ? "qrc:///images/iOS/CheckedCheckbox.png" : ""
	height: Utils.dp(20)
	width: height
	fillMode: Image.PreserveAspectFit
}
