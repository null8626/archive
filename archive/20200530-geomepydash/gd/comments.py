import json
from urllib.request import urlopen as fetchapi

class Comments:
    def getFromLevel(levelid):
        if str(levelid).isnumeric()==False:
            raise ValueError('Parameters must be Level ID. (In an integer form)')
        try:
            data = json.loads(fetchapi('https://gdbrowser.com/api/comments/'+str(levelid)+'?top').read())
            total = []
            for a in range(0, len(data)):
                total.append(GDClass(data[a]))
            return total
        except:
            raise ValueError('Invalid Level ID.')
    
    def getFromUser(userid):
        if str(userid).isnumeric()==False:
            raise ValueError('Parameters (user ID) must be integer value.')
        try:
            data = json.loads(fetchapi('https://gdbrowser.com/api/comments/'+str(userid)+'?type=commentHistory').read())
            total = []
            for a in range(0, len(data)):
                total.append(GDClass(data[a]))
            return total
        except:
            raise ConnectionRefusedError('Invalid User ID.')
    
    def getProfilePosts(userid):
        if str(userid).isnumeric()==False:
            raise ValueError('Parameters (user ID) must be integer value.')
        try:
            data = json.loads(fetchapi('https://gdbrowser.com/api/comments/'+str(userid)+'?type=profile').read())
            total = []
            for a in range(0, len(data)):
                total.append(GDClass(data[a]))
            return total
        except:
            raise ConnectionRefusedError('Invalid User ID.')