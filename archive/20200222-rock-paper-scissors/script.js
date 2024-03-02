function commenceWinner(n) {
                if (n==1){
                    document.getElementById('topContext').innerHTML = "Sorry... You lose!";
                    document.querySelector('body').style.backgroundColor = "red";
                } else if (n==2) {
                    document.getElementById('contextLeft').style.color = "black";
                    document.getElementById('contextRight').style.color = "black";
                    document.getElementById('topContext').style.color = "black";
                    document.getElementById('copyright').style.color = "black";
                    document.getElementById('topContext').innerHTML = "Draw!";
                    document.querySelector('body').style.backgroundColor = "yellow";
                } else if (n==0) {
                    document.getElementById('topContext').innerHTML = "Congratulations! You win!";
                    document.querySelector('body').style.backgroundColor = "lightgreen";
                }
                document.getElementById('tryAgain').style.visibility = 'visible';
            }
            //1 = Rock
            //2 = Paper
            //3 = Scissors
            function rockPaperScissors(x) {
                var handLeft = document.getElementById('pictureLeft');
                var handRight = document.getElementById('pictureRight');
                var imageLibrary = ["url('https://adeletedgithubpage.github.io/sources/rock-right.png')", "url('https://adeletedgithubpage.github.io/sources/paper-right.png')", "url('https://adeletedgithubpage.github.io/sources/scissors-right.png')"];
                // url('https://adeletedgithubpage.github.io/sources/rock-right.png')
                // url('https://adeletedgithubpage.github.io/sources/paper-right.png')
                // url('https://adeletedgithubpage.github.io/sources/scissors-right.png')
                // handRight.style.backgroundImage
                if (x==2) {
                    document.getElementById('pictureRight').style.backgroundImage = imageLibrary[1];
                } else if (x==3) {
                    document.getElementById('pictureRight').style.backgroundImage = imageLibrary[2];
                }
                // handRight.style.backgroundImage
                // handRight.style.backgroundImage
                if (x==1) {
                    handRight.style.backgroundImage = imageLibrary[0];
                }
                var id = x;
                var randomizer = Math.floor(Math.random() * 3); //GENERATE RANDOM NUMBER FROM 0 TO 2
                // 0 = ROCK
                // 1 = PAPER
                // 2 = SCISSORS
                if (randomizer==0){
                    //ROCK
                    handLeft.style.backgroundImage = "url('https://adeletedgithubpage.github.io/sources/rock-left.png')";
                    if (x==1) {
                        var idFunction = 2;
                    } else if (x==2) {
                        var idFunction = 0;
                    } else if (x==3) {
                        var idFunction = 1;
                    }
                } else if (randomizer==1) {
                    //PAPER
                    handLeft.style.backgroundImage = "url('https://adeletedgithubpage.github.io/sources/paper-left.png')";
                    if (x==1) {
                        var idFunction = 1;
                    } else if (x==2) {
                        var idFunction = 2;
                    } else if (x==3) {
                        var idFunction = 0;
                    }
                } else if (randomizer==2) {
                    //SCISSORS
                    handLeft.style.backgroundImage = "url('https://adeletedgithubpage.github.io/sources/scissors-left.png')";
                    if (x==1) {
                        var idFunction = 0;
                    } else if (x==2) {
                        var idFunction = 1;
                    } else if (x==3) {
                        var idFunction = 2;
                    }
                }
                document.getElementById('contextLeft').innerHTML = 'COMPUTER/AI';
                document.getElementById('contextRight').innerHTML = 'YOU';
                document.querySelector('.startPlaying').style.visibility = 'hidden';
                //COMMENCE THE RESULTS
                commenceWinner(idFunction);
            }