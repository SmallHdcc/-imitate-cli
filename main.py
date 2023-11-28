# 读取json文件并且解析
import json
import sys


# 读取json文件
def read_json_file(file_name):
    with open(file_name, 'r', encoding='utf-8') as f:
        data = json.load(f)
        return data


dic = read_json_file('config.json')

# 判断'value'在哪个键的值之中
for key in dic:
    if sys.argv[1] in dic[key]:
        print(key)
        break
