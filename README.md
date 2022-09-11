# archive
Archival of the projects i no longer maintain.

A few things to note before exploring this repository is that the projects in this repo are:
- unmodified, except for slight changes for privacy reasons
- some that i think is okay to be shown to the public
- not allowed to be modified through pull requests
- licensed under the WTFPL license
- expect the links here leading to nowhere and images not loading.

<details><summary><h2>Personal projects</h2></summary>

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
| `string_toolkit.h`    | June 22, 2021      | October 3, 2021    | C                 | Library               |
| Anime Luca            | July 10, 2021      | November 5, 2021   | JavaScript        | Discord bot           |
| topgg-cpp-sdk         | September 15, 2021 | November 14, 2021  | C++               | Library               |
| [jsfuck](https://github.com/null8626/jsfuck)                | October 9, 2021    | September 9, 2022  | C                 | Library               |
| [cprintf](https://github.com/null8626/cprintf)               | October 10, 2021   | June 11, 2022      | C                 | Library               |
| vython                | November 24, 2021  | November 28, 2021  | V, C              | Library                |
| ansi-adventure        | December 3, 2021   | December 12, 2021  | C                 | Console application(s) |
| jsfuck                | March 3, 2022      | May 30, 2022       | Rust              | Library                |
| weather-cli           | March 8, 2022      | July 13, 2022      | Rust              | Console application    |
| [sus](https://github.com/null8626/archive/tree/main/archive/20220428-sus) | April 28, 2022     | July 13, 2022      | C                 | Console application    |
| fast-fail             | May 5, 2022        | July 13, 2022      | TypeScript        | Console application    |

</details><details><summary><h2>Foreign projects</h2></summary>

List of foreign projects i used to collaborate in a chronological order.

| Project name                                                  | Joined at         | Left at           | Language   | Type        |
|---------------------------------------------------------------|-------------------|-------------------|------------|-------------|
| [ourcord](https://github.com/ourpalace/ourcord)               | December 13, 2020 | February 13, 2021 | TypeScript | Library     |
| Hexagon                                                       | March 29, 2021    | March 30, 2022    | JavaScript | Discord bot |
| [VXComplexity](https://top.gg/bot/685787142160777253)         | July 30, 2021     | November 5, 2021  | JavaScript | Discord bot |
| [`animality.h`](https://github.com/animality-xyz/animality.h) | August 7, 2021    | May 18, 2022      | C          | Library     |
| [Invitey](https://github.com/invitey)                         | November 6, 2021  | November 11, 2021 | JavaScript | Discord bot |
| [spinners](https://github.com/rhygg/spinners)                 | November 17, 2021 | January 6, 2022   | V, C       | Library     |

</details>

## Gallery

<details><summary><h3>somebot56 (2020)</h3></summary>

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

</details><details><summary><h3>username601 (2020 - 2022)</h3></summary>

<table align="center">
  <tbody>
    <tr>
      <td align="center">
        <!--<img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20200407-username601.png" width="250px;" alt=""/>-->
        <img src="https://avatars.githubusercontent.com/u/60427892" width="175px;" alt=""/>
        <br />
        <p><i>username601's profile picture (April 7, 2020 - May 22, 2020)</i></p>
      </td>
      <td align="center">
        <!--<img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20200522-username601.png" width="250px;" alt=""/>-->
        <img src="https://avatars.githubusercontent.com/u/60427892" width="175px;" alt=""/>
        <br />
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
        <!--<img src="https://raw.githubusercontent.com/null8626/archive/main/assets/20200527-username601.png" width="250px;" alt=""/>-->
        <img src="https://cdn.discordapp.com/attachments/704061031500218392/715146951976419328/unknown.png" width="455px;" height="256px;" alt=""/>
        <br />
        <p><i>username601's website front page (May 27, 2020)</i></p>
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
</table>

</details>