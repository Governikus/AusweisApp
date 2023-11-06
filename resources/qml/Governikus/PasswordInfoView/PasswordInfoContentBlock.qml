/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

QtObject {
	property int blockHeaderImageType: PasswordInfoImage.Type.NONE
	property string blockTitle
	property bool headline: false
	property var paragraphList
}
