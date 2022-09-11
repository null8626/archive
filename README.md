# archive
Archival of the projects i no longer maintain.

A few things to note before exploring this repository is that the projects in this repo are:
- unmodified, except for slight changes for privacy reasons
- some that i think is okay to be shown to the public
- not allowed to be modified through pull requests
- licensed under the WTFPL license
- expect the links here leading to nowhere and images not loading.

## Personal projects

List of projects i no longer maintain in a chronological order. Small scripts i've made are not included here.

| Project name          | Creation date      | Death date         | Language          | Type                  |
|-----------------------|--------------------|--------------------|-------------------|-----------------------|
| `games.bat`           | August 21, 2019    | August 21, 2019    | Batch             | Console application   |
| BatchOS               | November 13, 2019  | February 2, 2020   | Batch             | Console application   |
| school                | December 7, 2019   | December 7, 2019   | Visual Basic .NET | Graphical application |
| [UninstallWindows v1.0](https://github.com/null8626/archive/tree/main/archive/20191207-UninstallWindows%20v1.0) | December 7, 2019   | December 7, 2019   | Visual Basic .NET | Graphical application |
| Program Manager       | December 9, 2019   | December 9, 2019   | Visual Basic .NET | Graphical application |
| [DownloadButton](https://github.com/null8626/archive/tree/main/archive/20191211-DownloadButton)        | December 11, 2019  | December 15, 2019  | Visual Basic .NET | Graphical application |
| anything              | December 14, 2019  | December 16, 2019  | Visual Basic .NET | Graphical application |
| MemeMaker             | December 29, 2019  | December 30, 2019  | Visual Basic .NET | Graphical application |
| somebot56             | March 14, 2020     | April 7, 2020      | JavaScript        | Discord bot           |
| [username601](https://github.com/null8626/archive/tree/main/archive/20200407-username601)           | April 7, 2020      | January 20, 2022   | Python            | Discord bot           |
| [geomepydash](https://github.com/WilloIzCitron/geomepydash) | May 30, 2020 | July 3, 2020 | Python | Library |
| Useless API           | June 3, 2020       | April 1, 2021      | Python            | Web application       |
| [TerminalPad](https://github.com/null8626/archive/tree/main/archive/20200922-TerminalPad)           | September 22, 2020 | September 23, 2020 | C#                | Console application   |
| twemoji-parser        | November 11, 2020  | June 10, 2021      | Python            | Library               |
| news.js               | January 23, 2021   | June 10, 2021      | JavaScript        | Library               |
| [discord-bot-lib](https://github.com/VoltrexMaster/discord-bot-lib)       | April 3, 2021      | July 12, 2022      | JavaScript        | Library               |
| [`string_toolkit.h`](https://github.com/null8626/archive/tree/main/archive/20210915-string-toolkit)    | July 22, 2021      | October 3, 2021    | C                 | Library               |
| [string-toolkit](https://github.com/null8626/archive/tree/main/archive/20210915-string-toolkit)        | July 26, 2021      | August 3, 2021     | Python            | Library               |
| Anime Luca            | July 10, 2021      | November 5, 2021   | JavaScript        | Discord bot           |
| [topgg-cpp-sdk](https://bestofcpp.com/repo/vierofernando-topgg-cpp-sdk-cpp-utilities)         | September 15, 2021 | November 16, 2021  | C++               | Library               |
| [jsfuck](https://github.com/null8626/jsfuck)                | October 9, 2021    | September 9, 2022  | C                 | Library               |
| [cprintf](https://github.com/null8626/cprintf)               | October 10, 2021   | June 11, 2022      | C                 | Library               |
| vython                | November 24, 2021  | November 28, 2021  | V, C              | Library                |
| ansi-adventure        | December 3, 2021   | December 12, 2021  | C                 | Console application(s) |
| jsfuck                | March 3, 2022      | May 30, 2022       | Rust              | Library                |
| weather-cli           | March 8, 2022      | July 13, 2022      | Rust              | Console application    |
| [sus](https://github.com/null8626/archive/tree/main/archive/20220428-sus) | April 28, 2022     | July 13, 2022      | C                 | Console application    |
| fast-fail             | May 5, 2022        | July 13, 2022      | TypeScript        | Console application    |

## Gallery

### MemeMaker (2019)

<p align="center">Planned meme templates to be supported by `MemeMaker`:</p>

<table align="center">
  <tbody>
    <tr>
      <td align="center">
        <img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20191230-meme1.png" width="175px;" alt=""/>
      </td>
      <td align="center">
        <img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20191230-meme2.png" width="175px;" alt=""/>
      </td>
      <td align="center">
        <img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20191230-meme3.png" width="175px;" alt=""/>
      </td>
    </tr>
  </tbody>
</table>

### somebot56 (2020)

<table align="center">
  <tbody>
    <tr>
      <td>
      
```js
// https://api.qrserver.com/v1/create-qr-code/?size=150x150&data=qrcode

const Discord = require('discord.js')
const {prefix, token} = require('../config.json')

module.exports.run = async (message, bot, args) => {
    let msg = message.content.toLowerCase();
    // command = !qr
    var msgarr = msg.split("");
    for (i = 0; i < 4; i++) {
        msgarr.pop();
    }
    var wordToConvert = msgarr.join("");
    var afterConvertArray = [];
    var wtcArr = wordToConvert.split("");
    var id = 0;
    var alphabet = "abcdefghijklmnopqrstuvwxyz";
    var alphabet = alphabet.split("");
    for (i = 0; i < wordToConvert.length; i++) {
        if (wtcArr[i]==" ") {
            afterConvertArray.push("%20");
        } else if (alphabet.includes(wtcArr[i])==true) {
            afterConvertArray.push(wtcArr[i]);
        } else {
            var id = 1;
        }
    }
    if (id!=1) {
        var imglink = afterConvertArray.join("");
        message.channel.send({files: [{ attachment: imglink}]});
    } else {
        message.channel.send("Error: Invalid symbols detected.");
    }
}

module.exports.config = {
    name: "qrcode"
}
```

<p align="center"><i>somebot56's only surviving code snippet (April 4, 2020)</i></p>
      </td>
    </tr>
  </tbody>
</table>

### username601 (2020 - 2022)

<table align="center">
  <tbody>
    <tr>
      <td align="center">
        <img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20200407-username601.png" width="175px;" alt=""/>
        <br />
        <p><i>username601's profile picture (April 7, 2020 - May 22, 2020)</i></p>
      </td>
      <td align="center">
        <img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20200522-username601.png" width="175px;" alt=""/>
        <p><i>username601's profile picture (May 22, 2020 - January 20, 2022)</i></p>
      </td>
      <td align="center">
        <img src="https://www.boxed-up.co.uk/image/cache/data/international-post-box-350x350.png" width="175px;" alt=""/>
        <br />
        <p><i>username601's profile picture image reference</i></p>
      </td>
    </tr>
  </tbody>
</table>

<table align="center">
  <tbody>
    <tr>
      <td align="center">
        <img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20200527-username601.png" width="455px;" height="256px;" alt=""/>-
        <br />
        <p><i>username601's website (May 27, 2020)</i></p>
      </td>
    </tr>
  </tbody>
  <tbody>
    <tr>
      <td align="center">
        <img src="https://cdn.discordapp.com/attachments/704061031500218392/715146951976419328/unknown.png" width="455px;" height="256px;" alt=""/>
        <br />
        <p><i>username601's website command list (May 27, 2020)</i></p>
      </td>
    </tr>
  </tbody>
  <tbody>
    <tr>
      <td align="center">
        <img src="https://cdn.discordapp.com/attachments/699238761711665222/764429178815905792/unknown.png" width="455px;" height="256px;" alt=""/>
        <br />
        <p><i>username601's website (October 10, 2020)</i></p>
      </td>
    </tr>
  </tbody>
</table>

<table align="center">
  <tbody>
    <tr>
      <td align="center">
        <img src="https://cdn.discordapp.com/attachments/722752725267382323/794115016130560000/unknown.png" width="294px;" height="208px;" alt=""/>
        <br />
        <p><i>the infamous spinny <a href="https://top.gg">top.gg</a> page (December 31, 2020)</i></p>
      </td>
    </tr>
  </tbody>
</table>

#### Reviews

- Not all reviews are listed here. This is just the first page at the time of the bot's deletion.
- Every review listed here gives a 5 star rating. (No, i did not filter out the negative ones.)

| Reviewer                                               | Review date       | Review                            | Reply                                                     |
|--------------------------------------------------------|-------------------|-----------------------------------|-----------------------------------------------------------|
| [norizon](https://top.gg/user/8723421806292828160)     | November 5, 2020  | *I like to explode people's head* | *ah i see thank you for enjoying our explode command :^)* |
| [davon](https://top.gg/user/8422266759003439104) | December 11, 2020 | *good work!* | *thanks man!* |
| [jaffadev](https://top.gg/user/7689549961838133248) | December 13, 2020 | *OMG THIS IS THE BEST BOT EVER MATE IT SO PERFECT AAAAAAAAAAA I LOVE YOU NULL#8626 OWO<br /><br />No but on a serious note, the bot is definitely worth it. You should definitely invite as it has many features and is all around great to use!<br /><br />You have really outdone yourself on this bot null. 💝* | *thanks qt* |
| [AshtonMemer](https://top.gg/user/7772680608630374400) | December 31, 2020 | *if you like memes, economy, and funny images, then get this bot. if you don't like memes, economy, or funny images, then get this bot.* | *lmaoo thanks ashton* |
| [WilloIzCitron](https://top.gg/user/8333103529996255232) | December 31, 2020 | *all image is spinning now i like it* | *nice* |
| [flazepe](https://top.gg/user/7632495051489869824) | January 1, 2021 | *good bot for image generation and other multipurpose commands. i can tell that this was 100% made with effort* | *thanks and yes i love your sarcasm* |
| [Matthew.](https://top.gg/user/7669402925235109888) | January 3, 2021 | *i like :yes:* | *:yes:* |
| [MILLION](https://top.gg/user/3550838723584917504) | January 11, 2021 | *So much better than mee6* | *so true* |
| [alexcool](https://top.gg/user/7687843929961201664) | February 2, 2021 | *this bot page is inside a washing machine<br />also good bot 10/10 would recommend* | *thanks xd* |
| [ar](https://top.gg/user/1459010777929940992) | March 6, 2021 | *im reviewing because i want my face spinned here forever<br />also, yea, the bot is very cool too, i've been using it for a very long time :)* | *thank you!* |
