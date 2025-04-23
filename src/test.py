import requests
import json
import time
import datetime
import sys

stop =1255
n = 3

r = requests.get('https://api.swu.de/mobility/v1/stop/passage/Departures?StopNumber='+str(stop)+'&Limit='+str(n))
print(r._content)

r2 = requests.get('https://api.swu.de/mobility/v1/route/attributes/BaseData?ContentScope=extended&RouteNumber=5') #change the last number to your route maybe just copy the link and open in firefox as it structures the json file