<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru">
<context>
    <name>AbortedProgressView</name>
    <message>
        <source>Aborting process and informing the service provider</source>
        <extracomment>DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider</extracomment>
        <translation>Отмена процесса и информирование провайдера службы</translation>
    </message>
    <message>
        <source>Please wait a moment.</source>
        <extracomment>DESKTOP Information message about cancellation process with present network connectivity</extracomment>
        <translation>Подождите.</translation>
    </message>
    <message>
        <source>Network problems detected, trying to reach server within 30 seconds.</source>
        <extracomment>DESKTOP Information message about cancellation process without working network connectivity</extracomment>
        <translation>Обнаружены проблемы в сети, в течение 30 секунд выполняется подключение к серверу.</translation>
    </message>
</context>
<context>
    <name>App</name>
    <message>
        <source>The program remains available via the icon in the menu bar. Click on the %1 icon to reopen the user interface.</source>
        <extracomment>DESKTOP Content of the popup that is shown when the AA is closed and the close/minimize info was not disabled. macOS specific if autostart is enabled.</extracomment>
        <translation>Программа по-прежнему доступна через значок в строке меню. Нажмите на символ %1, чтобы снова открыть пользовательский интерфейс.</translation>
    </message>
    <message>
        <source>The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.</source>
        <extracomment>DESKTOP Content of the popup that is shown when the AA is closed and the close/minimize info was not disabled.</extracomment>
        <translation>Программа по-прежнему доступна через значок на панели задач. Нажмите на символ %1, чтобы снова открыть пользовательский интерфейс.</translation>
    </message>
    <message>
        <source>Failed to retrieve update information.</source>
        <extracomment>DESKTOP Message that the update data is invalid and can&apos;t be used.</extracomment>
        <translation>Сбой загрузки информации об обновлении.</translation>
    </message>
    <message>
        <source>An update is available (version %1).</source>
        <extracomment>DESKTOP An update was found which matches the current platform, the new version number is shown in the message.</extracomment>
        <translation>Доступно обновление (версия %1).</translation>
    </message>
    <message>
        <source>To close the app, tap the back button 2 times.</source>
        <extracomment>MOBILE Hint that is shown if the users pressed the &quot;back&quot; button on the top-most navigation level for the first time (a second press closes the app).</extracomment>
        <translation>Чтобы закрыть приложение, два раза нажмите кнопку «Назад».</translation>
    </message>
</context>
<context>
    <name>AuthCanceledView</name>
    <message>
        <source>This may take a few moments.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Это может занять некоторое время.</translation>
    </message>
    <message>
        <source>The authentication was cancelled successfully.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Аутентификация была успешно отменена.</translation>
    </message>
    <message>
        <source>Back to provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вернуться к провайдеру</translation>
    </message>
    <message>
        <source>Authentication was canceled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Аутентификация отменена</translation>
    </message>
    <message>
        <source>The authentication is being canceled at the provider (%1)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Аутентификация отменяется у провайдера (%1)</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Назад к начальной странице</translation>
    </message>
    <message>
        <source>The cancellation was triggered on the card reader.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Отмена была произведена  на устройстве чтения карт.</translation>
    </message>
    <message>
        <source>Back to setup</source>
        <extracomment>ALL_PLATFORM</extracomment>
        <translation>Назад к настройкам</translation>
    </message>
    <message>
        <source>You may start a new authentication after clicking the button.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Вы можете запустить  новую аутентификацию после, щелкнув на  кнопку.</translation>
    </message>
    <message>
        <source>You may start a new authentication after tapping the button.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Вы можете запустить  новую аутентификацию после нажатия кнопки.</translation>
    </message>
    <message>
        <source>Cancellation progress</source>
        <extracomment>ALL_PLATFORMS Name of an progress indicator during the cancellation of an authentication read by screen readers</extracomment>
        <translation>Ход выполнения процесса отмены</translation>
    </message>
</context>
<context>
    <name>AuthController</name>
    <message>
        <source>Identify</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Идентифицировать</translation>
    </message>
    <message>
        <source>Cancel authentication process</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Отмена процесса аутентификации</translation>
    </message>
    <message>
        <source>Acquiring provider certificate</source>
        <extracomment>DESKTOP Header of the progress status message during the authentication process.
----------
MOBILE Header of the progress status message during the authentication process.</extracomment>
        <translation>Получение сертификата провайдера</translation>
    </message>
    <message>
        <source>Authentication in progress</source>
        <extracomment>DESKTOP Header of the progress status message during the authentication process.
----------
MOBILE Header of the progress status message during the authentication process.</extracomment>
        <translation>Выполняется аутентификация</translation>
    </message>
    <message>
        <source>Please wait a moment.</source>
        <extracomment>DESKTOP Generic status message during the authentication process.
----------
DESKTOP Information message about cancellation process with present network connectivity
----------
MOBILE Generic status message during the authentication process.</extracomment>
        <translation>Подождите.</translation>
    </message>
    <message>
        <source>Please do not move the ID card.</source>
        <extracomment>DESKTOP Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
----------
MOBILE Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.</extracomment>
        <translation>Не перемещайте идентификационную карту.</translation>
    </message>
    <message>
        <source>Please observe the display of your card reader.</source>
        <extracomment>DESKTOP The card reader requests the user&apos;s attention.
----------
MOBILE The card reader requests the user&apos;s attention.</extracomment>
        <translation>Учитывайте указания на дисплее устройства чтения карт.</translation>
    </message>
    <message>
        <source>Send log</source>
        <extracomment>MOBILE</extracomment>
        <translation>Отправить файл журнала</translation>
    </message>
    <message>
        <source>Authenticate with provider</source>
        <extracomment>MOBILE A11y button to confirm the PIN and start the provider authentication</extracomment>
        <translation>Аутентификация с помощью провайдера</translation>
    </message>
    <message>
        <source>Authentication failed</source>
        <extracomment>MOBILE</extracomment>
        <translation>Сбой аутентификации</translation>
    </message>
    <message>
        <source>Please establish an internet connection.</source>
        <extracomment>DESKTOP Content of the message that no network connection is present during the authentication procedure.</extracomment>
        <translation>Подключитесь к Интернету.</translation>
    </message>
    <message>
        <source>No network connectivity</source>
        <extracomment>DESKTOP Header of the message that no network connection is present during the authentication procedure.</extracomment>
        <translation>Отсутствует сетевое подключение</translation>
    </message>
    <message>
        <source>Weak NFC signal. Please
 change the card position
 remove the mobile phone case (if present)
 connect the smartphone with a charging cable</source>
        <extracomment>DESKTOP The NFC signal is weak or unstable, the user is asked to change the card&apos;s position to (hopefully) reduce the distance to the NFC chip.</extracomment>
        <translation>Слабый сигнал NFC. Выполните следующее:
- Измените положение карты
- Снимите чехол с мобильного телефона (при наличии)
- Подключите смартфон к зарядному кабелю</translation>
    </message>
    <message>
        <source>Weak NFC signal. Please
 make sure the card is positioned correctly on the reader
 do note move the card while it is being accessed</source>
        <extracomment>DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.</extracomment>
        <translation>Слабый сигнал NFC. Выполните следующее:
- Убедитесь в том, что карта правильно вставлена в устройство чтения карт
- Не перемещайте карту, пока система получает к ней доступ</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>MOBILE</extracomment>
        <translation>Назад к начальной странице</translation>
    </message>
    <message>
        <source>Back to setup</source>
        <extracomment>MOBILE</extracomment>
        <translation>Назад к настройкам</translation>
    </message>
    <message>
        <source>Aborting process and informing the service provider</source>
        <extracomment>DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider</extracomment>
        <translation>Отмена процесса и информирование провайдера службы</translation>
    </message>
    <message>
        <source>Network problems detected, trying to reach server within 30 seconds.</source>
        <extracomment>DESKTOP Information message about cancellation process without working network connectivity</extracomment>
        <translation>Обнаружены проблемы в сети, в течение 30 секунд выполняется подключение к серверу.</translation>
    </message>
    <message>
        <source>Authentication progress</source>
        <extracomment>DESKTOP Name of an progress indicator during an authentication read by screen readers
----------
MOBILE Name of an progress indicator during an authentication read by screen readers</extracomment>
        <translation>Прогресс аутентификации</translation>
    </message>
</context>
<context>
    <name>AuthView</name>
    <message>
        <source>Identify</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Идентифицировать</translation>
    </message>
</context>
<context>
    <name>AutoRedirectDecision</name>
    <message>
        <source>Currently, the app automatically redirects you back to the service provider after authentication. This may not allow your screen reader to provide all information. To ensure you receive all information, enable manual redirection to the service provider.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>В настоящее время вы используете функцию автоматического перенаправления. Это может привести к тому, что скринридер пропустит некоторую информацию в ходе процесса аутентификации. Чтобы обеспечить получение всей информации, включите функцию ручного перенаправления.</translation>
    </message>
    <message>
        <source>You can change your preference at any time in the settings.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вы можете  в любое врем изменить в настройках ваши предпочтения.</translation>
    </message>
    <message>
        <source>Skip and keep automatic redirection</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Пропустить и сохранить автоматическое перенаправление</translation>
    </message>
    <message>
        <source>Enable manual redirection</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Включить перенаправление вручную</translation>
    </message>
    <message>
        <source>Optimize your settings for screen reading</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Оптимизировать настройки для скринридинга</translation>
    </message>
    <message>
        <source>Do not display this message in future.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не показывать это сообщение в будущем.</translation>
    </message>
</context>
<context>
    <name>AutostartView</name>
    <message>
        <source>Do you want to automatically start the %1 after boot?</source>
        <extracomment>DESKTOP Question if the App shall be started automatically after boot</extracomment>
        <translation>Запустить %1 автоматически после запуска?</translation>
    </message>
    <message>
        <source>Yes, start app automatically</source>
        <extracomment>DESKTOP Button to decide start the app automatically</extracomment>
        <translation>Да, запустить приложение автоматически</translation>
    </message>
    <message>
        <source>The %1 gets started on system boot, so that it can be automatically opened on an authentication.</source>
        <extracomment>DESKTOP Information text why autostart of the App is advisable 1/2</extracomment>
        <translation>%1 запускается при загрузке системы, что позволяет открыть его автоматически после аутентификации.</translation>
    </message>
    <message>
        <source>An authentication cannot be carried out if the %1 is not running. Then it has to be started manually. The automatic start is therefore recommended.</source>
        <extracomment>DESKTOP Information text why autostart of the App is advisable 2/2</extracomment>
        <translation>Аутентификация не может быть выполнена, если %1 не запущено. В этом случае его нужно запустить вручную. Поэтому рекомендуется использовать автоматический запуск.</translation>
    </message>
    <message>
        <source>No, don&apos;t start app automatically</source>
        <extracomment>DESKTOP Button to decide to not start the app automatically</extracomment>
        <translation>Нет, не запускать приложение автоматически</translation>
    </message>
</context>
<context>
    <name>BaseConfirmationPopup</name>
    <message>
        <source>OK</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>OK</translation>
    </message>
    <message>
        <source>Cancel</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Отмена</translation>
    </message>
</context>
<context>
    <name>BaseHeading</name>
    <message>
        <source>Heading</source>
        <extracomment>WINDOWS Screenreader announcement, that the current item is a heading.</extracomment>
        <translation>Заглавие</translation>
    </message>
</context>
<context>
    <name>BaseSkipOnboardingConfirmation</name>
    <message>
        <source>Set up the eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Настройте функцию онлайн-идентификации</translation>
    </message>
    <message>
        <source>Are you sure you want to skip the setup?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вы уверены, что хотите пропустить настройки?</translation>
    </message>
    <message>
        <source>Yes, skip setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Да, пропустить настройки</translation>
    </message>
    <message>
        <source>No, set up %1</source>
        <extracomment>ALL_PLATFORMS %1 will be replaced with the app name</extracomment>
        <translation>Нет, перейти к настройке %1</translation>
    </message>
    <message>
        <source>You can call up the initial setup again at any time under %1Help%2.</source>
        <extracomment>ALL_PLATFORMS The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Вы можете в любой момент вернутся к первоначальной настройке через раздел %1Справка%2.</translation>
    </message>
</context>
<context>
    <name>BaseTransportPinReminderView</name>
    <message>
        <source>Set up card PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Создание PIN-кода идентификационной карты</translation>
    </message>
    <message>
        <source>You need an %1ID Card PIN%2 in order to use the eID function. If you just have %1Transport PIN%2, you have to %1replace%2 it with an %1ID Card PIN once%2.</source>
        <extracomment>ALL_PLATFORMS %1 and %2 are replaced with bold highlighting</extracomment>
        <translation>Вам потребуется %1PIN-код идентификационной карты%2 чтобы использовать функцию онлайн-идентификации. Если у вас есть только %1временный PIN-код%2, вам нужно однократно %1заменить%2 его на %1PIN-код идентификационной карты%2.</translation>
    </message>
    <message>
        <source>What kind of PIN do you have?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Какой у вас вид PIN-кода?</translation>
    </message>
</context>
<context>
    <name>BuildHelper</name>
    <message>
        <source>Application</source>
        <translation>Приложение</translation>
    </message>
    <message>
        <source>Application Version</source>
        <translation>Версия приложения</translation>
    </message>
    <message>
        <source>Organization</source>
        <translation>Организация</translation>
    </message>
    <message>
        <source>Organization Domain</source>
        <translation>Домен организации</translation>
    </message>
    <message>
        <source>System</source>
        <translation>Система</translation>
    </message>
    <message>
        <source>Kernel</source>
        <translation>Ядро</translation>
    </message>
    <message>
        <source>Architecture</source>
        <translation>Архитектура</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Устройство</translation>
    </message>
    <message>
        <source>VersionCode</source>
        <translation>Код версии</translation>
    </message>
    <message>
        <source>Certificate</source>
        <translation>Сертификат</translation>
    </message>
    <message>
        <source>Qt Version</source>
        <translation>Версия Qt</translation>
    </message>
    <message>
        <source>OpenSSL Version</source>
        <translation>Версия OpenSSL</translation>
    </message>
</context>
<context>
    <name>CardNotActivatedBaseView</name>
    <message>
        <source>Activate ID card with PIN Reset Letter</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Активируйте идентификационную карту с помощью  письма для переустановки PIN-кода</translation>
    </message>
    <message>
        <source>How do I activate the eID function?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished">Запросить активацию функции онлайн-идентификации</translation>
    </message>
    <message>
        <source>eID function is not activated</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Функция онлайн-идентификации не активирована</translation>
    </message>
    <message>
        <source>Did you recently order a PIN Reset Letter with an activation code?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вы недавно запрашивали письмо для переустановки PIN-кода с кодом активации?</translation>
    </message>
    <message>
        <source>Activate your ID card using the activation code</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Активируйте вашу идентификационную карту с помощью кода активации</translation>
    </message>
    <message>
        <source>Yes, I already have an activation code</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Да, у меня уже есть код активации</translation>
    </message>
    <message>
        <source>Request the activation of the eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Запросить код активации функции онлайн-идентификации</translation>
    </message>
    <message>
        <source>No, I don&apos;t have an activation code</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Нет, у меня нет кода активации</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Отменить настройки</translation>
    </message>
    <message>
        <source>Enter your activation code of your present PIN Reset Letter into the following website.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Введите на следующем веб-сайте ваш код активации, указанный в актуальном письме для переустановки PIN-кода.</translation>
    </message>
    <message>
        <source>Enter activation code</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Введите  код активации</translation>
    </message>
</context>
<context>
    <name>CardPositionView</name>
    <message>
        <source>Retry</source>
        <extracomment>MOBILE</extracomment>
        <translation>Повторить попытку</translation>
    </message>
    <message>
        <source>Weak NFC signal</source>
        <extracomment>MOBILE</extracomment>
        <translation>Слабый сигнал NFC</translation>
    </message>
    <message>
        <source>Connection between ID card and smartphone cannot be established</source>
        <extracomment>MOBILE</extracomment>
        <translation>Не получается установить соединение между вашей идентификационной картой и смартфоном</translation>
    </message>
    <message>
        <source>This is what you can do:</source>
        <extracomment>MOBILE The NFC signal is weak, by repositioning the card the signal might improve.</extracomment>
        <translation>Попробуйте сделать следующее:</translation>
    </message>
    <message>
        <source>keep the ID card close to the device&apos;s backside</source>
        <extracomment>MOBILE</extracomment>
        <translation>Поднесите идентификационную карту к задней стороне устройства</translation>
    </message>
    <message>
        <source>change the position of the ID card until the connection is established</source>
        <extracomment>MOBILE</extracomment>
        <translation>Перемещайте идентификационную карту до тех пор, пока не будет установлено соединение</translation>
    </message>
    <message>
        <source>remove any present mobile phone cases</source>
        <extracomment>MOBILE</extracomment>
        <translation>Снимите все имеющиеся чехлы с мобильного телефона</translation>
    </message>
    <message>
        <source>connect your smartphone to a charger and turn off battery saver mode if necessary</source>
        <extracomment>MOBILE</extracomment>
        <translation>Подключите смартфон к зарядному устройству и при необходимости отключите режим энергосбережения</translation>
    </message>
</context>
<context>
    <name>CardReaderDelegate</name>
    <message>
        <source>Press space to open link.</source>
        <extracomment>DESKTOP Text read by screen reader if the text contains a web link to a card reader driver which may be opened.</extracomment>
        <translation>Нажмите на пробел, чтобы открыть ссылку в браузере.</translation>
    </message>
</context>
<context>
    <name>CardReaderView</name>
    <message>
        <source>Connected USB card readers</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Подключенные USB-устройства чтения карт</translation>
    </message>
    <message>
        <source>The connection to your system&apos;s smartcard service could not be established. You can try to resolve this issue and restart the scan.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Не удалось подключиться к службе смарт-карт вашей системы. Вы можете попытаться устранить проблему и перезапустить сканирование.</translation>
    </message>
    <message>
        <source>Restart smartcard scan</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Перезапустить сканирование смарт-карты</translation>
    </message>
    <message>
        <source>List of connected card readers.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Перечень подключенных устройств чтения карт.</translation>
    </message>
    <message>
        <source>After connecting a new card reader it may take a few seconds to recognize the driver.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>После подключения нового устройства чтения карт может потребоваться несколько секунд, чтобы распознать драйвер.</translation>
    </message>
    <message>
        <source>It may be necessary to restart your system after installing the driver.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>После установки драйвера может потребоваться перезагрузка системы.</translation>
    </message>
    <message>
        <source>Only connected card readers are shown here.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Здесь показаны только соединенные устройства чтения карт.</translation>
    </message>
    <message>
        <source>No card reader connected</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Устройство чтения карт не соединено</translation>
    </message>
</context>
<context>
    <name>ChangePinController</name>
    <message>
        <source>Setting new ID card PIN</source>
        <extracomment>DESKTOP Processing screen label while the card communication is running after the new PIN has been entered during PIN change process.
----------
MOBILE Processing screen label while the card communication is running after the new ID card PIN has been entered during PIN change process.</extracomment>
        <translation>Установить новый PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>Change ID card PIN</source>
        <extracomment>DESKTOP Processing screen label while the card communication is running after the old PIN has been entered during PIN change process.
----------
MOBILE Processing screen label while the card communication is running before the new ID card PIN has been entered during PIN change process.</extracomment>
        <translation>Изменить PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>Please wait a moment.</source>
        <extracomment>DESKTOP Generic progress message during PIN change process.</extracomment>
        <translation>Подождите.</translation>
    </message>
    <message>
        <source>Please do not move the ID card.</source>
        <extracomment>DESKTOP Processing screen text while the card communication is running after the PIN has been entered during PIN change process.
----------
MOBILE Loading screen during PIN change process, data communication is currently ongoing. Message is usually not visible since the password handling with basic reader is handled by EnterPasswordView.</extracomment>
        <translation>Не перемещайте идентификационную карту.</translation>
    </message>
    <message>
        <source>Please observe the display of your card reader.</source>
        <extracomment>DESKTOP Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
----------
MOBILE Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.</extracomment>
        <translation>Учитывайте указания на дисплее устройства чтения карт.</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Назад к начальной странице</translation>
    </message>
    <message>
        <source>Change PIN</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Изменить PIN-код</translation>
    </message>
    <message>
        <source>Weak NFC signal. Please
 change the card position
 remove the mobile phone case (if present)
 connect the smartphone with a charging cable</source>
        <extracomment>DESKTOP The NFC signal is weak or unstable, the user is asked to change the card&apos;s position to (hopefully) reduce the distance to the NFC chip.</extracomment>
        <translation>Слабый сигнал NFC. Выполните следующее:
- Измените положение карты
- Снимите чехол с мобильного телефона (при наличии)
- Подключите смартфон к зарядному кабелю</translation>
    </message>
    <message>
        <source>Weak NFC signal. Please
 make sure the card is positioned correctly on the reader
 do note move the card while it is being accessed</source>
        <extracomment>DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.</extracomment>
        <translation>Слабый сигнал NFC. Выполните следующее:
- Убедитесь в том, что карта правильно вставлена в устройство чтения карт
- Не перемещайте карту, пока система получает к ней доступ</translation>
    </message>
    <message>
        <source>Check ID card PIN</source>
        <extracomment>DESKTOP Processing screen label while the card communication is running after the PIN has been entered during PIN check process.</extracomment>
        <translation>Проверьте PIN-код идентификационной карты</translation>
    </message>
</context>
<context>
    <name>ChangePinInfoView</name>
    <message>
        <source>6-digit card PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>6-значный PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>Have your documents ready</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Приготовьте свои документы</translation>
    </message>
    <message>
        <source>You need:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вам понадобится:</translation>
    </message>
    <message>
        <source>Your %1ID card%2</source>
        <extracomment>ALL_PLATFORMS The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Ваша %1идентификационная карта%2</translation>
    </message>
    <message>
        <source>And either:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>А также:</translation>
    </message>
    <message>
        <source>Your %1card PIN%2 (self-chosen)</source>
        <extracomment>ALL_PLATFORMS The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Ваш %1PIN-код идентификационной карты%2 (выбранный вами)</translation>
    </message>
    <message>
        <source>or:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>или:</translation>
    </message>
    <message>
        <source>Letter from the PIN Reset Service containing the 6-digit PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Письмо службы сброса PIN-кода с 6-значным PIN-кодом</translation>
    </message>
</context>
<context>
    <name>ChangePinView</name>
    <message>
        <source>Change PIN</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Изменить PIN-код</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <translation>Отменить настройки</translation>
    </message>
</context>
<context>
    <name>ChangePinViewContent</name>
    <message>
        <source>What kind of PIN do you have?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Какой у вас PIN-код?</translation>
    </message>
</context>
<context>
    <name>ChangeTransportPinInfoView</name>
    <message>
        <source>5-digit Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>5-значный временный PIN-код</translation>
    </message>
    <message>
        <source>Have your documents ready</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Приготовьте свои документы</translation>
    </message>
    <message>
        <source>You need:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вам понадобится:</translation>
    </message>
    <message>
        <source>Your %1ID card%2</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ваша %1идентификационная карта%2</translation>
    </message>
    <message>
        <source>Letter with 5-digit %1Transport PIN%2</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Письмо с 5-значным %1временным PIN-кодом%2</translation>
    </message>
    <message>
        <source>You are not using the ID card online for the 1st time?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вы не в первый раз используете идентификационную карту онлайн?</translation>
    </message>
    <message>
        <source>You have to enter the Transport PIN only when activating the ID card for the first time. Your own card PIN is linked on all devices with your ID card as soon as you have set it.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вам нужно ввести временный PIN-код только при первой активации идентификационной карты. Сразу после установления вами PIN-кода для вашей карты он будет привязан на всех устройствах к вашей идентификационной карте.</translation>
    </message>
    <message>
        <source>In this case, tap on Back and choose 6-digit PIN.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>В этом случае нажмите &quot;Назад&quot; и выберите 6-значный PIN-код.</translation>
    </message>
</context>
<context>
    <name>CheckConnectivityView</name>
    <message>
        <source>No network connectivity</source>
        <extracomment>MOBILE</extracomment>
        <translation>Отсутствует сетевое подключение</translation>
    </message>
    <message>
        <source>Please establish an internet connection.</source>
        <extracomment>MOBILE No network connection, the user needs to active the network interface or abort the procedure.</extracomment>
        <translation>Подключитесь к Интернету.</translation>
    </message>
</context>
<context>
    <name>CheckIDCardNoNfcSuggestion</name>
    <message>
        <source>Pair a NFC-enabled smartphone</source>
        <extracomment>MOBILE</extracomment>
        <translation>Cопряжение смартфона с функцией NFC</translation>
    </message>
    <message>
        <source>If you want to use %1 on this device anyway, you can %2pair%3 another %2NFC-enabled smartphone%3 as a card reader.</source>
        <extracomment>MOBILE %1 will be replaced with the application name. The text between %2 and %3 will be emphasized.</extracomment>
        <translation>Если вы хотите использовать %1 на этом устройстве, вы можете %2сопрясти%3 другой %2 смартфон с фунуцией NFC%3 в качестве устройства чтения карт.</translation>
    </message>
    <message>
        <source>We recommend that you set up your ID card on another smartphone. You can call the setup again at any time under %1Help%2.</source>
        <extracomment>MOBILE The text between %1 and %2 will be emphasized.</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Alternatively you may use the %1 with an %2USB card reader%3 on your %2PC%3.</source>
        <extracomment>MOBILE %1 will be replaced with the application name. The text between %2 and %3 will be emphasized.</extracomment>
        <translation>Вы также можете пользоваться приложением %1 на вашем %2компьютере%3 с %2USB-устройством чтения карт%3.</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>MOBILE</extracomment>
        <translation>Отменить настройки</translation>
    </message>
    <message>
        <source>No NFC available</source>
        <extracomment>MOBILE</extracomment>
        <translation>Функция NFC недоступна</translation>
    </message>
    <message>
        <source>This device cannot read the ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Это устройство не может прочитать идентификационную карту</translation>
    </message>
</context>
<context>
    <name>CheckIDCardResultView</name>
    <message>
        <source>Check device and ID card</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Проверить устройство и идентификационную карту</translation>
    </message>
    <message>
        <source>Test Result</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Результат проверки</translation>
    </message>
    <message>
        <source>Continue</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Продолжить</translation>
    </message>
    <message>
        <source>How to proceed?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Что делать дальше?</translation>
    </message>
    <message>
        <source>What does that mean?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Что это значит?</translation>
    </message>
    <message>
        <source>You may now try the function: &quot;See my personal data&quot;. Click the &quot;%1&quot; button to do so now.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Проверьте функцию: «Просмотреть персональные данные». Щелкните для этого кнопку «%1».</translation>
    </message>
    <message>
        <source>You may now try the function: &quot;See my personal data&quot;. Tap the &quot;%1&quot; button to do so now.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Проверьте функцию: «Просмотреть персональные данные». Нажмите для этого кнопку «%1».</translation>
    </message>
    <message>
        <source>NFC supported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>NFC поддерживается</translation>
    </message>
    <message>
        <source>Your device doesn&apos;t support NFC</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ваше устройство не поддерживает NFC</translation>
    </message>
    <message>
        <source>NFC enabled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>NFC активировано</translation>
    </message>
    <message>
        <source>No supported card detected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Поддерживаемые карты не обнаружены</translation>
    </message>
    <message>
        <source>ID card detected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Обнаружена идентификационная карта</translation>
    </message>
    <message>
        <source>Extended length not supported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не поддерживается расширенная длина</translation>
    </message>
    <message>
        <source>Extended length supported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Поддерживается расширенная длина</translation>
    </message>
    <message>
        <source>ID card access failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой доступа к идентификационной карте</translation>
    </message>
    <message>
        <source>ID card access successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Успешный доступ к идентификационной карте</translation>
    </message>
    <message>
        <source>ID card PIN suspended</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Действие PIN-кода идентификационной карты приостановлено</translation>
    </message>
    <message>
        <source>ID card PIN blocked</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PIN-код идентификационной карты заблокирован</translation>
    </message>
    <message>
        <source>ID card functional</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Идентификационная карта готова к использованию</translation>
    </message>
    <message>
        <source>eID function disabled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Функция онлайн-идентификации деактивирована</translation>
    </message>
    <message>
        <source>eID function enabled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Функция онлайн-идентификации активирована</translation>
    </message>
    <message>
        <source>Card reader supported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Устройство чтения карт поддерживается</translation>
    </message>
    <message>
        <source>Smartphone as card reader detected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Обнаружен смартфон в качестве устройства чтения карт</translation>
    </message>
</context>
<context>
    <name>CheckIDCardSuggestionView</name>
    <message>
        <source>Unknown result code: %1</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неизвестный код результата: %1</translation>
    </message>
    <message>
        <source>Open website</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Открыть сайт</translation>
    </message>
    <message>
        <source>No supported card detected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Поддерживаемые карты не обнаружены</translation>
    </message>
    <message>
        <source>Retry</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Повторить попытку</translation>
    </message>
    <message>
        <source>No extended length</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не поддерживается расширенная длина</translation>
    </message>
    <message>
        <source>The NFC interface of your mobile device does not support Extended Length communication and cannot be used to read the ID card. Unfortunately, the %1 has no influence on this restriction.&lt;br&gt;&lt;br&gt;You can find smartphones compatible with the %1 on our website.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Интерфейс NFC вашего мобильного устройства не поддерживает связь по протоколу с расширенной длиной и не может использоваться для считывания идентификационной карты. К сожалению, %1 не влияет на это ограничение.&lt;br&gt;&lt;br&gt;Список совместимых с %1 смартфонов см. на нашем сайте.</translation>
    </message>
    <message>
        <source>ID card access failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой доступа к идентификационной карте</translation>
    </message>
    <message>
        <source>It was not possible to establish a stable connection with your ID card.&lt;br&gt;&lt;br&gt;Please start the check again. Try a different card position and make sure not to move the card during the test.&lt;br&gt;&lt;br&gt;If a connection to the ID card cannot be established even with different card positions, this indicates that the NFC interface of your mobile device cannot supply the ID card with sufficient power.&lt;br&gt;&lt;br&gt;Smartphones compatible with %1 can be found on our &lt;a href=&quot;%2&quot;&gt;website&lt;/a&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не удалось установить стабильное соединение с вашей идентификационной картой.&lt;br&gt;&lt;br&gt;Перезапустите проверку. Попробуйте изменить положение карты и убедитесь в том, что карта не смещается во время проверки.&lt;br&gt;&lt;br&gt;Если невозможно установить соединение с идентификационной картой в разных положениях, то это значит, что интерфейс NFC вашего мобильного устройства не подает достаточно тока на идентификационную карту.&lt;br&gt;&lt;br&gt;Список совместимых с %1 смартфонов см. на нашем &lt;a href=&quot;%2&quot;&gt;сайте&lt;/a&gt;.</translation>
    </message>
    <message>
        <source>ID card PIN suspended</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Действие PIN-кода идентификационной карты приостановлено</translation>
    </message>
    <message>
        <source>The ID card PIN has been entered incorrectly 2 times in a row. This is why you must first enter the 6-digit Card Access Number (CAN) for the next identification process. You can find it at the bottom right of the front of your ID card.</source>
        <extracomment>ALL_PLATFORMS Sentence 1 of 3 of CAN explanation</extracomment>
        <translation>PIN-код идентификационной карты введен неправильно два раза подряд. Поэтому перед следующей попыткой идентификации сначала необходимо ввести 6-значный код доступа (CAN). Вы найдете его в правом нижнем углу на лицевой стороне идентификационной карты.</translation>
    </message>
    <message>
        <source>You may now try the function: &quot;See my personal data&quot;.</source>
        <extracomment>ALL_PLATFORMS Sentence 2 of 3 of CAN explanation
----------
ALL_PLATFORMS Sentence 2 of 3 of PUK explanation</extracomment>
        <translation>Теперь проверьте функцию: «Просмотреть персональные данные».</translation>
    </message>
    <message>
        <source>There, you can also use the CAN to unblock the ID card PIN.</source>
        <extracomment>ALL_PLATFORMS Sentence 3 of 3 of CAN explanation</extracomment>
        <translation>Вы также можете использовать код CAN, чтобы разблокировать PIN-код идентификационной карты.</translation>
    </message>
    <message>
        <source>ID card PIN blocked</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PIN-код идентификационной карты заблокирован</translation>
    </message>
    <message>
        <source>The ID card PIN has been entered incorrectly 3 times. Therefore, you must first enter the 10-digit PUK during the next authentication process. You can find it in the PIN letter you received after applying for your ID card.</source>
        <extracomment>ALL_PLATFORMS Sentence 1 of 3 of PUK explanation</extracomment>
        <translation>PIN-код идентификационной карты введен неправильно трижды. Поэтому в следующем процессе аутентификации сначала необходимо ввести 10-значный PUK-код. Он указан в письме с PIN-кодом, которое вы получили после заказа идентификационной карты.</translation>
    </message>
    <message>
        <source>Have your PUK ready to unlock the ID card PIN.</source>
        <extracomment>ALL_PLATFORMS Sentence 3 of 3 of PUK explanation</extracomment>
        <translation>Подготовьте PUK-код для разблокировки PIN-кода идентификационной карты.</translation>
    </message>
    <message>
        <source>Try a different card position, make sure that you do not move the ID card during the check and that there are no other NFC cards (e.g. credit cards) near the device.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>попробуйте изменить положение карты; убедитесь в том, что идентификационная карта не смещается во время проверки и рядом с устройством нет других карт с NFC (например, банковских карт).</translation>
    </message>
    <message>
        <source>No supported ID card was detected. The %1 supports:&lt;p&gt;&lt;ul&gt;&lt;li&gt;German ID cards&lt;/li&gt;&lt;li&gt;Electronic residence permits (eAT)&lt;/li&gt;&lt;li&gt;eID cards for citizen of the EU/EEA/&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;If you have used one of the above documents and this error message still appears, please restart the check.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Поддерживаемые идентификационные карты не обнаружены. %1 поддерживает:&lt;p&gt;&lt;ul&gt;&lt;li&gt;идентификационные карты Германии;&lt;/li&gt;&lt;li&gt;электронные разрешения на временное пребывание (eAT);&lt;/li&gt;&lt;li&gt;карты онлайн-идентификации граждан ЕС/ЕЭП.&lt;/li&gt;&lt;/ul&gt;&lt;/p&gt;Если вы используете один из перечисленных документов, но данное сообщение об ошибке все равно появляется, перезапустите проверку.</translation>
    </message>
    <message>
        <source>You may continue the setup and change your PIN.</source>
        <extracomment>ALL_PLATFORMS Sentence 2 of 3 of CAN explanation
----------
ALL_PLATFORMS Sentence 2 of 3 of PUK explanation</extracomment>
        <translation>Вы можете продолжить настройку и изменить свой PIN-код.</translation>
    </message>
    <message>
        <source>I can&apos;t recall my PUK. How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>CheckIDCardView</name>
    <message>
        <source>Check device and ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Проверить устройство и идентификационную карту</translation>
    </message>
    <message>
        <source>To use the eID function, your device must meet certain technical requirements. Furthermore, the eID function must be activated.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Для использования функции онлайн-идентификации ваше устройство должно соответствовать определенным техническим требованиям. Также необходимо активировать функцию онлайн-идентификации.</translation>
    </message>
    <message>
        <source>Check if your device &amp; ID card are ready for use</source>
        <extracomment>MOBILE</extracomment>
        <translation>Убедитесь в том, что устройство и идентификационная карта готовы к использованию</translation>
    </message>
    <message>
        <source>Start check</source>
        <extracomment>MOBILE</extracomment>
        <translation>Запуск проверки</translation>
    </message>
</context>
<context>
    <name>CheckIDCardWorkflow</name>
    <message>
        <source>Check device and ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Проверить устройство и идентификационную карту</translation>
    </message>
    <message>
        <source>Checking ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Проверка идентификационной карты</translation>
    </message>
    <message>
        <source>Please do not move the ID card.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Не перемещайте идентификационную карту.</translation>
    </message>
</context>
<context>
    <name>ChooseReaderType</name>
    <message>
        <source>How would you like to read your ID card?</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Какой метод чтения идентификационной карты вы предпочитаете?</translation>
    </message>
    <message>
        <source>Connect an USB card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Подключите USB-устройство чтения карт</translation>
    </message>
    <message>
        <source>By card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Через устройство чтения карт</translation>
    </message>
    <message>
        <source>Use a NFC-enabled smartphone as card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Используйте смартфон в качестве устройства чтения карт</translation>
    </message>
    <message>
        <source>By smartphone</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Через смартфон</translation>
    </message>
</context>
<context>
    <name>CloseHandler</name>
    <message>
        <source>This will cancel the current operation and hide the UI of %1. You can restart the operation at any time.</source>
        <extracomment>DESKTOP Content of the popup that is shown when the AA is closed and a workflow is still active.</extracomment>
        <translation>Текущий процесс будет прерван, а пользовательский интерфейс %1 скрыт. Вы можете перезапустить процесс в любое время.</translation>
    </message>
    <message>
        <source>This will cancel the current operation and shut the %1 down. You will have to restart the %1 to restart the operation.</source>
        <extracomment>DESKTOP Content of the popup that is shown when the AA is shut down and a workflow is still active.</extracomment>
        <translation>Текущая операция будет отменена, и %1 будет остановлено. Чтобы снова начать операцию, вам нужно будет перезапустить %1.</translation>
    </message>
    <message>
        <source>Abort operation</source>
        <extracomment>DESKTOP Header of the popup that is shown when the AA is closed and a workflow is still active</extracomment>
        <translation>Отмена операции</translation>
    </message>
    <message>
        <source>How should the %1 be closed in the future?</source>
        <extracomment>DESKTOP Header of the popup that is shown when the AA is closed for the first time.</extracomment>
        <translation>Каким образом долно %1 закрываться в закрываться?</translation>
    </message>
    <message>
        <source>Close completely</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Завершить приложение полностью</translation>
    </message>
    <message>
        <source>Leave active in the background</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Оставить активным в фоновом режиме</translation>
    </message>
    <message>
        <source>If the %1 is closed, it is no longer available for authentication. You must then restart the app to authenticate yourself to service providers.</source>
        <extracomment>DESKTOP %1 is replaced with the application name</extracomment>
        <translation>Если приложение %1 завершено, оно больше не может быть использовано для аутентификации. Вам необходимо перезапустить приложение, чтобы выполнить самоаутентифицикацию для провайдеров службы.</translation>
    </message>
    <message>
        <source>Do not display this message in future.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Не показывать это сообщение в будущем.</translation>
    </message>
    <message>
        <source>If the %1 remains active in the background, it will open automatically as soon as you start an authentication. You can still open the %1 manually at any time.</source>
        <extracomment>DESKTOP %1 is replaced with the application name</extracomment>
        <translation>Если %1 останется активным в фоновом режиме, оно откроется автоматически, как только вы запустите аутентификацию. Вы также смоежете в любое время открыть %1 вручную.</translation>
    </message>
    <message>
        <source>Abort process</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Отменить процесс</translation>
    </message>
    <message>
        <source>You can change your selection at any time in the settings.</source>
        <extracomment>DESKTOP Note to the user that the setting is available in the settings</extracomment>
        <translation>Вы можете  в любое врем изменить в настройках ваш выбор.</translation>
    </message>
</context>
<context>
    <name>ConnectSacView</name>
    <message>
        <source>Pairing</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сопряжение</translation>
    </message>
    <message>
        <source>Pairing the device...</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сопряжение устройства...</translation>
    </message>
</context>
<context>
    <name>ContentArea</name>
    <message>
        <source>Change PIN</source>
        <extracomment>MOBILE</extracomment>
        <translation>Изменить PIN-код</translation>
    </message>
</context>
<context>
    <name>DarkModeButtonData</name>
    <message>
        <source>System</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Система</translation>
    </message>
    <message>
        <source>Dark</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Темная тема</translation>
    </message>
    <message>
        <source>Light</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Светлая тема</translation>
    </message>
    <message>
        <source>Set the app appearance to system mode</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Установить для приложения тему в зависимости от режима системы</translation>
    </message>
    <message>
        <source>Set the app appearance to dark mode</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Установить для приложения темную тему</translation>
    </message>
    <message>
        <source>Set the app appearance to light mode</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Установить для приложения светлую тему</translation>
    </message>
</context>
<context>
    <name>DataGroupDelegate</name>
    <message>
        <source>%1, optional right, element %2 of %3</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>%1, опциональное разрешение, элемент %2 из %3</translation>
    </message>
    <message>
        <source>%1, required right, element %2 of %3</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>%1, обязательное разрешение, элемент %2 из %3</translation>
    </message>
</context>
<context>
    <name>DebugSettings</name>
    <message>
        <source>Create dummy entries</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Создание фиктивных записей</translation>
    </message>
    <message>
        <source>Logfile</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Файл журнала</translation>
    </message>
    <message>
        <source>Show beta testing image</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Отображение символа бета-тестирования</translation>
    </message>
    <message>
        <source>Support CAN allowed mode</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Поддержка режима «Код CAN разрешен»</translation>
    </message>
    <message>
        <source>Skip rights page in CAN allowed mode</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Пропустить страницу с правами в режиме «Код CAN разрешен»</translation>
    </message>
    <message>
        <source>Reset hideable dialogs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сбросить скрываемые диалоговые окна</translation>
    </message>
    <message>
        <source>Show Transport PIN reminder, store feedback and close reminder dialogs.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Показывать напоминание временного PIN-кода, сохранять обратную связь и закрывать диалоговые окна с напоминаниями.</translation>
    </message>
</context>
<context>
    <name>DetachedLogView</name>
    <message>
        <source>Zoom:</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Изменение масштаба изображения:</translation>
    </message>
    <message>
        <source>Save log</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сохранить файл журнала</translation>
    </message>
    <message>
        <source>Logfiles (*.log)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Файлы журнала (*.log)</translation>
    </message>
    <message>
        <source>Filter</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Фильтр</translation>
    </message>
    <message>
        <source>Select level:</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Выберите уровень:</translation>
    </message>
    <message>
        <source>Select category:</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Выберите категорию:</translation>
    </message>
    <message>
        <source>Currently there are no log entries matching your filter.</source>
        <extracomment>DESKTOP No log entries, placeholder text.</extracomment>
        <translation>В настоящее время отсутствуют записи журнала, соответствующие вашему фильтру.</translation>
    </message>
    <message>
        <source>Filter. Activated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Фильтр. Активирован.</translation>
    </message>
    <message>
        <source>Filter. Deactivated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Фильтр. Деактивирован.</translation>
    </message>
    <message>
        <source>Current Log</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Текущий файл журнала</translation>
    </message>
</context>
<context>
    <name>DetachedLogViewWindow</name>
    <message>
        <source>Detached log viewer</source>
        <translation>Средство просмотра файлов журнала</translation>
    </message>
</context>
<context>
    <name>DeveloperModeWarning</name>
    <message>
        <source>Developer Mode: Enabled!</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Режим разработчика: активирован!</translation>
    </message>
    <message>
        <source>Disable</source>
        <extracomment>DESKTOP Global button to disable developer mode.</extracomment>
        <translation>Деактивировать</translation>
    </message>
</context>
<context>
    <name>DeveloperSettings</name>
    <message>
        <source>Developer options</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Параметры разработчика</translation>
    </message>
    <message>
        <source>Testmode for the self-authentication</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Тестовый режим для самоаутентификации</translation>
    </message>
    <message>
        <source>The internal card simulator allows to run an authentication in the test PKI without any ID card or card reader. Note that no other card reader can be used while the simulator is activated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Внутренний симулятор карт позволяет выполнять аутентификацию в тестовой ИОК без идентификационной карты или устройства чтения карт. Внимание: при активном симуляторе невозможно использовать другие устройства чтения карт.</translation>
    </message>
    <message>
        <source>Developer mode</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Режим разработчика</translation>
    </message>
    <message>
        <source>Custom config.json</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Заданный пользователем файл config.json</translation>
    </message>
    <message>
        <source>Place the config.json into the application folder to override the embedded config.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сохраните файл config.json в папке приложения, чтобы перезаписать встроенную конфигурацию.</translation>
    </message>
    <message>
        <source>Application folder: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Папка приложения: %1</translation>
    </message>
    <message>
        <source>Save config.json</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сохранить файл config.json</translation>
    </message>
    <message>
        <source>JSON config (*.json)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Файл конфигурации JSON (*.json)</translation>
    </message>
    <message>
        <source>Allow test sample card usage</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Разрешить использование тестовой карты</translation>
    </message>
    <message>
        <source>Internal card simulator</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Внутренний симулятор карт</translation>
    </message>
    <message>
        <source>The developer mode deactivates some security checks and the authentication process will continue even if some errors occur. Skipped errors will be shown as notifications. The developer mode is only usable with the test PKI.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Режим разработчика деактивирует некоторые проверки безопасности, и процесс аутентификации продолжается даже при возникновении ошибок. Пропущенные ошибки будут отображены как уведомления. Режим разработчика можно использовать только с тестовой ИОК.</translation>
    </message>
    <message>
        <source>Using the developer mode forces the notifications to be enabled.</source>
        <extracomment>DESKTOP Only visible when the user activates the developer mode in the settings.</extracomment>
        <translation>В режиме разработчика оповещения включаются принудительно.</translation>
    </message>
    <message>
        <source>Show notifications inside of %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Показывать уведомления внутри %1</translation>
    </message>
</context>
<context>
    <name>DevicesListDelegate</name>
    <message>
        <source>Device %1. %2.</source>
        <translation>Устройство %1. %2.</translation>
    </message>
</context>
<context>
    <name>DiagnosisView</name>
    <message>
        <source>Save to file</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сохранить как</translation>
    </message>
    <message>
        <source>Diagnosis is still running</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Диагностика еще выполняется</translation>
    </message>
    <message>
        <source>Diagnosis may be incomplete</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Диагностика может быть неполной</translation>
    </message>
    <message>
        <source>Textfiles (*.txt)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Текстовые файлы (*.txt)</translation>
    </message>
    <message>
        <source>System data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Системные данные</translation>
    </message>
    <message>
        <source>Save system data to textfile</source>
        <translation>Сохранить системные данные в текстовый файл</translation>
    </message>
    <message>
        <source>SystemData</source>
        <translation>Системные данные</translation>
    </message>
    <message>
        <source>Save system data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сохранить системные данные</translation>
    </message>
</context>
<context>
    <name>DominationHandler</name>
    <message>
        <source>Another application uses %1</source>
        <extracomment>DESKTOP The AA is currently remote controlled via the SDK interface, concurrent usage of the AA is not possible.</extracomment>
        <translation>Другое приложение использует %1</translation>
    </message>
</context>
<context>
    <name>EditRights</name>
    <message>
        <source>Proceed to %1 entry</source>
        <extracomment>DESKTOP %1 can be &quot;CAN&quot; or &quot;PIN&quot;
----------
MOBILE %1 can be &quot;CAN&quot; or &quot;PIN&quot;</extracomment>
        <translation>Далее к вводу %1</translation>
    </message>
    <message>
        <source>CAN</source>
        <extracomment>DESKTOP Inserted into &quot;Proceed to %1 entry&quot;
----------
IOS Inserted into &quot;Proceed to %1 entry&quot;</extracomment>
        <translation>CAN</translation>
    </message>
    <message>
        <source>PIN</source>
        <extracomment>DESKTOP Inserted into &quot;Proceed to %1 entry&quot;
----------
IOS Inserted into &quot;Proceed to %1 entry&quot;</extracomment>
        <translation>PIN</translation>
    </message>
    <message>
        <source>By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>При вводе CAN вы предоставляете указанному выше провайдеру доступ к следующим данным идентификационной карты:</translation>
    </message>
    <message>
        <source>By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>При вводе PIN-кода вы предоставляете указанному выше провайдеру доступ к следующим данным идентификационной карты:</translation>
    </message>
    <message>
        <source>Identify</source>
        <extracomment>MOBILE</extracomment>
        <translation>Идентифицировать</translation>
    </message>
</context>
<context>
    <name>EnterPasswordView</name>
    <message>
        <source>Enter CAN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Ввести CAN</translation>
    </message>
    <message>
        <source>Enter PUK</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Ввести PUK-код</translation>
    </message>
    <message>
        <source>Enter pairing code</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Ввести код сопряжения</translation>
    </message>
    <message>
        <source>Choose new ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Установить новый PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>Confirm new ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Подтвердить новый PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>Enter Transport PIN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Ввести временный PIN-код</translation>
    </message>
    <message>
        <source>Enter ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Ввести PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>You will find the Transport PIN in the letter that was sent to you after you applied for your ID card.</source>
        <extracomment>ALL_PLATFORMS The Transport PIN is required by AA, it needs to be change to an actual PIN. This is the description for the main text.</extracomment>
        <translation>Временный PIN-код указан в письме, отправленном вам после подачи заявки на получение идентифиакационой карты.</translation>
    </message>
    <message>
        <source>You have chosen the 6-digit ID card PIN yourself or received it via the PIN Reset Service.</source>
        <extracomment>ALL_PLATFORMS The AA expects the current ID card PIN with six digits in a PIN change. This is the description for the main text.</extracomment>
        <translation>Вы самостоятельно выбрали 6-значный PIN-код идентификационной карты или получили его от службы сброса PIN-кода.</translation>
    </message>
    <message>
        <source>Please enter your 6-digit ID card PIN.</source>
        <extracomment>ALL_PLATFORMS The AA expects a ID card PIN with six digits in an authentication. This is the description for the main text.</extracomment>
        <translation>Введите 6-значный PIN-код идентификационной карты.</translation>
    </message>
    <message>
        <source>You will enter this 6-digit PIN every time you want to use your ID card online.</source>
        <extracomment>ALL_PLATFORMS A new 6-digit ID card PIN needs to be supplied. This is the description for the main text.</extracomment>
        <translation>Вы будете вводить этот 6-значный PIN-код каждый раз, когда захотите использовать свою идентификационную карту в Интернете.</translation>
    </message>
    <message>
        <source>The PIN is going to be tied to your ID card. It always stays the same regardless of the device you want to use your ID card with.</source>
        <extracomment>ALL_PLATFORMS The new ID card PIN needs to be confirmed. This is the description for the main text.</extracomment>
        <translation>PIN-код будет привязан к вашей идентификационной карте. Он не меняется независимо от устройства, через которое вы хотите использовать свою идентификационную карту.</translation>
    </message>
    <message>
        <source>Do you have a 5-digit Transport PIN?</source>
        <extracomment>ALL_PLATFORMS Button to start a change of the Transport PIN.</extracomment>
        <translation>У вас есть 5-значный временный PIN-код?</translation>
    </message>
    <message>
        <source>Enter the pairing code shown on the device you want to pair.</source>
        <extracomment>MOBILE The pairing code for the smartphone is required. This is the description for the main text.</extracomment>
        <translation>Введите код сопряжения с устройства, которое вы хотите подключить.</translation>
    </message>
    <message>
        <source>Do you have a 6-digit ID card PIN?</source>
        <extracomment>ALL_PLATFORMS Button to switch to a 6-digit ID card PIN.</extracomment>
        <translation>У вас есть 6-значный PIN-код идентификационной карты?</translation>
    </message>
    <message>
        <source>Send CAN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Отправить CAN-код</translation>
    </message>
    <message>
        <source>Send PUK</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Отправить PUK-код</translation>
    </message>
    <message>
        <source>Send pairing code</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Отправить код сопряжения</translation>
    </message>
    <message>
        <source>Send new ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Отправить новый PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>Send Transport PIN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Отправить временный PIN-код</translation>
    </message>
    <message>
        <source>Send ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.</extracomment>
        <translation>Отправить PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>Enter the pairing code shown on your smartphone.</source>
        <extracomment>DESKTOP The pairing code needs to be supplied. This is the description for the main text.</extracomment>
        <translation>Введите указанный в смартфоне код сопряжения.</translation>
    </message>
    <message>
        <source>Please enter the CAN. You can find the CAN in the bottom right on the front of the ID card.</source>
        <extracomment>ALL_PLATFORMS The user is required to enter the 6-digit CAN. This is the description for the main text.</extracomment>
        <translation>Введите CAN. Вы найдете CAN в нижнем правом углу лицевой стороны идентификационной карты.</translation>
    </message>
    <message>
        <source>Please enter your 10-digit PUK.</source>
        <extracomment>ALL_PLATFORMS The PUK is required to unlock the ID card since the wrong ID card PIN entered three times. This is the description for the main text.</extracomment>
        <translation>Введите 10-значный PUK-код.</translation>
    </message>
    <message>
        <source>Confirm ID card PIN</source>
        <extracomment>ALL_PLATFORMS This is the large main text below the icon.</extracomment>
        <translation>Подтвердить PIN-код идентификационной карты</translation>
    </message>
</context>
<context>
    <name>GCollapsible</name>
    <message>
        <source>Currently selected is %1</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>В настоящее время выбрано %1</translation>
    </message>
</context>
<context>
    <name>GContinueButton</name>
    <message>
        <source>Continue</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Продолжить</translation>
    </message>
</context>
<context>
    <name>GProgressBar</name>
    <message>
        <source>Progress</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Процесс</translation>
    </message>
</context>
<context>
    <name>GRadioButton</name>
    <message>
        <source>checked</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>GStagedProgressBar</name>
    <message>
        <source>Step %1 of %2. This step is %3 percent complete.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Шаг %1 из %2 выполнен на %3 процента/процентов.</translation>
    </message>
</context>
<context>
    <name>GTextField</name>
    <message>
        <source>Maximum allowed length reached.</source>
        <translation>Достигнута максимально допустимая длина.</translation>
    </message>
</context>
<context>
    <name>GeneralSettings</name>
    <message>
        <source>Behavior</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Поведение</translation>
    </message>
    <message>
        <source>Network</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сеть</translation>
    </message>
    <message>
        <source>Use the proxy (%1) specified during the installation.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Используйте указанный при установке прокси-сервер (%1).</translation>
    </message>
    <message>
        <source>Appearance</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Внешний вид</translation>
    </message>
    <message>
        <source>Use system font</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Использовать системный шрифт</translation>
    </message>
    <message>
        <source>Toggling will restart the %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Переключение приведет к перезапуску %1</translation>
    </message>
    <message>
        <source>Close %1 window after authentication</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Закрыть окно %1 после аутентификации</translation>
    </message>
    <message>
        <source>Change language</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Изменить язык</translation>
    </message>
    <message>
        <source>Use images instead of animations</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Использовать изображения вместо анимаций</translation>
    </message>
    <message>
        <source>Accessibility</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Доступность</translation>
    </message>
    <message>
        <source>Hide key animations when entering PIN</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Скрыть анимацию клавиатуры при вводе PIN-кода</translation>
    </message>
    <message>
        <source>After identification, you will only be redirected back to the provider after confirmation. Otherwise, you will be redirected automatically after a few seconds.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>После идентификации вы будете перенаправлены обратно к провайдеру только после подтверждения. В остальных случаях вы будете перенаправлены автоматически через несколько секунд.</translation>
    </message>
    <message>
        <source>Manual redirection back to the provider</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Перенаправление вручную обратно к провайдеру</translation>
    </message>
    <message>
        <source>The %1 gets started on system boot, so that it can be opened automatically on an authentication. It has to be started manually otherwise.</source>
        <extracomment>DESKTOP Description for auto-start option</extracomment>
        <translation>Запуск %1 происходит при загрузке системы, чтобы его можно было открыть автоматически после аутентификации. В противном случае его нужно будет запускать вручную.</translation>
    </message>
    <message>
        <source>Automatically start %1 (recommended)</source>
        <extracomment>DESKTOP Text for auto-start option</extracomment>
        <translation>Автоматический запуск %1 (рекомендуется)</translation>
    </message>
    <message>
        <source>The %1 continues to run in the background after the application window is closed, so that it can be opened automatically on an authentication.</source>
        <extracomment>MACOS Description for attaching the AA to the menu bar/system tray</extracomment>
        <translation>%1 продолжает работать в фоновом режиме после закрытия окна приложения, чтобы его можно было автоматически открыть после аутентификации.</translation>
    </message>
    <message>
        <source>Attach %1 to menu bar (recommended)</source>
        <extracomment>MACOS Text for attaching the AA to the menu bar</extracomment>
        <translation>Прикрепить %1 к строке меню (рекомендуется)</translation>
    </message>
    <message>
        <source>Attach %1 to system tray (recommended)</source>
        <extracomment>WINDOWS Text for attaching the AA to the system tray</extracomment>
        <translation>Прикрепить %1 к панели задач (рекомендуется)</translation>
    </message>
</context>
<context>
    <name>GeneralWorkflow</name>
    <message>
        <source>Step %1 of 3</source>
        <translation>Шаг %1 из 3</translation>
    </message>
    <message>
        <source>The used card reader does not meet the technical requirements (Extended Length not supported).</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Используемое устройство чтения карт не соответствует техническим требованиям (не поддерживается расширенная длина).</translation>
    </message>
    <message>
        <source>Read out ID card with connected device</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Считать идентификационную карту подключенным устройством</translation>
    </message>
    <message>
        <source>Connect USB card reader or smartphone</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Подключите USB-устройство чтения карт или смартфон</translation>
    </message>
    <message>
        <source>Information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация</translation>
    </message>
    <message>
        <source>No ID card detected. Please ensure that your ID card is placed on the card reader.</source>
        <extracomment>DESKTOP The AA is waiting for an ID card to be inserted into the card reader.</extracomment>
        <translation>Идентификационная карта не обнаружена. Убедитесь в том, что идентификационная карта размещена на устройстве чтения карт.</translation>
    </message>
    <message>
        <source>No card reader detected. Connect an USB card reader or set up a smartphone as a card reader.</source>
        <extracomment>DESKTOP AA is waiting for the card reader or the ID card.</extracomment>
        <translation>Устройство чтения карт не обнаружено. Подключите USB-устройство чтения карт или используйте смартфон в качестве устройства чтения карт.</translation>
    </message>
    <message>
        <source>Please observe the display of your card reader.</source>
        <extracomment>DESKTOP The card reader is a comfort reader with its own display, the user is requested to pay attention to that display (instead of the AA).</extracomment>
        <translation>Учитывайте указания на дисплее устройства чтения карт.</translation>
    </message>
    <message>
        <source>Set up card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройка устройства чтения карт</translation>
    </message>
    <message>
        <source>No ID card detected. Please follow the instructions on your smartphone (connected to %1) to use it as card reader.</source>
        <extracomment>DESKTOP The AA is waiting for the smartphone to be placed on the id.</extracomment>
        <translation>Идентификационная карта не обнаружена. Следуйте инструкциям в смартфоне (подключен к %1), чтобы использовать его в качестве устройства чтения карт.</translation>
    </message>
    <message>
        <source>Please follow the instructions on your smartphone (connected to %1) or put the ID card on the card reader.</source>
        <extracomment>DESKTOP The AA is waiting for an ID card to be inserted into the card reader (or smartphone for that matter).</extracomment>
        <translation>Следуйте инструкциям в смартфоне (подключен к %1) или вставьте идентификационную карту в устройство чтения карт.</translation>
    </message>
    <message>
        <source>The device &quot;%1&quot; was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.</source>
        <extracomment>DESKTOP The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
----------
MOBILE The paired smartphone was removed since it did not respond to connection attempts. It needs to be paired again before using it.</extracomment>
        <translation>Сопряжение устройства &quot;%1&quot; не было выполнено, поскольку оно не отвечало на попытки соединения. Снова выполните сопряжение устройства для его использования в качестве устройства чтения карт.</translation>
    </message>
</context>
<context>
    <name>InputErrorView</name>
    <message>
        <source>Wrong CAN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неправильный CAN-код</translation>
    </message>
    <message>
        <source>Wrong PUK</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неправильный PUK-код</translation>
    </message>
    <message>
        <source>Wrong Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неправильный временный PIN-код</translation>
    </message>
    <message>
        <source>Wrong ID card PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неправильный PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>Wrong new ID card PIN confirmation</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неправильное подтверждение нового PIN-кода идентификационной карты</translation>
    </message>
</context>
<context>
    <name>InputSuccessView</name>
    <message>
        <source>PUK is correct</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Правильный PUK-код</translation>
    </message>
    <message>
        <source>You now have &lt;b&gt;3 more attempts&lt;/b&gt; to enter your correct &lt;b&gt;ID card PIN&lt;/b&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>У вас есть &lt;b&gt;ещё три попытки&lt;/b&gt;, чтобы ввести правильный &lt;b&gt;PIN-код идентификационной карты&lt;/b&gt;.</translation>
    </message>
    <message>
        <source>CAN is correct</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Правильный CAN-код</translation>
    </message>
    <message>
        <source>Please enter your &lt;b&gt;6-digit ID card PIN&lt;/b&gt;. &lt;b&gt;This is your last attempt&lt;/b&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Введите &lt;b&gt;6-значный PIN-код идентификационной карты&lt;/b&gt;. &lt;b&gt; Это ваша последняя попытка&lt;/b&gt;.</translation>
    </message>
    <message>
        <source>If you enter a wrong ID Card PIN again, the PIN will be blocked. This block may only be removed using the PUK.</source>
        <translation>Если вы ещё раз введете неправильный PIN-код идентификационной карты, PIN-код будет заблокирован. Разблокировать его можно будет только через PUK-код.</translation>
    </message>
    <message>
        <source>Please enter your &lt;b&gt;5-digit Transport PIN&lt;/b&gt;. &lt;b&gt;This is your last attempt&lt;/b&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Введите &lt;b&gt;5-значный временный PIN-код&lt;/b&gt;.&lt;b&gt; Это ваша последняя попытка&lt;/b&gt;.</translation>
    </message>
    <message>
        <source>If you enter a wrong Transport PIN again, the PIN will be blocked. This block may only be removed using the PUK.</source>
        <translation>Если вы ещё раз введете неправильный временный PIN-код, PIN-код будет заблокирован. Разблокировать его можно будет только через PUK-код.</translation>
    </message>
    <message>
        <source>You now have &lt;b&gt;3 more attempts&lt;/b&gt; to enter your correct &lt;b&gt;Transport PIN&lt;/b&gt;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>У вас есть &lt;b&gt;ещё три попытки&lt;/b&gt;, чтобы ввести правильный &lt;b&gt;временный PIN-код&lt;/b&gt;.</translation>
    </message>
    <message>
        <source>Transport PIN correct</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Правильный временный PIN-код</translation>
    </message>
    <message>
        <source>Now set your personal ID card PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Теперь установите ваш личный PIN-код идентификационной карты</translation>
    </message>
    <message>
        <source>The Transport PIN is now replaced by your personal 6-digit ID card PIN.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Теперь временный PIN-код заменен на ваш личный 6-значный PIN-код идентификационной карты.</translation>
    </message>
    <message>
        <source>Afterwards the %1Transport PIN is no longer required.%2</source>
        <extracomment>ALL_PLATFORMS The text between %1 and %2 will be emphasized.</extracomment>
        <translation>В дальнейшем %1временный PIN-код больше не потребуется.%2</translation>
    </message>
</context>
<context>
    <name>LanguageButtonData</name>
    <message>
        <source>German</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Немецкий</translation>
    </message>
    <message>
        <source>Set language to german</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Использовать немецкий язык</translation>
    </message>
    <message>
        <source>English</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Английский</translation>
    </message>
    <message>
        <source>Set language to english</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Использовать английский язык</translation>
    </message>
    <message>
        <source>Ukrainian</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Украинский</translation>
    </message>
    <message>
        <source>Set language to ukrainian</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Использовать украинский язык</translation>
    </message>
    <message>
        <source>Russian</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Русский</translation>
    </message>
    <message>
        <source>Set language to russian</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Использовать русский язык</translation>
    </message>
</context>
<context>
    <name>LicenseInformation</name>
    <message>
        <source>Software license</source>
        <extracomment>MOBILE</extracomment>
        <translation>Лицензия на программное обеспечение</translation>
    </message>
</context>
<context>
    <name>LinkQualityAnimation</name>
    <message>
        <source>Link quality unavailable.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Низкое качество соединения.</translation>
    </message>
    <message>
        <source>%1% link quality.</source>
        <extracomment>ALL_PLATFORMS %1 is replaced with a number between 0 and 100</extracomment>
        <translation>Качество соединения %1.</translation>
    </message>
</context>
<context>
    <name>LocalNetworkInfo</name>
    <message>
        <source>Go to application settings</source>
        <extracomment>IOS Link to application settings</extracomment>
        <translation>Перейти к настройкам приложения</translation>
    </message>
    <message>
        <source>Ensure that access to the local network is allowed in your settings.</source>
        <extracomment>IOS Let user know to check the application settings for local network permission</extracomment>
        <translation>Убедитесь в наличии доступа к локальной сети.</translation>
    </message>
</context>
<context>
    <name>LogFilesView</name>
    <message>
        <source>Select Log</source>
        <extracomment>MOBILE</extracomment>
        <translation>Выбрать файл журнала</translation>
    </message>
    <message>
        <source>Delete</source>
        <extracomment>MOBILE</extracomment>
        <translation>Удалить</translation>
    </message>
    <message>
        <source>All old logs will be deleted.</source>
        <extracomment>MOBILE All logfiles are about to be removed, user confirmation required.</extracomment>
        <translation>Все старые файлы журнала будут удалены.</translation>
    </message>
    <message>
        <source>Delete all logs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Удалить все файлы журнала</translation>
    </message>
</context>
<context>
    <name>LogTitleBarControls</name>
    <message>
        <source>Share log</source>
        <extracomment>MOBILE</extracomment>
        <translation>Поделиться файлом журнала</translation>
    </message>
    <message>
        <source>Delete all logs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Удалить все файлы журнала</translation>
    </message>
    <message>
        <source>Filter</source>
        <extracomment>MOBILE</extracomment>
        <translation>Фильтр</translation>
    </message>
</context>
<context>
    <name>LogView</name>
    <message>
        <source>Logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Файлы журнала</translation>
    </message>
    <message>
        <source>Save log</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сохранить файл журнала</translation>
    </message>
    <message>
        <source>Logfiles (*.log)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Файлы журнала (*.log)</translation>
    </message>
    <message>
        <source>The current log will be automatically deleted at exit.</source>
        <translation>Текущий файл журнала будет автоматически удален при выходе.</translation>
    </message>
    <message>
        <source>Detach log viewer</source>
        <translation>Показать файл журнала отдельно</translation>
    </message>
    <message>
        <source>All old logs will be deleted.</source>
        <extracomment>DESKTOP All logfiles are about to be removed, user confirmation required.</extracomment>
        <translation>Все старые файлы журнала будут удалены.</translation>
    </message>
    <message>
        <source>Delete all logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Удалить все файлы журнала</translation>
    </message>
    <message>
        <source>Delete</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Удалить</translation>
    </message>
    <message>
        <source>Filter</source>
        <extracomment>MOBILE</extracomment>
        <translation>Фильтр</translation>
    </message>
    <message>
        <source>Level</source>
        <extracomment>MOBILE</extracomment>
        <translation>Уровень</translation>
    </message>
    <message>
        <source>Category</source>
        <extracomment>MOBILE</extracomment>
        <translation>Категория</translation>
    </message>
    <message>
        <source>Currently there are no log entries matching your filter.</source>
        <extracomment>MOBILE No log entries, placeholder text.</extracomment>
        <translation>В настоящее время отсутствуют записи журнала, соответствующие вашему фильтру.</translation>
    </message>
</context>
<context>
    <name>LogViewDelegate</name>
    <message>
        <source>The log entry was copied to the clipboard.</source>
        <extracomment>DESKTOP Toast message used to confirm the copy of a log entry.
----------
MOBILE Toast message used to confirm the copy of a log entry.</extracomment>
        <translation>Запись журнала скопирована в буфер обмена.</translation>
    </message>
</context>
<context>
    <name>MainView</name>
    <message>
        <source>See my&lt;br&gt;personal data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Просмотреть&lt;br&gt;персональные данные</translation>
    </message>
    <message>
        <source>Settings</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройки</translation>
    </message>
    <message>
        <source>Help</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Справка</translation>
    </message>
    <message>
        <source>Check device and ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Проверить устройство и идентификационную карту</translation>
    </message>
    <message>
        <source>See my personal data</source>
        <extracomment>MOBILE</extracomment>
        <translation>Просмотреть персональные данные</translation>
    </message>
    <message>
        <source>Two finger swipe to scroll.</source>
        <translation>Пролистывание двумя пальцами.</translation>
    </message>
    <message>
        <source>Item %1 of %2</source>
        <translation>Элемент %1 из %2</translation>
    </message>
    <message>
        <source>Change PIN</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Изменить PIN-код</translation>
    </message>
    <message>
        <source>Three finger swipe to scroll.</source>
        <translation>Пролистывание тремя пальцами.</translation>
    </message>
</context>
<context>
    <name>MoreInformationLink</name>
    <message>
        <source>More information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Дополнительные сведения</translation>
    </message>
</context>
<context>
    <name>MoreView</name>
    <message>
        <source>Help</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Справка</translation>
    </message>
    <message>
        <source>General</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Общая информация</translation>
    </message>
    <message>
        <source>%1 version</source>
        <extracomment>MOBILE %1 is replaced with the application name</extracomment>
        <translation type="unfinished">Информация о версии</translation>
    </message>
    <message>
        <source>Software license</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Лицензия на программное обеспечение</translation>
    </message>
    <message>
        <source>Release notes</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Примечания к версии</translation>
    </message>
    <message>
        <source>Privacy statement</source>
        <extracomment>MOBILE</extracomment>
        <translation>Положение о конфиденциальности</translation>
    </message>
    <message>
        <source>Accessibility statement</source>
        <extracomment>MOBILE</extracomment>
        <translation>Заявление о доступности</translation>
    </message>
    <message>
        <source>Rate %1</source>
        <extracomment>MOBILE</extracomment>
        <translation>Оценить %1</translation>
    </message>
    <message>
        <source>Logs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Файлы журнала</translation>
    </message>
    <message>
        <source>Information</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Информация</translation>
    </message>
    <message>
        <source>List of Providers</source>
        <extracomment>MOBILE</extracomment>
        <translation>Список провайдеров</translation>
    </message>
    <message>
        <source>Data and logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Данные и файлы журнала</translation>
    </message>
    <message>
        <source>FAQ - Frequently asked questions</source>
        <extracomment>MOBILE</extracomment>
        <translation>Часто задаваемые вопросы</translation>
    </message>
    <message>
        <source>Contact</source>
        <extracomment>MOBILE</extracomment>
        <translation>Контакты</translation>
    </message>
    <message>
        <source>Show Logs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Показать файлы журнала</translation>
    </message>
    <message>
        <source>Send log to the support</source>
        <extracomment>MOBILE</extracomment>
        <translation>Отправить файл журнала в службу поддержки</translation>
    </message>
    <message>
        <source>Terms of use and software license</source>
        <extracomment>MOBILE</extracomment>
        <translation>Условия использования и лицензия на программное обеспечение</translation>
    </message>
    <message>
        <source>Start setup</source>
        <extracomment>MOBILE</extracomment>
        <translation>Начать настройку</translation>
    </message>
</context>
<context>
    <name>MoreViewDiagnosis</name>
    <message>
        <source>Logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Файлы журнала</translation>
    </message>
    <message>
        <source>Show logs</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Показать файлы журнала</translation>
    </message>
    <message>
        <source>Show system data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Показать системные данные</translation>
    </message>
    <message>
        <source>System data</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Системные данные</translation>
    </message>
</context>
<context>
    <name>MoreViewGeneral</name>
    <message>
        <source>Open website</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Открыть сайт</translation>
    </message>
    <message>
        <source>List of Providers</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Список провайдеров</translation>
    </message>
    <message>
        <source>FAQ - Frequently asked questions</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Часто задаваемые вопросы</translation>
    </message>
    <message>
        <source>Contact</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Контакты</translation>
    </message>
    <message>
        <source>Start setup</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Начать настройку</translation>
    </message>
    <message>
        <source>Setup</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройка</translation>
    </message>
</context>
<context>
    <name>MultiInfoContent</name>
    <message>
        <source>More information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Дополнительные сведения</translation>
    </message>
</context>
<context>
    <name>MultiInfoData</name>
    <message>
        <source>What is the card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Что такое PIN-код карты?</translation>
    </message>
    <message>
        <source>PIN information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Информация о PIN-коде</translation>
    </message>
    <message>
        <source>Where can I find the card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Где найти PIN-код карты?</translation>
    </message>
    <message>
        <source>How do I choose a secure PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Как выбрать безопасный PIN-код?</translation>
    </message>
    <message>
        <source>For your 6-digit PIN, choose a combination of numbers that cannot be guessed - i.e. neither &quot;123456&quot;, nor your date of birth, nor any other numbers printed on your ID card.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;How do I choose a secure PIN?&apos; paragraph 1/3</extracomment>
        <translation>Не выбирайте для 6-значного PIN-кода комбинации, которые легко отгадать (например, 123456, дату своего рождения или любые другие цифры с идентификационной карты).</translation>
    </message>
    <message>
        <source>You can change your 6-digit PIN at any time and an unlimited number of times as long as you know your valid PIN.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;How do I choose a secure PIN?&apos; paragraph 2/3</extracomment>
        <translation>Изменить 6-значный PIN-код можно в любое время и неограниченное количество раз, если вы знаете свой действительный PIN-код.</translation>
    </message>
    <message>
        <source>Keep your PIN secret and change it if another person becomes aware of it.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;How do I choose a secure PIN?&apos; paragraph 3/3</extracomment>
        <translation>Держите PIN-код в секрете и измените его, если о нем станет известно другому человеку.</translation>
    </message>
    <message>
        <source>What is the Transport PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Что такое временный PIN-код?</translation>
    </message>
    <message>
        <source>The Transport PIN is a %1one-time PIN%2 which is required for the initial %1setup of your ID card%2. Once you replace the %1Transport PIN with a card PIN%2, it is tied to your ID card and valid for all devices.</source>
        <extracomment>ALL_PLATFORMS ALL_PLATFORMS Answer to the question &apos;What is the Transport PIN?&apos;</extracomment>
        <translation>Временный PIN-код - это %1одноразовый PIN-код%2, который необходим для первоначальной %1настройки вашей идентификационной карты%2. Как только вы замените %1временный PIN-код на PIN-код карты%2, он будет привязан к вашей идентификационной карте и действителен для всех устройств.</translation>
    </message>
    <message>
        <source>Where do I find the Transport PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Где можно найти временный PIN-код?</translation>
    </message>
    <message>
        <source>The 5-digit Transport PIN is a %1one-time PIN%2, that you received per %1letter%2 when applying for the ID card.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where do I find the Transport PIN?&apos; paragraph 1/2</extracomment>
        <translation>5-значный временный PIN-код - это %1одноразовый PIN-код%2, указанный %1в письме%2, полученным вами при подачи заявки на получение идентифиакационой карты.</translation>
    </message>
    <message>
        <source>When you set up your ID card, you will %1replace%2 this 5-digit %1Transport PIN with%2 a 6-digit, %1self chosen card PIN%2.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where do I find the Transport PIN?&apos; paragraph 2/2
----------
ALL_PLATFORMS Description text explaining the PINs 3/7</extracomment>
        <translation>При настройке идентификационной карты вы %1замените%2 этот 5-значный %1временный PIN-код на%2 6-значный, %1 самостоятельно выбранный вами PIN-код карты%2.</translation>
    </message>
    <message>
        <source>Where do I find the PUK?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Где найти PUK-код?</translation>
    </message>
    <message>
        <source>The PUK is a%1 10-digit number%2 that you can find in the %1PIN letter%2 that was sent to you by mail after you %1applied for your ID card%2. You may find it to the %1right%2 of the 5-digit %1Transport PIN%2.</source>
        <extracomment>ALL_PLATFORMS ALL_PLATFORMS Answer to the question &apos;Where do I find the PUK?&apos;</extracomment>
        <translation>PUK-код - это%1 10-значное число%2, которое вы найдете в %1письме с PIN-кодом%2, отправленное вам по почте после подачи вами %1заявки на получение идентификационной карты%2. Он указан %1справа%2 от 5-значного %1временного PIN-кода%2.</translation>
    </message>
    <message>
        <source>Why is the PUK required?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Для чего необходим PUK-код?</translation>
    </message>
    <message>
        <source>The PUK is required if the %1card PIN has been entered incorrectly 3 times%2 in a row. As a result, the card PIN is blocked. By entering the PUK you will %1unblock the card PIN%2 and have%1 3 more attempts%2 to enter the correct PIN.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Why is the PUK required?&apos;</extracomment>
        <translation>PUK-код понадобится, если %1PIN-код карты будет введен неправильно 3 раза%2 подряд. Это приводит к блокировке PIN-кода карты. Введя PUK-код, вы %1разблокируете PIN-код карты %2 и получите %1еще 3 попытки%2 для ввода правильного PIN-кода.</translation>
    </message>
    <message>
        <source>My PUK does not work</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Мой PUK-код не работает</translation>
    </message>
    <message>
        <source>Make sure, that the letter at hand is the %1letter for your current ID card%2. The PIN letter is valid only for your current ID card. With a new ID card, you receive a new PIN letter with a new PUK.</source>
        <extracomment>ALL_PLATFORMS Text block regarding &quot;My PUK does not work&quot;</extracomment>
        <translation>Убедитесь, что используемое вами письмо - это %1 письмо для вашей действующей идентификационной-карты%2. PIN-код действителен только для вашей действующей идентификационной карты. При получении новой идентификационной карты вы получаете новое письмо с новым PIN-кодом и PUK-кодом.</translation>
    </message>
    <message>
        <source>If you applied for a PIN Reset Letter, make sure that you don&apos;t mix up the letters. The %1PIN letter you need contains the 5-digit Transport PIN and the PUK%2. The PIN Reset Letter contains the 6-digit card PIN and the activation code.</source>
        <extracomment>ALL_PLATFORMS Text block regarding &quot;My PUK does not work&quot;</extracomment>
        <translation>Если вы запросили письмо для сброса PIN-кода, убедитесь, что вы не перепутали письма. %1В письме с PIN-кодом, которое вам нужно, указан 5-значный временный PIN-код и PUK-код%2. Письмо для сброса PIN-кода содержит 6-значный PIN-код карты и код активации.</translation>
    </message>
    <message>
        <source>Why is the CAN required?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Для чего необходим CAN-код?</translation>
    </message>
    <message>
        <source>The card access number (CAN) is required when the %1card PIN has been entered incorrectly 2 times%2.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;When is the card access number (CAN) required?&apos;</extracomment>
        <translation>Номер доступа (CAN) понадобится, если %1PIN-код карты был дважды введен неправильно%2.</translation>
    </message>
    <message>
        <source>Why do I have to enter the CAN before a 3rd attempt?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Почему нужно вводить CAN-код перед третьей попыткой?</translation>
    </message>
    <message>
        <source>A 3rd incorrect entry blocks your PIN and you can no longer use the eID function until it is unblocked. Requesting the CAN ensures that %1direct access to your ID card%2 is given.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Why do I have to enter the CAN before a 3rd attempt?&apos;</extracomment>
        <translation>После третбего неправильного ввода ваш PIN-код будет заблокирован, и вы не сможете использовать функцию ондайн-идентификации до тех пор, пока он не будет разблокирован. Запрос CAN позволяет убедиться, что %1прямой доступ к идентификационной карте%2 обеспечен.</translation>
    </message>
    <message>
        <source>Where can I find the CAN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Где найти CAN-код?</translation>
    </message>
    <message>
        <source>The CAN is a 6-digit number that can be found on the %1bottom right%2 of the %1front of the ID card%2.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the CAN?&apos;</extracomment>
        <translation>Код CAN - это 6-значный номер, который вы найдете %1в нижнем правом%2 углу %1лицевой стороны идентификационной карты%2.</translation>
    </message>
    <message>
        <source>The Card Access Number (CAN) allows to access the imprinted data of the ID card. The CAN is a 6-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in picture).</source>
        <extracomment>ALL_PLATFORMS Description text of CAN-allowed authentication</extracomment>
        <translation>Код доступа (CAN) предоставляет доступ к данным, напечатанным на идентификационной карте. Код CAN - это 6-значный номер, указанный на лицевой стороне идентификационной карты. Он расположен внизу справа рядом со сроком действия (выделен на изображении).</translation>
    </message>
    <message>
        <source>No PIN known</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PIN-код неизвестен</translation>
    </message>
    <message>
        <source>You do not know your PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Знаете ли вы свой PIN-код?</translation>
    </message>
    <message>
        <source>You have not yet set a 6-digit card PIN and cannot find the PIN letter with the Transport PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вы еще не установили 6-значный PIN-код карты и не можете найти письмо с PIN-кодом, где указан временный PIN-код?</translation>
    </message>
    <message>
        <source>You set a card PIN when picking up your ID card or later by yourself, but you can no longer remember it?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вы установили PIN-код карты при получении идентификационной карты или позже самостоятельно, но не можете его вспомнить?</translation>
    </message>
    <message>
        <source>How do the PIN types differ?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Чем отличаются типы PIN-кодов?</translation>
    </message>
    <message>
        <source>Your ID card comes with a 5-digit %1Transport PIN%2 which you need to %1replace with%2 a 6-digit %1card PIN%2 that you choose yourself.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs 1/7</extracomment>
        <translation>К вашей идентификационной карте прилагается 5-значный %1временный PIN-код%2, который нужно %1заменить на %2 6-значный %1PIN-код карты%2, который вы выбираете самостоятельно.</translation>
    </message>
    <message>
        <source>5-digit Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>5-значный временный PIN-код</translation>
    </message>
    <message>
        <source>6-digit PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>6-значный PIN-код</translation>
    </message>
    <message>
        <source>The 5-digit Transport PIN is a %1one-time PIN%2 that was sent to you by %1mail%2 after you applied for your ID card.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs 2/7</extracomment>
        <translation>5-значный временный PIN-код - это%1 одноразовый PIN-код%2, отправленный вам %1по почте%2 после вашей заявки на получение идентификационой карты.</translation>
    </message>
    <message>
        <source>The card PIN is a 6-digit PIN that you set yourself. You always need this %1PIN%2 if you want to use the %1eID function%2.</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;what is the card pin?&apos;</extracomment>
        <translation>PIN-код карты - это 6-значный PIN-код, который вы устанавливаете самостоятельно. Этот %1PIN-код%2 понадобится вам каждый раз, когда вы захотите выполнить %1онлайн-идентификацию%2.</translation>
    </message>
    <message>
        <source>The card PIN was %1either%2...</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the card PIN?&apos; - Intro</extracomment>
        <translation>PIN-код карты был %1либо%2...</translation>
    </message>
    <message>
        <source>set by you when picking up the ID card %1at the citiziens&apos; office%2</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the card PIN?&apos; - Parapgraph 1/3</extracomment>
        <translation>установлен вами при получении идентификационной карты %1 в бюро обслуживания граждан,%2</translation>
    </message>
    <message>
        <source>set %1in the %3 using%2 the 5-digit %1Transport PIN%2</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the card PIN?&apos; - Parapgraph 2/3</extracomment>
        <translation>установлен %1 в %3 с помощью %2 5-значного %1временного PIN-кода,%2</translation>
    </message>
    <message>
        <source>requested using the %1PIN Reset Service%2 - you may continue using the 6-digit card PIN from the letter or change it via &quot;Change PIN&quot; to a self chosen card PIN</source>
        <extracomment>ALL_PLATFORMS Answer to the question &apos;Where can I find the card PIN?&apos; - Parapgraph 3/3</extracomment>
        <translation>запрошен через %1услугу сброса PIN-кода%2 - вы можете продолжать использовать 6-значный PIN-код карты из письма или изменить его через пункт меню «изменить PIN-код» на выбранный вами PIN-код карты</translation>
    </message>
    <message>
        <source>I just have a 5-digit Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>У меня есть только 5-значный временный PIN-код</translation>
    </message>
    <message>
        <source>You need to change the%1 5-digit Transport PIN%2 to your personal card PIN. Use %1Change PIN%2 from the startpage to do so.</source>
        <extracomment>ALL_PLATFORMS Explanation if only the Transport PIN is at hand. The text between %1 and %2 will be emphasized.</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>With this 6-digit PIN you prove online that the ID card belongs to you. %1No one can use the eID function without this PIN%2.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs 6/7</extracomment>
        <translation>Вводя 6-значный PIN-код, вы подтверждаете в Интернете то, что идентификационная карта принадлежит вам. %1Никто не сможет использовать функцию онлайн-идентификации без этого PIN-кода%2.</translation>
    </message>
    <message>
        <source>If you applied for a %1PIN Reset Letter%2, a new card PIN was already set for you. You can change your card PIN at %1any time in %3%2.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs (%1 is replaced with the application name) 7/7</extracomment>
        <translation>Если вы запросили %1письмо для сброса PIN-кода%2 , то для вас уже был установлен новый PIN-код карты. Вы можете изменить PIN-код своей карты в %1любое время в %3%2.</translation>
    </message>
    <message>
        <source>My Transport PIN does not work</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Мой временный PIN-код не работает</translation>
    </message>
    <message>
        <source>If your Transport PIN from the PIN letter does work, this might be caused by the following reasons:</source>
        <extracomment>ALL_PLATFORMS ALL_PLATFORMS Introduction why the Transport PIN might not work.</extracomment>
        <translation>Если ваш временный PIN-код, указанный к письме с PIN-кодом, это может иметь слежующие причины:</translation>
    </message>
    <message>
        <source>You already set a 6-digit PIN either at the competent authority or using the %1.</source>
        <extracomment>ALL_PLATFORMS Information why the Transport PIN is void: a 6-digit PIN was already set</extracomment>
        <translation>Вы уже создали 6-значный PIN-код в компетентном органе или через %1.</translation>
    </message>
    <message>
        <source>You applied for PIN Reset Letter (letter containing a 6-digit PIN and corresponding Activation Code).</source>
        <extracomment>ALL_PLATFORMS Information why the Transport PIN is void: the PIN Reset Service was used</extracomment>
        <translation>Вы запросили письмо для переустановки PIN-кода (письмо содержит 6-значный PIN-код и соответствующий код активации).</translation>
    </message>
    <message>
        <source>Once you set a 6-digit PIN, the Transport PIN is no longer valid. You may then only use the 6-digit PIN to authenticate yourself.</source>
        <extracomment>ALL_PLATFORMS Information that the Transport PIN is void once a 6-digit PIN was set.</extracomment>
        <translation>После установки 6-значного PIN-кода временный PIN-код становится недействительным. В дальнейшем для аутентификации может быть использован только 6-значный PIN-код.</translation>
    </message>
    <message>
        <source>The 6-digit card PIN is a %1number that you choose yourself%2 when you set up the eID function for the first time. It %1replaces%2 your%1 5-digit Transport PIN%2.</source>
        <extracomment>ALL_PLATFORMS Description text explaining the PINs 4/7</extracomment>
        <translation>6-значный PIN-код карты - это комбинация %1цифр, которую вы выбираете самостоятельно%2 при первой настройке функции онлайн-идентификации. Он %1заменяет%2 ваш%1 5-значный временный PIN-код%2.</translation>
    </message>
    <message>
        <source>Set up the ID card in three steps</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройте идентификационную карту в три шага</translation>
    </message>
    <message>
        <source>Set up a smartphone or card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройте смартфон или устройство чтения карт</translation>
    </message>
    <message>
        <source>To read your card, first pair an NFC-enabled smartphone or set up the USB card reader.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Чтобы считать карту, сначала установите соединение со смартфоном с функцией NFC или подключите USB-устройство чтения карт.</translation>
    </message>
    <message>
        <source>Read the ID card</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Считать идентификационную карту</translation>
    </message>
    <message>
        <source>This checks whether your device and ID card meet all the requirements.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Здесь осуществляется проверка соответствия вашего устройства и идентификационной карты всем требованиям.</translation>
    </message>
    <message>
        <source>Set up the PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Установление PIN-кода</translation>
    </message>
    <message>
        <source>You use the PIN every time you use your ID card online.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Вы будете использовать PIN-код каждый раз, когда будете пользоваться своей идентификационной картой в Интернете.</translation>
    </message>
    <message>
        <source>Set up the ID card in two steps</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройте идентификационную карту в два шага</translation>
    </message>
    <message>
        <source>If you require assistance with this, please contact our support team.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Если вам нужна помощь с этим, пожалуйста, свяжитесь с нашей службой поддержки.</translation>
    </message>
    <message>
        <source>Send mail</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Отправить сообщение по электронной почте</translation>
    </message>
    <message>
        <source>Support</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Поддержка</translation>
    </message>
    <message>
        <source>My smartphone does not show up</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Мой смартфон не отображается</translation>
    </message>
    <message>
        <source>If your smartphone does not appear in the list of available devices, there may be various reasons for this.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Если ваш смартфон не отображается в списке доступных устройств, этому могут быть разные причины.</translation>
    </message>
    <message>
        <source>Security software</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Защитное программное обеспечение</translation>
    </message>
    <message>
        <source>You may be using security software that prevents pairing.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Возможно, вы используете защитное программное обеспечение, которое блокирует сопряжение.</translation>
    </message>
    <message>
        <source>Updates</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Обновления</translation>
    </message>
    <message>
        <source>Make sure that you have installed the latest version on both devices.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Убедитесь, что на обоих устройствах установлена последняя версия.</translation>
    </message>
    <message>
        <source>Same local network</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Одна и та же локальная сеть</translation>
    </message>
    <message>
        <source>Both devices must be in the same network. This applies in the following cases:</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Оба устройства должны находиться в одной сети. Это применимо в следующих случаях:</translation>
    </message>
    <message>
        <source>Visibility of the devices</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Видимость устройств</translation>
    </message>
    <message>
        <source>The devices in the network must be visible to each other. Networks at the workplace, in public places or guest networks are therefore often not suitable, as functions such as &quot;Client Isolation&quot; or &quot;Broadcasting Filtering&quot; must not be active.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Устройства, подключенные к сети, должны быть видны друг другу. Поэтому сети на рабочем месте, в общественных местах или гостевые сети часто не подходят, поскольку такие функции, как «Client Isolation» или «Broadcasting Filtering», должны быть деактивированы.</translation>
    </message>
    <message>
        <source>If a device is not found in card reader mode, try to pair the device again via &quot;Pair device&quot;.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Если устройство не было найдено в режиме чтения карт, попробуйте ещё раз установить с ним соединение через пункт меню «сопряжение устройства».</translation>
    </message>
    <message>
        <source>&quot;Pair device&quot; instead of &quot;Activate card reader&quot;</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>«Сопряжение устройства» вместо «Активация устройства чтения карт»</translation>
    </message>
    <message>
        <source>Both devices in one WiFi network</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Оба устройства в одной сети Wi-Fi</translation>
    </message>
    <message>
        <source>The PC via LAN cable and the smartphone via WiFi in the same network</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Компьютер подключен к сети через кабель, а смартфон подключен к той же сети через Wi-Fi</translation>
    </message>
    <message>
        <source>Alternatively: set up a mobile WiFi hotspot via smartphone and dial in the device to be paired there.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Альтернативный вариант: настройте мобильную точку доступа WiFi через смартфон и подключите к ней устройство для сопряжения.</translation>
    </message>
    <message>
        <source>I can&apos;t recall my card PIN. How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>I can&apos;t recall neither my card PIN nor my transport PIN. How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>I can&apos;t recall my PUK. How do I set a new card PIN?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>MultiInfoHintContent</name>
    <message>
        <source>Hint</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished">Совет</translation>
    </message>
</context>
<context>
    <name>NavigationAction</name>
    <message>
        <source>Cancel</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Отмена</translation>
    </message>
    <message>
        <source>Back</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Назад</translation>
    </message>
    <message>
        <source>Close</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Закрыть</translation>
    </message>
</context>
<context>
    <name>NavigationView</name>
    <message>
        <source>Start</source>
        <translation>Пуск</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <source>Help</source>
        <translation>Справка</translation>
    </message>
    <message>
        <source>Card reader</source>
        <translation>Устройство чтения карт</translation>
    </message>
    <message>
        <source>%1 of %2</source>
        <extracomment>MOBILE Relative position of current navigation tab in navigation view. %1 is replaced with the current tab&apos;s index, %2 with the total count of tabs</extracomment>
        <translation>%1 из %2</translation>
    </message>
    <message>
        <source>Tab</source>
        <extracomment>MOBILE</extracomment>
        <translation>Вкладка</translation>
    </message>
    <message>
        <source>Selection</source>
        <extracomment>IOS Selected navigation tab.</extracomment>
        <translation>Выбор</translation>
    </message>
    <message>
        <source>Tab bar</source>
        <extracomment>IOS Name of a11y element of selected navigation tab.</extracomment>
        <translation>Строка вкладок</translation>
    </message>
    <message>
        <source>Selected</source>
        <extracomment>ANDROID Currently selected navigation tab of navigation view.</extracomment>
        <translation>Выбранная</translation>
    </message>
</context>
<context>
    <name>NfcConnectionInfoView</name>
    <message>
        <source>My ID card is not detected</source>
        <extracomment>MOBILE</extracomment>
        <translation>Моя идентификационная карта не обнаружена</translation>
    </message>
    <message>
        <source>If your ID card is not recognized when it is read, there could be various reasons.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Если ваша идентификационная карта не распознается при считывании, это может иметь различные причины.</translation>
    </message>
    <message>
        <source>NFC interface</source>
        <extracomment>MOBILE</extracomment>
        <translation>Интерфейс NFC</translation>
    </message>
    <message>
        <source>The NFC interface, and therefore the point at which the ID card must be placed, varies depending on the smartphone model. Find out where the NFC interface is on your smartphone model.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Расположение интерфейса NFC и, следовательно, точки, к которой нужно приложить идентификационную карту, зависит от модели смартфона. Узнайте, где находится интерфейс NFC на вашем смартфоне.</translation>
    </message>
    <message>
        <source>Tutorial: Using NFC</source>
        <extracomment>MOBILE</extracomment>
        <translation>Инструкция по пользованию NFC</translation>
    </message>
    <message>
        <source>Sources of interference</source>
        <extracomment>MOBILE</extracomment>
        <translation>Источники помех</translation>
    </message>
    <message>
        <source>Remove your phone case and allow the ID card to come into direct contact with the device. Low battery power can negatively impact NFC functionality, so make sure your smartphone is charged and not in power saving mode.</source>
        <translation>Обеспечьте прямой контакт устройства с идентификационной картой, сняв чехол с телефона. Низкий заряд батареи может негативно повлиять на работу функции NFC, поэтому убедитесь, что ваш смартфон заряжен и не находится в режиме энергосбережения.</translation>
    </message>
    <message>
        <source>Alternatives</source>
        <extracomment>MOBILE</extracomment>
        <translation>Альтернативы</translation>
    </message>
    <message>
        <source>If the connection does not work despite following the tips, you can use another smartphone as a card reader.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Если не получается установить соединение, несмотря на все советы, вы можете использовать другой смартфон в качестве устройства чтения карт.</translation>
    </message>
    <message>
        <source>Connect using a card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Установление связи с помощью устройства чтения карт</translation>
    </message>
    <message>
        <source>You can also use %1 on a PC and connect an USB card reader to read the ID card.</source>
        <extracomment>MOBILE &quot;%1&quot; is replaced with &quot;AusweisApp&quot;</extracomment>
        <translation>Вы также можете использовать %1 на компьютере и подключить USB-устройство чтения карт для считывания идентификационной карты.</translation>
    </message>
</context>
<context>
    <name>NfcWorkflow</name>
    <message>
        <source>Go to NFC settings</source>
        <extracomment>MOBILE</extracomment>
        <translation>Перейти к настройкам NFC</translation>
    </message>
    <message>
        <source>Start NFC scan</source>
        <extracomment>MOBILE</extracomment>
        <translation>Запустить сканирование NFC</translation>
    </message>
    <message>
        <source>NFC is switched off</source>
        <extracomment>MOBILE NFC is available but needs to be activated in the settings of the smartphone.</extracomment>
        <translation>Функция NFC выключена</translation>
    </message>
    <message>
        <source>Please enable NFC in your system settings.</source>
        <extracomment>MOBILE NFC is available but needs to be activated in the settings of the smartphone.</extracomment>
        <translation>Включите NFC в системных настройках.</translation>
    </message>
    <message>
        <source>NFC scan is not running</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC-сканирование не выполняется</translation>
    </message>
    <message>
        <source>Please start the NFC scan.</source>
        <extracomment>MOBILE NFC is available and enabled but needs to be started.</extracomment>
        <translation>Запустите NFC-сканирование.</translation>
    </message>
    <message>
        <source>NFC is not available</source>
        <extracomment>MOBILE</extracomment>
        <translation>Функция NFC недоступна</translation>
    </message>
    <message>
        <source>NFC is disabled</source>
        <extracomment>MOBILE</extracomment>
        <translation>Функция NFC деактивирована</translation>
    </message>
    <message>
        <source>Start scan</source>
        <extracomment>MOBILE NFC is available and enabled but needs to be started.</extracomment>
        <translation>Начать сканирование</translation>
    </message>
    <message>
        <source>Searching for ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Поиск идентификационной карты</translation>
    </message>
    <message>
        <source>Your device does not meet the technical requirements (Extended Length not supported).</source>
        <extracomment>MOBILE The NFC interface does not meet the minimum requirements.</extracomment>
        <translation>Ваше устройство не соответствует техническим требованиям (не поддерживается расширенная длина).</translation>
    </message>
    <message>
        <source>Please place your ID card on the top of the device&apos;s back side</source>
        <extracomment>IOS The ID card may be inserted, the authentication process may be started.</extracomment>
        <translation>Приложите идентификационную карту сверху к обратной стороне устройства</translation>
    </message>
    <message>
        <source>The device &quot;%1&quot; wants to use this smartphone as card reader and connect to your ID card</source>
        <extracomment>MOBILE %1 will be replaced with the name of the device.</extracomment>
        <translation>Устройство «%1» планирует использовать данный смартфон в качестве устройства чтения карт и установить соединение с вашей идентификационной картой</translation>
    </message>
    <message>
        <source>Keep one position for several seconds before trying another one and do not move the ID card after contact was established.</source>
        <extracomment>ANDROID Text that one ID card position should be kept for several seconds</extracomment>
        <translation>Удерживайте идентификационную карту в одном положении несколько секунд, прежде чем поменять положение, и не смещайте ее после установки соединения.</translation>
    </message>
    <message>
        <source>Please place your ID card directly on the device&apos;s back side</source>
        <extracomment>ANDROID Text regarding the ID card position on the device</extracomment>
        <translation>Приложите идентификационную карту непосредственно к задней стороне устройства</translation>
    </message>
    <message>
        <source>This device cannot read the ID card</source>
        <extracomment>MOBILE AA can&apos;t use NFC on this device, suggest to use SaK instead.</extracomment>
        <translation>Устройство не может считать идентификационную карту</translation>
    </message>
    <message>
        <source>If you want to use the online identification on this device anyway, you can %1pair%2 another %1NFC-enabled smartphone%2 as a card reader.</source>
        <extracomment>MOBILE AA can&apos;t use NFC on this device, suggest to use SaC instead. The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Если вы хотите выполнить онлайн-идентифифкацию на этом устройстве, вы можете%1сопрясти%2 другой %1смартфон с фунуцией NFC%2 в качестве устройства чтения карт.</translation>
    </message>
    <message>
        <source>Alternatively, you may use the %3 with a %1USB card reader%2 on your %1PC%2.</source>
        <extracomment>MOBILE AA can&apos;t use NFC on this device, suggest to use a PC instead. The text between %1 and %2 will be emphasized.</extracomment>
        <translation>Вы также можете использовать %3 на вашем %1компьютере%2 через %1USB-устройство чтения карт%2.</translation>
    </message>
    <message>
        <source>My ID card is not detected</source>
        <extracomment>MOBILE</extracomment>
        <translation>Моя идентификационная карта не обнаружена</translation>
    </message>
    <message>
        <source>Read ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Считывание идентификационной карты</translation>
    </message>
    <message>
        <source>Pair NFC-enabled smartphone</source>
        <extracomment>MOBILE</extracomment>
        <translation>Cопряжение смартфона с функцией NFC</translation>
    </message>
    <message>
        <source>Please place your ID card directly on the device&apos;s back side and try several positions.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Положите идентификационнцю-карту  на заднюю панель смартфона и попробуйте разные положения.</translation>
    </message>
    <message>
        <source>Once the connection has been established, %1hold%2 the card in the %1same position%2 until you are asked to remove it again.</source>
        <extracomment>MOBILE %1 + %2 are for bold formatting</extracomment>
        <translation>Как только соединение будет установлено, %1держите%2 -карту в %1том же положении%2, пока вас снова не попросят убрать карту.</translation>
    </message>
    <message>
        <source>Ensure a stable connection to the ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Установите постоянное соединение с идентификационной-картой</translation>
    </message>
    <message>
        <source>Connection to ID card lost</source>
        <extracomment>MOBILE</extracomment>
        <translation>Прервано соединение с идентификационной картой</translation>
    </message>
</context>
<context>
    <name>Notifications</name>
    <message>
        <source>Notification: %1</source>
        <extracomment>DESKTOP %1 will be replaced with a notification text</extracomment>
        <translation>Оповещени: %1</translation>
    </message>
</context>
<context>
    <name>NumberField</name>
    <message>
        <source>The number is hidden.</source>
        <extracomment>ALL_PLATFORMS Screenreader text for the password field</extracomment>
        <translation>Номер скрыт.</translation>
    </message>
    <message>
        <source>You entered %1 of %2 digits.</source>
        <extracomment>ALL_PLATFORMS Screenreader text for the password field</extracomment>
        <translation>Вы ввели %1 из %2 знаков.</translation>
    </message>
    <message>
        <source>Click to hide the number</source>
        <extracomment>DESKTOP Screenreader text for the eye icon to change the password visibility</extracomment>
        <translation>Щелкните, чтобы скрыть номер</translation>
    </message>
    <message>
        <source>Tap to hide the number</source>
        <extracomment>MOBILE Screenreader text for the eye icon to change the password visibility</extracomment>
        <translation>Нажмите, чтобы скрыть номер</translation>
    </message>
    <message>
        <source>Click to show the number</source>
        <extracomment>DESKTOP Screenreader text for the eye icon to change the password visibility</extracomment>
        <translation>Щелкните, чтобы показать номер</translation>
    </message>
    <message>
        <source>Tap to show the number</source>
        <extracomment>MOBILE Screenreader text for the eye icon to change the password visibility</extracomment>
        <translation>Нажмите, чтобы показать номер</translation>
    </message>
    <message>
        <source>The number is visible. Digits entered so far: %1</source>
        <extracomment>ALL_PLATFORMS Screenreader text for the password field</extracomment>
        <translation>Номер отображается. Цифры, введенные на данный момент: %1</translation>
    </message>
</context>
<context>
    <name>NumberPad</name>
    <message>
        <source>Number pad</source>
        <extracomment>MOBILE</extracomment>
        <translation>Цифровая клавиатура</translation>
    </message>
    <message>
        <source>Delete last digit</source>
        <extracomment>MOBILE A11y text for the &quot;delete&quot; button image.</extracomment>
        <translation>Удалить последний знак</translation>
    </message>
    <message>
        <source>Submit</source>
        <extracomment>MOBILE A11y text for the &quot;submit&quot; button image.</extracomment>
        <translation>Подтвердить</translation>
    </message>
    <message>
        <source>Delete last digit, disabled until input is present.</source>
        <extracomment>MOBILE A11y text for the &quot;delete&quot; button text when the button is disabled.</extracomment>
        <translation>Удалить последнюю цифру, деактивировано до начала ввода данных.</translation>
    </message>
    <message>
        <source>, disabled until input is complete.</source>
        <extracomment>MOBILE A11y text, appended onto the &quot;submit&quot; button text when the button is disabled.</extracomment>
        <translation>, деактивировано до завершения ввода данных.</translation>
    </message>
</context>
<context>
    <name>NumberPadButton</name>
    <message>
        <source>Disabled</source>
        <translation>Деактивировано</translation>
    </message>
</context>
<context>
    <name>OnboardingCheckIDView</name>
    <message>
        <source>Test result</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Результат проверки</translation>
    </message>
    <message>
        <source>Read ID card</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Считывание идентификационной карты</translation>
    </message>
    <message>
        <source>Please ensure that the ID card is placed on the card reader.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Убедитесь в том, что идентификационная карта размещена на устройстве чтения карт.</translation>
    </message>
    <message>
        <source>Please follow the instructions on your smartphone.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Следуйте инструкциям в вашем смартфоне.</translation>
    </message>
    <message>
        <source>Looking for ID card</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Поиск идентификационной карты</translation>
    </message>
    <message>
        <source>Connected to %1. Please follow the instructions on your smartphone.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Подключено к %1. Следуйте инструкциям в подключенном смартфоне.</translation>
    </message>
    <message>
        <source>Looking for device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Поиск устройства</translation>
    </message>
</context>
<context>
    <name>OnboardingCompletionView</name>
    <message>
        <source>provider list</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Список провайдеров</translation>
    </message>
    <message>
        <source>Complete setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Завершите настройку</translation>
    </message>
    <message>
        <source>If you are asked to authenticate yourself online on a provider&apos;s website, the provider will automatically redirect you to %1. The app guides you through the authentication process and then takes you back to the provider&apos;s website. You can find many examples of use in the %2.</source>
        <extracomment>ALL_PLATFORMS %1 will be replaced with the application name, %2 will be replaced with &quot;provider list&quot; as hyperlink.</extracomment>
        <translation>Если вас попросят пройти аутентификацию на сайте провайдера, вы автоматически будете перенаправлены в приложение %1. Приложение проведет вас через процесс аутентификации, а затем вернет на сайт провайдера. Вы можете найти множество примеров использования в списке провайдеров %2.</translation>
    </message>
    <message>
        <source>Use the eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Пользование функцией онлайн-идентификации</translation>
    </message>
    <message>
        <source>You can use %1 to authenticate yourself to providers online</source>
        <extracomment>ALL_PLATFORMS %1 will be replaced with the application name</extracomment>
        <translation>Вы можете использовать %1 для самоидентификации для провайдеров в Интернете</translation>
    </message>
</context>
<context>
    <name>OnboardingConfirmStageBaseView</name>
    <message>
        <source>Step %1 of %2 was successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Шаг %1 из %2 выполнен успешно</translation>
    </message>
    <message>
        <source>Hint</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Совет</translation>
    </message>
</context>
<context>
    <name>OnboardingConfirmationViewData</name>
    <message>
        <source>You can now use your ID card in combination with your 6-digit card PIN to identify yourself online.</source>
        <extracomment>DESKTOP Common part of the description of the confirmation view after the Change PIN stage.</extracomment>
        <translation>Теперь вы можете использовать вашу идентификационную карту в комбинации с 6-значным PIN-кодом карты для самоидентификации в Интернете.</translation>
    </message>
    <message>
        <source>You have now completed the main part of the setup - there is only one more query about personalized settings.</source>
        <extracomment>DESKTOP Description of the confirmation view about upcoming steps after the Change PIN stage.</extracomment>
        <translation>Вы завершили основную часть настройки. Остался лишь один вопрос, касающийся индивидуальных настроек.</translation>
    </message>
    <message>
        <source>You have now completed the main part of the setup - there are only two more queries about personalized settings.</source>
        <extracomment>DESKTOP Description of the confirmation view about upcoming steps after the Change PIN stage.</extracomment>
        <translation>Вы завершили основную часть настройки. Осталось лишь два вопроса, касающихся индивидуальных настроек.</translation>
    </message>
    <message>
        <source>Leave the ID card on the card reader.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp stage.</extracomment>
        <translation>Оставьте идентификационную карту на устройстве чтения карт.</translation>
    </message>
    <message>
        <source>Now have your PIN ready.</source>
        <extracomment>ALL_PLATFORMS Common part of the description of the confirmation view after the set up AusweisApp (with SaC) stage.</extracomment>
        <translation>Приготовьте свой PIN-код.</translation>
    </message>
    <message>
        <source>If you have already %1chosen a 6-digit card PIN%2 for this ID card (either in the %3 or at the competent authority) %1or%2 have ordered a %1PIN Reset Letter%2, this will apply.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp stage. %1 + %2 are for bold formatting, %3 is replaced with AusweisApp.</extracomment>
        <translation>Если вы уже %1установили 6-значный PIN-код%2 для этой идентификационной карты (в %3 или в компетентном органе) %1или%2 запросили %1письмо для сброса PIN-кода%2, то действительны они.</translation>
    </message>
    <message>
        <source>Otherwise, use your %1Transport PIN%2 that you received by letter when you first applied for the ID card. You then replace this %1one-time PIN%2 with your personal card PIN in the %3.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp with card reader stage. %1 + %2 are for bold formatting, %3 is replaced with AusweisApp.</extracomment>
        <translation>В обратном случае используйте %1временный PIN-код,%2 который вы получили в письме после вашей первой заявки на получение идентификационной карты. Затем замените этот %1одноразовый PIN-код%2 на ваш личный PIN-код карты в %3.</translation>
    </message>
    <message>
        <source>PIN has been successfully changed</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after the change PIN stage</extracomment>
        <translation>PIN-код успешно изменен</translation>
    </message>
    <message>
        <source>ID card detected</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after the set up AusweisApp stage</extracomment>
        <translation>Обнаружена идентификационная карта</translation>
    </message>
    <message>
        <source>Keep the position of your ID card on your smartphone so that it can be read immediately to request the PIN.</source>
        <extracomment>ALL_PLATFORMS Hint text of the confirmation view after the set up AusweisApp stage</extracomment>
        <translation>Не меняйте положение идентификационной карты на смартфоне, чтобы ее можно было непосредственно считать для запроса PIN-кода.</translation>
    </message>
    <message>
        <source>Your smartphone has been paired as a card reader and can be used to read your ID card in the next step.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp with SaC stage.</extracomment>
        <translation>Ваш смартфон сопряжен в качестве устройства чтения карт и может быть использован в следующем шаге для считывания вашей идентификационной карты.</translation>
    </message>
    <message>
        <source>Once your devices have been paired, you only need to select &quot;Activate card reader&quot; in the card reader menu on your smartphone in future to use your smartphone as a card reader for this device.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after pairing a SaC stage.</extracomment>
        <translation>После сопряжения устройств достаточно выбрать пункт «активировать устройство чтения карт» в меню устройства чтения карт на смартфоне, чтобы в дальнейшем использовать смартфон в качестве устройства чтения карт для этого устройства.</translation>
    </message>
    <message>
        <source>Your card reader has been recognized and can be used to read your ID card in the next step.</source>
        <extracomment>ALL_PLATFORMS Description of the confirmation view after detecting an USB Reader stage.</extracomment>
        <translation>Ваше устройство чтения карт обнаружено и может быть использовано в следующем шаге для чтения вашей идентификационной карты.</translation>
    </message>
    <message>
        <source>Pairing successful</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after pairing a SaC stage.</extracomment>
        <translation>Сопряжение выполнено успешно</translation>
    </message>
    <message>
        <source>PIN is correct</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after pairing a SaC stage.</extracomment>
        <translation>Правильный PIN-код</translation>
    </message>
    <message>
        <source>Card reader detected</source>
        <extracomment>ALL_PLATFORMS Headline of the confirmation view after after detecting an USB Reader stage.</extracomment>
        <translation>Обнаружено устройство чтения карт</translation>
    </message>
    <message>
        <source>If you want to change your PIN, select the “Change PIN” button on the start page.</source>
        <extracomment>MOBILE Description of the confirmation view after the Change PIN stage.</extracomment>
        <translation>Если вы хотите изменить свой PIN-код, выберите пункт «изменить PIN-код» на стартовой странице.</translation>
    </message>
    <message>
        <source>Card reader is ready</source>
        <extracomment>MOBILE Text that replaces _step x of y_ on the pair device confirm page</extracomment>
        <translation>Устройство чтения карт готово к использованию</translation>
    </message>
</context>
<context>
    <name>OnboardingConnectUsbReaderView</name>
    <message>
        <source>Connect an USB card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Подключите USB-устройство чтения карт</translation>
    </message>
    <message>
        <source>Set up card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройка устройства чтения карт</translation>
    </message>
</context>
<context>
    <name>OnboardingFailedView</name>
    <message>
        <source>Requirements not met</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Требования не выполнены</translation>
    </message>
    <message>
        <source>You cannot use the %1</source>
        <extracomment>ALL_PLATFORMS %1 is replaced with the name &quot;AusweisApp&quot;.</extracomment>
        <translation>Вы не можете использовать %1</translation>
    </message>
    <message>
        <source>Unfortunately you do not meet all the requirements necessary to use the %1.</source>
        <extracomment>ALL_PLATFORMS %1 is replaced with the name &quot;AusweisApp&quot;.</extracomment>
        <translation>К сожалению, у вас не выполнены все обязательные требования для использования %1.</translation>
    </message>
    <message>
        <source>You may restart the setup anytime under %1Help%2.</source>
        <extracomment>ALL_PLATFORMS %1 and %2 are replaced with bold emphasis.</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The setup will now be aborted - afterwards %1one prompt regarding personalized settings in the %3%2 will follow.</source>
        <extracomment>DESKTOP %1 and %2 are replaced with bold emphasis, %3 with the name &quot;AusweisApp&quot;.</extracomment>
        <translation>Настройка будет отменена - затем последует %1один вопрос по индивидуальным настройкам в %3%2.</translation>
    </message>
    <message>
        <source>The setup will now be aborted - afterwards %1two prompts regarding personalized settings in the %3%2 will follow.</source>
        <extracomment>DESKTOP %1 and %2 are replaced with bold emphasis, %3 with the name &quot;AusweisApp&quot;.</extracomment>
        <translation>Настройка будет отменена - затем последуют %1два вопроса по индивидуальным настройкам в %3%2.</translation>
    </message>
    <message>
        <source>The setup will now be aborted.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Настройка будет отменена.</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>MOBILE</extracomment>
        <translation>Отмена настройки</translation>
    </message>
</context>
<context>
    <name>OnboardingPairSacStartView</name>
    <message>
        <source>Set up smartphone as card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройка смартфона в качестве устройства чтения карт</translation>
    </message>
    <message>
        <source>Continue pairing</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Продолжение сопряжения</translation>
    </message>
    <message>
        <source>Install %1 on your smartphone</source>
        <extracomment>DESKTOP %1 is replaced with the name AusweisApp</extracomment>
        <translation>Установите %1 на вашем смартфоне</translation>
    </message>
    <message>
        <source>Scan the QR code, search your %1app store%2 for %3 or open the following link in your smartphone:</source>
        <extracomment>DESKTOP %1 and %2 are replaced with bold highlighting, %3 with the name AusweisApp</extracomment>
        <translation>Сканируйте QR-код, ищите в %1app store%2 приложение %3 или откройте следующую ссылку в своем смартфоне:</translation>
    </message>
    <message>
        <source>Open %1 on your smartphone</source>
        <extracomment>DESKTOP %1 is replaced with the name AusweisApp</extracomment>
        <translation>Откройте %1 в своем смартфоне</translation>
    </message>
    <message>
        <source>Select &quot;%1Set up %2%3&quot; there.</source>
        <extracomment>DESKTOP %1/%3 is replaced with bold highlighting, %2 with the name AusweisApp</extracomment>
        <translation>Выберите «%1Настройка%2%3».</translation>
    </message>
    <message>
        <source>You will then be asked whether you want to set up a PC/Mac too. Select the option &quot;%1Yes, I want to use a PC/Mac too%2&quot; here. You now select &quot;%1Continue pairing%2&quot; on both devices.</source>
        <extracomment>DESKTOP %1/%2 is replaced with bold highlighting</extracomment>
        <translation>Затем вас спросят, хотите ли вы также сделать настроки в компьютере/Mac. Выберите вариант «%1да, я хочу использовать также компьютер/Mac%2». Теперь выберите «%1продолжить сопряжение%2» на обоих устройствах.</translation>
    </message>
</context>
<context>
    <name>OnboardingPairSacView</name>
    <message>
        <source>Set up smartphone as card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Настройка смартфона в качестве устройства чтения карт</translation>
    </message>
    <message>
        <source>Click the arrow to show.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Нажмите на стрелку, чтобы показать.</translation>
    </message>
    <message>
        <source>Pairing instructions</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Инструкция по сопряжению</translation>
    </message>
    <message>
        <source>Available devices</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Доступные устройства</translation>
    </message>
    <message>
        <source>No device within reach</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Не обнаружены устройства в пределах досягаемости</translation>
    </message>
    <message>
        <source>My smartphone does not show up</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Мой смартфон не отображается</translation>
    </message>
    <message>
        <source>Ensure that the %1%2%3 on the smartphone that you want to use as card reader is %1up to date%3.</source>
        <extracomment>DESKTOP %1/%3 are replaced with bold highlighting, %2 with the name AusweisApp</extracomment>
        <translation>Убедитесь, что для %1%2%3 на смартфоне, который вы хотите использовать в качестве устройства чтения карт, %1установлены обновления%3.</translation>
    </message>
    <message>
        <source>Connect both devices to the %1same network%2, e.g. via WiFi.</source>
        <extracomment>DESKTOP %1/%2 are replaced with bold highlighting</extracomment>
        <translation>Подключите оба устройства к %1одной сети%2, например, через WiFi.</translation>
    </message>
    <message>
        <source>Select the %1Card reader%2 entry in the bottom menu bar on your smartphone.</source>
        <extracomment>DESKTOP %1/%2 are replaced with bold highlighting</extracomment>
        <translation>Выберите на смартфоне пункт меню %1устройство чтения карт%2 в нижней строке меню.</translation>
    </message>
    <message>
        <source>Then select %1Pair device%2 or, if a device was already paired, %1Pair new device%2.</source>
        <extracomment>DESKTOP %1/%2 are replaced with bold highlighting</extracomment>
        <translation>Затем выберите %1сопряжение устройства%2 или, если устройство уже было сопряжено - %1сопряжение нового устройства%2.</translation>
    </message>
    <message>
        <source>or</source>
        <extracomment>DESKTOP</extracomment>
        <translation>или</translation>
    </message>
    <message>
        <source>Afterwards, a pairing code is displayed on your smartphone. You may then select the device in the %1 list to enter the pairing code.</source>
        <extracomment>DESKTOP %1 will be replaced with the Available Devices list name</extracomment>
        <translation>Затем на экране смартфона появится код для сопряжения. В %1списке ниже вы сможете выбрать устройство для ввода кода сопряжения.</translation>
    </message>
    <message>
        <source>Use device</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Использовать устройство</translation>
    </message>
</context>
<context>
    <name>OnboardingPinErrorView</name>
    <message>
        <source>Try again</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Повторить попытку</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Отменить настройку</translation>
    </message>
</context>
<context>
    <name>OnboardingStartView</name>
    <message>
        <source>Setup</source>
        <extracomment>ALL_PLATFORMS Headline</extracomment>
        <translation>Настройки</translation>
    </message>
    <message>
        <source>Set up the eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Настройка функции онлайн-идентификации</translation>
    </message>
    <message>
        <source>Securely identify yourself online with the %1</source>
        <extracomment>ALL_PLATFORMS %1 is replaced with &quot;AusweisApp&quot;</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The eID function allows you to handle administrative or business matters easily and securely online.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To identify yourself to a service provider using the %3, you need your %1ID card%2, your %1PIN%2, and an %1NFC-enabled smartphone%2 or a USB card reader.</source>
        <extracomment>DESKTOP The text between %1 and %2 will be emphasized, %3 is replaced with &quot;AusweisApp&quot;</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>To identify yourself to a service provider using the %3, you need your %1ID card%2, your %1PIN%2, and an %1NFC-enabled smartphone%2.</source>
        <extracomment>Mobile The text between %1 and %2 will be emphasized, %3 is replaced with &quot;AusweisApp&quot;</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Start setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished">Начать настройку</translation>
    </message>
    <message>
        <source>The setup is recommended for first-time user. This way it can be ensured that all the technical requirements are met for your online authentication processes.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Are you using the app for the first time?</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Skip setup</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Пропустить настройки</translation>
    </message>
</context>
<context>
    <name>OnboardingView</name>
    <message>
        <source>Continue</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Продолжить</translation>
    </message>
    <message>
        <source>Skip this step</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Пропустить этот шаг</translation>
    </message>
</context>
<context>
    <name>PairingCodeInfoView</name>
    <message>
        <source>Pairing Information</source>
        <extracomment>MOBILE</extracomment>
        <translation>Информация о сопряжении</translation>
    </message>
    <message>
        <source>Open %1 on your %2other device%3.</source>
        <extracomment>MOBILE Assistance text for pairing new devices. Step 1 of 5</extracomment>
        <translation>Откройте %1 в другом %2вашем устройстве%3.</translation>
    </message>
    <message>
        <source>Make sure that both devices are on the %1same network%2 (e.g. WiFi).</source>
        <extracomment>MOBILE Assistance text for pairing new devices. Step 2 of 5. %1 and %2 are surrounding tags for bold font.</extracomment>
        <translation>Убедитесь, что оба устройства подключены %1к одной сети%2 (например, WiFi).</translation>
    </message>
    <message>
        <source>On your other device, go to %1Settings%2 and then %1Smartphone as card reader%2 resp. %1Manage pairings%2.</source>
        <extracomment>MOBILE Assistance text for pairing new devices. Step 3 of 5. %1 and %2 are surrounding tags for bold font.</extracomment>
        <translation>Зайдите на другом устройстве в %1Настройки%2 и выберите %1Смартфон в качестве устройства чтения карт%2/ %1Управление сопряжениями%2.</translation>
    </message>
    <message>
        <source>Choose this smartphone in the list to pair it.</source>
        <extracomment>MOBILE Assistance text for pairing new devices. Step 4 of 5</extracomment>
        <translation>Выберите данный смартфон в списке, чтобы выполнить сопряжение.</translation>
    </message>
    <message>
        <source>Enter the pairing code &quot;%1&quot;.</source>
        <extracomment>MOBILE Provide pairing code. Step 5 of 5</extracomment>
        <translation>Введите код сопряжения «%1».</translation>
    </message>
    <message>
        <source>Please follow these steps:</source>
        <extracomment>MOBILE</extracomment>
        <translation>Выполните следующие шаги:</translation>
    </message>
</context>
<context>
    <name>PairingFailedView</name>
    <message>
        <source>Pairing to &quot;%1&quot; failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой сопряжения с «%1»</translation>
    </message>
    <message>
        <source>Pairing failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой сопряжения</translation>
    </message>
</context>
<context>
    <name>PairingProcessInfo</name>
    <message>
        <source>Connect both devices to the %1same network%2, e.g. via WLAN.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 2 of 5. %1 and %2 are for bold formatting.</extracomment>
        <translation>Соедините оба устройства с %1одной сетью%2, например, через WiFi.</translation>
    </message>
    <message>
        <source>Then select %1Pair (new) device%2. You may need to activate NFC first.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 4 of 5. %1 and %2 are for bold formatting.</extracomment>
        <translation>Затем выберите %1Сопряжение (нового) устройства%2. Возможно, вам сначала нужно будет включить NFC.</translation>
    </message>
    <message>
        <source>As soon as a pairing code is displayed on the smartphone, you can use it for pairing here.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 5 of 5</extracomment>
        <translation>Как только на смартфоне высветится код сопряжения, вы сможете использовать его здесь для сопряжения.</translation>
    </message>
    <message>
        <source>My smartphone does not show up</source>
        <extracomment>ALL_PLATFORM</extracomment>
        <translation>Мой смартфон не отображается</translation>
    </message>
    <message>
        <source>%3Install and open%4 the %3%1%4 (at least version %2) on your NFC-enabled smartphone that you want to use as a card reader.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 1 of 5. %1 is a placeholder for the app name. %2 is a version placeholder. %3 and %4 are for bold formatting.</extracomment>
        <translation>%3Установите и откройте%4 приложение %3%1%4 (минимум версию %2) на смартфоне с поддержкой NFC, который вы хотите использовать в качестве устройства чтения карт.</translation>
    </message>
    <message>
        <source>On the NFC-enabled smartphone, select the %1Card reader%2 menu item in the lower menu bar. You may need to skip the initial setup first.</source>
        <extracomment>ALL_PLATFORM Assistance text for pairing new devices. Step 3 of 5. %1 and %2 are for bold formatting.</extracomment>
        <translation>На смартфоне с функцией NFC выберите пункт меню %1Устройство чтения карт%2 в строке меню снизу. Возможно, сначала нужно будет пропустить начальную настройку.</translation>
    </message>
</context>
<context>
    <name>PairingSuccessView</name>
    <message>
        <source>Pairing successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сопряжение выполнено успешно</translation>
    </message>
    <message>
        <source>Your device &quot;%1&quot; was successfully paired</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ваше устройство «%1» было успешно сопряжено</translation>
    </message>
    <message>
        <source>Your smartphone as card reader has been paired and can now be used to read your ID card when you start a process.</source>
        <extracomment>ALL_PLATFORMS The pairing with the SaC server was successful.</extracomment>
        <translation>Ваш смартфон карт был сопряжен в качестве устройства чтения карт и теперь может быть использован для считывания вашей идентификационной карты при запуске процесса.</translation>
    </message>
    <message>
        <source>In the future, you only need to select &quot;Activate card reader&quot; in the Card reader menu on the smartphone to use your smartphone as a card reader for this device.</source>
        <extracomment>ALL_PLATFORMS The pairing with the SaC server was successful.</extracomment>
        <translation>В дальнейшем, чтобы использовать смартфон в качестве устройства чтения карт для этого устройства, достаточно выбрать пункт «Активировать устройство чтения карт» в меню устройства чтения карт вашего смартфона.</translation>
    </message>
</context>
<context>
    <name>PinResetHints</name>
    <message>
        <source>How do I activate the eID function?</source>
        <extracomment>ALL_PLATFORMS Hint title to assist the user on how to set a new PIN</extracomment>
        <translation type="unfinished">Запросить активацию функции онлайн-идентификации</translation>
    </message>
    <message>
        <source>My PUK is used up. How do I set a new PIN?</source>
        <extracomment>ALL_PLATFORMS Hint title to assist the user on how to set a new PIN</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>PinSelectionButtons</name>
    <message>
        <source>Set by yourself or received by the PIN Reset Service</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Создается пользователем самостоятельно</translation>
    </message>
    <message>
        <source>6-digit PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>6-значный PIN-код</translation>
    </message>
    <message>
        <source>Received by letter while ordering your ID card</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Получен по почте в письме с PIN-кодом</translation>
    </message>
    <message>
        <source>5-digit Transport PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>5-значный временный PIN-код</translation>
    </message>
    <message>
        <source>Lost, forgotten, or never received it</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Утерян, забыт или не получен вовсе</translation>
    </message>
    <message>
        <source>No PIN</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>PIN-код отсутствует</translation>
    </message>
</context>
<context>
    <name>PkiSwitch</name>
    <message>
        <source>%1 more clicks to toggle the environment (prod/test) for integrated functions.</source>
        <extracomment>DESKTOP Used in notifications when the user taps the icon</extracomment>
        <translation>%1 Нажатие (-я/-й) для переключения среды (произв./тест.) для встроенных функций.</translation>
    </message>
    <message>
        <source>%1 more taps to toggle the environment (prod/test) for integrated functions.</source>
        <extracomment>MOBILE Used in notifications when the user taps the icon</extracomment>
        <translation>%1 Нажатие (-я/-й) для переключения среды (произв./тест.) для встроенных функций.</translation>
    </message>
    <message>
        <source>Testmode for the integrated functions activated.</source>
        <extracomment>ALL_PLATFORMS Used in notifications when the user taps the icon</extracomment>
        <translation>Тестовый режим для встроенных функций активирован.</translation>
    </message>
    <message>
        <source>Testmode for the integrated functions deactivated.</source>
        <extracomment>ALL_PLATFORMS Used in notifications when the user taps the icon</extracomment>
        <translation>Тестовый режим для встроенных функций деактивирован.</translation>
    </message>
</context>
<context>
    <name>PreparationInfoView</name>
    <message>
        <source>Your ID card</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ваша идентификационная карта</translation>
    </message>
    <message>
        <source>A smartphone or a card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Смартфон или устройство чтения карт</translation>
    </message>
    <message>
        <source>Your PIN</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Ваш PIN-код</translation>
    </message>
    <message>
        <source>I can&apos;t recall my PIN</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Я не могу вспомнить свой PIN-код</translation>
    </message>
    <message>
        <source>A NFC-enabled smartphone</source>
        <extracomment>MOBILE</extracomment>
        <translation>Смартфон с функцией NFC</translation>
    </message>
    <message>
        <source>What is required to use %1?</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Что нужно для использования %1?</translation>
    </message>
    <message>
        <source>The following documents are allowed:</source>
        <extracomment>MOBILE</extracomment>
        <translation>Допускаются следующие документы:</translation>
    </message>
    <message>
        <source>german ID card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Германская идентификационная карта</translation>
    </message>
    <message>
        <source>electronic residence permit</source>
        <extracomment>MOBILE</extracomment>
        <translation>Электронное разрешение на временное проживание</translation>
    </message>
    <message>
        <source>eID card for EU citizens</source>
        <extracomment>MOBILE</extracomment>
        <translation>Карта онлайн-идентификации граждан ЕС</translation>
    </message>
    <message>
        <source>You may use your %2NFC-enabled smartphone%3. To do this you will have to install %1 on this smartphone. Alternatively you may use an %2USB card reader with your PC%3.</source>
        <extracomment>DESKTOP %1 = Application Name, %2 + %3 = Bold Tags</extracomment>
        <translation>Вы можете использовать свой смартфон %2 с функцией NFC%3. Для этого вам необходимо установить %1 на этом смартфоне. Вы также можете использовать %USB-устройство чтения карт с %2компьютером%3.</translation>
    </message>
    <message>
        <source>You have received a one-time PIN, the %1Transport PIN%2, as a letter from your competent authority. You can replace this with a%1 6-digit card PIN%2 in the %3 or at the Citizens&apos; Registration Office. If you do not have a PIN or do not remember your card PIN, click here:</source>
        <extracomment>DESKTOP %1 + %2 = Bold Tags, %3 = AusweisApp</extracomment>
        <translation>Вы получили одноразовый PIN-код %1временный PIN-код%2 в письме от компетентнго органа. Вы можете заменить его на%1 6-значный PIN-код карты%2 в %3 или в бюро обслуживания населения. Если у вас нет PIN-кода или вы не помните PIN-код своей карты, нажмите сюда:</translation>
    </message>
    <message>
        <source>You have received a one-time PIN, the %1Transport PIN%2, as a letter from your competent authority. You can replace this with a%1 6-digit card PIN%2 in the %3 or at the Citizens&apos; Registration Office. If you do not have a PIN or do not remember your card PIN, tap here:</source>
        <extracomment>MOBILE %1 + %2 = Bold Tags, %3 = AusweisApp</extracomment>
        <translation>Вы получили одноразовый PIN-код %1временный PIN-код%2 в письме от компетентнго органа. Вы можете заменить его на%1 6-значный PIN-код карты%2 в %3 или в бюро обслуживания населения. Если у вас нет PIN-кода или вы не помните PIN-код своей карты, нажмите сюда:</translation>
    </message>
    <message>
        <source>The chip in your ID card is read using %1NFC%2. To do this, simply place the ID card on the %1back of the smartphone%2.</source>
        <extracomment>MOBILE %1 + %2 = Bold Tags</extracomment>
        <translation>Чип в вашей идентификационной карте считывается с помощью %1NFC%2. Просто приложите для этого идентификационную карту %1задней стороне смартфона%2.</translation>
    </message>
    <message>
        <source>You could either use the %1German ID card%2, an %1electronic residence permit%2 or the %1eID card for EU citizens%2.</source>
        <extracomment>DESKTOP %1 + %2 = Bold Tags</extracomment>
        <translation>Вы можете использовать либо %1германскую идентификационную карту%2, %1электронное разрешение на временное жительство%2, либо %1карту eID для граждан ЕС%2.</translation>
    </message>
    <message>
        <source>Abort setup</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Отменить настройки</translation>
    </message>
</context>
<context>
    <name>PrimaryDeviceDecisionView</name>
    <message>
        <source>Do you want to also use the %1 on a PC/Mac?</source>
        <extracomment>MOBILE</extracomment>
        <translation>Хотите ли вы также использовать %1 на компьютере/Mac?</translation>
    </message>
    <message>
        <source>In order to also perform online authentications on a PC/Mac, you may pair your smartphone as a card reader. To do so, you need to have the %1 installed on both devices.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Чтобы выполнять онлайн-аутентификацию и на компьютере/Mac, вы можете соединить свой смартфон в качестве устройства чтения карт. Для этого на обоих устройствах должно быть установлено приложение %1.</translation>
    </message>
    <message>
        <source>No, I just want to use this device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Нет, я хочу использовать только это устройство</translation>
    </message>
    <message>
        <source>Yes, I also want to use a PC/Mac</source>
        <extracomment>MOBILE</extracomment>
        <translation>Да, я хочу использовать также компьютер/Mac</translation>
    </message>
</context>
<context>
    <name>PrivacyStatement</name>
    <message>
        <source>Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Ваши персональные данные не сохраняются и не обрабатываются. Подробная информация об обработке персональных данных: см. %1.</translation>
    </message>
    <message>
        <source>data privacy statement</source>
        <extracomment>ALL_PLATFORMS Text of the self authentication html link inside of a sentence</extracomment>
        <translation>Заявление о конфиденциальности данных</translation>
    </message>
</context>
<context>
    <name>ProviderInfoButton</name>
    <message>
        <source>Show more information about the service provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Показать больше информации о провайдере службы</translation>
    </message>
    <message>
        <source>Service Provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>ProxyCredentialsPopup</name>
    <message>
        <source>Sign in</source>
        <extracomment>DESKTOP Text of the button in the proxy credentials popup.
----------
DESKTOP Title of the proxy credentials popup.</extracomment>
        <translation>Войти в систему</translation>
    </message>
    <message>
        <source>The proxy %1 requires username and password.</source>
        <extracomment>DESKTOP Text of the proxy credentials popup. An example for %1 is http://proxy.example.com:1337.</extracomment>
        <translation>Прокси-сервер %1 запрашивает имя пользователя и пароль.</translation>
    </message>
    <message>
        <source>Proxy credential username</source>
        <extracomment>DESKTOP Accessible name.</extracomment>
        <translation>Имя пользователя для прокси-сервера</translation>
    </message>
    <message>
        <source>Username</source>
        <extracomment>DESKTOP Label of the textfield for the username.</extracomment>
        <translation>Имя пользователя</translation>
    </message>
    <message>
        <source>Proxy credential password</source>
        <extracomment>DESKTOP Accessible name.</extracomment>
        <translation>Пароль для прокси-сервера</translation>
    </message>
    <message>
        <source>Password</source>
        <extracomment>DESKTOP Label of the textfield for the password.</extracomment>
        <translation>Пароль</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>An error occurred in log handling: %1</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Произошла ошибка при управлении файлом журнала: %1</translation>
    </message>
    <message>
        <source>Please describe the error that occurs. Our support is available to you in German and English.</source>
        <translation>Опишите возникшую ошибку. Мы предоставляем поддержку на немецком и английском языках.</translation>
    </message>
    <message>
        <source>You may want to attach the logfile which can be saved from the error dialog.</source>
        <translation>Вы можете приложить файл журнала, сохраненный из диалогового окна ошибки.</translation>
    </message>
    <message>
        <source>Error code</source>
        <translation>Код ошибки</translation>
    </message>
    <message>
        <source>Service URL</source>
        <translation>URL службы</translation>
    </message>
    <message>
        <source>Parameter of occurred error:</source>
        <translation>Параметры возникшей ошибки:</translation>
    </message>
    <message>
        <source>Critical errors:</source>
        <translation>Критические ошибки:</translation>
    </message>
</context>
<context>
    <name>ReaderConfigurationInfo</name>
    <message>
        <source>Unknown reader</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неизвестное устройство чтения карт</translation>
    </message>
</context>
<context>
    <name>ReaderFoundConfirmation</name>
    <message>
        <source>Found new smartphone as card reader that is suitable for the ID card. The workflow may now be continued.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Найден новый смартфон в качестве устройства чтения карт, подходящий для считывания идентификационной карты. Процесс может быть продолжен.</translation>
    </message>
    <message>
        <source>Found new USB card reader that is suitable for the ID card. The workflow may now be continued.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Найдено новое USB-устройство чтения карт, подходящее для идентификационной карты. Теперь можно продолжить процесс.</translation>
    </message>
    <message>
        <source>Continue</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Продолжить</translation>
    </message>
</context>
<context>
    <name>RedirectView</name>
    <message>
        <source>Remove the ID card from the card reader</source>
        <extracomment>DESKTOP Hint to user that the ID card should be removed</extracomment>
        <translation>Извлеките идентификационную карту из устройства чтения карт</translation>
    </message>
    <message>
        <source>You may now remove your ID card from the device.</source>
        <extracomment>MOBILE Hint to user that the ID card should be removed</extracomment>
        <translation>Можно извлечь идентификационную карту из устройства.</translation>
    </message>
    <message>
        <source>If you have any questions or encounter any errors during the process, please contact the corresponding provider.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Если у вас возникли вопросы или вы обнаружили ошибки в процессе, обратитесь, пожалуйста, к соответствующему провайдеру.</translation>
    </message>
    <message>
        <source>You will be automatically redirected to the provider in a few seconds. If you are not automatically redirected, click the &quot;%1&quot; button.</source>
        <extracomment>DESKTOP Redirect information when automatic redirect is enabled</extracomment>
        <translation>Через несколько секунд вас автоматически перенаправят к провайдеру. Если перенаправление не произойдет автоматически, нажмите на кнопку «%1».</translation>
    </message>
    <message>
        <source>Click the button to complete the authentication and return to the provider.</source>
        <extracomment>DESKTOP Redirect information when automatic redirect is disabled</extracomment>
        <translation>Нажмите кнопку, чтобы завершить аутентификацию и вернуться к провайдеру.</translation>
    </message>
    <message>
        <source>You will be automatically redirected to the provider in a few seconds. If you are not automatically redirected, tap the &quot;%1&quot; button.</source>
        <extracomment>MOBILE Redirect information when automatic redirect is enabled</extracomment>
        <translation>Через несколько секунд вас автоматически перенаправят к провайдеру. Если перенаправление не произойдет автоматически, нажмите на кнопку «%1».</translation>
    </message>
    <message>
        <source>Tap the button to complete the authentication and return to the provider.</source>
        <extracomment>MOBILE Redirect information when automatic redirect is disabled</extracomment>
        <translation>Нажмите кнопку, чтобы завершить аутентификацию и вернуться к провайдеру.</translation>
    </message>
    <message>
        <source>Authentication successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Аутентификация выполнена успешно</translation>
    </message>
</context>
<context>
    <name>ReleaseNotes</name>
    <message>
        <source>Retry</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Повторить попытку</translation>
    </message>
</context>
<context>
    <name>ReleaseNotesView</name>
    <message>
        <source>Release notes</source>
        <extracomment>MOBILE</extracomment>
        <translation>Примечания к версии</translation>
    </message>
</context>
<context>
    <name>RemoteReaderDelegate</name>
    <message>
        <source>Smartphone named &quot;%1&quot;.</source>
        <extracomment>DESKTOP Name of remote device. %1 is replaced with the name.</extracomment>
        <translation>Имя смартфона «%1».</translation>
    </message>
    <message>
        <source>Status: &quot;%1&quot;.</source>
        <extracomment>DESKTOP Status of remote device. %1 is replaced with the status.</extracomment>
        <translation>Статус: &quot;%1&quot;.</translation>
    </message>
    <message>
        <source>Pair</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сопряжение</translation>
    </message>
    <message>
        <source>Unpair</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Разорвать сопряжение</translation>
    </message>
    <message>
        <source>%1 device &quot;%2&quot;</source>
        <extracomment>DESKTOP Text of pairing button, %1 will be Pair/Unpair and %2 is replaced with device name</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>RemoteReaderView</name>
    <message>
        <source>Paired devices</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сопряженные устройства</translation>
    </message>
    <message>
        <source>Add pairing</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Добавить сопряжение</translation>
    </message>
    <message>
        <source>Last connected</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Последние подключенные</translation>
    </message>
</context>
<context>
    <name>RemoteServiceController</name>
    <message>
        <source>Card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Устройство чтения карт</translation>
    </message>
</context>
<context>
    <name>RemoteServiceSettings</name>
    <message>
        <source>Manage pairings</source>
        <extracomment>MOBILE</extracomment>
        <translation>Управление сопряжениями</translation>
    </message>
    <message>
        <source>Paired devices</source>
        <extracomment>MOBILE</extracomment>
        <translation>Сопряженные устройства</translation>
    </message>
    <message>
        <source>Available</source>
        <extracomment>MOBILE</extracomment>
        <translation>Доступно</translation>
    </message>
    <message>
        <source>Last connected</source>
        <extracomment>MOBILE</extracomment>
        <translation>Последние подключенные</translation>
    </message>
    <message>
        <source>Tap to remove device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Нажмите для удаления устройства</translation>
    </message>
    <message>
        <source>Remove</source>
        <extracomment>MOBILE</extracomment>
        <translation>Удалить</translation>
    </message>
    <message>
        <source>Do you want to remove the pairing of the device &quot;%1&quot;?</source>
        <extracomment>MOBILE</extracomment>
        <translation>Удалить сопряжение устройства «%1»?</translation>
    </message>
    <message>
        <source>Remove pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Удалить сопряжение</translation>
    </message>
    <message>
        <source>Add pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Добавить сопряжение</translation>
    </message>
    <message>
        <source>Tap to pair</source>
        <extracomment>MOBILE</extracomment>
        <translation>Нажмите для сопряжения</translation>
    </message>
    <message>
        <source>Please connect your WiFi to use another smartphone as card reader (SaC).</source>
        <extracomment>MOBILE Wifi is not enabled and no new devices can be paired.</extracomment>
        <translation>Подключитесь к сети Wi-Fi, чтобы использовать другой смартфон в качестве устройства чтения карт (SaC).</translation>
    </message>
    <message>
        <source>Enable WiFi</source>
        <extracomment>MOBILE</extracomment>
        <translation>Активировать Wi-Fi</translation>
    </message>
    <message>
        <source>Pairing code</source>
        <extracomment>MOBILE</extracomment>
        <translation>Код сопряжения</translation>
    </message>
    <message>
        <source>Pairing the device...</source>
        <extracomment>MOBILE</extracomment>
        <translation>Сопряжение устройства...</translation>
    </message>
    <message>
        <source>Tap to use device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Нажмите, чтобы использовать устройство</translation>
    </message>
</context>
<context>
    <name>RemoteServiceView</name>
    <message>
        <source>WiFi not active</source>
        <extracomment>MOBILE</extracomment>
        <translation>Wi-Fi не активирован</translation>
    </message>
    <message>
        <source>NFC not active</source>
        <extracomment>MOBILE</extracomment>
        <translation>Функция NFC не активирована</translation>
    </message>
    <message>
        <source>Card access in progress</source>
        <extracomment>MOBILE</extracomment>
        <translation>Выполняется доступ к карте</translation>
    </message>
    <message>
        <source>Card reader ready</source>
        <extracomment>MOBILE</extracomment>
        <translation>Устройство чтения карт готово к использованию</translation>
    </message>
    <message>
        <source>To do this, start a process on a paired device.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Запустите для этого процесс на сопряженном устройстве.</translation>
    </message>
    <message>
        <source>Enable WiFi</source>
        <extracomment>MOBILE</extracomment>
        <translation>Активировать Wi-Fi</translation>
    </message>
    <message>
        <source>Enable NFC</source>
        <extracomment>MOBILE</extracomment>
        <translation>Активировать NFC</translation>
    </message>
    <message>
        <source>Pair device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Выполнить сопряжение устройства</translation>
    </message>
    <message>
        <source>Activate card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Активировать устройство чтения карт</translation>
    </message>
    <message>
        <source>You can use this Smartphone as a card reader for the %1 on other devices e.g. a laptop.

To do this you first have to pair that device with this smartphone.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Можно использовать данный смартфон в качестве устройства чтения карт для %1 в других устройствах, например в ноутбуке.

Для этого сначала следует выполнить сопряжение данного устройства с этим смартфоном.</translation>
    </message>
    <message>
        <source>Card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Устройство чтения карт</translation>
    </message>
    <message>
        <source>Paired Devices</source>
        <extracomment>MOBILE</extracomment>
        <translation>Сопряженные устройства</translation>
    </message>
    <message>
        <source>Pair new device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Выполнить сопряжение нового устройства</translation>
    </message>
    <message>
        <source>Waiting for pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ожидание сопряжения</translation>
    </message>
    <message>
        <source>Start pairing of a new device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Начать сопряжение нового устройства</translation>
    </message>
    <message>
        <source>Where do I enter the pairing code?</source>
        <extracomment>MOBILE</extracomment>
        <translation>Где следует вводить код сопряжения?</translation>
    </message>
    <message>
        <source>Cancel pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Отменить сопряжение</translation>
    </message>
    <message>
        <source>Paired devices may use this Smartphone as a card reader now.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Теперь сопряженные устройства могут использовать этот смартфон в качестве устройства чтения карт.</translation>
    </message>
    <message>
        <source>NFC is not available</source>
        <extracomment>MOBILE</extracomment>
        <translation>Функция NFC недоступна</translation>
    </message>
    <message>
        <source>Stop card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Остановить устройство чтения карт</translation>
    </message>
    <message>
        <source>No device paired</source>
        <extracomment>MOBILE</extracomment>
        <translation>Нет сопряженного устройства</translation>
    </message>
    <message>
        <source>Card reader not active</source>
        <extracomment>MOBILE</extracomment>
        <translation>Уустройство чтения карт не активно</translation>
    </message>
    <message>
        <source>Use this smartphone as a card reader for a paired device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Использовать этот смартфон в качестве устройства чтения карт для сопряженного устройства</translation>
    </message>
    <message>
        <source>Activate the card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>Активировать устройство чтения карт</translation>
    </message>
    <message>
        <source>Both devices have to be on the same network (e.g. WiFi).</source>
        <extracomment>MOBILE The remote service is active. Hint that both devices need to be connected to the same network.</extracomment>
        <translation>Оба устройства должны быть подключены к одной сети (например, Wi-Fi).</translation>
    </message>
    <message>
        <source>Pairing code:</source>
        <extracomment>MOBILE</extracomment>
        <translation>Код сопряжения:</translation>
    </message>
    <message>
        <source>This allows the paired devices to use this smartphone as a card reader.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Это позволит сопряженным устройствам использовать этот смартфон в качестве устройства чтения карт.</translation>
    </message>
    <message>
        <source>Enter the pairing code</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ввести код сопряжения</translation>
    </message>
    <message>
        <source>Enter the pairing code &quot;%1&quot; in the %2 on your other device. Both devices have to be on the same network (e.g. WiFi).</source>
        <extracomment>MOBILE %1 is replaced with the pairing code, %2 with the name &quot;AusweisApp&quot;</extracomment>
        <translation>Ввести код сопряжения %1 в  %2  на другом устройстве. Оба устройства должны находиться в одной сети (напимер, WiFi).</translation>
    </message>
    <message>
        <source>Pairing progress</source>
        <extracomment>MOBILE Name of an progress indicator during the pairing process read by screen readers</extracomment>
        <translation>Ход сопряжения</translation>
    </message>
</context>
<context>
    <name>RemoteWorkflow</name>
    <message>
        <source>Enable WiFi</source>
        <extracomment>MOBILE</extracomment>
        <translation>Активировать Wi-Fi</translation>
    </message>
    <message>
        <source>To use the remote service WiFi has to be activated. Please activate WiFi in your device settings.</source>
        <extracomment>MOBILE The WiFi module needs to be enabled in the system settings to use the remote service.</extracomment>
        <translation>Для использования удаленной службы необходимо активировать Wi-Fi. Активируйте Wi-Fi в настройках устройства.</translation>
    </message>
    <message>
        <source>Wifi disabled</source>
        <extracomment>MOBILE</extracomment>
        <translation>Wi-Fi деактивирован</translation>
    </message>
    <message>
        <source>Determine card</source>
        <extracomment>MOBILE</extracomment>
        <translation>Определение карты</translation>
    </message>
    <message>
        <source>The connected smartphone as card reader (SaC) unfortunately does not meet the technical requirements (Extended Length not supported).</source>
        <extracomment>MOBILE The device does not support Extended Length and can not be used as card reader.</extracomment>
        <translation>К сожалению, подключенный в качестве устройства чтения карт (SaC) смартфон не соответствует техническим требованиям (не поддерживается расширенная длина).</translation>
    </message>
    <message>
        <source>Connected to %1. Please follow the instructions on the connected smartphone.</source>
        <extracomment>MOBILE The connection to the smartphone was established, the ID card may be inserted.</extracomment>
        <translation>Подключено к %1. Следуйте инструкциям в подключенном смартфоне.</translation>
    </message>
    <message>
        <source>Manage pairings</source>
        <extracomment>MOBILE</extracomment>
        <translation>Управление сопряжениями</translation>
    </message>
    <message>
        <source>No smartphone as card reader connected</source>
        <extracomment>MOBILE</extracomment>
        <translation>Смартфон в качестве устройства чтения карт не подключен</translation>
    </message>
    <message>
        <source>Allow a connection on a paired smartphone or pair a new smartphone.</source>
        <extracomment>MOBILE No paired and reachable device was found, hint that the remote device needs to be actually started for this feature.</extracomment>
        <translation>Разрешите подключение к сопряженному смартфону или выполните сопряжение нового смартфона.</translation>
    </message>
</context>
<context>
    <name>RequestedRights</name>
    <message>
        <source>Write access (update)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Доступ для записи (обновление)</translation>
    </message>
    <message>
        <source>Read access (optional)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Доступ для чтения (опционально)</translation>
    </message>
    <message>
        <source>Read access</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Доступ для чтения</translation>
    </message>
</context>
<context>
    <name>ResultErrorView</name>
    <message>
        <source>Show Details</source>
        <extracomment>MOBILE</extracomment>
        <translation>Показать подробную информацию</translation>
    </message>
</context>
<context>
    <name>ResultView</name>
    <message>
        <source>Send email</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Отправить сообщение электронной почты</translation>
    </message>
    <message>
        <source>Save log</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сохранить файл журнала</translation>
    </message>
    <message>
        <source>Logfiles (*.log)</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Файлы журнала (*.log)</translation>
    </message>
    <message>
        <source>See details</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Показать подробную информацию</translation>
    </message>
    <message>
        <source>OK</source>
        <extracomment>MOBILE</extracomment>
        <translation>OK</translation>
    </message>
    <message>
        <source>Hint</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Совет</translation>
    </message>
</context>
<context>
    <name>ScreenshotPreventer</name>
    <message>
        <source>Attention: Screen Recording in progress</source>
        <extracomment>MOBILE</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Attention: Screenshot taken</source>
        <extracomment>IOS</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>SecurityAndPrivacySettings</name>
    <message>
        <source>Numeric keypad</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Цифровая клавиатура</translation>
    </message>
    <message>
        <source>Shuffle keys</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Случайное расположение кнопок экранной клавиатуры</translation>
    </message>
    <message>
        <source>Hide key animations</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Скрыть анимацию клавиатуры</translation>
    </message>
    <message>
        <source>Makes it difficult for outsiders to detect PIN entry</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Затрудняет посторонним распознование PIN-кода при вводе</translation>
    </message>
</context>
<context>
    <name>SelfAuthStartView</name>
    <message>
        <source>See my personal data</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Просмотреть персональные данные</translation>
    </message>
    <message>
        <source>This way you can make sure that everything is working and that the data stored on your eID card is up to date before you identify yourself to a provider.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Таким образом до прохождения самоидентификации для провайдера вы можете убедиться в том, что всё работает и данные, хранящиеся на вашей карте онлайн-идентификации, актуальны.</translation>
    </message>
    <message>
        <source>To do this, you go through an authentication process at Governikus GmbH &amp; Co. KG, the manufacturer of the %1.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Пройдите для этого аутентификацию у Governikus GmbH &amp; Co. KG - производителя %1.</translation>
    </message>
    <message>
        <source>Use eID function</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Использование функции онлайн-идентификации</translation>
    </message>
    <message>
        <source>Have a look which data is stored on your eID card</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Посмотрите, какие данные хранятся на вашей карте онлайн-идентификации</translation>
    </message>
    <message>
        <source>Identify</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Идентифицировать</translation>
    </message>
</context>
<context>
    <name>SelfAuthenticationData</name>
    <message>
        <source>Read data</source>
        <extracomment>DESKTOP Title of the self authentication result data view</extracomment>
        <translation>Считать данные</translation>
    </message>
    <message>
        <source>Identify</source>
        <extracomment>MOBILE</extracomment>
        <translation>Идентифицировать</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>MOBILE</extracomment>
        <translation>Назад к начальной странице</translation>
    </message>
</context>
<context>
    <name>SelfAuthenticationHeader</name>
    <message>
        <source>Successfully read data.</source>
        <extracomment>ALL_PLATFORMS Status message that the self authentication successfully completed (1/2).</extracomment>
        <translation>Данные считаны успешно.</translation>
    </message>
    <message>
        <source>You may now remove your ID card from the device.</source>
        <extracomment>ALL_PLATFORMS Status message that the self authentication successfully completed (2/2).</extracomment>
        <translation>Можно извлечь идентификационную карту из устройства.</translation>
    </message>
</context>
<context>
    <name>SettingsView</name>
    <message>
        <source>Settings</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Настройки</translation>
    </message>
    <message>
        <source>General</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Общая информация</translation>
    </message>
    <message>
        <source>Smartphone as card reader</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Смартфон в качестве устройства чтения карт</translation>
    </message>
    <message>
        <source>USB card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>USB-устройство чтения карт</translation>
    </message>
    <message>
        <source>Security and privacy</source>
        <extracomment>DESKTOP
----------
ANDROID</extracomment>
        <translation>Безопасность и защита данных</translation>
    </message>
    <message>
        <source>Debug options</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Параметры отладки</translation>
    </message>
    <message>
        <source>Developer options</source>
        <extracomment>DESKTOP
----------
MOBILE</extracomment>
        <translation>Параметры разработчика</translation>
    </message>
    <message>
        <source>Change language</source>
        <extracomment>MOBILE</extracomment>
        <translation>Изменить язык</translation>
    </message>
    <message>
        <source>Device name</source>
        <extracomment>MOBILE</extracomment>
        <translation>Имя устройства</translation>
    </message>
    <message>
        <source>Enter PIN on this device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Ввести PIN-код на этом устройстве</translation>
    </message>
    <message>
        <source>Shuffle keys</source>
        <extracomment>MOBILE</extracomment>
        <translation>Случайное расположение кнопок экранной клавиатуры</translation>
    </message>
    <message>
        <source>Hide key animations</source>
        <extracomment>MOBILE</extracomment>
        <translation>Скрыть анимацию клавиатуры</translation>
    </message>
    <message>
        <source>Skip rights page</source>
        <extracomment>MOBILE</extracomment>
        <translation>Пропустить страницу с разрешениями</translation>
    </message>
    <message>
        <source>Testmode for the self-authentication</source>
        <extracomment>MOBILE</extracomment>
        <translation>Тестовый режим для самоаутентификации</translation>
    </message>
    <message>
        <source>Internal card simulator</source>
        <extracomment>MOBILE</extracomment>
        <translation>Внутренний симулятор карт</translation>
    </message>
    <message>
        <source>Developer mode</source>
        <extracomment>MOBILE</extracomment>
        <translation>Режим разработчика</translation>
    </message>
    <message>
        <source>Use a more tolerant mode</source>
        <extracomment>MOBILE</extracomment>
        <translation>Использовать менее чувствительный режим</translation>
    </message>
    <message>
        <source>Create dummy entries</source>
        <extracomment>MOBILE</extracomment>
        <translation>Создание фиктивных записей</translation>
    </message>
    <message>
        <source>New Logfile</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Новый файл журнала</translation>
    </message>
    <message>
        <source>15 days old Logfile</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Файл журнала, созданный 15 дней назад</translation>
    </message>
    <message>
        <source>Show requested rights on this device as well</source>
        <extracomment>MOBILE</extracomment>
        <translation>Показывать запрошенные права также и в этом устройстве</translation>
    </message>
    <message>
        <source>Manage pairings</source>
        <extracomment>MOBILE</extracomment>
        <translation>Управление сопряжениями</translation>
    </message>
    <message>
        <source>Show Transport PIN reminder, store feedback and close reminder dialogs.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Показывать напоминание временного PIN-кода, сохранять обратную связь и закрывать диалоговые окна с напоминаниями.</translation>
    </message>
    <message>
        <source>Reset hideable dialogs</source>
        <extracomment>MOBILE</extracomment>
        <translation>Сбросить скрываемые диалоговые окна</translation>
    </message>
    <message>
        <source>Toggling will restart the %1</source>
        <extracomment>MOBILE</extracomment>
        <translation>Переключение приведет к перезапуску %1</translation>
    </message>
    <message>
        <source>Use system font</source>
        <extracomment>MOBILE</extracomment>
        <translation>Использовать системный шрифт</translation>
    </message>
    <message>
        <source>Appearance</source>
        <extracomment>MOBILE</extracomment>
        <translation>Внешний вид</translation>
    </message>
    <message>
        <source>Add and remove devices</source>
        <extracomment>MOBILE</extracomment>
        <translation>Добавить/удалить устройства</translation>
    </message>
    <message>
        <source>On-site reading</source>
        <extracomment>MOBILE</extracomment>
        <translation>Чтение данных на месте</translation>
    </message>
    <message>
        <source>Support CAN allowed mode for on-site reading</source>
        <extracomment>MOBILE</extracomment>
        <translation>Поддержка режима «Код CAN разрешен» для чтения данных на месте</translation>
    </message>
    <message>
        <source>Allow test sample card usage</source>
        <extracomment>MOBILE</extracomment>
        <translation>Разрешить использование тестовой карты</translation>
    </message>
    <message>
        <source>Simulate a test sample card in authentications</source>
        <extracomment>MOBILE</extracomment>
        <translation>Моделировать тестовую карту в процессах аутентификации</translation>
    </message>
    <message>
        <source>Makes it difficult for outsiders to detect PIN entry</source>
        <extracomment>MOBILE</extracomment>
        <translation>Затрудняет посторонним распознавание PIN-кода при вводе</translation>
    </message>
    <message>
        <source>Use images instead of animations</source>
        <extracomment>MOBILE</extracomment>
        <translation>Использовать изображения вместо анимации</translation>
    </message>
    <message>
        <source>Accessibility</source>
        <extracomment>MOBILE</extracomment>
        <translation>Доступность</translation>
    </message>
    <message>
        <source>Hide key animations when entering PIN</source>
        <extracomment>MOBILE</extracomment>
        <translation>Скрыть анимацию клавиатуры при вводе PIN-кода</translation>
    </message>
    <message>
        <source>After identification, you will only be redirected back to the provider after confirmation. Otherwise, you will be redirected automatically after a few seconds.</source>
        <extracomment>MOBILE</extracomment>
        <translation>После идентификации вы будете перенаправлены обратно к провайдеру только после подтверждения. В иных случаях вы будете перенаправлены автоматически через несколько секунд.</translation>
    </message>
    <message>
        <source>Manual redirection back to the provider</source>
        <extracomment>MOBILE</extracomment>
        <translation>Перенаправление обратно к провайдеру вручную</translation>
    </message>
    <message>
        <source>Extend Transport PIN</source>
        <extracomment>MOBILE</extracomment>
        <translation>Удлените временный PIN-код</translation>
    </message>
    <message>
        <source>Disable</source>
        <extracomment>MOBILE</extracomment>
        <translation>Деактивировать</translation>
    </message>
    <message>
        <source>Later</source>
        <extracomment>MOBILE</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Restart</source>
        <extracomment>IOS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Restart now</source>
        <extracomment>ANDROID</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The font change applies only after restarting the application.</source>
        <extracomment>MOBILE</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Restart required</source>
        <extracomment>MOBILE</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Prevent screenshots</source>
        <extracomment>ANDROID</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Screenshots are prevented while your ID card data is displayed and you enter your PIN/CAN/PUK. If your screen is recorded, you will be notified of the potential collection of sensitive data.</source>
        <extracomment>ANDROID</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>In the event of a screenshot or video recording of the screen, you will be notified about the possible collection of sensitive data.</source>
        <extracomment>IOS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Detect screen recording</source>
        <extracomment>IOS</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>SetupDesktopUsage</name>
    <message>
        <source>Set up %1 on the PC/Mac</source>
        <extracomment>MOBILE</extracomment>
        <translation>Настройки %1 на компьютере/Mac</translation>
    </message>
    <message>
        <source>Install %1</source>
        <extracomment>MOBILE</extracomment>
        <translation>Установить %1</translation>
    </message>
    <message>
        <source>In order to install the %1, open the following website on your PC/Mac:</source>
        <extracomment>MOBILE %1 is replaced with the name &quot;AusweisApp&quot;</extracomment>
        <translation>Откройте следующий сайт на вашем компьютере/Mac, чтобы установить %1:</translation>
    </message>
    <message>
        <source>Open %1</source>
        <extracomment>MOBILE %1 is replaced with the name &quot;AusweisApp&quot;</extracomment>
        <translation>Открыть %1</translation>
    </message>
    <message>
        <source>Open the %1 on your PC/Mac.</source>
        <extracomment>MOBILE %1 is replaced with the name &quot;AusweisApp&quot;</extracomment>
        <translation>Откройте %1 в вашем компьютере/Mac.</translation>
    </message>
    <message>
        <source>Follow the instruction on your second device</source>
        <extracomment>MOBILE</extracomment>
        <translation>Следуйте инструкциям на втором устройстве</translation>
    </message>
    <message>
        <source>Now follow the instruction for the setup on your PC/Mac. If the setup does not start automatically, you may find it under %1Help%2.</source>
        <extracomment>MOBILE The text between %1 and %2 will be emphasized.</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Continue with pairing</source>
        <extracomment>MOBILE</extracomment>
        <translation>Продолжить сопряжение</translation>
    </message>
</context>
<context>
    <name>SimulatorWorkflow</name>
    <message>
        <source>Simulator</source>
        <extracomment>MOBILE</extracomment>
        <translation>Симулятор</translation>
    </message>
    <message>
        <source>Continue</source>
        <extracomment>MOBILE</extracomment>
        <translation>Продолжить</translation>
    </message>
    <message>
        <source>Switch to:</source>
        <extracomment>MOBILE</extracomment>
        <translation>Перейти к:</translation>
    </message>
    <message>
        <source>NFC</source>
        <extracomment>MOBILE</extracomment>
        <translation>NFC</translation>
    </message>
    <message>
        <source>WiFi</source>
        <extracomment>MOBILE</extracomment>
        <translation>Wi-Fi</translation>
    </message>
</context>
<context>
    <name>SkipOnboardingConfirmation</name>
    <message>
        <source>Only skip this step, if you already set up a 6-digit ID card PIN and a card reader.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Пропустите этот шаг, только если вы уже создали 6-значный PIN-код идентификационной карты и настроили устройство чтения карт.</translation>
    </message>
    <message>
        <source>Only skip this step, if you already used your ID card online and know your 6-digit ID card PIN.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Пропустите этот шаг, только если вы уже использовали вашу идентификационную карту в Интернете и помните ваш 6-значный PIN-код идентификационной карты.</translation>
    </message>
    <message>
        <source>If you want to use the eID function on your PC, install %1 there and follow the initial setup.</source>
        <extracomment>MOBILE %1 will be replaced with the name of the application</extracomment>
        <translation>Если вы хотите использовать функцию онлайн-идентификации на вашем компьютере, установите на нем %1 и выполните первоначальную настройку.</translation>
    </message>
</context>
<context>
    <name>StoreFeedbackPopup</name>
    <message>
        <source>Are you satisfied with %1?</source>
        <extracomment>ANDROID Header of the app rating popup.</extracomment>
        <translation>Вам нравится %1?</translation>
    </message>
    <message>
        <source>We would be very grateful if you could leave a rating on the Google Play Store!</source>
        <extracomment>ANDROID Content of the app rating popup.</extracomment>
        <translation>Мы будем благодарны, если вы оцените приложение в Google Play Store!</translation>
    </message>
    <message>
        <source>Do not ask again</source>
        <extracomment>ANDROID</extracomment>
        <translation>Не спрашивать снова</translation>
    </message>
    <message>
        <source>Rate app</source>
        <extracomment>ANDROID</extracomment>
        <translation>Оценить приложение</translation>
    </message>
</context>
<context>
    <name>SuggestionData</name>
    <message>
        <source>Continue</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Продолжить</translation>
    </message>
</context>
<context>
    <name>TabbedPane</name>
    <message>
        <source>Content of tab &quot;%1&quot;</source>
        <extracomment>DESKTOP %1 will be replaced with the title of the tab</extracomment>
        <translation>Содержание вкладки %1</translation>
    </message>
    <message>
        <source>Sidebar</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Боковая панель</translation>
    </message>
</context>
<context>
    <name>TabbedPaneDelegate</name>
    <message>
        <source>%1 of %2</source>
        <extracomment>DESKTOP %1 is the current selected Page of %2 Pages</extracomment>
        <translation>%1 из %2</translation>
    </message>
    <message>
        <source>Tab selected</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Вкладка выбрана</translation>
    </message>
    <message>
        <source>Tab</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Вкладка</translation>
    </message>
</context>
<context>
    <name>TabbedReaderView</name>
    <message>
        <source>Card readers</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Устройства чтения карт</translation>
    </message>
    <message>
        <source>Smartphone as card reader</source>
        <translation>Смартфон в качестве устройства чтения карт</translation>
    </message>
    <message>
        <source>USB card reader</source>
        <translation>USB-устройство чтения карт</translation>
    </message>
</context>
<context>
    <name>TechnologySwitch</name>
    <message>
        <source>Readout mode</source>
        <extracomment>MOBILE</extracomment>
        <translation>Режим считывания</translation>
    </message>
    <message>
        <source>by NFC</source>
        <extracomment>MOBILE</extracomment>
        <translation>через NFC</translation>
    </message>
    <message>
        <source>by smartphone as card reader</source>
        <extracomment>MOBILE</extracomment>
        <translation>через смартфон в качестве устройства чтения карт</translation>
    </message>
    <message>
        <source>by internal card simulator</source>
        <extracomment>MOBILE</extracomment>
        <translation>через внутренний симулятор карт</translation>
    </message>
</context>
<context>
    <name>TitleBar</name>
    <message>
        <source>Start page</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Начальная страница</translation>
    </message>
    <message>
        <source>The update can only be performed after the current operation has been completed.</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <source>Open settings view of %1</source>
        <translation>Открыть настройки %1</translation>
    </message>
    <message>
        <source>Notifications</source>
        <translation>Оповещения</translation>
    </message>
    <message>
        <source>Show in-app notifications of %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Показать внутренние оповещения в приложении %1</translation>
    </message>
    <message>
        <source>Hide in-app notifications of %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Скрыть внутренние оповещения в приложении %1</translation>
    </message>
    <message>
        <source>Update available (version %1)</source>
        <extracomment>DESKTOP %1 will be replaced with an application version number</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>TitleBarNavigation</name>
    <message>
        <source>Cancel</source>
        <extracomment>MOBILE</extracomment>
        <translation>Отмена</translation>
    </message>
    <message>
        <source>Back</source>
        <extracomment>MOBILE</extracomment>
        <translation>Назад</translation>
    </message>
    <message>
        <source>Close</source>
        <extracomment>MOBILE</extracomment>
        <translation>Закрыть</translation>
    </message>
</context>
<context>
    <name>TransactionInfoButton</name>
    <message>
        <source>Show more information about the transaction.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Information about the transaction.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Transactional information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Информация о транзакциях</translation>
    </message>
    <message>
        <source>The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Указанный выше провайдер не требует хранения каких-либо данных на вашей идентификационной карте, требуется только подтверждение наличия действующей идентификационной карты.</translation>
    </message>
</context>
<context>
    <name>TransactionInfoView</name>
    <message>
        <source>Transactional information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Информация о транзакциях</translation>
    </message>
</context>
<context>
    <name>TrayIconView</name>
    <message>
        <source>Yes, attach app to menu bar</source>
        <extracomment>MACOS Button to decide to attach app to the menu bar on macOS</extracomment>
        <translation>Да, прикрепить приложение к строке меню</translation>
    </message>
    <message>
        <source>Yes, attach app to tray</source>
        <extracomment>WINDOWS Button to decide to attach app to the tray on Windows</extracomment>
        <translation>Да, прикрепить приложение к панели задач</translation>
    </message>
    <message>
        <source>The %1 continues to run in the background after the application window is closed, so that it can be automatically opened on an authentication.</source>
        <extracomment>DESKTOP Information text why attaching the app to the tray/menu bar is advisable 1/3</extracomment>
        <translation>%1 продолжает работать в фоновом режиме после закрытия окна приложения, чтобы его можно было автоматически открыть после аутентификации.</translation>
    </message>
    <message>
        <source>An authentication cannot be carried out if the %1 is not running. Then the app has to be started manually before another authentication.</source>
        <extracomment>DESKTOP Information text why attaching the app to the tray/menu bar is advisable 2/3</extracomment>
        <translation>Аутентификация не может быть выполнена, если приложение %1 не запущено. В этом случае перед следующей аутентификацией необходимо запустить приложение вручную.</translation>
    </message>
    <message>
        <source>Attaching the app to the menu bar is therefore recommended.</source>
        <extracomment>MACOS Information text why attaching the app to the tray/menu bar is advisable 3/3</extracomment>
        <translation>Поэтому рекомендуется прикрепить приложение к строке меню.</translation>
    </message>
    <message>
        <source>Attaching the app to the tray is therefore recommended.</source>
        <extracomment>WINDOWS Information text why attaching the app to the tray/menu bar is advisable 3/3</extracomment>
        <translation>Поэтому рекомендуется прикрепить приложение к панели задач.</translation>
    </message>
    <message>
        <source>No, don&apos;t attach app to menu bar</source>
        <extracomment>MACOS Button to decide not to attach app to the menu bar on macOS</extracomment>
        <translation>Нет, не прикрепить приложение к строке меню</translation>
    </message>
    <message>
        <source>No, don&apos;t attach app to tray</source>
        <extracomment>WINDOWS Button to decide not to attach app to the tray on Windows</extracomment>
        <translation>Нет, не прикрепить приложение к панели задач</translation>
    </message>
    <message>
        <source>Do you want to attach the %1 to the menu bar?</source>
        <extracomment>MACOS Question if the App shall be attached to the menu bar on macOS</extracomment>
        <translation>Прикрепить %1 к строке меню?</translation>
    </message>
    <message>
        <source>Do you want to attach the %1 to the tray?</source>
        <extracomment>WINDOWS Question if the App shall be attached to the tray on windows</extracomment>
        <translation>Прикрепить %1 к панели задач?</translation>
    </message>
</context>
<context>
    <name>UpdateOptions</name>
    <message>
        <source>When you restart %1 and every 24 hours, it automatically checks for updates. Updates are not performed automatically. If this option is disabled, you have to manually check for updates in the settings.</source>
        <extracomment>DESKTOP %1 is replaced with the application name</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Automatically check for software updates at program start (recommended)</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Show update</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Показать обновление</translation>
    </message>
    <message>
        <source>Start manual search for software update</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Searching for software updates...</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Abort search</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>UpdateView</name>
    <message>
        <source>Application update</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Обновление приложения</translation>
    </message>
    <message>
        <source>An update for the outdated installed version (%1) is available for download.</source>
        <extracomment>DESKTOP %1 is replaced with the current version number</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Update information is being checked...</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Update failed</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Application update could not be performed</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished">Назад к начальной странице</translation>
    </message>
    <message>
        <source>Retry</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Повторить попытку</translation>
    </message>
    <message>
        <source>Show update</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished">Показать обновление</translation>
    </message>
    <message>
        <source>A newer update (version %1) has been found and is available for download.</source>
        <extracomment>DESKTOP %1 will be replaced with the update version number</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Newer version %1 available</source>
        <extracomment>DESKTOP %1 will be replaced with the update version number</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Warning - Your operating system is no longer supported</source>
        <extracomment>DESKTOP Header of the popup that is shown when the requested update is not compatible with the OS.</extracomment>
        <translation>Предупреждение: ваша операционная система больше не поддерживается</translation>
    </message>
    <message>
        <source>Troubleshooting and user support for the %1 is no longer available for your current operating system. Version %2 has not been tested with your current operating system, use is at your own risk.</source>
        <extracomment>DESKTOP Text of the popup that is shown when the requested update is not compatible with the OS.</extracomment>
        <translation>Для используемой вами операционной системы более не доступны устранение неисправностей и поддержка пользователей для %1. Версия %2 не тестировалась на текущей операционной системе, ответственность за ее использование несете вы.</translation>
    </message>
    <message>
        <source>Update anyway</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Все равно обновить</translation>
    </message>
    <message>
        <source>Warning - Update failed</source>
        <extracomment>DESKTOP Header of the popup that is shown when the app download failed.</extracomment>
        <translation>Предупреждение: сбой обновления</translation>
    </message>
    <message>
        <source>Update available</source>
        <extracomment>DESKTOP</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>UpdateViewButtonRow</name>
    <message>
        <source>Cancel update</source>
        <extracomment>DESKTOP Cancel the download of the update on Windows</extracomment>
        <translation>Отмена обновления</translation>
    </message>
    <message>
        <source>Start update</source>
        <extracomment>DESKTOP Start to download the update and execute it on Windows</extracomment>
        <translation>Запустить обновление</translation>
    </message>
    <message>
        <source>Download progress</source>
        <extracomment>DESKTOP Name of an progress indicator during a download read by screen readers</extracomment>
        <translation>Ход скачивания</translation>
    </message>
    <message>
        <source>The update (version %1) is being performed...</source>
        <extracomment>DESKTOP %1 is replaced with the version number of the software update.</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>UpdateViewInformation</name>
    <message>
        <source>New version</source>
        <extracomment>DESKTOP Information about the available, new version number.</extracomment>
        <translation>Новая версия</translation>
    </message>
    <message>
        <source>Release date</source>
        <extracomment>DESKTOP Date when the available update was released.</extracomment>
        <translation>Дата выхода</translation>
    </message>
    <message>
        <source>Download size</source>
        <extracomment>DESKTOP Download size of the available update in megabyte.</extracomment>
        <translation>Размер обновления</translation>
    </message>
    <message>
        <source>Download link</source>
        <extracomment>DESKTOP Plaintext link to the update download.</extracomment>
        <translation>Ссылка для загрузки</translation>
    </message>
    <message>
        <source>Checksum link</source>
        <extracomment>DESKTOP Link to download checksum to verify the downloaded update file.</extracomment>
        <translation>Ссылка для контрольной суммы</translation>
    </message>
</context>
<context>
    <name>Utils</name>
    <message>
        <source>Tap to open the following website in your browser: %1</source>
        <extracomment>MOBILE Hint that a link is present, which will open in the browser</extracomment>
        <translation>Нажмите, чтобы открыть следующий веб-сайт в вашем браузере: %1</translation>
    </message>
    <message>
        <source>Press space to open the following website in your browser: %1</source>
        <extracomment>DESKTOP Hint that a link is present, which will open in the browser</extracomment>
        <translation>Нажмите на &quot;пробел&quot;, чтобы открыть следующий веб-сайт в браузере: : %1</translation>
    </message>
</context>
<context>
    <name>VersionInformation</name>
    <message>
        <source>Developer options activated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Опции разработчика активированы.</translation>
    </message>
    <message>
        <source>Developer options deactivated.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Опции разработчика деактивированы.</translation>
    </message>
    <message>
        <source>%1 version</source>
        <extracomment>MOBILE %1 is replaced with the application name</extracomment>
        <translation type="unfinished">Информация о версии</translation>
    </message>
    <message>
        <source>Advanced settings activated.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Расширенные настройки активированы.</translation>
    </message>
    <message>
        <source>Advanced settings deactivated.</source>
        <extracomment>MOBILE</extracomment>
        <translation>Расширенные настройки деактивированы.</translation>
    </message>
    <message>
        <source>Open website</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Открыть сайт</translation>
    </message>
    <message>
        <source>Privacy statement</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Положение о конфиденциальности</translation>
    </message>
    <message>
        <source>Accessibility statement</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Заявление о доступности</translation>
    </message>
    <message>
        <source>%1 more taps to toggle the advanced settings.</source>
        <extracomment>MOBILE Used in notifications when the user taps the version information</extracomment>
        <translation>Еще %1 нажатия, чтобы перейти в расширенные настройки.</translation>
    </message>
</context>
<context>
    <name>WhiteListSurveyView</name>
    <message>
        <source>Feedback</source>
        <extracomment>MOBILE</extracomment>
        <translation>Обратная связь</translation>
    </message>
    <message>
        <source>Send device data?</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Header</extracomment>
        <translation>Отправить данные устройства?</translation>
    </message>
    <message>
        <source>Would you like to help us to improve the %1?</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text</extracomment>
        <translation>Хотите помочь нам сделать %1 лучше?</translation>
    </message>
    <message>
        <source>Supplying your device characteristics helps us to gather reliable information about the compatibility of your device.</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text</extracomment>
        <translation>Предоставление параметров вашего устройства помогает нам собирать достоверные данные о совместимости устройств.</translation>
    </message>
    <message>
        <source>The transmission is anonymous. No personal data is collected or transmitted!</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Part of content text</extracomment>
        <translation>Передача анонимная. Персональные данные не собираются и не передаются!</translation>
    </message>
    <message>
        <source>Collected data</source>
        <translation>Собранные данные</translation>
    </message>
    <message>
        <source>Thank you for your assistance!</source>
        <extracomment>MOBILE Request to the user if the device information should be shared for statistics (Whitelist) - Thank you message</extracomment>
        <translation>Благодарим за помощь!</translation>
    </message>
    <message>
        <source>Do not send</source>
        <extracomment>MOBILE</extracomment>
        <translation>Не отправлять</translation>
    </message>
    <message>
        <source>Send</source>
        <extracomment>MOBILE</extracomment>
        <translation>Отправить</translation>
    </message>
</context>
<context>
    <name>governikus::AccessRoleAndRightsUtil</name>
    <message>
        <source>Optional data</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Дополнительные данные</translation>
    </message>
    <message>
        <source>Residence permit II</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Разрешение на временное проживание II</translation>
    </message>
    <message>
        <source>Residence permit I</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Разрешение на временное проживание I</translation>
    </message>
    <message>
        <source>Community-ID</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Идентификатор места жительства</translation>
    </message>
    <message>
        <source>Address</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Адрес</translation>
    </message>
    <message>
        <source>RFU</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>РЧУ</translation>
    </message>
    <message>
        <source>Birth name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Фамилия до брака</translation>
    </message>
    <message>
        <source>Gender</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Пол</translation>
    </message>
    <message>
        <source>Nationality</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Национальность</translation>
    </message>
    <message>
        <source>Place of birth</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Место рождения</translation>
    </message>
    <message>
        <source>Date of birth</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Дата рождения</translation>
    </message>
    <message>
        <source>Doctoral degree</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Докторская степень</translation>
    </message>
    <message>
        <source>Religious / artistic name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Религиозное имя/псевдоним</translation>
    </message>
    <message>
        <source>Family name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Фамилия</translation>
    </message>
    <message>
        <source>Given name(s)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Имя/имена</translation>
    </message>
    <message>
        <source>Valid until</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Действителен до</translation>
    </message>
    <message>
        <source>Issuing country</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Страна выдачи</translation>
    </message>
    <message>
        <source>Document type</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Тип документа</translation>
    </message>
    <message>
        <source>Installation of qualified signature certificates</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Установка квалифицированных сертификатов подписи</translation>
    </message>
    <message>
        <source>Installation of signature certificates</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Установка сертификатов подписи</translation>
    </message>
    <message>
        <source>PIN Management</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Управление PIN-кодами</translation>
    </message>
    <message>
        <source>CAN allowed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Код CAN разрешен</translation>
    </message>
    <message>
        <source>Privileged terminal</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Привилегированное оконечное устройство</translation>
    </message>
    <message>
        <source>Pseudonym</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Псевдоним</translation>
    </message>
    <message>
        <source>Address verification</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Подтверждение адреса</translation>
    </message>
    <message>
        <source>Age verification</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Подтверждение возраста</translation>
    </message>
    <message>
        <source>Unknown (reserved)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неизвестно (зарезервировано)</translation>
    </message>
    <message>
        <source>Unknown</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неизвестно</translation>
    </message>
</context>
<context>
    <name>governikus::AppUpdateDataModel</name>
    <message>
        <source>The received data is broken. Check your network connection and try to restart the update.</source>
        <extracomment>DESKTOP Text of the popup that is shown when the app download failed because of a wrong checksum.</extracomment>
        <translation>Полученные данные повреждены. Проверьте сетевое подключение и попытайтесь перезапустить обновление.</translation>
    </message>
    <message>
        <source>The update could not be started automatically after a successful download. Please try to do a manual update. You can find the downloaded file %1here%2.</source>
        <extracomment>DESKTOP Text of the popup that is shown when the execution of the update failed (1/2).</extracomment>
        <translation>Не удалось автоматически запустить обновление после успешной загрузки. Попытайтесь выполнить обновление вручную. Загруженный файл расположен %1здесь%2.</translation>
    </message>
    <message>
        <source>An unknown network error occurred. Check your network connection and try to restart the update.</source>
        <extracomment>DESKTOP Generic text of the popup that is shown when the app download failed.</extracomment>
        <translation>Неизвестная ошибка сети. Проверьте сетевое подключение и попытайтесь перезапустить обновление.</translation>
    </message>
    <message>
        <source>If this does not help, contact our %1support%2.</source>
        <extracomment>DESKTOP Text of the popup that is shown when the execution of the update failed (2/2).</extracomment>
        <translation>Если это не помогает, свяжитесь со %1службой поддержки%2.</translation>
    </message>
    <message>
        <source>An update is available (version %1).</source>
        <extracomment>DESKTOP %1 is replaced with the version number of the software update</extracomment>
        <translation>Доступно обновление (версия %1).</translation>
    </message>
    <message>
        <source>hh:mm AP</source>
        <extracomment>DESKTOP Time format according to https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Last search on %1 at %2</source>
        <extracomment>DESKTOP %1 will be replaced with the date and %2 with the time of the last search.</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Your version %1 of %2 is up to date.</source>
        <extracomment>DESKTOP %1 is replaced with the version number of the software and %2 is replaced with the application name.</extracomment>
        <translation>Ваша версия %2 %1 является актуальной.</translation>
    </message>
    <message>
        <source>An update information for your platform is not available.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация об обновлении для вашей платформы недоступна.</translation>
    </message>
    <message>
        <source>An update is available but retrieving the information failed.</source>
        <extracomment>DESKTOP The updater found an update but not all required update information are valid, this should be a very rare case.</extracomment>
        <translation>Обновление доступно, но произошел сбой при загрузке информации.</translation>
    </message>
    <message>
        <source>The update information could not be retrieved. Please check your network connection.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Не удалось загрузить информацию об обновлении. Проверьте сетевое подключение.</translation>
    </message>
    <message>
        <source>dd.MM.yyyy</source>
        <extracomment>DESKTOP Date format according to https://doc.qt.io/qt/qdate.html#toString</extracomment>
        <translation>dd.MM.yyyy</translation>
    </message>
</context>
<context>
    <name>governikus::ApplicationModel</name>
    <message>
        <source>Could not load license text.</source>
        <translation>Не удалось загрузить текст лицензии.</translation>
    </message>
    <message>
        <source>Error while saving config to &quot;%1&quot;</source>
        <translation>Ошибка при сохранении конфигурации в указанном месте: «%1»</translation>
    </message>
</context>
<context>
    <name>governikus::AuthModel</name>
    <message>
        <source>Reason:</source>
        <extracomment>ALL_PLATFORMS Failure code (string) of current workflow error.</extracomment>
        <translation>Причина:</translation>
    </message>
    <message>
        <source>Redirect failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой переадресации</translation>
    </message>
    <message>
        <source>Authentication canceled</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Authentication failed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой аутентификации</translation>
    </message>
    <message>
        <source>Authentication successful</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Аутентификация выполнена успешно</translation>
    </message>
    <message>
        <source>Back to provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вернуться к провайдеру</translation>
    </message>
    <message>
        <source>Back to start page</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Назад к начальной странице</translation>
    </message>
    <message>
        <source>Connection to ID card lost</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Прервано соединение с идентификационной картой</translation>
    </message>
    <message>
        <source>Provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished">Провайдер</translation>
    </message>
</context>
<context>
    <name>governikus::CardInfo</name>
    <message>
        <source>not inserted</source>
        <comment>Karte</comment>
        <extracomment>ALL_PLATFORMS No card is present/inserted. The text is only used in DiagnosisView.</extracomment>
        <translation>не вставлена</translation>
    </message>
    <message>
        <source>unknown type</source>
        <comment>Karte</comment>
        <extracomment>ALL_PLATFORMS An unknown card is present/inserted. The text is only used in DiagnosisView.</extracomment>
        <translation>неизвестный тип</translation>
    </message>
    <message>
        <source>ID card (PA/eAT/eID)</source>
        <extracomment>ALL_PLATFORMS An ID card is present/inserted. The text is only used in DiagnosisView.</extracomment>
        <translation>Идентификационная карта (PA/eAT/eID)</translation>
    </message>
</context>
<context>
    <name>governikus::CertificateDescriptionModel</name>
    <message>
        <source>Provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Провайдер</translation>
    </message>
    <message>
        <source>Certificate issuer</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Издатель сертификата</translation>
    </message>
    <message>
        <source>Name, address and mail address of the provider</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Имя, адрес и почтовый адрес провайдера</translation>
    </message>
    <message>
        <source>Purpose</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Цель</translation>
    </message>
    <message>
        <source>Indication of the bodies responsible for the provider, that verify the compliance with data security regulations</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Указание органов по защите данных, ответственных за провайдера</translation>
    </message>
    <message>
        <source>Provider Information</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Информация о провайдере</translation>
    </message>
    <message>
        <source>Validity</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Действие</translation>
    </message>
</context>
<context>
    <name>governikus::DiagnosisController</name>
    <message>
        <source>unknown</source>
        <translation>неизвестно</translation>
    </message>
    <message>
        <source>not available</source>
        <translation>недоступно</translation>
    </message>
</context>
<context>
    <name>governikus::DiagnosisModel</name>
    <message>
        <source>Time of diagnosis</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Время диагностики</translation>
    </message>
    <message>
        <source>Initial diagnosis running, please wait.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Выполняется первичная диагностика, ждите.</translation>
    </message>
    <message>
        <source>PC/SC driver information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация о драйвере ПК/SC</translation>
    </message>
    <message>
        <source>Diagnosis is running...</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Выполняется диагностика...</translation>
    </message>
    <message>
        <source>Card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Устройство чтения карт</translation>
    </message>
    <message>
        <source>Paired smartphones</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сопряженные смартфоны</translation>
    </message>
    <message>
        <source>Antivirus information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация об антивирусной программе</translation>
    </message>
    <message>
        <source>No Antivirus information available on this platform.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация об антивирусной программе на этой платформе недоступна.</translation>
    </message>
    <message>
        <source>Firewall information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация о брандмауэре</translation>
    </message>
    <message>
        <source>No Firewall information available on this platform.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация о брандмауэре на этой платформе недоступна.</translation>
    </message>
    <message>
        <source>Yes</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Да</translation>
    </message>
    <message>
        <source>No</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Нет</translation>
    </message>
    <message>
        <source>Failed to retrieve date &amp; time</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Не удалось получить дату и время</translation>
    </message>
    <message>
        <source>d. MMMM yyyy, hh:mm:ss AP</source>
        <extracomment>DESKTOP Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>d MMMM yyyy г., hh:mm:ss</translation>
    </message>
    <message>
        <source>&lt;Not set&gt;</source>
        <extracomment>DESKTOP Interface has no hardware address set</extracomment>
        <translation>&lt;Не установлено&gt;</translation>
    </message>
    <message>
        <source>Hardware address: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Адрес аппаратного обеспечения: %1</translation>
    </message>
    <message>
        <source>No IP addresses assigned</source>
        <extracomment>DESKTOP Interface has no IP addresses assigned</extracomment>
        <translation>Нет присвоенных IP-адресов</translation>
    </message>
    <message>
        <source>IPv4 address: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Адрес IPv4: %1</translation>
    </message>
    <message>
        <source>IPv6 address: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Адрес IPv6: %1</translation>
    </message>
    <message>
        <source>Unknown address: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Неизвестный адрес: %1</translation>
    </message>
    <message>
        <source>Hostname: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Имя хоста: %1</translation>
    </message>
    <message>
        <source>Port: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Порт: %1</translation>
    </message>
    <message>
        <source>Type: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Тип: %1</translation>
    </message>
    <message>
        <source>Capabilities: %1</source>
        <extracomment>DESKTOP list of the capabitlities of the proxy connection</extracomment>
        <translation>Функциональные возможности: %1</translation>
    </message>
    <message>
        <source>Ping test to proxy: Successful</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Пинг-тест прокси-сервера: успешно</translation>
    </message>
    <message>
        <source>Ping test to proxy: Failed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Пинг-тест прокси-сервера: сбой</translation>
    </message>
    <message>
        <source>Connection test with proxy: Successful</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Проверка связи с прокси-сервером: успешно</translation>
    </message>
    <message>
        <source>Connection test with proxy: Failed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Проверка связи с прокси-сервером: сбой</translation>
    </message>
    <message>
        <source>No proxy found</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Прокси-сервер не найден</translation>
    </message>
    <message>
        <source>Connection test without proxy: Successful</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Проверка связи без прокси-сервера: успешно</translation>
    </message>
    <message>
        <source>Connection test without proxy: Failed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Проверка связи без прокси-сервера: сбой</translation>
    </message>
    <message>
        <source>Proxy information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация о прокси-сервере</translation>
    </message>
    <message>
        <source>No Antivirus software detected.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Антивирусное программное обеспечение не обнаружено.</translation>
    </message>
    <message>
        <source>Last updated: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Последнее обновление: %1</translation>
    </message>
    <message>
        <source>Executable path: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Путь к исполняемому файлу: %1</translation>
    </message>
    <message>
        <source>Antivirus detection failed.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сбой обнаружения антивируса.</translation>
    </message>
    <message>
        <source>No 3rd party firewalls detected</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Брандмауэры третьих сторон не обнаружены</translation>
    </message>
    <message>
        <source>Enabled: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Активировано: %1</translation>
    </message>
    <message>
        <source>Up to date: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Актуальное состояние: %1</translation>
    </message>
    <message>
        <source>Firewalls from 3rd party vendors</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Брандмауэры сторонних поставщиков</translation>
    </message>
    <message>
        <source>Outgoing %1 rule</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Исходящее правило %1</translation>
    </message>
    <message>
        <source>Exists: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Существует: %1</translation>
    </message>
    <message>
        <source>Incoming %1 rule</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Входящее правило %1</translation>
    </message>
    <message>
        <source>Windows firewall rules</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Правила брандмауэра Windows</translation>
    </message>
    <message>
        <source>Windows firewall profiles</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Профили брандмауэра Windows</translation>
    </message>
    <message>
        <source>An error occurred while trying to gather firewall information. Please check the log for more information.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Произошла ошибка при сборе информации о брандмауэре. Проверьте файл журнала для получения подробной информации.</translation>
    </message>
    <message>
        <source>PC/SC information</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация о ПК/SC</translation>
    </message>
    <message>
        <source>Version</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Версия</translation>
    </message>
    <message>
        <source>Vendor: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Производитель: %1</translation>
    </message>
    <message>
        <source>Version: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Версия: %1</translation>
    </message>
    <message>
        <source>File path: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Путь к файлу: %1</translation>
    </message>
    <message>
        <source>Components</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Компоненты</translation>
    </message>
    <message>
        <source>Driver</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Драйвер</translation>
    </message>
    <message>
        <source>No devices paired.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Нет сопряженных устройств.</translation>
    </message>
    <message>
        <source>dd.MM.yyyy, hh:mm:ss</source>
        <extracomment>DESKTOP Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>dd.MM.yyyy hh:mm:ss</translation>
    </message>
    <message>
        <source>Last connection: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Последнее соединение: %1</translation>
    </message>
    <message>
        <source>No information found for this certificate.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Информация для данного сертификата не найдена.</translation>
    </message>
    <message>
        <source>Connected Card readers</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Подключенные устройства чтения карт</translation>
    </message>
    <message>
        <source>No supported reader found.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Поддерживаемые устройства чтения карт не найдены.</translation>
    </message>
    <message>
        <source>Basic card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Базовое устройство чтения карт</translation>
    </message>
    <message>
        <source>Standard / comfort card reader</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Стандартное/комфортное устройство чтения карт</translation>
    </message>
    <message>
        <source>Card: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Карта: %1</translation>
    </message>
    <message>
        <source>Retry counter: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Осталось попыток: %1</translation>
    </message>
    <message>
        <source>No driver installed</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Драйверы не установлены</translation>
    </message>
    <message>
        <source>Network</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Сеть</translation>
    </message>
    <message>
        <source>Antivirus and firewall</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Антивирус и брандмауэр</translation>
    </message>
    <message>
        <source>Service addresses</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Служебные адреса</translation>
    </message>
    <message>
        <source>Not bound</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Не привязано</translation>
    </message>
    <message>
        <source>Encountered error: %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Обнаруженная ошибка: %1</translation>
    </message>
</context>
<context>
    <name>governikus::ECardApiResult</name>
    <message>
        <source>An unexpected error has occurred during processing.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>При обработке произошла непредвиденная ошибка.</translation>
    </message>
    <message>
        <source>Use of the function by the client application is not permitted.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Использование функции запрещено приложением клиента.</translation>
    </message>
    <message>
        <source>An internal error has occurred during processing.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>При обработке произошла внутренняя ошибка.</translation>
    </message>
    <message>
        <source>There was some problem with a provided or omitted parameter.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Проблема с предоставленным или пропущенным параметром.</translation>
    </message>
    <message>
        <source>The API function is unknown.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неизвестная функция интерфейса API.</translation>
    </message>
    <message>
        <source>The framework or layer has not been initialized.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Инфраструктура или слой не инициализированы.</translation>
    </message>
    <message>
        <source>The active session has been terminated.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Активный сеанс завершен.</translation>
    </message>
    <message>
        <source>A Communication error occurred during processing.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>При обработке произошла ошибка связи.</translation>
    </message>
    <message>
        <source>The operation was terminated as the set time was exceeded.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Процесс прерван из-за превышения установленного времени.</translation>
    </message>
    <message>
        <source>The operation was aborted as an invalid channel handle was used.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Процесс прерван из-за недействительного дескриптора канала.</translation>
    </message>
    <message>
        <source>A trusted channel could not be opened.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не удалось открыть доверенный канал.</translation>
    </message>
    <message>
        <source>The operation was aborted as an unknown protocol was used.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Процесс прерван из-за неизвестного протокола.</translation>
    </message>
    <message>
        <source>The operation was aborted as an unknown cipher suite was used.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Процесс прерван из-за неизвестного алгоритма шифрования.</translation>
    </message>
    <message>
        <source>The operation was aborted as an unknown web service binding was used.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Процесс прерван из-за неизвестной привязки веб-сервиса.</translation>
    </message>
    <message>
        <source>The card is missing or was removed.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Карта отсутствует или удалена.</translation>
    </message>
    <message>
        <source>The new PIN and the confirmation do not match.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Новый PIN-код и его подтверждение не совпадают.</translation>
    </message>
    <message>
        <source>The format of the PIN is wrong.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Неправильный формат PIN-кода.</translation>
    </message>
    <message>
        <source>Signature certificate key generation is not possible.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Невозможно создать ключ для сертификата подписи.</translation>
    </message>
    <message>
        <source>The process has been cancelled.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Процесс прерван.</translation>
    </message>
    <message>
        <source>One or more certificate checks failed. The operation will be aborted due to security reasons.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой при одной или нескольких проверках сертификата. Процесс будет прерван в целях безопасности.</translation>
    </message>
    <message>
        <source>This action cannot be performed. The eID function of your ID card is not activated.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Невозможно выполнить данное действие. Функция онлайн-идентификации вашей идентификационной карты не активирована.</translation>
    </message>
    <message>
        <source>The authenticity of your ID card could not be verified. Please make sure that you are using a genuine ID card. Please note that test applications require the use of a test ID card.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не удалось подтвердить подлинность идентификационной карты. Убедитесь в том, что используете подлинную идентификационную карту. Внимание: для тестовых приложений необходимо использовать тестовую идентификационную карту.</translation>
    </message>
    <message>
        <source>The age verification failed.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой подтверждения возраста.</translation>
    </message>
    <message>
        <source>The community verification failed.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сбой подтверждения места жительства.</translation>
    </message>
    <message>
        <source>The ID card is invalid or disabled.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Идентификационная карта недействительна или деактивирована.</translation>
    </message>
</context>
<context>
    <name>governikus::GlobalStatus</name>
    <message>
        <source>A secure connection to the provider could not be established. Check the network connection and try again later. If this doesn&apos;t help, contact our %1support%2.</source>
        <extracomment>ALL_PLATFORMS Technical error description. Generic message with link to support section of the homepage. %1: HTML hyperlink start tag %2: HTML hyperlink end tag</extracomment>
        <translation>Не удалось установить безопасное соединение с провайдером. Проверьте сетевое подключение и повторите попытку. Если это не поможет, свяжитесь с нашей %1службой поддержки%2.</translation>
    </message>
    <message>
        <source>No error occurred.</source>
        <extracomment>ALL_PLATFORMS No actual error occurred, required to provide a message for status code No_Error.</extracomment>
        <translation>Ошибок нет.</translation>
    </message>
    <message>
        <source>An unexpected error has occurred during processing.</source>
        <extracomment>ALL_PLATFORMS An unknown error from any of the subcomponents (PCSC, EcardAPI, QtNetwork, ...) occurred.</extracomment>
        <translation>При обработке произошла непредвиденная ошибка.</translation>
    </message>
    <message>
        <source>Cannot start authentication. An operation is already active.</source>
        <extracomment>ALL_PLATFORMS An ActivationHandler is requested even though an operation is currently still running.</extracomment>
        <translation>Невозможно запустить аутентификацию. Уже активен другой процесс.</translation>
    </message>
    <message>
        <source>Restart the authentication process and make sure that the position of the ID card does not change during the reading process.</source>
        <extracomment>ALL_PLATFORMS The card was removed after the PACE channel was established.</extracomment>
        <translation>Перезапустите процесс аутентификации и убедитесь, что положение идентификационной карты не менялось во время считывания.</translation>
    </message>
    <message>
        <source>The authenticity of your ID card could not be confirmed.</source>
        <extracomment>ALL_PLATFORMS The certificates supplied by the card did not pass the authenticity check, further operation is not allowed.</extracomment>
        <translation>Не удалось подтвердить подлинность идентификационной карты.</translation>
    </message>
    <message>
        <source>The program received an unknown message from the server.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Программа получила неизвестное сообщение с сервера.</translation>
    </message>
    <message>
        <source>The program received an unexpected message from the server.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Программа получила неожиданное сообщение с сервера.</translation>
    </message>
    <message>
        <source>Using the developer mode is only allowed in a test environment.</source>
        <extracomment>ALL_PLATFORMS The developer mode is enabled but a productive environment was detected.</extracomment>
        <translation>Режим разработчика можно использовать только в тестовой среде.</translation>
    </message>
    <message>
        <source>Pre-verification failed.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Сбой предварительного подтверждения.</translation>
    </message>
    <message>
        <source>No unique AT CVC</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Нет уникального AT CVC</translation>
    </message>
    <message>
        <source>No unique DV CVC</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Нет уникального DV CVC</translation>
    </message>
    <message>
        <source>Authentication failed.</source>
        <extracomment>ALL_PLATFORMS DidAuthenticateEAC2, AA or the ID card declined the certificates.</extracomment>
        <translation>Сбой аутентификации.</translation>
    </message>
    <message>
        <source>The length of the data sent to the ID card was not accepted. Either the data is faulty or your card reader does not support Extended Length communication and cannot be used to read the ID card. Unfortunately, the %1 has no influence on this restriction.</source>
        <extracomment>ALL_PLATFORMS A card command failed because the data length was wrong or the card reader does not support Extended Length.</extracomment>
        <translation>Размер данных, отправленных на идентификационную карту, не был принят. Либо данные неверны, либо ваше устройство чтения карт не поддерживает связь по протоколу с расширенной длиной и не может использоваться для считывания идентификационной карты. К сожалению, %1 не влияет на это ограничение.</translation>
    </message>
    <message>
        <source>No certificate description available.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Нет доступных описаний сертификата.</translation>
    </message>
    <message>
        <source>No subject url available in certificate description.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Нет доступных URL темы в описании сертификата.</translation>
    </message>
    <message>
        <source>The certificate description does not match the certificate.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Описание сертификата не совпадает с сертификатом.</translation>
    </message>
    <message>
        <source>The subject URL in the certificate description and the TCToken URL do not satisfy the same origin policy.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>URL темы в описании сертификата и URL TCToken не отвечают требованиям единой исходной политики.</translation>
    </message>
    <message>
        <source>The program received an error from the server.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Программа получила ошибку с сервера.</translation>
    </message>
    <message>
        <source>Hash of TLS certificate not in certificate description (issuer: %1). This indicates a misconfiguration or manipulation of the certificate. Please check that your antivirus software and firewall are not interfering with TLS traffic.</source>
        <extracomment>ALL_PLATFORMS The TLS certificate was not folded with the Authorization Certificate, thus violating the security requirements. Might also be caused by a firewall and/or the antivirus software.</extracomment>
        <translation>В описании сертификата отсутствует хэш сертификата TLS (издатель: %1). Это указывает на неправильную конфигурацию или манипуляции с сертификатом. Убедитесь в том, что антивирусное программное обеспечение и брандмауэр не препятствуют трафику TLS.</translation>
    </message>
    <message>
        <source>The TCToken contained no data.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>В TCToken нет данных.</translation>
    </message>
    <message>
        <source>The service is temporarily not available. Please try again later.</source>
        <extracomment>ALL_PLATFORMS A server has responded with an HTTP error code 503.</extracomment>
        <translation>Служба временно недоступна. Повторите попытку позднее.</translation>
    </message>
    <message>
        <source>Establishing a connection is taking too long.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Соединение устанавливается слишком долго.</translation>
    </message>
    <message>
        <source>Establishing a connection via the proxy did not succeed.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Не удалось установить соединение через прокси-сервер.</translation>
    </message>
    <message>
        <source>It wasn&apos;t possible to connect to the server: the server sent a non-standard response.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Не удалось подключиться к серверу: сервер отправил нестандартный ответ.</translation>
    </message>
    <message>
        <source>It wasn&apos;t possible to connect to the server: a secure connection could not be established.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Не удалось подключиться к серверу: не удалось установить безопасное соединение.</translation>
    </message>
    <message>
        <source>Application was invoked with wrong parameters.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Приложение вызвано с неправильными параметрами.</translation>
    </message>
    <message>
        <source>An unknown network error occurred. Check your network connection and try to restart the app.</source>
        <extracomment>ALL_PLATFORMS Other critical network error by Qt.</extracomment>
        <translation>Неизвестная ошибка сети. Проверьте сетевое подключение и попытайтесь перезапустить приложение.</translation>
    </message>
    <message>
        <source>The selected card reader cannot be accessed anymore.</source>
        <extracomment>ALL_PLATFORMS The card reader was removed after the PACE channel was established.</extracomment>
        <translation>К выбранному устройство чтения карт больше нельзя получить доступ.</translation>
    </message>
    <message>
        <source>The server provided no or incomplete information. Your personal data could not be read out.</source>
        <extracomment>ALL_PLATFORMS The result of the self authentication could not be retrieved from the eService.</extracomment>
        <translation>Сервер не поставляет данные или поставляет их не полностью. Не удалось считать ваши персональные данные.</translation>
    </message>
    <message>
        <source>Error while connecting to the provider. The TLS connection uses an unsupported key algorithm or length.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Ошибка при соединении с провайдером. TLS-соединение использует неподдерживаемый алгоритм шифрования или длину.</translation>
    </message>
    <message>
        <source>Error while connecting to the server. The TLS certificate uses an unsupported key algorithm or length. Certificate issuer: %1</source>
        <extracomment>ALL_PLATFORMS Received a TLS certificate that uses an invalid algorithm or key length.</extracomment>
        <translation>Ошибка при соединении с сервером. TLS-сертификат использует неподдерживаемые алгоритм ключа или длину. Издатель сертификата: %1</translation>
    </message>
    <message>
        <source>Empty redirect URL</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Пустой URL-адрес перенаправления</translation>
    </message>
    <message>
        <source>Expected redirect, got %1</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Ожидаемое перенаправление, получено %1</translation>
    </message>
    <message>
        <source>Invalid scheme: %1</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Недействительная схема: %1</translation>
    </message>
    <message>
        <source>Malformed redirect URL: %1</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Искаженный URL-адрес перенаправления: %1</translation>
    </message>
    <message>
        <source>You have canceled the process.</source>
        <extracomment>ALL_PLATFORMS The user cancelled the authentication in the UI.</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>The process has been aborted because of an internal error.</source>
        <extracomment>ALL_PLATFORMS An internal error occurred before fetching TcToken.</extracomment>
        <translation>Процесс прерван из-за внутренней ошибки.</translation>
    </message>
    <message>
        <source>The process has been cancelled by the card reader.</source>
        <extracomment>ALL_PLATFORMS The user cancelled the authentication on his card reader.</extracomment>
        <translation>Процесс прерван устройством чтения карт.</translation>
    </message>
    <message>
        <source>The maximum time was exceeded during input process.</source>
        <extracomment>ALL_PLATFORMS The card reader signalled that it timed out.</extracomment>
        <translation>Превышено максимальное время ввода.</translation>
    </message>
    <message>
        <source>Card does not exist</source>
        <extracomment>ALL_PLATFORMS Internal error, either PCSC, SaK or card reader could not find the ID card.</extracomment>
        <translation>Карта не существует</translation>
    </message>
    <message>
        <source>The eID function of your ID card is not yet activated.</source>
        <extracomment>ALL_PLATFORMS The eID function of the ID card is not active.</extracomment>
        <translation>Функция онлайн-идентификации вашей идентификационной карты еще не активирована.</translation>
    </message>
    <message>
        <source>An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.</source>
        <extracomment>ALL_PLATFORMS Communication with the card failed due to the specification of the TR (Technische Richtlinie),</extracomment>
        <translation>Произошла ошибка при обмене данными с идентификационной картой. Убедитесь в том, что идентификационная карта правильно расположена на устройстве чтения карт, и повторите попытку.</translation>
    </message>
    <message>
        <source>A protocol error occurred. Please make sure that your ID card is placed correctly on the card reader and try again. If the problem occurs again, please contact our support at %1.</source>
        <extracomment>ALL_PLATFORMS Communication with the card failed due to the specification of the TR (Technische Richtlinie). The protocol was faulty or invalid values were requested/received. %1 is a html link to the support.</extracomment>
        <translation>Произошла ошибка протокола. Убедитесь в том, что идентификационная карта правильно расположена на устройстве чтения карт, и повторите попытку. Если проблема сохраняется, свяжитесь со службой поддержки %1.</translation>
    </message>
    <message>
        <source>The given PIN is not correct.</source>
        <extracomment>ALL_PLATFORMS The ID card declined the PIN.</extracomment>
        <translation>Указан неправильный PIN-код.</translation>
    </message>
    <message>
        <source>The given Card Access Number (CAN) is not correct.</source>
        <extracomment>ALL_PLATFORMS The ID card declined the CAN.</extracomment>
        <translation>Указан неправильный код доступа (CAN).</translation>
    </message>
    <message>
        <source>The given PUK is not correct.</source>
        <extracomment>ALL_PLATFORMS The ID card declined the PUK.</extracomment>
        <translation>Указан неправильный PUK-код.</translation>
    </message>
    <message>
        <source>The PIN is not blocked.</source>
        <extracomment>ALL_PLATFORMS It was attempted to unlock the ID card via PUK even though it was not locked in the first place. This scenario is avoided in the UI by hiding the respective UI elements.</extracomment>
        <translation>PIN-код не заблокирован.</translation>
    </message>
    <message>
        <source>The entered PUK has already been used 10 times and can no longer be used to unblock the PIN.</source>
        <extracomment>ALL_PLATFORMS The card declined the PUK since it was entered wrongfully 10 times.</extracomment>
        <translation>введенный PUK-код уже вводился 10 раз и больше не может использоваться для разблокирования PIN-кода.</translation>
    </message>
    <message>
        <source>The new PIN and the confirmation do not match.</source>
        <extracomment>ALL_PLATFORMS The card reader signalled that the new PIN was not confirmed correctly.</extracomment>
        <translation>Новый PIN-код и его подтверждение не совпадают.</translation>
    </message>
    <message>
        <source>The length of the new PIN is not valid.</source>
        <extracomment>ALL_PLATFORMS The card reader declined the new PIN since its length was invalid.</extracomment>
        <translation>Неправильная длина нового PIN-кода.</translation>
    </message>
    <message>
        <source>The validity verification of the card failed.</source>
        <extracomment>ALL_PLATFORMS The validity verification of the card failed.</extracomment>
        <translation>Сбой проверки действия карты.</translation>
    </message>
    <message>
        <source>The smartphone as card reader (SaC) connection was aborted.</source>
        <extracomment>ALL_PLATFORMS The connection to the smartphone card reader (SaK) was lost.</extracomment>
        <translation>Соединение со смартфоном, используемым в качестве устройства чтения карт (SaC), разорвано.</translation>
    </message>
    <message>
        <source>Smartphone as card reader (SaC) connection request was invalid.</source>
        <extracomment>ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was invalid (missing device ID).</extracomment>
        <translation>Ошибка запроса соединения со смартфоном, используемым в качестве устройства чтения карт (SaC).</translation>
    </message>
    <message>
        <source>Your smartphone as card reader (SaC) version is incompatible with the local version. Please install the latest %1 version on both your smartphone and your computer.</source>
        <extracomment>ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was invalid (API mismatch).</extracomment>
        <translation>Версия смартфона, используемого в качестве устройства чтения карт (SaC), несовместима с локальной версией. Установите последнюю версию %1 на смартфон и компьютер.</translation>
    </message>
    <message>
        <source>An error occurred while trying to establish a connection to the smartphone as card reader (SaC).</source>
        <extracomment>ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) failed due to network errors (Host not found, OS error, ...)</extracomment>
        <translation>Произошла ошибка при установке соединения со смартфоном, используемым в качестве устройства чтения карт (SaC).</translation>
    </message>
    <message>
        <source>The smartphone to be paired has rejected the connection. Please check the pairing code.</source>
        <extracomment>ALL_PLATFORMS The requested connection to the smartphone card reader (SaK) was rejected by the device.</extracomment>
        <translation>Смартфон, предназначенный для сопряжения, отклонил соединение. Проверьте код сопряжения.</translation>
    </message>
    <message>
        <source>File not found.</source>
        <extracomment>ALL_PLATFORMS Download of the file failed with HTTP error code 404.</extracomment>
        <translation>Файл не найден.</translation>
    </message>
    <message>
        <source>Cannot save file.</source>
        <extracomment>ALL_PLATFORMS Saving the downloaded file on the local disk failed.</extracomment>
        <translation>Невозможно сохранить файл.</translation>
    </message>
    <message>
        <source>Received data were corrupted.</source>
        <extracomment>ALL_PLATFORMS The downloaded file contained unsupported/invalid data.</extracomment>
        <translation>Полученные данные повреждены.</translation>
    </message>
    <message>
        <source>Received data does not contain data for the current platform.</source>
        <extracomment>ALL_PLATFORMS The downloaded file does not contain data for the current platform.</extracomment>
        <translation>Полученные данные не содержат данных для текущей платформы.</translation>
    </message>
    <message>
        <source>Download aborted.</source>
        <extracomment>ALL_PLATFORMS The download is aborted.</extracomment>
        <translation>Загрузка отменена.</translation>
    </message>
    <message>
        <source>A new process to start the update could not be launched.</source>
        <extracomment>ALL_PLATFORMS Starting the update failed.</extracomment>
        <translation>Не удалось запустить новый процесс обновления.</translation>
    </message>
    <message>
        <source>The program did not receive a StartPaosResponse message from the server.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Программа не получила сообщение StartPaosResponse с сервера.</translation>
    </message>
    <message>
        <source>The server could not process the client request.</source>
        <extracomment>ALL_PLATFORMS Technical error description.</extracomment>
        <translation>Серверу не удалось обработать клиентский запрос.</translation>
    </message>
    <message>
        <source>The service encountered an internal error while processing a request.</source>
        <extracomment>ALL_PLATFORMS A server has responded with an HTTP error code 5xx.</extracomment>
        <translation>Служба столкнулась с внутренней ошибкой при обработке запроса.</translation>
    </message>
    <message>
        <source>The service reported an error while processing a client request.</source>
        <extracomment>ALL_PLATFORMS A server has responded with an HTTP error code 4xx.</extracomment>
        <translation>Служба сообщила об ошибке при обработке клиентского запроса.</translation>
    </message>
    <message>
        <source>%1 Support</source>
        <extracomment>ALL_PLATFORMS Link text to the app support. %1 is the app name.</extracomment>
        <translation>%1 Поддержка</translation>
    </message>
</context>
<context>
    <name>governikus::HttpHandler</name>
    <message>
        <source>Invalid request (%1)</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Недействительный запрос (%1)</translation>
    </message>
    <message>
        <source>Your browser sent a request that couldn&apos;t be interpreted.</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Браузер отправил запрос, который не удалось интерпретировать.</translation>
    </message>
    <message>
        <source>Error message:</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Сообщение об ошибке:</translation>
    </message>
    <message>
        <source>Unknown request: %1</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Неизвестный запрос: %1</translation>
    </message>
    <message>
        <source>Would you like to report this error?</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Сообщить об этой ошибке?</translation>
    </message>
    <message>
        <source>Report now</source>
        <extracomment>ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.</extracomment>
        <translation>Сообщить сейчас</translation>
    </message>
</context>
<context>
    <name>governikus::LogFilesModel</name>
    <message>
        <source>Current log</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Текущий файл журнала</translation>
    </message>
    <message>
        <source>dd.MM.yyyy hh:mm:ss</source>
        <extracomment>ALL_PLATFORMS Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>dd.MM.yyyy, hh:mm:ss</translation>
    </message>
</context>
<context>
    <name>governikus::LogModel</name>
    <message>
        <source>The logfile is disabled.</source>
        <translation>Файл журнала деактивирован.</translation>
    </message>
    <message>
        <source>Error while saving logfile to &quot;%1&quot;</source>
        <translation>Ошибка при сохранении файла журнала в указанном месте: «%1»</translation>
    </message>
    <message>
        <source>Mobile logfile</source>
        <translation>Мобильный файл журнала</translation>
    </message>
    <message>
        <source>&lt;Please describe the error&gt;</source>
        <translation>&lt;Опишите ошибку&gt;</translation>
    </message>
    <message>
        <source>Send application log per email...</source>
        <extracomment>ANDROID</extracomment>
        <translation>Отправить файл журнала приложения по электронной почте...</translation>
    </message>
    <message>
        <source>Share application log...</source>
        <extracomment>ANDROID</extracomment>
        <translation>Поделиться файлом журнала приложения...</translation>
    </message>
    <message>
        <source>Could not mail log! Please configure your mail client first.</source>
        <translatorcomment>Не удалось отправить файл журнала! Сначала сконфигурируйте почтовую программу.</translatorcomment>
        <translation>Не удалось отправить файл журнала! Сначала сконфигурируйте почтовую программу.</translation>
    </message>
    <message>
        <source>Could not share log! Connection to OS failed.</source>
        <translation>Не удалось поделиться файлом журнала! Произошел сбой соединения с операционной системой.</translation>
    </message>
    <message>
        <source>Could not mail log! Connection to OS failed.</source>
        <translation>Не удалось отправить файл журнала! Произошел сбой соединения с операционной системой.</translation>
    </message>
</context>
<context>
    <name>governikus::NfcReader</name>
    <message>
        <source>ID card detected. Please do not move the device!</source>
        <extracomment>IOS Feedback when a new ID card has been detected</extracomment>
        <translation>Обнаружена идентификационная карта. Не перемещайте устройство!</translation>
    </message>
    <message>
        <source>Please place your ID card on the top of the device&apos;s back side.</source>
        <extracomment>IOS The ID card may be inserted, the authentication process may be started.</extracomment>
        <translation>Расположите идентификационную карту сверху на обратной стороне устройства.</translation>
    </message>
    <message>
        <source>Scanning process has been finished successfully.</source>
        <extracomment>IOS The current session was stopped without errors.</extracomment>
        <translation>Процесс сканирования успешно завершен.</translation>
    </message>
</context>
<context>
    <name>governikus::NotificationModel</name>
    <message>
        <source>hh:mm:ss</source>
        <extracomment>ALL_PLATFORMS Time format according to https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>hh:mm:ss</translation>
    </message>
</context>
<context>
    <name>governikus::NumberModel</name>
    <message>
        <source>You have entered an incorrect, 5-digit Transport PIN 3 times, your %1Transport PIN is now blocked%2. To remove the block, the%1 10-digit PUK%2 must be entered first.</source>
        <extracomment>ALL_PLATFORMS The Transport PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. %1 + %2 are used to emphasize.</extracomment>
        <translation>Вы трижды ввели неправильный 5-значный временный PIN-код; ваш %1временный PIN-код заблокирован%2. Для разблокирования сначала введите%1 10-значный PUK-код%2.</translation>
    </message>
    <message>
        <source>You have entered an incorrect, 10-digit PUK. Please try again.</source>
        <extracomment>ALL_PLATFORMS The PUK entered wrongfully and needs to be supplied again.</extracomment>
        <translation>Вы ввели неправильный 10-значный PUK-код. Повторите попытку.</translation>
    </message>
    <message>
        <source>The input does not match. Please choose a new ID card PIN.</source>
        <extracomment>ALL_PLATFORMS Error message if the new pin confirmation mismatches.</extracomment>
        <translation>Введенные данные не совпадают. Выберите новый PIN-код идентификационной карты.</translation>
    </message>
    <message>
        <source>You have entered an incorrect, 6-digit ID card PIN.</source>
        <extracomment>ALL_PLATFORMS The wrong ID card PIN was entered on the first attempt. Part 1/2</extracomment>
        <translation>Вы ввели неправильный 6-значный PIN-код идентификационной карты.</translation>
    </message>
    <message>
        <source>You have%1 2 further attempts%2 to enter the correct ID card PIN.</source>
        <extracomment>ALL_PLATFORMS The wrong ID card PIN was entered on the first attempt. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>У вас осталось %1ещё две попытки%2, чтобы ввести правильный PIN-код идентификационной карты.</translation>
    </message>
    <message>
        <source>You have entered an %1incorrect, 6-digit ID card PIN 2 times%2.</source>
        <extracomment>ALL_PLATFORMS The wrong ID card PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 1/2</extracomment>
        <translation>Вы %1дважды ввели неправильный 6-значный PIN-код идентификационной карты%2.</translation>
    </message>
    <message>
        <source>For a 3rd attempt, the%1 6-digit Card Access Number (CAN)%2 must be entered first. You can find your CAN in the %1bottom right on the front of your ID card%2.</source>
        <extracomment>ALL_PLATFORMS The wrong Transport PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 2/2
----------
ALL_PLATFORMS The wrong ID card PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>Для третьей попытки сначала введите%1 6-значный код доступа (CAN)%2. Код CAN указан %1внизу справа на передней стороне идентификационной карты%2.</translation>
    </message>
    <message>
        <source>You have entered an incorrect, 6-digit ID card PIN 3 times. Your %1ID card PIN is now blocked%2.</source>
        <extracomment>ALL_PLATFORMS The ID card PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. Part 1/2</extracomment>
        <translation>Вы трижды ввели неправильный 6-значный PIN-код идентификационной карты. %1PIN-код идентификационной карты заблокирован%2.</translation>
    </message>
    <message>
        <source>To remove the block, the%1 10-digit PUK%2 must be entered first. You can find the PUK in the bottom %1right next%2 to the Transport PIN in the %1authority&apos;s letter%2.</source>
        <extracomment>ALL_PLATFORMS The ID card PIN was entered wrongfully three times, the ID card needs to be unlocked using the PUK. Part 2/2</extracomment>
        <translation>Для разблокирования сначала введите%1 10-значный PUK-код%2. PUK-код указан возле временного PIN-кода %1внизу справа%2 на передней стороне %1письма компетентного органа%2.</translation>
    </message>
    <message>
        <source>You have entered an %1incorrect Card Access Number (CAN)%2. Please try again. You can find your CAN in the %1bottom right on the front of your ID card%2.</source>
        <extracomment>ALL_PLATFORMS The CAN was entered wrongfully and needs to be supplied again. %1 + %2 are used to emphasize.</extracomment>
        <translation>Вы ввели %1неправильный код доступа (CAN)%2. Повторите попытку. Код CAN указан %1внизу справа на передней стороне идентификационной карты%2.</translation>
    </message>
    <message>
        <source>You have entered an incorrect, 5-digit Transport PIN.</source>
        <extracomment>ALL_PLATFORMS The wrong Transport PIN was entered on the first attempt. Part 1/2</extracomment>
        <translation>Вы ввели неправильный 5-значный временный PIN-код.</translation>
    </message>
    <message>
        <source>You have%1 2 further attempts%2 to enter the correct Transport PIN. The 5-digit Transport PIN may be found on the %1bottom left of your PIN letter%2.</source>
        <extracomment>ALL_PLATFORMS The wrong Transport PIN was entered on the first attempt. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>У вас %1осталось ещё две попытки%2, чтобы ввести правильный временный PIN-код. 5-значный временный PIN код указан %1внизу слева в письме с PIN-кодом%2.</translation>
    </message>
    <message>
        <source>You have entered an %1incorrect, 5-digit Transport PIN 2 times%2.</source>
        <extracomment>ALL_PLATFORMS The wrong Transport PIN was entered twice, the next attempt requires the CAN for additional verification. %1 + %2 are used to emphasize. Part 1/2</extracomment>
        <translation>Вы %1дважды ввели неправильный 5-значный временный PIN-код%2.</translation>
    </message>
    <message>
        <source>%1An incorrect PIN has been entered 2 times%2 at the last use of your ID card.</source>
        <extracomment>ALL_PLATFORMS Once per workflow info text shown when an ID card with one PIN attempt left has been detected. %1 + %2 are used to emphasize. Part 1/2</extracomment>
        <translation>%1PIN-код дважды введен неправильно%2 при последнем использовании вашей идентификационной карты.</translation>
    </message>
    <message>
        <source>For a 3rd attempt, the%1 6-digit Card Access Number (CAN)%2 must be entered first. You can find your CAN %1in the bottom right on the front of your ID card%2.</source>
        <extracomment>ALL_PLATFORMS Once per workflow info text shown when an ID card with one PIN attempt left has been detected. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>Для третьей попытки сначала %1введите 6-значный код доступа (CAN)%2. Код CAN%1указан внизу справа на передней стороне идентификационной карты%2.</translation>
    </message>
    <message>
        <source>%1An incorrect PIN has been entered 3 times%2 at the last use of your ID card.</source>
        <extracomment>ALL_PLATFORMS Once per workflow info text shown when a blocked ID card has been detected. %1 + %2 are used to emphasize. Part 1/2</extracomment>
        <translation>%1PIN-код трижды введен неправильно%2 при последнем использовании вашей идентификационной карты.</translation>
    </message>
    <message>
        <source>Therefore you have to enter the %1PUK%2 first to %1unlock the ID card PIN%2.</source>
        <extracomment>ALL_PLATFORMS Once per workflow info text shown when a blocked ID card has been detected. %1 + %2 are used to emphasize. Part 2/2</extracomment>
        <translation>Для этого введите сначала %1PUK-код%2, чтобы %1разблокировать PIN-код идентификационной карты%2.</translation>
    </message>
</context>
<context>
    <name>governikus::PinResetInformationModel</name>
    <message>
        <source>You may turn to the competent authority and reset the ID card PIN there.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Вы можете обратиться в компетентный орган и там установить новый PIN-код идентификационной карты.</translation>
    </message>
    <message>
        <source>Find competent authority</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Найти компетентный орган</translation>
    </message>
    <message>
        <source>You may turn to your competent authority to activate the eID function.</source>
        <extracomment>ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Request PIN Reset Letter</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Online via PIN Reset Service</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished">Онлайн через службу сброса PIN-кода</translation>
    </message>
    <message>
        <source>At your competent authority</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished">В вашем компетентном органе</translation>
    </message>
    <message>
        <source>You may request a PIN Reset Letter with a new PIN and it&apos;s according activation code on the following website.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished">Вы можете запросить на следующем веб-сайте письмо для переустановки PIN-кода с кодом активации.</translation>
    </message>
    <message>
        <source>You may request a PIN Reset Letter with a new PIN and it&apos;s according activation code on the following website to activate the eID function.</source>
        <extracomment>ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated</extracomment>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>governikus::ReaderModel</name>
    <message>
        <source>Driver installed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Драйвер установлен</translation>
    </message>
    <message>
        <source>The smartcard service of your system is not reachable.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Служба смарт-карт в вашей системе недоступна.</translation>
    </message>
    <message>
        <source>No driver installed</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Драйверы не установлены</translation>
    </message>
    <message>
        <source>Please download and install the driver you can find at: %1</source>
        <extracomment>ALL_PLATFORMS The driver for card reader needs to be installed, the download link is provided in the message.</extracomment>
        <translation>Скачайте и установите драйвер: %1</translation>
    </message>
    <message>
        <source>This card reader is not officially supported and may not work as expected.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Это устройство чтения карт не поддерживается официально и может работать неправильно.</translation>
    </message>
    <message>
        <source>hh:mm:ss AP</source>
        <extracomment>ALL_PLATFORMS Time format according to https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>hh:mm:ss</translation>
    </message>
    <message>
        <source>The list of card readers was last updated at %1.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Список устройств чтения карт последний раз обновлялся %1.</translation>
    </message>
</context>
<context>
    <name>governikus::RedirectRequest</name>
    <message>
        <source>Cannot reach local %1</source>
        <extracomment>ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.</extracomment>
        <translation>Не удается связаться с локальным приложением %1</translation>
    </message>
    <message>
        <source>Your local %1 is not running. Please start your local %1 and try again.</source>
        <extracomment>ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.</extracomment>
        <translation>Ваше локальное приложение %1 не запущено. Запустите локальное приложение %1 и повторите попытку.</translation>
    </message>
    <message>
        <source>Would you like to try again?</source>
        <extracomment>ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.</extracomment>
        <translation>Повторить попытку?</translation>
    </message>
    <message>
        <source>Try again</source>
        <extracomment>ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.</extracomment>
        <translation>Повторить попытку</translation>
    </message>
</context>
<context>
    <name>governikus::ReleaseInformationModel</name>
    <message>
        <source>No release information present, make sure you are connected to the internet.</source>
        <translation>Информация о версии не найдена, проверьте подключение к Интернету.</translation>
    </message>
</context>
<context>
    <name>governikus::RemoteDeviceModel</name>
    <message>
        <source>Not connected</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не подключено</translation>
    </message>
    <message>
        <source>Available</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Доступно</translation>
    </message>
    <message>
        <source>Paired, but unsupported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Сопряжено, но не поддерживается</translation>
    </message>
    <message>
        <source>Unsupported</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не поддерживается</translation>
    </message>
    <message>
        <source>Not paired</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Не сопряжено</translation>
    </message>
    <message>
        <source>dd.MM.yyyy hh:mm AP</source>
        <extracomment>ALL_PLATFORMS Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString</extracomment>
        <translation>dd.MM.yyyy hh:mm</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Недоступно</translation>
    </message>
    <message>
        <source>Tap to pair</source>
        <extracomment>MOBILE</extracomment>
        <translation>Нажмите для сопряжения</translation>
    </message>
    <message>
        <source>was</source>
        <extracomment>ALL_PLATFORMS Describes the former name of the device and is shown as: &quot;New_Name (was Old_Name)&quot;</extracomment>
        <translation>прежнее название</translation>
    </message>
</context>
<context>
    <name>governikus::RemoteServiceModel</name>
    <message>
        <source>Please pay attention to the display on your other device &quot;%1&quot;.</source>
        <extracomment>MOBILE The smartphone is connected as card reader (SaK) and currently processing an authentication request. The user is asked to pay attention the its screen.</extracomment>
        <translation>Соблюдайте указания на дисплее другого вашего устройства «%1».</translation>
    </message>
    <message>
        <source>Please connect your WiFi to use your smartphone as a card reader (SaC).</source>
        <extracomment>ALL_PLATFORMS The WiFi feature is not enabled but required to use the smartphone as a card reader (SaK).</extracomment>
        <translation>Подключитесь к сети Wi-Fi, чтобы использовать смартфон в качестве устройства чтения карт (SaC).</translation>
    </message>
    <message>
        <source>The NFC radio standard is required for communication with the ID card.

Please enable NFC to use your smartphone as a card reader (SaC).</source>
        <extracomment>ALL_PLATFORMS NFC is available but not active.</extracomment>
        <translation>Для обмена данными с идентификационной картой требуется стандарт радиосвязи NFC.

Активируйте NFC, чтобы использовать смартфон в качестве устройства чтения карт (SaC).</translation>
    </message>
</context>
<context>
    <name>governikus::RemoteServiceSettings</name>
    <message>
        <source>Smartphone as card reader (SaC)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Смартфон в качестве устройства чтения карт (SaC)</translation>
    </message>
</context>
<context>
    <name>governikus::SelfData</name>
    <message>
        <source>This data has not been stored in this chip generation.</source>
        <extracomment>ALL_PLATFORMS The requested data is not stored on this chip&apos;s generation.</extracomment>
        <translation>Эти данные не сохранены в этом поколении чипсетов.</translation>
    </message>
    <message>
        <source>Address</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Адрес</translation>
    </message>
    <message>
        <source>dd.MM.yyyy</source>
        <extracomment>ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString</extracomment>
        <translation>dd.MM.yyyy</translation>
    </message>
    <message>
        <source>xx.MM.yyyy</source>
        <extracomment>ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString with unknown day
----------
ALL_PLATFORMS Additional date format with unknown day</extracomment>
        <translation>хх.ММ.yyyy</translation>
    </message>
    <message>
        <source>xx.xx.yyyy</source>
        <extracomment>ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString with unknown day and month</extracomment>
        <translation>хх.хх.yyyy</translation>
    </message>
    <message>
        <source>Family name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Фамилия</translation>
    </message>
    <message>
        <source>Birth name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Фамилия до брака</translation>
    </message>
    <message>
        <source>Given name(s)</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Имя/имена</translation>
    </message>
    <message>
        <source>Doctoral degree</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Докторская степень</translation>
    </message>
    <message>
        <source>Date of birth</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Дата рождения</translation>
    </message>
    <message>
        <source>Place of birth</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Место рождения</translation>
    </message>
    <message>
        <source>Document type</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Тип документа</translation>
    </message>
    <message>
        <source>Nationality</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Национальность</translation>
    </message>
    <message>
        <source>Religious / artistic name</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Религиозное имя/псевдоним</translation>
    </message>
    <message>
        <source>Issuing country</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Страна выдачи</translation>
    </message>
    <message>
        <source>Residence permit I</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Разрешение на временное проживание I</translation>
    </message>
    <message>
        <source>Valid until</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Действителен до</translation>
    </message>
</context>
<context>
    <name>governikus::StateChangePin</name>
    <message>
        <source>You have successfully changed your ID card PIN.</source>
        <extracomment>ALL_PLATFORMS The ID card PIN was changed successfully (1/2).</extracomment>
        <translation>Вы успешно изменили PIN-код для идентификационной карты.</translation>
    </message>
    <message>
        <source>You may now remove your ID card from the device.</source>
        <translation>Можно извлечь идентификационную карту из устройства.</translation>
    </message>
</context>
<context>
    <name>governikus::StateCheckRefreshAddress</name>
    <message>
        <source>Sending data to provider</source>
        <extracomment>ALL_PLATFORMS Status message after the communication between card and server is completed, the result is being forwarded to the provider.</extracomment>
        <translation>Отправка данных провайдеру</translation>
    </message>
</context>
<context>
    <name>governikus::StateConnectCard</name>
    <message>
        <source>The used ID card type is not accepted by the server.</source>
        <extracomment>IOS</extracomment>
        <translation>Тип используемой идентификационной карты не принимается сервером.</translation>
    </message>
</context>
<context>
    <name>governikus::StateDidAuthenticateEac1</name>
    <message>
        <source>Provider is being verified</source>
        <extracomment>ALL_PLATFORMS Status message after the PIN was entered, Terminal Authentication.</extracomment>
        <translation>Провайдер проходит проверку</translation>
    </message>
</context>
<context>
    <name>governikus::StateDidAuthenticateEac2</name>
    <message>
        <source>eID is being verified</source>
        <extracomment>ALL_PLATFORMS Status message after the PIN was entered, Card Authentication.</extracomment>
        <translation>Проверка онлайн-идентификации</translation>
    </message>
</context>
<context>
    <name>governikus::StateEnterPacePassword</name>
    <message>
        <source>Access denied.</source>
        <extracomment>IOS The current session was interrupted because of a wrong password.</extracomment>
        <translation>Доступ отклонен.</translation>
    </message>
</context>
<context>
    <name>governikus::StateEstablishPaceChannel</name>
    <message>
        <source>The secure channel is opened</source>
        <extracomment>ALL_PLATFORMS First status message after the PIN was entered.</extracomment>
        <translation>Защищенный канал открыт</translation>
    </message>
</context>
<context>
    <name>governikus::StateGetSelfAuthenticationData</name>
    <message>
        <source>Getting results from server</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Получение результатов с сервера</translation>
    </message>
</context>
<context>
    <name>governikus::StateGetTcToken</name>
    <message>
        <source>Fetch TCToken</source>
        <translation>Получить TCToken</translation>
    </message>
</context>
<context>
    <name>governikus::StateTransmit</name>
    <message>
        <source>Reading data</source>
        <extracomment>ALL_PLATFORMS Status message after the PIN was entered, communication between eID server and card is running.</extracomment>
        <translation>Считывание данных</translation>
    </message>
</context>
<context>
    <name>governikus::StateUnfortunateCardPosition</name>
    <message>
        <source>Weak NFC signal</source>
        <extracomment>The NFC signal is weak or unstable. The scan is stopped with this information in the iOS dialog.</extracomment>
        <translation>Слабый сигнал NFC</translation>
    </message>
</context>
<context>
    <name>governikus::Survey</name>
    <message>
        <source>Vendor</source>
        <translation>Производитель</translation>
    </message>
    <message>
        <source>Model Name</source>
        <translation>Имя модели</translation>
    </message>
    <message>
        <source>Model Number</source>
        <translation>Номер модели</translation>
    </message>
    <message>
        <source>Build Number</source>
        <translation>Номер сборки</translation>
    </message>
    <message>
        <source>OS version</source>
        <translation>Версия ОС</translation>
    </message>
    <message>
        <source>Kernel version</source>
        <translation>Версия ядра</translation>
    </message>
    <message>
        <source>Max. NFC Packet Length</source>
        <translation>Макс. длина пакета NFC</translation>
    </message>
    <message>
        <source>%1 Version</source>
        <translation>Версия %1</translation>
    </message>
    <message>
        <source>NFC Tag Type</source>
        <translation>Тип NFC-метки</translation>
    </message>
</context>
<context>
    <name>governikus::TrayIcon</name>
    <message>
        <source>Application was started.</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Приложение запущено.</translation>
    </message>
    <message>
        <source>Open</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Открыть</translation>
    </message>
    <message>
        <source>Quit %1</source>
        <extracomment>DESKTOP</extracomment>
        <translation>Выйти из %1</translation>
    </message>
</context>
<context>
    <name>governikus::UiPluginProxy</name>
    <message>
        <source>Reverse proxy plugin is enabled</source>
        <translation>Активирован плагин обратного прокси-сервера</translation>
    </message>
</context>
<context>
    <name>governikus::UiPluginQml</name>
    <message>
        <source>Information</source>
        <translation>Информация</translation>
    </message>
</context>
<context>
    <name>governikus::UiPluginWebService</name>
    <message>
        <source>An unknown program uses the required port (%1). Please exit the other program and try again!</source>
        <extracomment>ALL_PLATFORMS An unknown program is using the local port on which the AA listens.</extracomment>
        <translation>Неизвестная программа использует требуемый порт (%1). Завершите работу другой программы и повторите попытку!</translation>
    </message>
    <message>
        <source>The program (%1) uses the required port (%2). Please close %1 and try again!</source>
        <extracomment>ALL_PLATFORMS A known program is using the local port on which the AA listens.</extracomment>
        <translation>Программа (%1) использует требуемый порт (%2). Закройте %1 и повторите попытку!</translation>
    </message>
    <message>
        <source>You tried to start a newer version (%1) of currently running %2. Please stop the current version (%3) and start again!</source>
        <extracomment>ALL_PLATFORMS The external request to show the UI requested a newer version than the one currently installed.</extracomment>
        <translation>Вы попытались запустить более новую версию (%1) уже запущенного приложения %2. Остановите текущую версию (%3) и повторите попытку!</translation>
    </message>
    <message>
        <source>You tried to start an older version (%1) of currently running %2. Please open the currently running version (%3)!</source>
        <extracomment>ALL_PLATFORMS The external request to show the UI requested an older version than the one currently installed.</extracomment>
        <translation>Вы попытались запустить более старую версию (%1) уже запущенного приложения %2. Откройте текущую запущенную версию (%3)!</translation>
    </message>
    <message>
        <source>Reverse-Proxy of %1 is started and this instance cannot rebind port. Please ask your administrator!</source>
        <translation>Запущен обратный прокси-сервер %1, и этот экземпляр не может повторно привязать порт. Обратитесь к своему администратору!</translation>
    </message>
    <message>
        <source>Cannot start authentication</source>
        <extracomment>ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.</extracomment>
        <translation>Невозможно запустить аутентификацию</translation>
    </message>
    <message>
        <source>An operation is already in progress.</source>
        <extracomment>ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.</extracomment>
        <translation>Уже выполняется другой процесс.</translation>
    </message>
    <message>
        <source>Would you like to try again?</source>
        <extracomment>ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.</extracomment>
        <translation>Повторить попытку?</translation>
    </message>
    <message>
        <source>Try again</source>
        <extracomment>ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.</extracomment>
        <translation>Повторить попытку</translation>
    </message>
    <message>
        <source>The browser connection was lost.</source>
        <extracomment>ALL_PLATFORMS No HTTP connection present.</extracomment>
        <translation>Соединение с браузером прервано.</translation>
    </message>
    <message>
        <source>Invalid request (%1)</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Недействительный запрос (%1)</translation>
    </message>
    <message>
        <source>Your browser sent a request that couldn&apos;t be interpreted.</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Браузер отправил запрос, который не удалось интерпретировать.</translation>
    </message>
    <message>
        <source>Error message:</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Сообщение об ошибке:</translation>
    </message>
    <message>
        <source>Would you like to report this error?</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Сообщить об этой ошибке?</translation>
    </message>
    <message>
        <source>Report now</source>
        <extracomment>ALL_PLATFORMS Invalid request by the browser, part of an HTML error page</extracomment>
        <translation>Сообщить сейчас</translation>
    </message>
    <message>
        <source>The connection to the browser was lost while redirecting to the provider (%1). No automatic redirection could be performed.</source>
        <extracomment>ALL_PLATFORMS The connection to the browser was lost/timed out..</extracomment>
        <translation>При перенаправлении к провайдеру (%1) было потеряно соединение с браузером. Не удалось выполнить автоматическое перенаправление.</translation>
    </message>
</context>
<context>
    <name>governikus::WorkflowModel</name>
    <message>
        <source>%1 error report - %2</source>
        <translation>Отчет об ошибках %1 — %2</translation>
    </message>
    <message>
        <source>Contact your local citizens&apos; office (Bürgeramt) to apply for a new ID card or to unblock the ID card.</source>
        <translation>Обратитесь в местное ведомство по делам граждан (Bürgeramt), чтобы разблокировать идентификационную карту или заказать новую.</translation>
    </message>
    <message>
        <source>The used ID card is not accepted by the server. Please remove the ID card from your device or card reader and use a suitable ID card or contact the service provider.</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Error code: %1</source>
        <extracomment>ALL_PLATFORMS</extracomment>
        <translation>Код ошибки: %1</translation>
    </message>
</context>
</TS>
