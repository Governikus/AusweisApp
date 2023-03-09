/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15

QtObject {
	enum Type {
		PIN,
		CHOOSE_PIN,
		TRANSPORT_PIN,
		SMARTPHONE_AS_CARD_READER,
		PUK,
		CAN,
		CAN_ALLOWED,
		CHANGE_PIN,
		SMART_BLOCKING_CODE,
		NO_PIN
	}

	property url buttonLink: ""
	property string buttonText: ""
	property list<PasswordInfoContentBlock> contentList
	property string hint: ""
	property int imageType: PasswordInfoImage.Type.NONE

	//: LABEL ALL_PLATFORMS
	property string linkText: qsTr("More information")
	property string title
}
