import QtQuick 2.10

import Governikus.Global 1.0

Rectangle {
	property alias text: textContent.text
	height: textContent.height + 2 * Constants.component_spacing

	Text {
		id: textContent
		anchors.centerIn: parent
		color: Constants.white
		width: parent.width * 0.8
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h1_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}
}
