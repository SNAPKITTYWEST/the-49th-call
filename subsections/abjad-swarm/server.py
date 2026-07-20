#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# server.py — quantum-swarm API on localhost:7733
# POST /swarm {"prompt": "...", "n": 8}
# GET  /health

import json, sys, os
from http.server import HTTPServer, BaseHTTPRequestHandler
from swarm import run_swarm

class SwarmHandler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        pass  # silent

    def do_POST(self):
        if self.path != '/swarm':
            self._send(404, {'error': 'not found'})
            return
        length = int(self.headers.get('Content-Length', 0))
        body   = self.rfile.read(length)
        try:
            req    = json.loads(body)
            n      = max(1, min(int(req.get('n', 8)), 300))
            answer = run_swarm(n)
            self._send(200, {
                'answer':   answer,
                'abjad':    answer % 1000,
                'n_agents': n,
                'engine':   'SUBLEQ+QENG+collectivekitty-186',
            })
        except Exception as e:
            self._send(500, {'error': str(e)})

    def do_GET(self):
        if self.path == '/health':
            self._send(200, {'status': 'ok', 'engine': 'SUBLEQ+QENG', 'port': 7733})
        else:
            self._send(404, {'error': 'not found'})

    def _send(self, code, data):
        body = json.dumps(data, indent=2).encode()
        self.send_response(code)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Content-Length', len(body))
        self.end_headers()
        self.wfile.write(body)

if __name__ == '__main__':
    port = 7733
    srv  = HTTPServer(('0.0.0.0', port), SwarmHandler)
    print(f'quantum-swarm on http://localhost:{port}')
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        print('halted')
