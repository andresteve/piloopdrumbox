import os

class Py_to_pd:
    def __init__(self, pd_path, send_port):
        self.path = pd_path
        self.port = send_port

    def send2Pd(self, channel, message=''):
        """
        Send messages from Python to PD.
        channel: Link each message to a channel to be received in PD
            0: DSP status (0: off, 1: on)
            1: select_kit (int value)
        message: should be a string msg to be send as variable to PD
        """
        os.system("echo '" + str(channel) + " " + str(message) + ";' | "+
            self.path + "pdsend " + str(self.port))

    def audio_on(self):
        """
        turn on DSP in PD
        channel: 0
        """
        message = '1'
        self.send2Pd(0, message)

    def audio_off(self):
        """
        turn off DSP in PD
        channel: 0
        """
        message = '0'
        self.send2Pd(0, message)

    def select_kit(self, kit):
        """
        kit: numeric value that for selecting a kit
        channel: 1
        """
        self.send2Pd(1, kit)

    def press_button(self, button):
        """
        button: numeric value of the pressed button (loop: 1-8, drumbox: 9-16)
        channel: 2
        """
        if button > 16 or button < 1:
            raise("button_error: range of buttons should be between 1:16")
        else:
            self.send2Pd(2, button)
