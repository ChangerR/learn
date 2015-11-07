#!/usr/bin/python

from pymongo import MongoClient
import random

def random_name():
    len = random.randint(2,8)
    name = ""
    for i in range(len):
        name += random.choice("abcdefghijklmnopqrstuvwxyz");
    return name

#print random_name()

conn = MongoClient("changer.site",27017)
db = conn.test
db.authenticate("changer","root+changer=0")
#db.person.drop();
#db.person.insert({"name":"he","age":12});

#for i in range(100000):
#    n = random_name();
#    db.person.insert({"id":i,"name":n,"sex":random.choice(["man","female"]),"age":random.randint(10,99)})
a20 = db.person.find({"age":56})
for p in a20:
    print p
