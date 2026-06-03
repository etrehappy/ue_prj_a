"""
-------------------------
Written by Claude Sonnet 30.05.2026
-------------------------

Simple Character Server for Project_A (local development).
Stores character data in characters.json on disk.

Endpoints:
  GET  /api/characters?accountId=...
  GET  /api/characters/{characterId}?accountId=...
  POST /api/characters/{characterId}   body: full save data JSON
"""

import json
import os
import logging
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse, parse_qs

HOST = "127.0.0.1"
PORT = 7784
# DATA_FILE = os.path.join(os.path.dirname(__file__), "characters.json")
base_path = os.path.dirname(sys.executable)
DATA_FILE = os.path.join(base_path, "characters.json")

logging.basicConfig(level=logging.INFO, format="[%(asctime)s] %(levelname)s %(message)s", datefmt="%H:%M:%S")
log = logging.getLogger("CharacterServer")

# Structure: { accountId: { characterId: { ...save data... } } }
db: dict = {}


def load_db():
    if not os.path.exists(DATA_FILE):
        # Seed two test characters for account_admin
        seed = {
            "account_admin": {
                "warrior_01": {
                    "characterId": "warrior_01", "name": "Thorin", "className": "Warrior",
                    "level": 5, "experience": 1200,
                    "currentHealth": 150.0, "maxHealth": 150.0,
                    "lastMapId": "Map", "lastPosition": {"x": 0, "y": 0, "z": 100}
                },
                "mage_01": {
                    "characterId": "mage_01", "name": "Gandalf", "className": "Mage",
                    "level": 3, "experience": 400,
                    "currentHealth": 80.0, "maxHealth": 80.0,
                    "lastMapId": "Map", "lastPosition": {"x": 0, "y": 0, "z": 100}
                }
            }
        }
        with open(DATA_FILE, "w") as f:
            json.dump(seed, f, indent=2)
        log.info("Created default characters.json with seed data.")

    global db
    with open(DATA_FILE, "r") as f:
        db = json.load(f)
    log.info(f"Loaded character data for {len(db)} account(s).")


def save_db():
    with open(DATA_FILE, "w") as f:
        json.dump(db, f, indent=2)


class CharacterHandler(BaseHTTPRequestHandler):

    def log_message(self, format, *args):
        log.info(f"{self.address_string()} - {format % args}")

    def send_json(self, code: int, data):
        body = json.dumps(data).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def read_json_body(self):
        length = int(self.headers.get("Content-Length", 0))
        if length == 0:
            return {}
        return json.loads(self.rfile.read(length).decode("utf-8"))

    def get_account_id_from_query(self, query: dict):
        return query.get("accountId", [None])[0]

    def do_GET(self):
        parsed = urlparse(self.path)
        query = parse_qs(parsed.query)
        path_parts = [p for p in parsed.path.split("/") if p]

        # GET /api/characters?accountId=...
        if path_parts == ["api", "characters"]:
            account_id = self.get_account_id_from_query(query)
            if not account_id:
                self.send_json(400, {"message": "accountId is required."})
                return

            characters = db.get(account_id, {})
            result = []
            for char_id, data in characters.items():
                result.append({
                    "characterId": char_id,
                    "name": data.get("name", ""),
                    "level": data.get("level", 1),
                    "className": data.get("className", "Unknown")
                })
            log.info(f"Character list for {account_id}: {len(result)} character(s).")
            self.send_json(200, result)

        # GET /api/characters/{characterId}?accountId=...
        elif len(path_parts) == 3 and path_parts[:2] == ["api", "characters"]:
            char_id = path_parts[2]
            account_id = self.get_account_id_from_query(query)
            if not account_id:
                self.send_json(400, {"message": "accountId is required."})
                return

            char_data = db.get(account_id, {}).get(char_id)
            if not char_data:
                self.send_json(404, {"message": f"Character '{char_id}' not found."})
                return

            log.info(f"Loaded character {char_id} for {account_id}.")
            self.send_json(200, char_data)
        else:
            self.send_json(404, {"message": "Not found."})

    def do_POST(self):
        parsed = urlparse(self.path)
        path_parts = [p for p in parsed.path.split("/") if p]

        # POST /api/characters/{characterId}
        if len(path_parts) == 3 and path_parts[:2] == ["api", "characters"]:
            char_id = path_parts[2]
            body = self.read_json_body()
            account_id = body.get("accountId", "")

            if not account_id:
                self.send_json(400, {"message": "accountId is required in body."})
                return

            if account_id not in db:
                db[account_id] = {}

            db[account_id][char_id] = body
            save_db()
            log.info(f"Saved character {char_id} for {account_id}.")
            self.send_json(200, {"message": "Saved."})
        else:
            self.send_json(404, {"message": "Not found."})


if __name__ == "__main__":
    load_db()
    server = HTTPServer((HOST, PORT), CharacterHandler)
    log.info(f"Character server running at http://{HOST}:{PORT}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        log.info("Shutting down.")
        server.shutdown()