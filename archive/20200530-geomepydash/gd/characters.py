class Characters:
    def Gatekeeper(expression):
        if expression=='dark':
            return '&char=gatekeeper.dark'
        else:
            return '&char=gatekeeper'
    def Keymaster(expression):
        if expression=='huh':
            return '&char=keymaster.huh'
        elif expression=='scared':
            return '&char=keymaster.scared'
        elif expression=='scream':
            return '&char=keymaster.scream'
        else:
            return '&char=keymaster' 
    def Monster(expression):
        if expression=='eyes':
            return '&char=monster.eyes'
        else:
            return '&char=monster'
    def Potbor(expression):
        if expression=='annoyed':
            return '&char=potbor.'+str(expression)
        elif expression=='huh':
            return '&char=potbor.'+str(expression)
        elif expression=='mad':
            return '&char=potbor.'+str(expression)
        elif expression=='right':
            return '&char=potbor.'+str(expression)
        elif expression=='talk':
            return '&char=potbor.'+str(expression)
        elif expression=='tired':
            return '&char=potbor.'+str(expression)
        else:
            return '&char=potbor'
    def Scratch(expression):
        if expression=='annoyed':
            return '&char=scratch.'+str(expression)
        elif expression=='huh':
            return '&char=scratch.'+str(expression)
        elif expression=='mad':
            return '&char=scratch.'+str(expression)
        elif expression=='right':
            return '&char=scratch.'+str(expression)
        elif expression=='talk':
            return '&char=scratch.'+str(expression)
        elif expression=='tired':
            return '&char=scratch.'+str(expression)
        else:
            return '&char=scratch'
    def Shopkeeper(expression):
        if expression=='annoyed':
            return '&char=shopkeeper.annoyed'
        else:
            return '&char=shopkeeper'
    def Spooky():
        return '&char=spooky'
    def Custom(link):
        return '&url='+str(link)+'&resize=1'