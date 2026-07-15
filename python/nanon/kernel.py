import ast
import code
import contextlib
import functools
import json
import os
import socket
import struct
import sys
import traceback


_SOCKET_PATH = "/tmp/nanon.sock"

_GLOBALS_DICT = {}


class SocketWriter:
    def __init__(self, event, send):
        self.event = event
        self.send = send

    def write(self, text):
        # Do not place code that logs to stdout/stderr in this function!
        if text:
            self.send({
                "event": self.event,
                "text": text,
            })

    def flush(self):
        pass


class ReprSocketWriter(SocketWriter):

    def write(self, text):
        self.send({
            "event": self.event,
            "text": repr(text) + "\n",
        })


class Worker:

    def __init__(self, connection, request):
        self.connection = connection
        self.request = request

    @contextlib.contextmanager
    def close_connection(self):
        yield
        self.connection.close()

    def handle(self):
        try:
            self._handle()
        finally:
            self.connection.close()

    @staticmethod
    def _is_expression(code):
        try:
            tree = ast.parse(code, mode="exec")
        except SyntaxError:
            return False
        return len(tree.body) == 1 and isinstance(tree.body[-1], ast.Expr)

    def _handle(self):
        stdout = SocketWriter("stdout", self.send)
        stderr = SocketWriter("stderr", self.send)
        repr = ReprSocketWriter("repr", self.send)

        if self._is_expression(self.request):
            sys.displayhook = repr.write

            console = code.InteractiveConsole(locals=_GLOBALS_DICT)
            code_obj = code.compile_command(self.request, symbol="single")

            cb = functools.partial(console.runcode, code_obj)
        else:
            cb = functools.partial(exec, self.request, _GLOBALS_DICT)

        with (
            contextlib.redirect_stdout(stdout),
            contextlib.redirect_stderr(stderr),
        ):
            try:
                cb()
                result = {
                    "event": "result",
                    "success": True,
                }
            except Exception:
                traceback.print_exc()
                result = {
                    "event": "result",
                    "success": False,
                    "traceback": traceback.format_exc(),
                }

        self.send(result)

    def send(self, response):
        payload = json.dumps(response)
        data = payload.encode("utf-8")
        header = struct.pack("!I", len(data))

        self.connection.sendall(header + data)


class Kernel:

    def __init__(self, socket_path=_SOCKET_PATH):

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
            connection, request = self.accept_request()
            if request is None:
                connection.close()
                continue

            worker = Worker(connection, request)
            worker.handle()

    def accept_request(self):

        self.sock.listen(1)

        connection, _ = self.sock.accept()

        data = connection.recv(1024)
        if not data:
            return connection, None

        length_bytes = data[:4]
        length = struct.unpack("!I", length_bytes)[0]
        payload_bytes = data[4:4+length]
        payload = payload_bytes.decode("utf-8")

        return connection, payload


if __name__ == "__main__":
    kernel = Kernel()
    kernel.loop()
