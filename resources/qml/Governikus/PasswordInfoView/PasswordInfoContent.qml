/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

QtObject {
	enum Type {
		PIN,
		SMART_PIN,
		CHOOSE_PIN,
		CHOOSE_SMART_PIN,
		TRANSPORT_PIN,
		PUK,
		CAN,
		CAN_ALLOWED,
		CHANGE_PIN,
		SMART_BLOCKING_CODE,
		NO_PIN,
		EMPTY
	}

	property url buttonLink: ""
	property string buttonText: ""
	property list<PasswordInfoContentBlock> contentList
	property string hint: ""
	property string hintButtonText: ""

	//: LABEL ALL_PLATFORMS
	property string linkText: qsTr("More information")
	property string title
}
