import Queue
import cherrypy
import json
import os
import subprocess
import sys
import threading
import uuid


def json_fail():
    def json_fail_handler(handler):
        def f(*args, **kwargs):
            value = handler(*args, **kwargs)
            if isinstance(value, dict) and value.get('status', None) == 'fail':
                cherrypy.serving.response.status = value.get('failcode', 400)
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


class AsynchronousFileReader(threading.Thread):

    def __init__(self, fd, queue):
        super(AsynchronousFileReader, self).__init__()
        self.fd = fd
        self.queue = queue

    def run(self):
        for line in iter(self.fd.readline, b''):
            print "from stdout: {0!r}".format(line)
            self.queue.put(line)

    def eof(self):
        return not self.is_alive() and self.queue.empty()


class BoardFrontend(object):

    def __init__(self, controller_cmd):
        self.proc = subprocess.Popen(controller_cmd, stdin=subprocess.PIPE,
                                     stdout=subprocess.PIPE)
        self.stdout_queue = Queue.Queue()
        self.stdout_reader = AsynchronousFileReader(self.proc.stdout,
                                                    self.stdout_queue)
        self.stdout_reader.start()
        self.responses = dict()
        self.lock = threading.Lock()
        self._board = None

        cherrypy.engine.subscribe('stop', self.stop)

    def stop(self):
        self.proc.stdin.close()
        self.proc.wait()

    def _send_command(self, cmd):
        request_id = uuid.uuid4().hex
        if not isinstance(cmd, basestring):
            cmd = json.dumps(cmd)
        with self.lock:
            print "to stdin: {0!r}".format(request_id + cmd + '\n')
            self.proc.stdin.write(request_id + cmd + '\n')
        for line in iter(self._proc_readline, None):
            self.responses[line[:32]] = line[32:].strip()
            if request_id in self.responses:
                proc_resp = self.responses[request_id].split(None, 1)
                resp = {'status': proc_resp[0]}
                if len(proc_resp) > 1:
                    try:
                        resp['data'] = json.loads(proc_resp[1])
                    except ValueError:
                        resp['data'] = proc_resp[1]
                return resp
        # if we hit this point, reading from self.proc.stdout timed out
        return {'status': 'fail', 'failcode': 500,
                'data': 'internal server error'}

    def _proc_readline(self):
        if self.stdout_reader.eof():
            return None
        try:
            return self.stdout_queue.get(timeout=5)
        except Queue.Empty:
            return None

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

    @cherrypy.expose
    @json_api
    def blank(self):
        return self._send_command({'mode': 'fill', 'color': 0})


if __name__ == '__main__':
    def subdir(dir):
        return os.path.abspath(os.path.join(os.path.dirname(__file__), dir))

    if len(sys.argv) == 1:
        print 'usage: {0} [BOARD CONTROLLER COMMAND]'.format(sys.argv[0])
        sys.exit(1)
    cherrypy.quickstart(BoardFrontend(sys.argv[1:]), '', {
        '/static': {'tools.staticdir.on': True,
                    'tools.staticdir.dir': subdir('static')},
        '/static/bootstrap': {'tools.staticdir.on': True,
                              'tools.staticdir.dir': subdir('bootstrap/dist')},
    })
