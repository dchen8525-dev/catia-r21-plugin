# -*- coding: utf-8 -*-
from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import os

PREFIX = "DLXX"
COUNT_FILE = "count.txt"

def get_current_count():
    if os.path.exists(COUNT_FILE):
        with open(COUNT_FILE, "r", encoding="utf-8") as f:
            return int(f.read().strip())
    return 1

def save_count(num):
    with open(COUNT_FILE, "w", encoding="utf-8") as f:
        f.write(str(num))

serial_num = get_current_count()

class CodeHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        global serial_num
        if self.path == "/api/getCode":
            num_str = f"{serial_num:04d}"
            part_code = f"{PREFIX}{num_str}"
            resp_data = {
                "code": 200,
                "message": "success",
                "data": {"partCode": part_code}
            }
            self.send_response(200)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.end_headers()
            self.wfile.write(json.dumps(resp_data).encode("utf-8"))
            serial_num += 1
            save_count(serial_num)
        else:
            self.send_response(404)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.end_headers()
            err_data = {"code": 404, "message": "api not found"}
            self.wfile.write(json.dumps(err_data).encode("utf-8"))

if __name__ == "__main__":
    host = "localhost"
    port = 8080
    server = HTTPServer((host, port), CodeHandler)
    print("Server running at http://localhost:8080/api/getCode")
    print("Press Ctrl+C to stop")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        server.server_close()
        print("\nServer stopped")