from ubuntuuitoolkit._custom_proxy_objects._common \
    import is_maliit_process_running
from ubuntu_keyboard.emulators.keyboard import Keyboard


def dismiss_osk():
    if is_maliit_process_running():
        osk = Keyboard()
        osk.dismiss()
