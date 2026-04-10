import requests
import json
import base64
import os

url = 'https://bmn.jinr.int/wp-json/wp/v2'
header = {'Authorization': 'Basic ' + os.environ.get('DOCS_CREDENTIALS')}
postID = os.environ.get('DOCS_ALL_MACROS_POST_ID')

json_data = None
with open('result.json') as json_file:
        json_data = json.load(json_file)

result = requests.post(url + "/pages/" + postID , headers=header, json=json_data)