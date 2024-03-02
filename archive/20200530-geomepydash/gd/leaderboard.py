import json
from urllib.request import urlopen as fetchapi

class Leaderboard:
    def topPlayers(count):
        if count>5000:
            raise OverflowError('Too many!')
        elif count<1:
            raise OverflowError('Too small!')
        else:
            data = json.loads(fetchapi('https://gdbrowser.com/api/leaderboard?count='+str(count)).read())
            try:
                total = []
                for a in range(0, len(data)):
                    total.append(GDClass(data[a]))
            except:
                raise ConnectionRefusedError('Invalid Query')
            return total
    def topCreators(count):
        if count>5000:
            raise OverflowError('Too many!')
        elif count<1:
            raise OverflowError('Too small!')
        else:
            data = json.loads(fetchapi('https://gdbrowser.com/api/leaderboard?creator&count='+str(count)).read())
            try:
                total = []
                for a in range(0, len(data)):
                    total.append(GDClass(data[a]))
                return total
            except Exception as e:
                return e
def analyze(levelid):
    if str(levelid).isnumeric()==False:
        raise ValueError('Requires level ID, not level name')
    else:
        data = json.loads(fetchapi('https://gdbrowser.com/api/analyze/'+str(levelid)).read())
        total = GDClass(data)
        return total