import os
import re
import subprocess
from flask import Flask, request, jsonify
from pathlib import Path

from consts import PORT_KEY, SESSION_NAME_KEY, SESSION_SEARCH_ID_KEY

app=Flask(__name__)

def GetUsedPorts():
    # 执行docker ps --format "{{.Ports}}"，并捕获对应的返回内容
    result = subprocess.run(['docker', 'ps', '--format', '"{{.Ports}}"'], capture_output=True, text=True)
    output = result.stdout

    usedPorts = set()

    for line in output.strip().split("\n"):
        matches = re.findall(r'0\.0\.0\.0:(\d+)->', line)
        usedPorts.update(map(int, matches))

    return usedPorts

def CreateServerImpl(sessionName, sessionSearchID):
    ports = GetUsedPorts()
    print(ports)

#TODO: Remove when using docket in the future
nextAvailablePort = 7777

def CreateServerLocalTest(sessionName, sessionSearchID):
    global nextAvailablePort

    # 获取环境变量%UNREAL_EDITOR%的值
    unreal_editor = os.environ.get("UNREAL_EDITOR")
    if not unreal_editor:
        raise EnvironmentError("环境变量 UNREAL_EDITOR 未设置")

    # 从当前文件所在目录开始，向上查找 Crunch.uproject
    check_dir = Path(__file__).resolve().parent
    uproject = None
    for parent in [check_dir, *check_dir.parents]:
        candidate = parent / "Crunch.uproject"
        if candidate.exists():
            uproject = candidate
            break
    if not uproject:
        raise FileNotFoundError("在当前文件的上级目录中未找到 Crunch.uproject")

    subprocess.Popen([
        unreal_editor,
        uproject,
        '-server',
        '-log',
        '-epicapp="ServerClient"',
        f'-SESSION_NAME={sessionName}',
        f'-SESSION_SEARCH_ID={sessionSearchID}',
        f'-PORT={nextAvailablePort}'
    ])

    usedPort = nextAvailablePort
    nextAvailablePort += 1
    return usedPort

@app.route('/Sessions', methods=['POST'])
def CreateServer():
    print(dict(request.headers))
    print(dict(request.get_json()))

    sessionName = request.get_json().get(SESSION_NAME_KEY)
    sessionSearchID = request.get_json().get(SESSION_SEARCH_ID_KEY)
    port = CreateServerLocalTest(sessionName, sessionSearchID)
    return jsonify({"status": "success", PORT_KEY: port}), 200

if __name__== "__main__":
    # app.run(host="0.0.0.0", port=5000)
    CreateServerImpl("", "")
