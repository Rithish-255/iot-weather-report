#!/usr/bin/env python3
# Simple simulator: POSTs random sensor data to server

import time
import requests
import random
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://localhost:3000/data', help='Server URL')
parser.add_argument('--interval', type=float, default=5.0, help='Send interval seconds')
args = parser.parse_args()

print("Simulator posting to", args.url)
try:
    while True:
        payload = {
            "device": "simulator-1",
            "timestamp": int(time.time()),
            "temperature_dht": round(20 + random.uniform(-2.5, 2.5), 1),
            "humidity": round(50 + random.uniform(-10, 10), 1),
            "temperature_bmp": round(20 + random.uniform(-2.0, 2.0), 1),
            "pressure_hpa": round(1013 + random.uniform(-5.0, 5.0), 1)
        }
        try:
            r = requests.post(args.url, json=payload, timeout=5)
            print(time.strftime('%X'), 'sent', payload, '->', r.status_code)
        except Exception as e:
            print('POST error:', e)
        time.sleep(args.interval)
except KeyboardInterrupt:
    print("Simulator stopped.")
