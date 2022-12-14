from PIL import Image, ImageDraw, ImageFont
from io import BytesIO
from time import strftime, gmtime
from datetime import datetime as t
from .colorthief import Smart_ColorThief

class CustomPanel:
    async def __buffer_from_url(self, url: str , *args, **kwargs):
        res = await self.ctx.bot.http._HTTPClient__session.get(url)
        res = await res.read()
        return Image.open(BytesIO(res))

    def __invert(self, color):
        if sum(color) >= 126: return (0, 0, 0)
        return (255, 255, 255)

    async def __get_color_accent(self, url: str , right: bool = False):
        res = Smart_ColorThief(self.ctx, url)
        return await res.get_color(right=right)

    def __init__(
        self,
        ctx,
        title: str = "Title text",
        subtitle: str = "Subtitle text",
        description: str = "Description text here",
        icon: str = "https://cdn.discordapp.com/embed/avatars/0.png",
        font: str = "NotoSansDisplay-Bold.otf",
        min_width: int = 500,
        margin: int = 20,
        spotify = None,
        auto_draw: bool = False
    ):
        """
        Creates a custom panel.
        Can be used for Spotify commands.
        Example:

        spotify = CustomPanel(ctx, spotify=ctx.author.activity)
        await spotify.draw()
        await spotify.send_as_attachment(content=f"This is your spotify activity, {ctx.author.display_name}!")
        spotify.close()
        """
        
        self.ctx = ctx
        self.__get_font = (lambda s: ImageFont.truetype(self.ctx.bot.util.fonts_dir + "/" + font, s))
        self.TITLE_TEXT = title
        self.TITLE_FONT = self.__get_font(30)
        self.SUBTITLE_TEXT = subtitle
        self.SUBTITLE_FONT = self.__get_font(20)
        self.DESC_TEXT = description
        self.DESC_FONT = self.__get_font(15)
        self.COVER_URL = icon
        self.MARGIN_INPUT = margin
        self.MIN_WIDTH_INPUT = min_width

        if auto_draw:
            self.draw()
    
    async def draw(self):
        """ Draws the whole thing. """
        
        self.COVER = await self.__buffer_from_url(self.COVER_URL)
        self.COVER = self.COVER.resize((200, 200))
        self.BACKGROUND_COLOR = await self.__get_color_accent(self.COVER_URL, right=True)
        self.FOREGROUND_COLOR = self.__invert(self.BACKGROUND_COLOR)

        if len(self.TITLE_TEXT) > 25: self.TITLE_TEXT = self.TITLE_TEXT[:50] + "..."
        if len(self.SUBTITLE_TEXT) > 35: self.SUBTITLE_TEXT = self.SUBTITLE_TEXT[:35] + "..."
        if len(self.DESC_TEXT) > 45: self.DESC_TEXT = self.DESC_TEXT[:45] + "..."

        self.TITLE_SIZE = self.TITLE_FONT.getsize(self.TITLE_TEXT)
        self.SUBTITLE_SIZE = self.SUBTITLE_FONT.getsize(self.SUBTITLE_TEXT)
        self.DESC_SIZE = self.DESC_FONT.getsize(self.DESC_TEXT)
        self.WIDTH = max([self.TITLE_SIZE[0], self.SUBTITLE_SIZE[0], self.DESC_SIZE[0]]) + 270

        if (self.MIN_WIDTH_INPUT and (self.WIDTH < self.MIN_WIDTH_INPUT)):
            self.WIDTH = self.MIN_WIDTH_INPUT
        
        self.MARGIN_LEFT = 200 + self.MARGIN_INPUT
        self.MARGIN_RIGHT = self.WIDTH - self.MARGIN_INPUT
        self.MARGIN_TOP = self.MARGIN_INPUT

        self.MAIN = Image.new(mode="RGB", color=self.BACKGROUND_COLOR, size=(self.WIDTH, 200))
        self.DRAW = ImageDraw.Draw(self.MAIN)
        self.DRAW.text((self.MARGIN_LEFT, self.MARGIN_TOP), self.TITLE_TEXT, font=self.TITLE_FONT, fill=self.FOREGROUND_COLOR)
        self.DRAW.text((self.MARGIN_LEFT, self.MARGIN_TOP + 38), self.SUBTITLE_TEXT, font=self.SUBTITLE_FONT, fill=self.FOREGROUND_COLOR)
        self.DRAW.text((self.MARGIN_LEFT, self.MARGIN_TOP + 65), self.DESC_TEXT, font=self.DESC_FONT, fill=self.FOREGROUND_COLOR)

        self.MAIN.paste(self.COVER, (0, 0))
    
    def get_buffer(self):
        if not hasattr(self, "MAIN"):
            raise OSError("Canvas has not been drawn yet.")
        
        BytesIO_array = BytesIO()
        self.MAIN.save(BytesIO_array, format="PNG")
        BytesIO_array.seek(0)
        return BytesIO_array
    
    async def send_as_attachment(self, content=None):
        return await self.ctx.send_image(self.get_buffer(), content=content)
    
    def close(self):
        """ Clears all the garbage out. """
        
        del (
            self.MAIN,
            self.COVER,
            self.COVER_URL,
            self.DESC_FONT,
            self.DESC_SIZE,
            self.DESC_TEXT,
            self.DRAW,
            self.FOREGROUND_COLOR,
            self.MARGIN_INPUT,
            self.MARGIN_LEFT,
            self.MARGIN_RIGHT,
            self.MARGIN_TOP,
            self.MIN_WIDTH_INPUT,
            self.SUBTITLE_FONT,
            self.SUBTITLE_SIZE,
            self.SUBTITLE_TEXT,
            self.TITLE_FONT,
            self.TITLE_SIZE,
            self.TITLE_TEXT,
            self.WIDTH,
            self.ctx
        )