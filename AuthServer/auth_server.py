"""
-------------------------
Written by Claude Sonnet 30.05.2026
-------------------------

Simple Auth Server for Project_A (local development).
Endpoints:
  POST /api/auth/login     { "username": "...", "password": "..." }
  POST /api/auth/logout    { "accountId": "..." }  + Bearer header
  POST /api/auth/validate  { "accountId": "..." }  + Bearer header
"""

import json
import os
import secrets
import logging
from http.server import BaseHTTPRequestHandler, HTTPServer
from datetime import datetime, timedelta

# ---------------------------------------------------------------------------
# Config
# ---------------------------------------------------------------------------
HOST = "127.0.0.1"
PORT = 7783
TOKEN_TTL_MINUTES = 120
#USERS_FILE = os.path.join(os.path.dirname(__file__), "users.json")
base_path = os.path.dirname(sys.executable)
USERS_FILE = os.path.join(base_path, "users.json")

logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] %(levelname)s %(message)s",
    datefmt="%H:%M:%S",
)
log = logging.getLogger("AuthServer")

# ---------------------------------------------------------------------------
# In-memory storage
# ---------------------------------------------------------------------------
# { accountId: { "passwordHash": "...", "username": "..." } }
accounts: dict = {}

# { sessionToken: { "accountId": "...", "expiresAt": datetime } }
sessions: dict = {}


def load_users():
    """Load users from users.json. Creates the file with a test user if missing."""
    if not os.path.exists(USERS_FILE):
        default = {
            "account_admin": {"username": "admin", "password": "admin"},
            "account_player1": {"username": "player1", "password": "1234"},
        }
        with open(USERS_FILE, "w") as f:
            json.dump(default, f, indent=2)
        log.info("Created default users.json with test accounts.")

    with open(USERS_FILE, "r") as f:
        data = json.load(f)

    # accounts dict: accountId -> { username, password }
    for account_id, info in data.items():
        accounts[account_id] = info
    log.info(f"Loaded {len(accounts)} account(s) from users.json.")


def find_account_by_credentials(username: str, password: str):
    """Returns accountId if credentials match, else None."""
    for account_id, info in accounts.items():
        if info["username"].lower() == username.lower() and info["password"] == password:
            return account_id
    return None


def create_session(account_id: str) -> str:
    token = secrets.token_hex(32)
    sessions[token] = {
        "accountId": account_id,
        "expiresAt": datetime.utcnow() + timedelta(minutes=TOKEN_TTL_MINUTES),
    }
    return token


def validate_token(token: str):
    """Returns accountId if token is valid and not expired, else None."""
    session = sessions.get(token)
    if not session:
        return None
    if datetime.utcnow() > session["expiresAt"]:
        del sessions[token]
        return None
    return session["accountId"]


def extract_bearer_token(handler) -> str:
    auth_header = handler.headers.get("Authorization", "")
    if auth_header.startswith("Bearer "):
        return auth_header[7:]
    return ""


# ---------------------------------------------------------------------------
# HTTP Handler
# ---------------------------------------------------------------------------
class AuthHandler(BaseHTTPRequestHandler):

    def log_message(self, format, *args):
        log.info(f"{self.address_string()} - {format % args}")

    def send_json(self, code: int, data: dict):
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
        raw = self.rfile.read(length)
        try:
            text = raw.decode("utf-8")
            return json.loads(text)
        except (json.JSONDecodeError, UnicodeDecodeError) as e:
            # log raw body for debugging (repr -> visible quotes, control chars)
            log.warning(f"Failed to parse request body: {e}; raw={repr(raw)}")
            return None

    def do_POST(self):
        if self.path == "/api/auth/login":
            self.handle_login()
        elif self.path == "/api/auth/logout":
            self.handle_logout()
        elif self.path == "/api/auth/validate":
            self.handle_validate()
        else:
            self.send_json(404, {"message": "Not found"})

    # --- /api/auth/login ---
    def handle_login(self):
        body = self.read_json_body()
        if body is None:
            self.send_json(400, {"message": "Invalid JSON in request body."})
            return

        username = body.get("username", "").strip()
        password = body.get("password", "")

        if not username or not password:
            self.send_json(400, {"message": "Username and password are required."})
            return

        account_id = find_account_by_credentials(username, password)
        if not account_id:
            log.warning(f"Login failed for username='{username}'")
            self.send_json(401, {"message": "Invalid username or password."})
            return

        token = create_session(account_id)
        log.info(f"Login success: accountId={account_id}")
        self.send_json(200, {"accountId": account_id, "sessionToken": token})

    # --- /api/auth/logout ---
    def handle_logout(self):
        token = extract_bearer_token(self)
        if token and token in sessions:
            del sessions[token]
            log.info(f"Logout: token invalidated.")
        self.send_json(200, {"message": "Logged out."})

    # --- /api/auth/validate ---
    def handle_validate(self):
        token = extract_bearer_token(self)
        account_id = validate_token(token)
        if not account_id:
            self.send_json(401, {"message": "Invalid or expired session token."})
            return
        self.send_json(200, {"accountId": account_id, "sessionToken": token})


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------
if __name__ == "__main__":
    load_users()
    server = HTTPServer((HOST, PORT), AuthHandler)
    log.info(f"Auth server running at http://{HOST}:{PORT}")
    log.info("Press Ctrl+C to stop.")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        log.info("Shutting down.")
        server.shutdown()