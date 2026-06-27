
import contextlib
import json
import os
import socket
import struct
import traceback
from io import StringIO


_SOCKET_PATH = "/tmp/nanon.sock"


class Kernel:

    def __init__(self, socket_path=_SOCKET_PATH):
        self.globals_dict = {}

        # Remove existing connections
        try:
            os.unlink(socket_path)
        except OSError:
            if os.path.exists(socket_path):
                raise

        self.sock = socket.socket(socket.AF_UNIX)
        self.sock.bind(socket_path)

    def loop(self):

        while True:
            print('looping...')
            request = self.read_request()
            response = self.handle(request)
            self.send(response)

    def read_request(self):

        self.sock.listen(1)

        self.connection, client = self.sock.accept()

        data = self.connection.recv(1024)
        if not data:
            return None

        length_bytes = data[:4]
        length = struct.unpack("!I", length_bytes)[0]
        payload_bytes = data[4:4+length]
        payload = payload_bytes.decode("utf-8")

        return payload

    def handle(self, request):

        stdout = StringIO()
        stderr = StringIO()

        with (
            contextlib.redirect_stdout(stdout),
            contextlib.redirect_stderr(stderr),
        ):
            try:
                exec(request, self.globals_dict)
                success = True
            except Exception:
                stderr.write(traceback.format_exc())
                success = False

        stdout_text = stdout.getvalue()
        stderr_text = stderr.getvalue()

        return {
            "success": success,
            "stdout": stdout_text,
            "stderr": stderr_text,
        }

    def send(self, response):
        payload = json.dumps(response)
        data = payload.encode("utf-8")
        header = struct.pack("!I", len(data))

        self.connection.sendall(header + data)

        self.connection.close()


if __name__ == "__main__":
    kernel = Kernel()
    kernel.loop()
