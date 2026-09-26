from flask import Flask, render_template, jsonify, request, send_file
import datetime
import os

app = Flask(__name__)

API_KEY = "A9XK39-PS-DEVICE-KEY-8821"

entry_count = 0
exit_count = 0
current_crowd = 0

LOG_FILE = "crowd_history.txt"


def save_to_file(entry, exit_, crowd):
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    line = f"{timestamp} | ENTRY: {entry} | EXIT: {exit_} | CROWD: {crowd}\n"
    with open(LOG_FILE, "a") as f:
        f.write(line)


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/get_data")
def get_data():
    return jsonify({
        "entry": entry_count,
        "exit": exit_count,
        "crowd": current_crowd
    })


@app.route("/update", methods=["POST"])
def update():
    global entry_count, exit_count, current_crowd

    key = request.headers.get("X-API-KEY")
    if key != API_KEY:
        return jsonify({"error": "Invalid API key"}), 401

    data = request.get_json()

    new_entry = int(data.get("entry", entry_count))
    new_exit = int(data.get("exit", exit_count))

 
    if new_entry < entry_count:
        new_entry = entry_count
    if new_exit < exit_count:
        new_exit = exit_count

    entry_changed = (new_entry != entry_count)
    exit_changed = (new_exit != exit_count)

    entry_count = new_entry
    exit_count = new_exit
    current_crowd = entry_count - exit_count

    if entry_changed or exit_changed:
        save_to_file(entry_count, exit_count, current_crowd)

    return jsonify({
        "status": "ok",
        "entry": entry_count,
        "exit": exit_count,
        "crowd": current_crowd
    })


@app.route("/api/history")
def api_history():
    if not os.path.exists(LOG_FILE):
        return jsonify({"history": []})

    with open(LOG_FILE, "r") as f:
        content = f.readlines()

    return jsonify({"history": content})


@app.route("/download_history")
def download_history():
    if not os.path.exists(LOG_FILE):
        return "History file not found", 404
    return send_file(LOG_FILE, as_attachment=True)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
