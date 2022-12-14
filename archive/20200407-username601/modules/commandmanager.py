from json import loads
from discord import Embed, Color, Client

class BotCommands:
    __slots__ = ('client', 'categories', 'commands', 'length')

    def __init__(self, client):
        self.client = client
    
    async def initiate(self):
        try:
            if isinstance(self.client, Client):
                self.client = self.client.http._HTTPClient__session
            
            raw_data = await self.client.get("https://raw.githubusercontent.com/null8626/username601/master/assets/json/commands.json")
            raw_data = loads(await raw_data.text())
            self.categories = list(map(lambda i: list(i.keys())[0], raw_data))[:-1]
            self.commands = []
            for i in range(len(self.categories)):
                for j in raw_data[i][self.categories[i]]:
                    self.commands.append({
                        "name": j["n"], "function": j["f"], "parameters": j["p"], "apis": j["a"], "category": self.categories[i], "type": "COMMAND"
                    })
            self.length = len(self.commands)
        except Exception as e:
            return print(f"error: please put config.ini file in the same directory\nand/or make sure commands.json is stored in <JSON_DIR key in config.ini file>.\n\nraw error message: {e}")
    
    async def invalid_args(self, ctx, name = None):
        name = name or ctx.command.name
        command_info = [i for i in self.commands if i["name"] == name][0]
        embed = Embed(title="Invalid arguments", color=Color.red())
        embed.add_field(name="Description", value=command_info["function"], inline=False)
        embed.add_field(name="Usage", value="```"+"\n".join([i.split(": ")[1] for i in command_info["parameters"]])+"```", inline=False)
        
        if ctx.command.aliases:
            embed.add_field(name="Command Aliases", value=", ".join([f'`{i}`' for i in ctx.command.aliases]), inline=False)
        await ctx.reply(embed=embed)
        del command_info, embed
    
    def get_commands_from_category(self, category_name):
        category = list(filter(lambda x: x.lower().startswith(category_name) or category_name in x.lower(), self.categories))
        if not category: return
        return [i for i in self.commands if i['category'].lower() == category[0].lower()]