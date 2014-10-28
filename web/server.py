import cherrypy
import json
import subprocess
import sys
import threading
import uuid


def json_fail():
    def json_fail_handler(handler):
        def f(*args, **kwargs):
            value = handler(*args, **kwargs)
            if value['status'] == 'fail':
                cherrypy.serving.response.status = 400
            return value
        return f

    request = cherrypy.serving.request
    request.handler = json_fail_handler(request.handler)

cherrypy.tools.json_fail = cherrypy.Tool('before_handler',
                                         json_fail, priority=20)


def json_api(f):
    for dec in [cherrypy.tools.json_in(), cherrypy.tools.json_out(),
                cherrypy.tools.json_fail()]:
        f = dec(f)
    return f


class BoardFrontend(object):

    def __init__(self, controller_cmd):
        self.proc = subprocess.Popen(controller_cmd, stdin=subprocess.PIPE,
                                     stdout=subprocess.PIPE)
        self.responses = dict()
        self.lock = threading.Lock()
        self._board = None

    def _send_command(self, cmd):
        request_id = uuid.uuid4().bytes
        if not isinstance(cmd, basestring):
            cmd = json.dumps(cmd)
        with self.lock:
            self.proc.stdin.write(request_id + cmd + '\n')
        for line in iter(self._proc_readline, b''):
            self.responses[line[:16]] = line[16:].strip()
            if request_id in self.responses:
                proc_resp = self.responses[request_id].split(None, 1)
                resp = {'status': proc_resp[0]}
                if len(proc_resp) > 1:
                    try:
                        resp['data'] = json.loads(proc_resp[1])
                    except ValueError:
                        resp['data'] = proc_resp[1]
                return resp

    def _proc_readline(self):
        with self.lock:
            return self.proc.stdout.readline()

    @cherrypy.expose
    def index(self):
        return 'html added and removed here'

    @cherrypy.expose
    @json_api
    def board(self):
        if not self._board:
            self._board = self._send_command('board')
        return self._board

    @cherrypy.expose
    @json_api
    def state(self):
        if hasattr(cherrypy.request, 'json'):
            return self._send_command(cherrypy.request.json)
        else:
            return {'status': 'fail', 'data': 'not implemented'}


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print 'usage: {0} [BOARD CONTROLLER COMMAND]'.format(sys.argv[0])
        sys.exit(1)
    cherrypy.quickstart(BoardFrontend(sys.argv[1:]))
