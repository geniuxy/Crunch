from flask import Flask, request, jsonify
import subprocess
import consts
import re

from consts import PORT_KEY

app=Flask(__name__)

#TODO: Remove when using docket in the future
nextAvailablePort = 7777

@app.route('/Sessions', methods=['POST'])
def CreateServer():
    print(dict(request.headers))
    print(dict(request.get_json()))

    port = nextAvailablePort
    return jsonify({"status": "success", PORT_KEY: port}), 200

if __name__== "__main__":
    app.run(host="0.0.0.0", port=80)

